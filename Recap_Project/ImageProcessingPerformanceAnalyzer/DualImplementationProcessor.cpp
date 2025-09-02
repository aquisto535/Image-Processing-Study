#include "DualImplementationProcessor.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>

//  입력 검증
void DualImplementationProcessor::validateInput(const cv::Mat& input) {
    if (input.empty()) {
        throw std::invalid_argument("�Է� �̹����� ����ֽ��ϴ�.");
    }

    if (input.depth() != CV_8U) {
        throw std::invalid_argument("�Է� �̹����� 8��Ʈ unsigned���� �մϴ�.");
    }
}

// Raw 이미지 헬퍼 함수들
unsigned char* DualImplementationProcessor::createRawImage(int width, int height, int channels) {
    return new unsigned char[width * height * channels];
}

void DualImplementationProcessor::destroyRawImage(unsigned char* image) {
    delete[] image;
}

void DualImplementationProcessor::copyMatToRaw(const cv::Mat& input, unsigned char* raw_data) {
    int total_pixels = input.rows * input.cols * input.channels();
    memcpy(raw_data, input.data, total_pixels);
}

void DualImplementationProcessor::copyRawToMat(const unsigned char* raw_data, cv::Mat& output) {
    int total_pixels = output.rows * output.cols * output.channels();
    memcpy(output.data, raw_data, total_pixels);
}

// ===================밝기 조절=====================

cv::Mat DualImplementationProcessor::BrightnessAdjustment::opencv_version(const cv::Mat& input, int brightness) {
    validateInput(input);

    cv::Mat result;
	input.convertTo(result, -1, 1.0, brightness); // alpha=1.0, beta=brightness

    // 0-255 범위로 클리핑
	cv::threshold(result, result, 255, 255, cv::THRESH_TRUNC); // 255 초과는 255로
	cv::threshold(result, result, 0, 0, cv::THRESH_TOZERO); // 0 미만은 0으로

    return result;
}

// A raw implementation to understand the performance bottlenecks.
// It involves manual memory management and per-pixel processing.
cv::Mat DualImplementationProcessor::BrightnessAdjustment::custom_raw_version(const cv::Mat& input, int brightness) {
    validateInput(input);

    int width = input.cols;
    int height = input.rows;
    int channels = input.channels();

    unsigned char* raw_input = input.data;
    unsigned char* raw_output = new unsigned char[width * height * channels];

    for (int i = 0; i < width * height * channels; i++) {
        int new_value = raw_input[i] + brightness;

        // Manual clipping
        if (new_value > 255) new_value = 255;
        if (new_value < 0) new_value = 0;

        raw_output[i] = static_cast<unsigned char>(new_value);
    }

    cv::Mat result(height, width, input.type(), raw_output);
    cv::Mat result_copy = result.clone(); // Deep copy is necessary before freeing memory

    delete[] raw_output;

    return result_copy;
}

// An optimized version that avoids repeated memory allocation and uses direct memory access.
cv::Mat DualImplementationProcessor::BrightnessAdjustment::custom_version(const cv::Mat& input, int brightness)
{
    validateInput(input);

	cv::Mat result = input.clone(); // Clone to avoid modifying the original image
    
	if (result.isContinuous()) // Check if the data is continuous in memory
    {
        unsigned char* data = result.data;
        int total_pixels = result.rows * result.cols * result.channels();
        for (int i = 0; i < total_pixels; i++) {
            int new_value = data[i] + brightness;
            data[i] = static_cast<unsigned char>(std::max(0, std::min(255, new_value)));
        }
    } else
    {
        for (int y = 0; y < result.rows; y++) {
            unsigned char* row_ptr = result.ptr<unsigned char>(y);
            int row_pixels = result.cols * result.channels();
            for (int x = 0; x < row_pixels; x++) {
                int new_value = row_ptr[x] + brightness;
                row_ptr[x] = static_cast<unsigned char>(std::max(0, std::min(255, new_value)));
            }
        }
    }
    
    return result;
}

// ==================== 명암비 조절 ====================

cv::Mat DualImplementationProcessor::ContrastAdjustment::opencv_version(const cv::Mat& input, double contrast) {
    validateInput(input);

    cv::Mat result;
    input.convertTo(result, -1, contrast, 0);

    // 0-255 범위로 클리핑
    cv::threshold(result, result, 255, 255, cv::THRESH_TRUNC);
    cv::threshold(result, result, 0, 0, cv::THRESH_TOZERO);

    return result;
}

cv::Mat DualImplementationProcessor::ContrastAdjustment::custom_version(const cv::Mat& input, double contrast) {
    validateInput(input);

    int width = input.cols;
    int height = input.rows;
    int channels = input.channels();

    // Raw 데이터 추출
    unsigned char* raw_input = input.data;

    // Raw한 명암비 조절 수행
    unsigned char* raw_output = new unsigned char[width * height * channels];

    for (int i = 0; i < width * height * channels; i++) {
        double new_value = (raw_input[i] - 128) * contrast + 128;

        // Raw한 클리핑
        if (new_value > 255) new_value = 255;
        if (new_value < 0) new_value = 0;

        raw_output[i] = static_cast<unsigned char>(new_value);
    }

    // 결과를 Mat으로 변환
    cv::Mat result(height, width, input.type(), raw_output);
    cv::Mat result_copy = result.clone();

    // Raw 메모리 해제
    delete[] raw_output;

    return result_copy;
}

cv::Mat DualImplementationProcessor::ContrastAdjustment::custom_optimized_version(const cv::Mat& input, double contrast) {
    validateInput(input);

    cv::Mat result = input.clone();

    if (result.isContinuous()) {
        unsigned char* data = result.data;
        int total_pixels = result.rows * result.cols * result.channels();

        for (int i = 0; i < total_pixels; i++) {
            double new_value = (data[i] - 128) * contrast + 128;

            // Raw한 클리핑
            if (new_value > 255) new_value = 255;
            if (new_value < 0) new_value = 0;

            data[i] = static_cast<unsigned char>(new_value);
        }
    }
    else {
        for (int y = 0; y < result.rows; y++) {
            unsigned char* row_ptr = result.ptr<unsigned char>(y);
            int row_pixels = result.cols * result.channels();

            for (int x = 0; x < row_pixels; x++) {
                double new_value = (row_ptr[x] - 128) * contrast + 128;

                // Raw한 클리핑
                if (new_value > 255) new_value = 255;
                if (new_value < 0) new_value = 0;

                row_ptr[x] = static_cast<unsigned char>(new_value);
            }
        }
    }

    return result;
}

// ==================== 감마 보정 ====================

cv::Mat DualImplementationProcessor::GammaCorrection::opencv_version(const cv::Mat& input, double gamma) {
    validateInput(input);

    cv::Mat result;
    input.convertTo(result, CV_32F, 1.0 / 255.0);
    cv::pow(result, gamma, result);
    result.convertTo(result, CV_8U, 255.0);

    return result;
}

// A raw implementation for gamma correction with manual memory management.
cv::Mat DualImplementationProcessor::GammaCorrection::custom_raw_version(const cv::Mat& input, double gamma) {
    validateInput(input);

    int width = input.cols;
    int height = input.rows;
    int channels = input.channels();

    unsigned char* raw_input = input.data;
    unsigned char* raw_output = new unsigned char[width * height * channels];

    for (int i = 0; i < width * height * channels; i++) {
        double normalized = raw_input[i] / 255.0;
        double corrected = std::pow(normalized, gamma);
        double final_value = corrected * 255.0;

        if (final_value > 255) final_value = 255;
        if (final_value < 0) final_value = 0;

        raw_output[i] = static_cast<unsigned char>(final_value);
    }

    cv::Mat result(height, width, input.type(), raw_output);
    cv::Mat result_copy = result.clone();

    delete[] raw_output;

    return result_copy;
}

// An optimized version using a Look-Up Table (LUT) to avoid repeated pow() calculations.
cv::Mat DualImplementationProcessor::GammaCorrection::custom_version(const cv::Mat& input, double gamma) {
    validateInput(input);

    // Create Look-Up Table (LUT) on the stack
    unsigned char lut[256];
    for (int i = 0; i < 256; i++) {
        lut[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);
    }

    cv::Mat result = input.clone(); // work on a copy

    if (result.isContinuous()) {
        unsigned char* data = result.data;
        int total_elements = result.total() * result.channels();
        for (int i = 0; i < total_elements; i++) {
            data[i] = lut[data[i]];
        }
    } else {
        for (int i = 0; i < result.rows; i++) {
            unsigned char* row_ptr = result.ptr<unsigned char>(i);
            for (int j = 0; j < result.cols * result.channels(); j++) {
                row_ptr[j] = lut[row_ptr[j]];
            }
        }
    }
    return result;
}

// ==================== 영상 반전 ====================

cv::Mat DualImplementationProcessor::ImageInversion::opencv_version(const cv::Mat& input) {
    validateInput(input);

    cv::Mat result;
    cv::bitwise_not(input, result);

    return result;
}

cv::Mat DualImplementationProcessor::ImageInversion::custom_version(const cv::Mat& input) {
    validateInput(input);

    int width = input.cols;
    int height = input.rows;
    int channels = input.channels();

    // Raw 데이터 추출
    unsigned char* raw_input = input.data;

    // Raw한 영상 반전 수행
    unsigned char* raw_output = new unsigned char[width * height * channels];

    for (int i = 0; i < width * height * channels; i++) {
        raw_output[i] = 255 - raw_input[i];
    }

    // 결과를 Mat으로 변환
    cv::Mat result(height, width, input.type(), raw_output);
    cv::Mat result_copy = result.clone();

    // Raw 메모리 해제
    delete[] raw_output;

    return result_copy;
}

cv::Mat DualImplementationProcessor::ImageInversion::custom_optimized_version(const cv::Mat& input) {
    validateInput(input);

    cv::Mat result = input.clone();

    if (result.isContinuous()) {
        unsigned char* data = result.data;
        int total_pixels = result.rows * result.cols * result.channels();

        for (int i = 0; i < total_pixels; i++) {
            data[i] = 255 - data[i];
        }
    }
    else {
        for (int y = 0; y < result.rows; y++) {
            unsigned char* row_ptr = result.ptr<unsigned char>(y);
            int row_pixels = result.cols * result.channels();

            for (int x = 0; x < row_pixels; x++) {
                row_ptr[x] = 255 - row_ptr[x];
            }
        }
    }

    return result;
}

// ==================== 히스토그램 평활화 ====================
cv::Mat DualImplementationProcessor::HistogramEqualization::opencv_version(const cv::Mat& input) {
    validateInput(input);

    cv::Mat result;
    cv::equalizeHist(input, result);

    return result;
}

cv::Mat DualImplementationProcessor::HistogramEqualization::custom_version(const cv::Mat& input) {
    validateInput(input);

    if (input.channels() != 1) {
        throw std::invalid_argument("히스토그램 평활화는 그레이스케일 이미지만 지원합니다.");
    }

    // Raw한 히스토그램 계산
    std::vector<int> histogram = calculateHistogramRaw(input);

    // Raw한 누적분포함수 계산
    std::vector<int> cdf = calculateCumulativeHistogramRaw(histogram);

    int width = input.cols;
    int height = input.rows;

    // Raw 데이터 추출
    unsigned char* raw_input = input.data;

    // Raw한 히스토그램 평활화 수행
    unsigned char* raw_output = new unsigned char[width * height];

    int total_pixels = width * height;

    for (int i = 0; i < total_pixels; i++) {
        unsigned char pixel_value = raw_input[i];
        int new_value = (cdf[pixel_value] * 255) / total_pixels;

        // Raw한 클리핑
        if (new_value > 255) new_value = 255;
        if (new_value < 0) new_value = 0;

        raw_output[i] = static_cast<unsigned char>(new_value);
    }

    // 결과를 Mat으로 변환
    cv::Mat result(height, width, CV_8UC1, raw_output);
    cv::Mat result_copy = result.clone();

    // Raw 메모리 해제
    delete[] raw_output;

    return result_copy;
}

cv::Mat DualImplementationProcessor::HistogramEqualization::custom_optimized_version(const cv::Mat& input) {
    validateInput(input);

    if (input.channels() != 1) {
        throw std::invalid_argument("히스토그램 평활화는 그레이스케일 이미지만 지원합니다.");
    }

    // Raw한 히스토그램 계산
    std::vector<int> histogram = calculateHistogramRaw(input);

    // Raw한 누적분포함수 계산
    std::vector<int> cdf = calculateCumulativeHistogramRaw(histogram);

    // 이미지 변환 (최적화)
    cv::Mat result = input.clone();
    int total_pixels = input.rows * input.cols;

    if (result.isContinuous()) {
        unsigned char* data = result.data;

        for (int i = 0; i < total_pixels; i++) {
            unsigned char pixel_value = data[i];
            int new_value = (cdf[pixel_value] * 255) / total_pixels;

            // Raw한 클리핑
            if (new_value > 255) new_value = 255;
            if (new_value < 0) new_value = 0;

            data[i] = static_cast<unsigned char>(new_value);
        }
    }
    else {
        for (int y = 0; y < result.rows; y++) {
            unsigned char* row_ptr = result.ptr<unsigned char>(y);

            for (int x = 0; x < result.cols; x++) {
                unsigned char pixel_value = row_ptr[x];
                int new_value = (cdf[pixel_value] * 255) / total_pixels;

                // Raw한 클리핑
                if (new_value > 255) new_value = 255;
                if (new_value < 0) new_value = 0;

                row_ptr[x] = static_cast<unsigned char>(new_value);
            }
        }
    }

    return result;
}

// ====================  색상 공간 변환 ====================

cv::Mat DualImplementationProcessor::ColorSpaceConversion::rgbToGrayscale_opencv(const cv::Mat& input) {
    validateInput(input);

    if (input.channels() != 3) {
        throw std::invalid_argument("RGB to Grayscale 변환은 3채널 이미지만 지원합니다.");
    }

    cv::Mat result;
    cv::cvtColor(input, result, cv::COLOR_BGR2GRAY);

    return result;
}

cv::Mat DualImplementationProcessor::ColorSpaceConversion::rgbToGrayscale_custom(const cv::Mat& input) {
    validateInput(input);

    if (input.channels() != 3) {
        throw std::invalid_argument("RGB to Grayscale 변환은 3채널 이미지만 지원합니다.");
    }

    int width = input.cols;
    int height = input.rows;

    // Raw 데이터 추출
    unsigned char* raw_input = input.data;

    // Raw한 RGB to Grayscale 변환 수행
    unsigned char* raw_output = new unsigned char[width * height];

    for (int i = 0; i < width * height; i++) {
        int bgr_index = i * 3;

        // BGR to Grayscale 변환 공식 (raw한 계산)
        double gray = 0.299 * raw_input[bgr_index + 2] +  // Red
            0.587 * raw_input[bgr_index + 1] +  // Green
            0.114 * raw_input[bgr_index + 0];   // Blue

        // Raw한 클리핑
        if (gray > 255) gray = 255;
        if (gray < 0) gray = 0;

        raw_output[i] = static_cast<unsigned char>(gray);
    }

    // 결과를 Mat으로 변환
    cv::Mat result(height, width, CV_8UC1, raw_output);
    cv::Mat result_copy = result.clone();

    // Raw 메모리 해제
    delete[] raw_output;

    return result_copy;
}

cv::Mat DualImplementationProcessor::ColorSpaceConversion::rgbToHsv_opencv(const cv::Mat& input) {
    validateInput(input);

    if (input.channels() != 3) {
        throw std::invalid_argument("RGB to HSV 변환은 3채널 이미지만 지원합니다.");
    }

    cv::Mat result;
    cv::cvtColor(input, result, cv::COLOR_BGR2HSV);

    return result;
}

cv::Mat DualImplementationProcessor::ColorSpaceConversion::rgbToHsv_custom(const cv::Mat& input) {
    validateInput(input);

    if (input.channels() != 3) {
        throw std::invalid_argument("RGB to HSV 변환은 3채널 이미지만 지원합니다.");
    }

    int width = input.cols;
    int height = input.rows;

    // Raw 데이터 추출
    unsigned char* raw_input = input.data;

    // Raw한 RGB to HSV 변환 수행
    unsigned char* raw_output = new unsigned char[width * height * 3];

    for (int i = 0; i < width * height; i++) {
        int bgr_index = i * 3;
        int hsv_index = i * 3;

        // BGR 값 추출
        double b = raw_input[bgr_index + 0] / 255.0;
        double g = raw_input[bgr_index + 1] / 255.0;
        double r = raw_input[bgr_index + 2] / 255.0;

        double max_val = std::max({ r, g, b });
        double min_val = std::min({ r, g, b });
        double delta = max_val - min_val;

        // Value (V) - raw한 계산
        double v = max_val;

        // Saturation (S) - raw한 계산
        double s = (max_val != 0) ? (delta / max_val) : 0;

        // Hue (H) - raw한 계산
        double h = 0;
        if (delta != 0) {
            if (max_val == r) {
                h = ((g - b) / delta) * 60;
            }
            else if (max_val == g) {
                h = ((b - r) / delta + 2) * 60;
            }
            else {
                h = ((r - g) / delta + 4) * 60;
            }

            if (h < 0) h += 180;
        }

        // HSV 값을 0-255 범위로 변환 및 클리핑
        int h_int = static_cast<int>(h);
        int s_int = static_cast<int>(s * 255);
        int v_int = static_cast<int>(v * 255);

        // Raw한 클리핑
        if (h_int > 179) h_int = 179;
        if (h_int < 0) h_int = 0;
        if (s_int > 255) s_int = 255;
        if (s_int < 0) s_int = 0;
        if (v_int > 255) v_int = 255;
        if (v_int < 0) v_int = 0;

        raw_output[hsv_index + 0] = static_cast<unsigned char>(h_int);
        raw_output[hsv_index + 1] = static_cast<unsigned char>(s_int);
        raw_output[hsv_index + 2] = static_cast<unsigned char>(v_int);
    }

    // 결과를 Mat으로 변환
    cv::Mat result(height, width, CV_8UC3, raw_output);
    cv::Mat result_copy = result.clone();

    // Raw 메모리 해제
    delete[] raw_output;

    return result_copy;
}

// ====================  내부 헬퍼 함수들  ====================

std::vector<int> DualImplementationProcessor::calculateHistogramRaw(const cv::Mat& input) {
    std::vector<int> histogram(256, 0);

    if (input.isContinuous()) {
        unsigned char* data = input.data;
        int total_pixels = input.rows * input.cols;

        for (int i = 0; i < total_pixels; i++) {
            histogram[data[i]]++;
        }
    }
    else {
        for (int y = 0; y < input.rows; y++) {
            const unsigned char* row_ptr = input.ptr<unsigned char>(y);

            for (int x = 0; x < input.cols; x++) {
                histogram[row_ptr[x]]++;
            }
        }
    }

    return histogram;
}

std::vector<int> DualImplementationProcessor::calculateCumulativeHistogramRaw(const std::vector<int>& histogram) {
    std::vector<int> cdf(256, 0);

    cdf[0] = histogram[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    return cdf;
}