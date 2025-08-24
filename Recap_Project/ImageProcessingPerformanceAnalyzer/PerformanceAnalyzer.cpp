#include "PerformanceAnalyzer.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace cv;
using namespace std;

// 결과 정확도 비교 (다중 지표)
PerformanceAnalyzer::AccuracyMetrics PerformanceAnalyzer::compareResults(
    const Mat& opencv_result, const Mat& custom_result) {

    AccuracyMetrics metrics;

    // 1. MSE (Mean Squared Error) 계산
    Mat diff;
    absdiff(opencv_result, custom_result, diff);
    diff.convertTo(diff, CV_32F);

    Scalar mse_scalar = mean(diff.mul(diff));
    metrics.mse = mse_scalar[0];

    // 2. PSNR (Peak Signal-to-Noise Ratio) 계산
    if (metrics.mse > 0) {
        metrics.psnr = 20.0 * log10(255.0 / sqrt(metrics.mse));
    }
    else {
        metrics.psnr = 100.0;  // 완벽히 동일한 경우
    }

    // 3. SSIM (Structural Similarity Index) 계산
    metrics.ssim = calculateSSIM(opencv_result, custom_result);

    // 4. 히스토그램 유사도 (그레이스케일인 경우)
    if (opencv_result.channels() == 1) {
        vector<Mat> opencv_imgs = { opencv_result };
        vector<Mat> custom_imgs = { custom_result };

        Mat opencv_hist, custom_hist;
        int histSize = 256;
        float range[] = { 0, 256 };
        const float* histRange = { range };

        int channels[] = { 0 }; // 채널도 명시적인 배열로 전달하는 것이 더 안전합니다.

        calcHist(&opencv_imgs[0], 1, channels, Mat(), opencv_hist, 1, &histSize, &histRange);
        calcHist(&custom_imgs[0], 1, channels, Mat(), custom_hist, 1, &histSize, &histRange);

        // 정규화
        normalize(opencv_hist, opencv_hist, 0, 1, NORM_MINMAX, -1, Mat());
        normalize(custom_hist, custom_hist, 0, 1, NORM_MINMAX, -1, Mat());

        // 상관계수 계산
        metrics.histogram_similarity = compareHist(opencv_hist, custom_hist, HISTCMP_CORREL) * 100.0;
    }
    else {
        metrics.histogram_similarity = 100.0;  // 컬러 이미지는 100%로 가정
    }

    return metrics;
}

// SSIM 계산 (단순화 버전)
double PerformanceAnalyzer::calculateSSIM(const Mat& img1, const Mat& img2) {
    Mat img1_32f, img2_32f;
    img1.convertTo(img1_32f, CV_32F);
    img2.convertTo(img2_32f, CV_32F);

    // 평균 계산
    Scalar mu1 = mean(img1_32f);
    Scalar mu2 = mean(img2_32f);

    // 분산 및 공분산 계산 (단순화)
    Mat img1_sq, img2_sq, img1_img2;
    multiply(img1_32f, img1_32f, img1_sq);
    multiply(img2_32f, img2_32f, img2_sq);
    multiply(img1_32f, img2_32f, img1_img2);

    Scalar sigma1_sq = mean(img1_sq) - Scalar(mu1[0] * mu1[0]);
    Scalar sigma2_sq = mean(img2_sq) - Scalar(mu2[0] * mu2[0]);
    Scalar sigma12 = mean(img1_img2) - Scalar(mu1[0] * mu2[0]);

    // SSIM 상수
    double C1 = (0.01 * 255) * (0.01 * 255);
    double C2 = (0.03 * 255) * (0.03 * 255);

    // SSIM 계산
    double numerator = (2 * mu1[0] * mu2[0] + C1) * (2 * sigma12[0] + C2);
    double denominator = (mu1[0] * mu1[0] + mu2[0] * mu2[0] + C1) * (sigma1_sq[0] + sigma2_sq[0] + C2);

    return numerator / denominator;
}

// 벤치마크 실행 및 결과 저장
BenchmarkResult PerformanceAnalyzer::runComparison(
    const string& algorithm_name,
    function<Mat(const Mat&)> opencv_func,
    function<Mat(const Mat&)> custom_func,
    const Mat& input_image,
    int iterations) {

    BenchmarkResult result;
    result.algorithmName = algorithm_name;
    result.imageSize = input_image.rows * input_image.cols;
    result.imageType = (input_image.channels() == 1) ? "grayscale" : "color";

    Mat opencv_result, custom_result;

    // OpenCV 함수 성능 측정
    result.opencvTime = measureExecutionTime([&]() {
        opencv_result = opencv_func(input_image);
        }, iterations);

    // Custom 함수 성능 측정
    result.customTime = measureExecutionTime([&]() {
        custom_result = custom_func(input_image);
        }, iterations);

    // 성능 비율 계산
    result.speedupRatio = result.customTime / result.opencvTime;

    // 정확도 분석
    auto accuracy_metrics = compareResults(opencv_result, custom_result);

    // 종합 정확도 점수 (가중 평균)
    result.accuracy = (accuracy_metrics.ssim * 100 * 0.4) +      // SSIM 40%
        (min(accuracy_metrics.psnr / 40.0, 1.0) * 100 * 0.3) +  // PSNR 30%
        (accuracy_metrics.histogram_similarity * 0.3);            // Histogram 30%

    // 메모리 사용량 측정 (단순화)
    result.memoryUsage = input_image.total() * input_image.elemSize();

    // 결과 저장
    results.push_back(result);

    return result;
}

// XML 리포트 생성
void PerformanceAnalyzer::generateReport(const string& output_file) {
    FileStorage fs(output_file, FileStorage::WRITE);

    fs << "PerformanceAnalysisReport" << "{";
    fs << "GeneratedDate" << getCurrentDateTime();
    fs << "TotalAlgorithms" << static_cast<int>(results.size());

    fs << "Results" << "[";
    for (const auto& result : results) {
        fs << "{";
        fs << "Algorithm" << result.algorithmName;
        fs << "OpenCVTime" << result.opencvTime;
        fs << "CustomTime" << result.customTime;
        fs << "PerformanceRatio" << result.speedupRatio;
        fs << "Accuracy" << result.accuracy;
        fs << "ImageSize" << static_cast<double>(result.imageSize);
        fs << "ImageType" << result.imageType;
        fs << "MemoryUsage" << static_cast<int>(result.memoryUsage);
        fs << "}";
    }
    fs << "]";

    fs << "}";
    fs.release();

    cout << "📊 리포트 생성 완료: " << output_file << endl;
}

// 현재 날짜/시간 문자열 반환
string PerformanceAnalyzer::getCurrentDateTime() {
    auto now = chrono::system_clock::now();
    auto time_t = chrono::system_clock::to_time_t(now);
    auto tm = *localtime(&time_t);

    stringstream ss;
    ss << put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

