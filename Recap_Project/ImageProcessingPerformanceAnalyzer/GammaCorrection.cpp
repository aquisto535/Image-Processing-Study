#include "GammaCorrection.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <iostream>

// ============================================================================
// 공개 인터페이스 구현
// ============================================================================

cv::Mat GammaCorrection::opencv_version(const cv::Mat& src, double gamma) {
    if (!isValidGamma(gamma)) {
        throw std::invalid_argument("Gamma value must be between " +
            std::to_string(MIN_GAMMA) + " and " +
            std::to_string(MAX_GAMMA));
    }

    if (src.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    // OpenCV LUT 방식
    cv::Mat lookupTable = createGammaLUT(gamma);
    cv::Mat dst;
    cv::LUT(src, lookupTable, dst);

    return dst;
}

cv::Mat GammaCorrection::custom_version(const cv::Mat& src, double gamma) {
    if (!isValidGamma(gamma)) {
        throw std::invalid_argument("Gamma value must be between " +
            std::to_string(MIN_GAMMA) + " and " +
            std::to_string(MAX_GAMMA));
    }

    if (src.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    // 채널 수에 따라 처리 방식 결정
    if (src.channels() == 1) {
        return processGrayscale(src, gamma);
    }
    else if (src.channels() == 3) {
        return processColor(src, gamma);
    }
    else {
        throw std::invalid_argument("Unsupported number of channels: " +
            std::to_string(src.channels()));
    }
}

cv::Mat GammaCorrection::custom_optimized_version(const cv::Mat& src, double gamma) {
    if (!isValidGamma(gamma)) {
        throw std::invalid_argument("Gamma value must be between " +
            std::to_string(MIN_GAMMA) + " and " +
            std::to_string(MAX_GAMMA));
    }

    if (src.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    // Custom LUT 생성 및 적용
    cv::Mat lookupTable = createGammaLUT(gamma);
    cv::Mat dst = cv::Mat::zeros(src.size(), src.type());

    if (src.channels() == 1) {
        // 그레이스케일 처리
        for (int i = 0; i < src.rows; i++) {
            const uchar* src_ptr = src.ptr<uchar>(i);
            uchar* dst_ptr = dst.ptr<uchar>(i);
            const uchar* lut_ptr = lookupTable.ptr<uchar>(0);

            for (int j = 0; j < src.cols; j++) {
                dst_ptr[j] = lut_ptr[src_ptr[j]];
            }
        }
    }
    else if (src.channels() == 3) {
        // 컬러 처리
        for (int i = 0; i < src.rows; i++) {
            const cv::Vec3b* src_ptr = src.ptr<cv::Vec3b>(i);
            cv::Vec3b* dst_ptr = dst.ptr<cv::Vec3b>(i);
            const uchar* lut_ptr = lookupTable.ptr<uchar>(0);

            for (int j = 0; j < src.cols; j++) {
                dst_ptr[j][0] = lut_ptr[src_ptr[j][0]];  // B
                dst_ptr[j][1] = lut_ptr[src_ptr[j][1]];  // G
                dst_ptr[j][2] = lut_ptr[src_ptr[j][2]];  // R
            }
        }
    }

    return dst;
}

// ============================================================================
// 유틸리티 함수 구현
// ============================================================================

bool GammaCorrection::isValidGamma(double gamma) {
    return (gamma >= MIN_GAMMA && gamma <= MAX_GAMMA);
}

cv::Mat GammaCorrection::compareHistograms(const cv::Mat& original, const cv::Mat& corrected) {
    cv::Mat hist_original = drawHistogram(original, cv::Scalar(255, 0, 0));  // 파란색
    cv::Mat hist_corrected = drawHistogram(corrected, cv::Scalar(0, 255, 0)); // 초록색

    // 히스토그램 합성
    cv::Mat combined;
    cv::addWeighted(hist_original, 0.5, hist_corrected, 0.5, 0, combined);

    return combined;
}

cv::Mat GammaCorrection::multiGammaComparison(const cv::Mat& src, const std::vector<double>& gammaValues) {
    if (gammaValues.empty()) {
        throw std::invalid_argument("Gamma values vector is empty");
    }

    std::vector<cv::Mat> results;
    results.push_back(src.clone()); // 원본 추가

    for (double gamma : gammaValues) {
        if (isValidGamma(gamma)) {
            cv::Mat corrected = opencv_version(src, gamma);

            // 감마 값 텍스트 추가
            cv::Mat labeled = corrected.clone();
            std::string gammaText = "γ=" + std::to_string(gamma).substr(0, 4);
            cv::putText(labeled, gammaText, cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

            results.push_back(labeled);
        }
    }

    // 타일 형태로 배열
    int cols = static_cast<int>(std::ceil(std::sqrt(results.size())));
    int rows = static_cast<int>(std::ceil(static_cast<double>(results.size()) / cols));

    cv::Mat tiled = cv::Mat::zeros(src.rows * rows, src.cols * cols, src.type());

    for (size_t i = 0; i < results.size(); i++) {
        int row = static_cast<int>(i / cols);
        int col = static_cast<int>(i % cols);

        cv::Rect roi(col * src.cols, row * src.rows, src.cols, src.rows);
        results[i].copyTo(tiled(roi));
    }

    return tiled;
}

// ============================================================================
// 내부 헬퍼 함수 구현
// ============================================================================

cv::Mat GammaCorrection::createGammaLUT(double gamma) {
    cv::Mat lookupTable(1, LUT_SIZE, CV_8U);
    uchar* ptr = lookupTable.ptr();

    double invGamma = 1.0 / gamma;

    for (int i = 0; i < LUT_SIZE; i++) {
        double normalized = i / 255.0;
        double corrected = std::pow(normalized, invGamma);
        ptr[i] = cv::saturate_cast<uchar>(corrected * 255.0);
    }

    return lookupTable;
}

uchar GammaCorrection::applyGammaToPixel(uchar pixel, double gamma) {
    double normalized = pixel / 255.0;
    double corrected = std::pow(normalized, 1.0 / gamma);
    return cv::saturate_cast<uchar>(corrected * 255.0);
}

cv::Mat GammaCorrection::processGrayscale(const cv::Mat& src, double gamma) {
    cv::Mat dst = cv::Mat::zeros(src.size(), CV_8UC1);

    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            uchar pixel = src.at<uchar>(i, j);
            dst.at<uchar>(i, j) = applyGammaToPixel(pixel, gamma);
        }
    }

    return dst;
}

cv::Mat GammaCorrection::processColor(const cv::Mat& src, double gamma) {
    cv::Mat dst = cv::Mat::zeros(src.size(), CV_8UC3);

    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            cv::Vec3b pixel = src.at<cv::Vec3b>(i, j);
            cv::Vec3b& dst_pixel = dst.at<cv::Vec3b>(i, j);

            // 각 채널에 개별적으로 감마 보정 적용
            dst_pixel[0] = applyGammaToPixel(pixel[0], gamma);  // B
            dst_pixel[1] = applyGammaToPixel(pixel[1], gamma);  // G
            dst_pixel[2] = applyGammaToPixel(pixel[2], gamma);  // R
        }
    }

    return dst;
}

cv::Mat GammaCorrection::drawHistogram(const cv::Mat& src, const cv::Scalar& color) {
    // 히스토그램 계산
    std::vector<cv::Mat> channels;
    if (src.channels() == 1) {
        channels.push_back(src);
    }
    else {
        cv::split(src, channels);
    }

    cv::Mat hist;
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };

    cv::calcHist(&channels[0], 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

    // 히스토그램 정규화
    cv::normalize(hist, hist, 0, HIST_HEIGHT, cv::NORM_MINMAX, -1, cv::Mat());

    // 히스토그램 이미지 생성
    cv::Mat histImage = cv::Mat::zeros(HIST_HEIGHT, HIST_WIDTH, CV_8UC3);

    int bin_w = cvRound((double)HIST_WIDTH / histSize);

    for (int i = 1; i < histSize; i++) {
        cv::line(histImage,
            cv::Point(bin_w * (i - 1), HIST_HEIGHT - cvRound(hist.at<float>(i - 1))),
            cv::Point(bin_w * i, HIST_HEIGHT - cvRound(hist.at<float>(i))),
            color, 2, 8, 0);
    }

    return histImage;
}
