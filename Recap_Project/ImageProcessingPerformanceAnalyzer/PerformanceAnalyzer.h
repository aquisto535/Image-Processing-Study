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
 * @brief ���� �˰������� ��ġ��ũ ����� �����ϴ� ����ü�Դϴ�.
 */
struct BenchmarkResult {
    std::string algorithmName; // �˰����� �̸�
    long imageSize;            // ó���� �̹��� ũ�� (�ȼ� ��)
    std::string imageType;     // �̹��� Ÿ�� (e.g., "grayscale", "color")
    double opencvTime;         // OpenCV �Լ� ���� �ð� (ms)
    double customTime;         // ���� ������ �Լ� ���� �ð� (ms)
    double speedupRatio;       // ���� ��� ���� (OpenCV ���)
    double accuracy;           // ��Ȯ�� ���� (%)
    size_t memoryUsage;        // �޸� ��뷮 (bytes)
};

/**
 * @class PerformanceAnalyzer
 * @brief ���� ó�� �˰������� ������ �����ϰ� �м��ϴ� Ŭ�����Դϴ�.
 */
class PerformanceAnalyzer {
public:
    /**
     * @brief ������
     */
    PerformanceAnalyzer() = default;

    /**
     * @brief OpenCV ������ Custom ������ ������ �� �м��մϴ�.
     * @param algorithm_name �м��� �˰������� �̸�
     * @param opencv_func OpenCV ��� �Լ�
     * @param custom_func ���� ������ �Լ�
     * @param input_image �׽�Ʈ�� ����� �Է� �̹���
     * @param iterations �ݺ� ���� Ƚ��
     * @return ��ġ��ũ ����� ��� BenchmarkResult ��ü
     */
    BenchmarkResult runComparison(
        const std::string& algorithm_name,
        std::function<cv::Mat(const cv::Mat&)> opencv_func,
        std::function<cv::Mat(const cv::Mat&)> custom_func,
        const cv::Mat& input_image,
        int iterations = 100);

    /**
     * @brief ���ݱ����� �м� ����� XML ���Ϸ� �����մϴ�.
     * @param output_file ������ ���� ���
     */
    void generateReport(const std::string& output_file);

    /**
     * @brief ����� ��� ��ġ��ũ ����� ��ȯ�մϴ�.
     * @return BenchmarkResult ���Ϳ� ���� const ����
     */
    const std::vector<BenchmarkResult>& getResults() const;

    /**
     * @brief Ư�� �Լ��� ���� �ð��� �����մϴ�.
     * @tparam Func ������ �Լ��� Ÿ�� (���� ���� �Լ�)
     * @param func ������ �Լ� ��ü
     * @param iterations �ݺ� ���� Ƚ��
     * @return ��� ���� �ð� (ms)
     */
    template<typename Func>
    double measureExecutionTime(Func func, int iterations)
    {
        // ù ������ JIT ������ �� �غ� �ð����� ���� �ְ�� �� �����Ƿ� ����(warm-up) ����
		func(); //Ŀ���� �Լ��� OpenCV �Լ� ��ο� ���� ���� ���� ����

		auto start = std::chrono::high_resolution_clock::now(); //high_resolution_clock�� ������ ���� ���� �ػ��� �ð踦 ����
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
     * @brief �� ��� �̹��� ���� ��Ȯ���� �ٹ������ �����ϱ� ���� ���� ����ü
     */
    struct AccuracyMetrics {
        double mse = 0.0;                 // Mean Squared Error
        double psnr = 0.0;                // Peak Signal-to-Noise Ratio
        double ssim = 0.0;                // Structural Similarity Index
        double histogram_similarity = 0.0; // ������׷� ���絵
    };

    /**
     * @brief �� �̹����� ��Ȯ���� ���Ͽ� AccuracyMetrics�� ��ȯ�մϴ�.
     */
    AccuracyMetrics compareResults(const cv::Mat& original_image, const cv::Mat& result_image);

    /**
     * @brief �� �̹����� SSIM ���� ����մϴ�.
     */
    double calculateSSIM(const cv::Mat& img1, const cv::Mat& img2);

    /**
     * @brief ���� ��¥�� �ð� ���ڿ��� ��ȯ�մϴ�.
     */
    std::string getCurrentDateTime();

    // ��� ��ġ��ũ ����� �����ϴ� ����
    std::vector<BenchmarkResult> results;
};

#endif // PERFORMANCE_ANALYZER_H#pragma once
