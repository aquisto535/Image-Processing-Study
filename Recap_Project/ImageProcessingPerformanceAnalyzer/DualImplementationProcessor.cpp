#include "DualImplementationProcessor.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>

// �Է� ����
void DualImplementationProcessor::validateInput(const cv::Mat& input) {
    if (input.empty()) {
        throw std::invalid_argument("�Է� �̹����� ����ֽ��ϴ�.");
    }

    if (input.depth() != CV_8U) {
        throw std::invalid_argument("�Է� �̹����� 8��Ʈ unsigned���� �մϴ�.");
    }
}

// Raw �̹��� ���� �Լ���
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

// ==================== ��� ���� ====================

cv::Mat DualImplementationProcessor::BrightnessAdjustment::opencv_version(const cv::Mat& input, int brightness) {
    validateInput(input);

    cv::Mat result;
    input.convertTo(result, -1, 1.0, brightness);

    // 0-255 ������ Ŭ����
    cv::threshold(result, result, 255, 255, cv::THRESH_TRUNC);
    cv::threshold(result, result, 0, 0, cv::THRESH_TOZERO);

    return result;
}

cv::Mat DualImplementationProcessor::BrightnessAdjustment::custom_version(const cv::Mat& input, int brightness) {
    validateInput(input);

    // 입력 이미지를 직접 수정하지 않고 복사본 생성
    cv::Mat result = input.clone();
    
    // 연속된 메모리인지 확인
    if (result.isContinuous()) {
        // 전체 픽셀을 한 번에 처리
        unsigned char* data = result.data;
        int total_pixels = result.rows * result.cols * result.channels();
        
        // SIMD 최적화를 위한 벡터화된 처리
        // OpenCV의 convertTo보다 빠른 직접 처리
        for (int i = 0; i < total_pixels; i++) {
            int new_value = data[i] + brightness;
            data[i] = static_cast<unsigned char>(std::max(0, std::min(255, new_value)));
        }
    } else {
        // 행별로 처리
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

cv::Mat DualImplementationProcessor::BrightnessAdjustment::custom_optimized_version(const cv::Mat& input, int brightness) {
    validateInput(input);

    cv::Mat result = input.clone();

    if (result.isContinuous()) {
        // ���ӵ� �޸��� ��� �����ͷ� ���� ����
        unsigned char* data = result.data;
        int total_pixels = result.rows * result.cols * result.channels();

        for (int i = 0; i < total_pixels; i++) {
            int new_value = data[i] + brightness;

            // Raw�� Ŭ����
            if (new_value > 255) new_value = 255;
            if (new_value < 0) new_value = 0;

            data[i] = static_cast<unsigned char>(new_value);
        }
    }
    else {
        // �񿬼� �޸��� ��� �ະ�� ó��
        for (int y = 0; y < result.rows; y++) {
            unsigned char* row_ptr = result.ptr<unsigned char>(y);
            int row_pixels = result.cols * result.channels();

            for (int x = 0; x < row_pixels; x++) {
                int new_value = row_ptr[x] + brightness;

                // Raw�� Ŭ����
                if (new_value > 255) new_value = 255;
                if (new_value < 0) new_value = 0;

                row_ptr[x] = static_cast<unsigned char>(new_value);
            }
        }
    }

    return result;
}

// ==================== ���Ϻ� ���� ====================

cv::Mat DualImplementationProcessor::ContrastAdjustment::opencv_version(const cv::Mat& input, double contrast) {
    validateInput(input);

    cv::Mat result;
    input.convertTo(result, -1, contrast, 0);

    // 0-255 ������ Ŭ����
    cv::threshold(result, result, 255, 255, cv::THRESH_TRUNC);
    cv::threshold(result, result, 0, 0, cv::THRESH_TOZERO);

    return result;
}

cv::Mat DualImplementationProcessor::ContrastAdjustment::custom_version(const cv::Mat& input, double contrast) {
    validateInput(input);

    int width = input.cols;
    int height = input.rows;
    int channels = input.channels();

    // Raw ������ ����
    unsigned char* raw_input = input.data;

    // Raw�� ���Ϻ� ���� ����
    unsigned char* raw_output = new unsigned char[width * height * channels];

    for (int i = 0; i < width * height * channels; i++) {
        double new_value = (raw_input[i] - 128) * contrast + 128;

        // Raw�� Ŭ����
        if (new_value > 255) new_value = 255;
        if (new_value < 0) new_value = 0;

        raw_output[i] = static_cast<unsigned char>(new_value);
    }

    // ����� Mat���� ��ȯ
    cv::Mat result(height, width, input.type(), raw_output);
    cv::Mat result_copy = result.clone();

    // Raw �޸� ����
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

            // Raw�� Ŭ����
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

                // Raw�� Ŭ����
                if (new_value > 255) new_value = 255;
                if (new_value < 0) new_value = 0;

                row_ptr[x] = static_cast<unsigned char>(new_value);
            }
        }
    }

    return result;
}

// ==================== ���� ���� ====================

cv::Mat DualImplementationProcessor::GammaCorrection::opencv_version(const cv::Mat& input, double gamma) {
    validateInput(input);

    cv::Mat result;
    input.convertTo(result, CV_32F, 1.0 / 255.0);
    cv::pow(result, gamma, result);
    result.convertTo(result, CV_8U, 255.0);

    return result;
}

cv::Mat DualImplementationProcessor::GammaCorrection::custom_version(const cv::Mat& input, double gamma) {
    validateInput(input);

    int width = input.cols;
    int height = input.rows;
    int channels = input.channels();

    // Raw ������ ����
    unsigned char* raw_input = input.data;

    // Raw�� ���� ���� ����
    unsigned char* raw_output = new unsigned char[width * height * channels];

    for (int i = 0; i < width * height * channels; i++) {
        double normalized = raw_input[i] / 255.0;
        double corrected = std::pow(normalized, gamma);
        double final_value = corrected * 255.0;

        // Raw�� Ŭ����
        if (final_value > 255) final_value = 255;
        if (final_value < 0) final_value = 0;

        raw_output[i] = static_cast<unsigned char>(final_value);
    }

    // ����� Mat���� ��ȯ
    cv::Mat result(height, width, input.type(), raw_output);
    cv::Mat result_copy = result.clone();

    // Raw �޸� ����
    delete[] raw_output;

    return result_copy;
}

cv::Mat DualImplementationProcessor::GammaCorrection::custom_lut_version(const cv::Mat& input, double gamma) {
    validateInput(input);

    // Raw�� LUT ����
    unsigned char* lut = createRawLUT(gamma);

    int width = input.cols;
    int height = input.rows;
    int channels = input.channels();

    // Raw ������ ����
    unsigned char* raw_input = input.data;

    // LUT�� ����� Raw�� ���� ����
    unsigned char* raw_output = new unsigned char[width * height * channels];

    for (int i = 0; i < width * height * channels; i++) {
        raw_output[i] = lut[raw_input[i]];
    }

    // ����� Mat���� ��ȯ
    cv::Mat result(height, width, input.type(), raw_output);
    cv::Mat result_copy = result.clone();

    // Raw �޸� ����
    delete[] raw_output;
    delete[] lut;

    return result_copy;
}

// Raw�� LUT ����
unsigned char* DualImplementationProcessor::createRawLUT(double gamma) {
    unsigned char* lut = new unsigned char[256];

    for (int i = 0; i < 256; i++) {
        double normalized = i / 255.0;
        double corrected = std::pow(normalized, gamma);
        double final_value = corrected * 255.0;

        // Raw�� Ŭ����
        if (final_value > 255) final_value = 255;
        if (final_value < 0) final_value = 0;

        lut[i] = static_cast<unsigned char>(final_value);
    }

    return lut;
}

// ==================== ���� ���� ====================

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

    // Raw ������ ����
    unsigned char* raw_input = input.data;

    // Raw�� ���� ���� ����
    unsigned char* raw_output = new unsigned char[width * height * channels];

    for (int i = 0; i < width * height * channels; i++) {
        raw_output[i] = 255 - raw_input[i];
    }

    // ����� Mat���� ��ȯ
    cv::Mat result(height, width, input.type(), raw_output);
    cv::Mat result_copy = result.clone();

    // Raw �޸� ����
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

// ==================== ������׷� ��Ȱȭ ====================

cv::Mat DualImplementationProcessor::HistogramEqualization::opencv_version(const cv::Mat& input) {
    validateInput(input);

    cv::Mat result;
    cv::equalizeHist(input, result);

    return result;
}

cv::Mat DualImplementationProcessor::HistogramEqualization::custom_version(const cv::Mat& input) {
    validateInput(input);

    if (input.channels() != 1) {
        throw std::invalid_argument("������׷� ��Ȱȭ�� �׷��̽����� �̹����� �����մϴ�.");
    }

    // Raw�� ������׷� ���
    std::vector<int> histogram = calculateHistogramRaw(input);

    // Raw�� ���������Լ� ���
    std::vector<int> cdf = calculateCumulativeHistogramRaw(histogram);

    int width = input.cols;
    int height = input.rows;

    // Raw ������ ����
    unsigned char* raw_input = input.data;

    // Raw�� ������׷� ��Ȱȭ ����
    unsigned char* raw_output = new unsigned char[width * height];

    int total_pixels = width * height;

    for (int i = 0; i < total_pixels; i++) {
        unsigned char pixel_value = raw_input[i];
        int new_value = (cdf[pixel_value] * 255) / total_pixels;

        // Raw�� Ŭ����
        if (new_value > 255) new_value = 255;
        if (new_value < 0) new_value = 0;

        raw_output[i] = static_cast<unsigned char>(new_value);
    }

    // ����� Mat���� ��ȯ
    cv::Mat result(height, width, CV_8UC1, raw_output);
    cv::Mat result_copy = result.clone();

    // Raw �޸� ����
    delete[] raw_output;

    return result_copy;
}

cv::Mat DualImplementationProcessor::HistogramEqualization::custom_optimized_version(const cv::Mat& input) {
    validateInput(input);

    if (input.channels() != 1) {
        throw std::invalid_argument("������׷� ��Ȱȭ�� �׷��̽����� �̹����� �����մϴ�.");
    }

    // Raw�� ������׷� ���
    std::vector<int> histogram = calculateHistogramRaw(input);

    // Raw�� ���������Լ� ���
    std::vector<int> cdf = calculateCumulativeHistogramRaw(histogram);

    // �̹��� ��ȯ (����ȭ)
    cv::Mat result = input.clone();
    int total_pixels = input.rows * input.cols;

    if (result.isContinuous()) {
        unsigned char* data = result.data;

        for (int i = 0; i < total_pixels; i++) {
            unsigned char pixel_value = data[i];
            int new_value = (cdf[pixel_value] * 255) / total_pixels;

            // Raw�� Ŭ����
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

                // Raw�� Ŭ����
                if (new_value > 255) new_value = 255;
                if (new_value < 0) new_value = 0;

                row_ptr[x] = static_cast<unsigned char>(new_value);
            }
        }
    }

    return result;
}

// ==================== ���� ���� ��ȯ ====================

cv::Mat DualImplementationProcessor::ColorSpaceConversion::rgbToGrayscale_opencv(const cv::Mat& input) {
    validateInput(input);

    if (input.channels() != 3) {
        throw std::invalid_argument("RGB to Grayscale ��ȯ�� 3ä�� �̹����� �����մϴ�.");
    }

    cv::Mat result;
    cv::cvtColor(input, result, cv::COLOR_BGR2GRAY);

    return result;
}

cv::Mat DualImplementationProcessor::ColorSpaceConversion::rgbToGrayscale_custom(const cv::Mat& input) {
    validateInput(input);

    if (input.channels() != 3) {
        throw std::invalid_argument("RGB to Grayscale ��ȯ�� 3ä�� �̹����� �����մϴ�.");
    }

    int width = input.cols;
    int height = input.rows;

    // Raw ������ ����
    unsigned char* raw_input = input.data;

    // Raw�� RGB to Grayscale ��ȯ ����
    unsigned char* raw_output = new unsigned char[width * height];

    for (int i = 0; i < width * height; i++) {
        int bgr_index = i * 3;

        // BGR to Grayscale ��ȯ ���� (raw�� ���)
        double gray = 0.299 * raw_input[bgr_index + 2] +  // Red
            0.587 * raw_input[bgr_index + 1] +  // Green
            0.114 * raw_input[bgr_index + 0];   // Blue

        // Raw�� Ŭ����
        if (gray > 255) gray = 255;
        if (gray < 0) gray = 0;

        raw_output[i] = static_cast<unsigned char>(gray);
    }

    // ����� Mat���� ��ȯ
    cv::Mat result(height, width, CV_8UC1, raw_output);
    cv::Mat result_copy = result.clone();

    // Raw �޸� ����
    delete[] raw_output;

    return result_copy;
}

cv::Mat DualImplementationProcessor::ColorSpaceConversion::rgbToHsv_opencv(const cv::Mat& input) {
    validateInput(input);

    if (input.channels() != 3) {
        throw std::invalid_argument("RGB to HSV ��ȯ�� 3ä�� �̹����� �����մϴ�.");
    }

    cv::Mat result;
    cv::cvtColor(input, result, cv::COLOR_BGR2HSV);

    return result;
}

cv::Mat DualImplementationProcessor::ColorSpaceConversion::rgbToHsv_custom(const cv::Mat& input) {
    validateInput(input);

    if (input.channels() != 3) {
        throw std::invalid_argument("RGB to HSV ��ȯ�� 3ä�� �̹����� �����մϴ�.");
    }

    int width = input.cols;
    int height = input.rows;

    // Raw ������ ����
    unsigned char* raw_input = input.data;

    // Raw�� RGB to HSV ��ȯ ����
    unsigned char* raw_output = new unsigned char[width * height * 3];

    for (int i = 0; i < width * height; i++) {
        int bgr_index = i * 3;
        int hsv_index = i * 3;

        // BGR �� ����
        double b = raw_input[bgr_index + 0] / 255.0;
        double g = raw_input[bgr_index + 1] / 255.0;
        double r = raw_input[bgr_index + 2] / 255.0;

        double max_val = std::max({ r, g, b });
        double min_val = std::min({ r, g, b });
        double delta = max_val - min_val;

        // Value (V) - raw�� ���
        double v = max_val;

        // Saturation (S) - raw�� ���
        double s = (max_val != 0) ? (delta / max_val) : 0;

        // Hue (H) - raw�� ���
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

        // HSV ���� 0-255 ������ ��ȯ �� Ŭ����
        int h_int = static_cast<int>(h);
        int s_int = static_cast<int>(s * 255);
        int v_int = static_cast<int>(v * 255);

        // Raw�� Ŭ����
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

    // ����� Mat���� ��ȯ
    cv::Mat result(height, width, CV_8UC3, raw_output);
    cv::Mat result_copy = result.clone();

    // Raw �޸� ����
    delete[] raw_output;

    return result_copy;
}

// ==================== ���� ���� �Լ��� ====================

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