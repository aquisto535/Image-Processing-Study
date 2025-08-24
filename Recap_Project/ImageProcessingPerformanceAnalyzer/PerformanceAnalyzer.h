#ifndef PERFORMANCE_ANALYZER_H
#define PERFORMANCE_ANALYZER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <iostream>

/**
 * @struct BenchmarkResult
 * @brief 단일 알고리즘의 벤치마크 결과를 저장하는 구조체입니다.
 */
struct BenchmarkResult {
    std::string algorithmName; // 알고리즘 이름
    long imageSize;            // 처리된 이미지 크기 (픽셀 수)
    std::string imageType;     // 이미지 타입 (e.g., "grayscale", "color")
    double opencvTime;         // OpenCV 함수 실행 시간 (ms)
    double customTime;         // 직접 구현한 함수 실행 시간 (ms)
    double speedupRatio;       // 성능 향상 비율 (OpenCV 대비)
    double accuracy;           // 정확도 점수 (%)
    size_t memoryUsage;        // 메모리 사용량 (bytes)
};

/**
 * @class PerformanceAnalyzer
 * @brief 영상 처리 알고리즘의 성능을 측정하고 분석하는 클래스입니다.
 */
class PerformanceAnalyzer {
public:
    /**
     * @brief 생성자
     */
    PerformanceAnalyzer() = default;

    /**
     * @brief OpenCV 구현과 Custom 구현의 성능을 비교 분석합니다.
     * @param algorithm_name 분석할 알고리즘의 이름
     * @param opencv_func OpenCV 기반 함수
     * @param custom_func 직접 구현한 함수
     * @param input_image 테스트에 사용할 입력 이미지
     * @param iterations 반복 측정 횟수
     * @return 벤치마크 결과가 담긴 BenchmarkResult 객체
     */
    BenchmarkResult runComparison(
        const std::string& algorithm_name,
        std::function<cv::Mat(const cv::Mat&)> opencv_func,
        std::function<cv::Mat(const cv::Mat&)> custom_func,
        const cv::Mat& input_image,
        int iterations = 100);

    /**
     * @brief 지금까지의 분석 결과를 XML 파일로 저장합니다.
     * @param output_file 저장할 파일 경로
     */
    void generateReport(const std::string& output_file);

    /**
     * @brief 저장된 모든 벤치마크 결과를 반환합니다.
     * @return BenchmarkResult 벡터에 대한 const 참조
     */
    const std::vector<BenchmarkResult>& getResults() const;

    /**
     * @brief 특정 함수의 실행 시간을 측정합니다.
     * @tparam Func 측정할 함수의 타입 (보통 람다 함수)
     * @param func 측정할 함수 객체
     * @param iterations 반복 측정 횟수
     * @return 평균 실행 시간 (ms)
     */
    template<typename Func>
    double measureExecutionTime(Func func, int iterations) {
        // 첫 실행은 JIT 컴파일 등 준비 시간으로 인해 왜곡될 수 있으므로 웜업(warm-up) 실행
        func();

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> total_time = end - start;
        return total_time.count() / iterations;
    }

private:
    /**
     * @struct AccuracyMetrics
     * @brief 두 결과 이미지 간의 정확도를 다방면으로 측정하기 위한 내부 구조체
     */
    struct AccuracyMetrics {
        double mse = 0.0;                 // Mean Squared Error
        double psnr = 0.0;                // Peak Signal-to-Noise Ratio
        double ssim = 0.0;                // Structural Similarity Index
        double histogram_similarity = 0.0; // 히스토그램 유사도
    };

    /**
     * @brief 두 이미지의 정확도를 비교하여 AccuracyMetrics를 반환합니다.
     */
    AccuracyMetrics compareResults(const cv::Mat& opencv_result, const cv::Mat& custom_result);

    /**
     * @brief 두 이미지의 SSIM 값을 계산합니다.
     */
    double calculateSSIM(const cv::Mat& img1, const cv::Mat& img2);

    /**
     * @brief 현재 날짜와 시간 문자열을 반환합니다.
     */
    std::string getCurrentDateTime();

    // 모든 벤치마크 결과를 저장하는 벡터
    std::vector<BenchmarkResult> results;
};

#endif // PERFORMANCE_ANALYZER_H#pragma once
