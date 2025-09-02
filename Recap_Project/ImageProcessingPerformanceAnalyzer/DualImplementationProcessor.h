#ifndef DUAL_IMPLEMENTATION_PROCESSOR_H
#define DUAL_IMPLEMENTATION_PROCESSOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

class DualImplementationProcessor {
public:
    // Brightness Adjustment methods
    class BrightnessAdjustment {
    public:
        // OpenCV-based implementation
        static cv::Mat opencv_version(const cv::Mat& input, int brightness);

        // A raw, unoptimized custom implementation
        static cv::Mat custom_raw_version(const cv::Mat& input, int brightness);

        // An optimized custom implementation
        static cv::Mat custom_version(const cv::Mat& input, int brightness);
    };

    // Contrast Adjustment methods
    class ContrastAdjustment {
    public:
        // OpenCV ����
        static cv::Mat opencv_version(const cv::Mat& input, double contrast);

        // Custom ���� (������ raw�� ����)
        static cv::Mat custom_version(const cv::Mat& input, double contrast);

        // ����ȭ�� Custom ����
        static cv::Mat custom_optimized_version(const cv::Mat& input, double contrast);
    };

    // Gamma Correction methods
    class GammaCorrection {
    public:
        // OpenCV-based implementation
        static cv::Mat opencv_version(const cv::Mat& input, double gamma);

        // A raw, unoptimized custom implementation
        static cv::Mat custom_raw_version(const cv::Mat& input, double gamma);

        // An optimized custom implementation using a LUT
        static cv::Mat custom_version(const cv::Mat& input, double gamma);
    };

    // Image Inversion methods
    class ImageInversion {
    public:
        // OpenCV ����
        static cv::Mat opencv_version(const cv::Mat& input);

        // Custom ���� (������ raw�� ����)
        static cv::Mat custom_version(const cv::Mat& input);

        // ����ȭ�� Custom ����
        static cv::Mat custom_optimized_version(const cv::Mat& input);
    };

    // ������׷� ��Ȱȭ Ŭ����
    class HistogramEqualization {
    public:
        // OpenCV ����
        static cv::Mat opencv_version(const cv::Mat& input);

        // Custom ���� (���������Լ� ���, raw�� ����)
        static cv::Mat custom_version(const cv::Mat& input);

        // ����ȭ�� Custom ����
        static cv::Mat custom_optimized_version(const cv::Mat& input);
    };

    // ���� ���� ��ȯ Ŭ����
    class ColorSpaceConversion {
    public:
        // RGB to Grayscale
        static cv::Mat rgbToGrayscale_opencv(const cv::Mat& input);
        static cv::Mat rgbToGrayscale_custom(const cv::Mat& input);

        // RGB to HSV
        static cv::Mat rgbToHsv_opencv(const cv::Mat& input);
        static cv::Mat rgbToHsv_custom(const cv::Mat& input);
    };

private:
    // ���� ���� �Լ��� (raw�� ����)
    static void validateInput(const cv::Mat& input);
    static unsigned char* createRawLUT(double gamma);
    static std::vector<int> calculateHistogramRaw(const cv::Mat& input);
    static std::vector<int> calculateCumulativeHistogramRaw(const std::vector<int>& histogram);

    // Raw �̹��� ó�� ����
    static unsigned char* createRawImage(int width, int height, int channels);
    static void destroyRawImage(unsigned char* image);
    static void copyMatToRaw(const cv::Mat& input, unsigned char* raw_data);
    static void copyRawToMat(const unsigned char* raw_data, cv::Mat& output);
};

#endif // DUAL_IMPLEMENTATION_PROCESSOR_H