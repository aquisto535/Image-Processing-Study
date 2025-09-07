#include "HistogramEqualization.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <iostream>

// ============================================================================
// 공개 인터페이스 구현
// ============================================================================

cv::Mat HistogramEqualization::opencv_version(const cv::Mat& src) {
    if (src.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    cv::Mat dst;

    if (src.channels() == 1) {
        // 그레이스케일 이미지
        cv::equalizeHist(src, dst);
    }
    else if (src.channels() == 3) {
        // 컬러 이미지: YUV 변환 후 Y 채널만 평활화
        cv::Mat yuv;
        cv::cvtColor(src, yuv, cv::COLOR_BGR2YUV);

        std::vector<cv::Mat> channels;
        cv::split(yuv, channels);

        cv::equalizeHist(channels[0], channels[0]);

        cv::merge(channels, yuv);
        cv::cvtColor(yuv, dst, cv::COLOR_YUV2BGR);
    }
    else {
        throw std::invalid_argument("Unsupported number of channels: " +
            std::to_string(src.channels()));
    }

    return dst;
}

cv::Mat HistogramEqualization::custom_version(const cv::Mat& src) {
    if (src.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    if (src.channels() == 1) {
        return processGrayscale(src);
    }
    else if (src.channels() == 3) {
        return processColor(src);
    }
    else {
        throw std::invalid_argument("Unsupported number of channels: " +
            std::to_string(src.channels()));
    }
}

cv::Mat HistogramEqualization::adaptive_version(const cv::Mat& src, double clipLimit,
    cv::Size tileGridSize) {
    if (src.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    cv::Mat dst;
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(clipLimit, tileGridSize);

    if (src.channels() == 1) {
        clahe->apply(src, dst);
    }
    else if (src.channels() == 3) {
        cv::Mat yuv;
        cv::cvtColor(src, yuv, cv::COLOR_BGR2YUV);

        std::vector<cv::Mat> channels;
        cv::split(yuv, channels);

        clahe->apply(channels[0], channels[0]);

        cv::merge(channels, yuv);
        cv::cvtColor(yuv, dst, cv::COLOR_YUV2BGR);
    }
    else {
        throw std::invalid_argument("Unsupported number of channels: " +
            std::to_string(src.channels()));
    }

    return dst;
}

// ============================================================================
// 히스토그램 계산 및 분석 구현
// ============================================================================

std::vector<int> HistogramEqualization::calculateHistogram(const cv::Mat& src) {
    if (src.channels() != 1) {
        throw std::invalid_argument("Input must be single channel image");
    }

    std::vector<int> histogram(HIST_SIZE, 0);

    for (int i = 0; i < src.rows; i++) {
        const uchar* row_ptr = src.ptr<uchar>(i);
        for (int j = 0; j < src.cols; j++) {
            histogram[row_ptr[j]]++;
        }
    }

    return histogram;
}

std::vector<float> HistogramEqualization::calculateCDF(const std::vector<int>& histogram) {
    std::vector<float> cdf(HIST_SIZE, 0);

    // 누적합 계산
    cdf[0] = static_cast<float>(histogram[0]);
    for (int i = 1; i < HIST_SIZE; i++) {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    // 정규화 (0-255 범위)
    float totalPixels = cdf[HIST_SIZE - 1];
    if (totalPixels > 0) {
        for (int i = 0; i < HIST_SIZE; i++) {
            cdf[i] = (cdf[i] / totalPixels) * 255.0f;
        }
    }

    return cdf;
}

double HistogramEqualization::measureUniformity(const std::vector<int>& histogram) {
    int totalPixels = std::accumulate(histogram.begin(), histogram.end(), 0);
    if (totalPixels == 0) return 0.0;

    double expectedCount = static_cast<double>(totalPixels) / HIST_SIZE;
    double chiSquare = 0.0;

    for (int count : histogram) {
        double diff = count - expectedCount;
        chiSquare += (diff * diff) / expectedCount;
    }

    // 균등성을 0-1 범위로 정규화
    double maxChiSquare = expectedCount * HIST_SIZE;
    return std::max(0.0, 1.0 - (chiSquare / maxChiSquare));
}

double HistogramEqualization::compareHistograms(const std::vector<int>& hist1,
    const std::vector<int>& hist2) {
    if (hist1.size() != hist2.size()) {
        throw std::invalid_argument("Histograms must have same size");
    }

    // 코사인 유사도 계산
    double dotProduct = 0.0;
    double norm1 = 0.0, norm2 = 0.0;

    for (size_t i = 0; i < hist1.size(); i++) {
        dotProduct += hist1[i] * hist2[i];
        norm1 += hist1[i] * hist1[i];
        norm2 += hist2[i] * hist2[i];
    }

    if (norm1 == 0.0 || norm2 == 0.0) return 0.0;

    return dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
}

// ============================================================================
// 시각화 및 검증 구현
// ============================================================================

cv::Mat HistogramEqualization::createComparisonImage(const cv::Mat& original,
    const cv::Mat& equalized) {
    cv::Mat originalHist = drawHistogram(original, cv::Scalar(255, 0, 0));
    cv::Mat equalizedHist = drawHistogram(equalized, cv::Scalar(0, 255, 0));

    // 이미지 크기 조정
    cv::Mat originalResized, equalizedResized;
    cv::Size targetSize(HIST_WIDTH, HIST_HEIGHT);

    cv::resize(original, originalResized, targetSize);
    cv::resize(equalized, equalizedResized, targetSize);

    // 그레이스케일을 컬러로 변환
    if (originalResized.channels() == 1) {
        cv::cvtColor(originalResized, originalResized, cv::COLOR_GRAY2BGR);
    }
    if (equalizedResized.channels() == 1) {
        cv::cvtColor(equalizedResized, equalizedResized, cv::COLOR_GRAY2BGR);
    }

    // 4분할 이미지 생성
    cv::Mat result = cv::Mat::zeros(HIST_HEIGHT * 2, HIST_WIDTH * 2, CV_8UC3);

    // 좌상: 원본 이미지
    originalResized.copyTo(result(cv::Rect(0, 0, HIST_WIDTH, HIST_HEIGHT)));

    // 우상: 평활화된 이미지
    equalizedResized.copyTo(result(cv::Rect(HIST_WIDTH, 0, HIST_WIDTH, HIST_HEIGHT)));

    // 좌하: 원본 히스토그램
    originalHist.copyTo(result(cv::Rect(0, HIST_HEIGHT, HIST_WIDTH, HIST_HEIGHT)));

    // 우하: 평활화된 히스토그램
    equalizedHist.copyTo(result(cv::Rect(HIST_WIDTH, HIST_HEIGHT, HIST_WIDTH, HIST_HEIGHT)));

    // 텍스트 레이블 추가
    cv::putText(result, "Original", cv::Point(10, 30),
        cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
    cv::putText(result, "Equalized", cv::Point(HIST_WIDTH + 10, 30),
        cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
    cv::putText(result, "Original Histogram", cv::Point(10, HIST_HEIGHT + 30),
        cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
    cv::putText(result, "Equalized Histogram", cv::Point(HIST_WIDTH + 10, HIST_HEIGHT + 30),
        cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

    return result;
}

cv::Mat HistogramEqualization::drawHistogram(const cv::Mat& src, const cv::Scalar& color,
    bool showCDF) {
    cv::Mat grayImage;
    if (src.channels() == 3) {
        cv::cvtColor(src, grayImage, cv::COLOR_BGR2GRAY);
    }
    else {
        grayImage = src.clone();
    }

    std::vector<int> histogram = calculateHistogram(grayImage);
    cv::Mat histImage = drawHistogramInternal(histogram, color, HIST_WIDTH, HIST_HEIGHT);

    if (showCDF) {
        std::vector<float> cdf = calculateCDF(histogram);

        // CDF를 히스토그램 높이에 맞게 정규화
        std::vector<float> normalizedCDF(cdf.size());
        for (size_t i = 0; i < cdf.size(); i++) {
            normalizedCDF[i] = (cdf[i] / 255.0f) * HIST_HEIGHT;
        }

        // CDF 그리기
        int binWidth = cvRound(static_cast<double>(HIST_WIDTH) / HIST_SIZE);
        for (int i = 1; i < HIST_SIZE; i++) {
            cv::line(histImage,
                cv::Point(binWidth * (i - 1), HIST_HEIGHT - cvRound(normalizedCDF[i - 1])),
                cv::Point(binWidth * i, HIST_HEIGHT - cvRound(normalizedCDF[i])),
                cv::Scalar(0, 255, 255), 1, 8, 0);  // 노란색으로 CDF 표시
        }
    }

    return histImage;
}

HistogramEqualization::QualityMetrics HistogramEqualization::analyzeQuality(
    const cv::Mat& original, const cv::Mat& equalized) {

    QualityMetrics metrics;

    // 그레이스케일로 변환
    cv::Mat originalGray, equalizedGray;
    if (original.channels() == 3) {
        cv::cvtColor(original, originalGray, cv::COLOR_BGR2GRAY);
    }
    else {
        originalGray = original.clone();
    }

    if (equalized.channels() == 3) {
        cv::cvtColor(equalized, equalizedGray, cv::COLOR_BGR2GRAY);
    }
    else {
        equalizedGray = equalized.clone();
    }

    // 균등성 개선도
    auto originalHist = calculateHistogram(originalGray);
    auto equalizedHist = calculateHistogram(equalizedGray);

    double originalUniformity = measureUniformity(originalHist);
    double equalizedUniformity = measureUniformity(equalizedHist);
    metrics.uniformityImprovement = equalizedUniformity - originalUniformity;

    // 대비 향상도
    double originalContrast = measureContrast(originalGray);
    double equalizedContrast = measureContrast(equalizedGray);
    metrics.contrastEnhancement = equalizedContrast / originalContrast;

    // 정보량 변화 (엔트로피)
    double originalEntropy = calculateEntropy(originalGray);
    double equalizedEntropy = calculateEntropy(equalizedGray);
    metrics.informationContent = equalizedEntropy / originalEntropy;

    // MSE 계산
    cv::Mat diff;
    cv::absdiff(originalGray, equalizedGray, diff);
    diff.convertTo(diff, CV_64F);
    cv::Scalar mseScalar = cv::mean(diff.mul(diff));
    metrics.mse = mseScalar[0];

    // PSNR 계산
    if (metrics.mse > 0) {
        metrics.psnr = 10.0 * std::log10(255.0 * 255.0 / metrics.mse);
    }
    else {
        metrics.psnr = std::numeric_limits<double>::infinity();
    }

    return metrics;
}

bool HistogramEqualization::validateHistogramEqualization(const cv::Mat& testImage) {
    std::cout << "\n=== 히스토그램 평활화 정확도 검증 ===" << std::endl;

    cv::Mat opencvResult = opencv_version(testImage);
    cv::Mat customResult = custom_version(testImage);

    // 그레이스케일로 변환
    cv::Mat opencvGray, customGray;
    if (opencvResult.channels() == 3) {
        cv::cvtColor(opencvResult, opencvGray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(customResult, customGray, cv::COLOR_BGR2GRAY);
    }
    else {
        opencvGray = opencvResult.clone();
        customGray = customResult.clone();
    }

    // 히스토그램 계산 및 비교
    auto opencvHist = calculateHistogram(opencvGray);
    auto customHist = calculateHistogram(customGray);

    double histogramSimilarity = compareHistograms(opencvHist, customHist) * 100.0;

    // MSE 계산
    cv::Mat diff;
    cv::absdiff(opencvGray, customGray, diff);
    diff.convertTo(diff, CV_64F);
    cv::Scalar mseScalar = cv::mean(diff.mul(diff));
    double mse = mseScalar[0];

    // PSNR 계산
    double psnr = (mse > 0) ? 10.0 * std::log10(255.0 * 255.0 / mse) : -1;

    std::cout << "히스토그램 유사도: " << histogramSimilarity << "%" << std::endl;
    std::cout << "MSE: " << mse << std::endl;
    std::cout << "PSNR: " << psnr << " dB" << std::endl;

    // 검증 결과
    bool validationPassed = (histogramSimilarity >= SIMILARITY_THRESHOLD * 100) && (mse < 25.0);
    std::cout << "검증 결과: " << (validationPassed ? "통과" : "실패") << std::endl;

    if (!validationPassed) {
        std::cout << "경고: 히스토그램 평활화 구현이 OpenCV와 차이가 있습니다." << std::endl;
    }

    return validationPassed;
}

// ============================================================================
// 내부 헬퍼 함수 구현
// ============================================================================

cv::Mat HistogramEqualization::processGrayscale(const cv::Mat& src) {
    // 히스토그램 계산
    std::vector<int> histogram = calculateHistogram(src);

    // CDF 계산
    std::vector<float> cdf = calculateCDF(histogram);

    // 픽셀 값 재매핑
    return remapPixels(src, cdf);
}

cv::Mat HistogramEqualization::processColor(const cv::Mat& src) {
    // YUV 색공간으로 변환
    cv::Mat yuv;
    cv::cvtColor(src, yuv, cv::COLOR_BGR2YUV);

    std::vector<cv::Mat> channels;
    cv::split(yuv, channels);

    // Y 채널에 대해서만 히스토그램 평활화
    channels[0] = processGrayscale(channels[0]);

    cv::Mat result;
    cv::merge(channels, yuv);
    cv::cvtColor(yuv, result, cv::COLOR_YUV2BGR);

    return result;
}

cv::Mat HistogramEqualization::remapPixels(const cv::Mat& src, const std::vector<float>& cdf) {
    cv::Mat dst = cv::Mat::zeros(src.size(), CV_8UC1);

    for (int i = 0; i < src.rows; i++) {
        const uchar* src_ptr = src.ptr<uchar>(i);
        uchar* dst_ptr = dst.ptr<uchar>(i);

        for (int j = 0; j < src.cols; j++) {
            uchar pixel = src_ptr[j];
            dst_ptr[j] = cv::saturate_cast<uchar>(cdf[pixel]);
        }
    }

    return dst;
}

double HistogramEqualization::calculateEntropy(const cv::Mat& src) {
    std::vector<int> histogram = calculateHistogram(src);
    int totalPixels = std::accumulate(histogram.begin(), histogram.end(), 0);

    double entropy = 0.0;
    for (int count : histogram) {
        if (count > 0) {
            double probability = static_cast<double>(count) / totalPixels;
            entropy -= probability * std::log2(probability);
        }
    }

    return entropy;
}

double HistogramEqualization::measureContrast(const cv::Mat& src) {
    cv::Scalar mean, stddev;
    cv::meanStdDev(src, mean, stddev);
    return stddev[0];  // 표준편차를 대비 측정값으로 사용
}

cv::Mat HistogramEqualization::drawHistogramInternal(const std::vector<int>& histogram,
    const cv::Scalar& color,
    int width, int height) {
    cv::Mat histImage = cv::Mat::zeros(height, width, CV_8UC3);

    // 히스토그램 정규화
    int maxValue = *std::max_element(histogram.begin(), histogram.end());
    if (maxValue == 0) return histImage;

    int binWidth = cvRound(static_cast<double>(width) / HIST_SIZE);

    for (int i = 1; i < HIST_SIZE; i++) {
        int val1 = cvRound(static_cast<double>(histogram[i - 1]) * height / maxValue);
        int val2 = cvRound(static_cast<double>(histogram[i]) * height / maxValue);

        cv::line(histImage,
            cv::Point(binWidth * (i - 1), height - val1),
            cv::Point(binWidth * i, height - val2),
            color, 2, 8, 0);
    }

    return histImage;
}