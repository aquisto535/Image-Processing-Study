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
    // 밝기 조절 클래스
    class BrightnessAdjustment {
    public:
        // OpenCV 버전
        static cv::Mat opencv_version(const cv::Mat& input, int brightness);

        // Custom 버전 (완전히 raw한 로직)
        static cv::Mat custom_version(const cv::Mat& input, int brightness);

        // 최적화된 Custom 버전 (ptr 사용)
        static cv::Mat custom_optimized_version(const cv::Mat& input, int brightness);
    };

    // 명암비 조절 클래스
    class ContrastAdjustment {
    public:
        // OpenCV 버전
        static cv::Mat opencv_version(const cv::Mat& input, double contrast);

        // Custom 버전 (완전히 raw한 로직)
        static cv::Mat custom_version(const cv::Mat& input, double contrast);

        // 최적화된 Custom 버전
        static cv::Mat custom_optimized_version(const cv::Mat& input, double contrast);
    };

    // 감마 보정 클래스
    class GammaCorrection {
    public:
        // OpenCV 버전
        static cv::Mat opencv_version(const cv::Mat& input, double gamma);

        // Custom 버전 (완전히 raw한 로직)
        static cv::Mat custom_version(const cv::Mat& input, double gamma);

        // LUT 기반 Custom 버전 (raw한 LUT 구현)
        static cv::Mat custom_lut_version(const cv::Mat& input, double gamma);
    };

    // 영상 반전 클래스
    class ImageInversion {
    public:
        // OpenCV 버전
        static cv::Mat opencv_version(const cv::Mat& input);

        // Custom 버전 (완전히 raw한 로직)
        static cv::Mat custom_version(const cv::Mat& input);

        // 최적화된 Custom 버전
        static cv::Mat custom_optimized_version(const cv::Mat& input);
    };

    // 히스토그램 평활화 클래스
    class HistogramEqualization {
    public:
        // OpenCV 버전
        static cv::Mat opencv_version(const cv::Mat& input);

        // Custom 버전 (누적분포함수 사용, raw한 로직)
        static cv::Mat custom_version(const cv::Mat& input);

        // 최적화된 Custom 버전
        static cv::Mat custom_optimized_version(const cv::Mat& input);
    };

    // 색상 공간 변환 클래스
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
    // 내부 헬퍼 함수들 (raw한 구현)
    static void validateInput(const cv::Mat& input);
    static unsigned char* createRawLUT(double gamma);
    static std::vector<int> calculateHistogramRaw(const cv::Mat& input);
    static std::vector<int> calculateCumulativeHistogramRaw(const std::vector<int>& histogram);

    // Raw 이미지 처리 헬퍼
    static unsigned char* createRawImage(int width, int height, int channels);
    static void destroyRawImage(unsigned char* image);
    static void copyMatToRaw(const cv::Mat& input, unsigned char* raw_data);
    static void copyRawToMat(const unsigned char* raw_data, cv::Mat& output);
};

#endif // DUAL_IMPLEMENTATION_PROCESSOR_H