#include "DualImplementationProcessor.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cmath>

// Note: All comments are in English to avoid encoding issues.

// --- Input Validation ---
void DualImplementationProcessor::validateInput(const cv::Mat& input) {
    if (input.empty()) {
        throw std::invalid_argument("Input image is empty.");
    }
    if (input.depth() != CV_8U) {
        throw std::invalid_argument("Input image must be 8-bit unsigned.");
    }
}

// --- Brightness Adjustment ---
cv::Mat DualImplementationProcessor::BrightnessAdjustment::opencv_version(const cv::Mat& input, int brightness) {
    validateInput(input);
    cv::Mat result;
    input.convertTo(result, -1, 1.0, brightness);
    return result;
}

cv::Mat DualImplementationProcessor::BrightnessAdjustment::custom_raw_version(const cv::Mat& input, int brightness) {
    validateInput(input);
    int width = input.cols;
    int height = input.rows;
    int channels = input.channels();
    unsigned char* raw_input = input.data;
    unsigned char* raw_output = new unsigned char[width * height * channels];

    for (int i = 0; i < width * height * channels; i++) {
        int new_value = raw_input[i] + brightness;
        new_value = std::max(0, std::min(255, new_value));
        raw_output[i] = static_cast<unsigned char>(new_value);
    }

    cv::Mat result(height, width, input.type(), raw_output);
    cv::Mat result_copy = result.clone();
    delete[] raw_output;
    return result_copy;
}

cv::Mat DualImplementationProcessor::BrightnessAdjustment::custom_version(const cv::Mat& input, int brightness) {
    validateInput(input);
    cv::Mat result = input.clone();
    if (result.isContinuous()) {
        unsigned char* data = result.data;
        int total_pixels = result.total() * result.channels();
        for (int i = 0; i < total_pixels; i++) {
            int new_value = data[i] + brightness;
            data[i] = cv::saturate_cast<uchar>(new_value);
        }
    } else {
        for (int y = 0; y < result.rows; y++) {
            unsigned char* row_ptr = result.ptr<unsigned char>(y);
            for (int x = 0; x < result.cols * result.channels(); x++) {
                int new_value = row_ptr[x] + brightness;
                row_ptr[x] = cv::saturate_cast<uchar>(new_value);
            }
        }
    }
    return result;
}


// --- Gamma Correction ---
cv::Mat DualImplementationProcessor::GammaCorrection::opencv_version(const cv::Mat& input, double gamma) {
    validateInput(input);
    cv::Mat result;
    input.convertTo(result, CV_32F, 1.0 / 255.0);
    cv::pow(result, gamma, result);
    result.convertTo(result, CV_8U, 255.0);
    return result;
}

cv::Mat DualImplementationProcessor::GammaCorrection::custom_raw_version(const cv::Mat& input, double gamma) {
    validateInput(input);
    int width = input.cols;
    int height = input.rows;
    int channels = input.channels();
    unsigned char* raw_input = input.data;
    unsigned char* raw_output = new unsigned char[width * height * channels];
    
    double gamma_inv = 1.0 / gamma;
    for (int i = 0; i < width * height * channels; i++) {
        double final_value = std::pow(raw_input[i] / 255.0, gamma_inv) * 255.0;
        raw_output[i] = cv::saturate_cast<uchar>(final_value);
    }

    cv::Mat result(height, width, input.type(), raw_output);
    cv::Mat result_copy = result.clone();
    delete[] raw_output;
    return result_copy;
}

cv::Mat DualImplementationProcessor::GammaCorrection::custom_version(const cv::Mat& input, double gamma) {
    validateInput(input);
    unsigned char lut[256];
    for (int i = 0; i < 256; i++) {
        lut[i] = cv::saturate_cast<uchar>(std::pow(i / 255.0, 1.0/gamma) * 255.0);
    }
    cv::Mat result;
    cv::LUT(input, cv::Mat(1, 256, CV_8U, lut), result);
    return result;
}

// --- Contrast Adjustment ---
cv::Mat DualImplementationProcessor::ContrastAdjustment::opencv_version(const cv::Mat& input, double contrast) {
    validateInput(input);
    cv::Mat result;
    input.convertTo(result, -1, contrast, 0);
    return result;
}

cv::Mat DualImplementationProcessor::ContrastAdjustment::custom_version(const cv::Mat& input, double contrast) {
    validateInput(input);
    cv::Mat result = input.clone();
    for (int y = 0; y < result.rows; y++) {
        for (int x = 0; x < result.cols; x++) {
            for (int c = 0; c < result.channels(); c++) {
                double new_value = (result.at<cv::Vec3b>(y, x)[c] - 128) * contrast + 128;
                result.at<cv::Vec3b>(y, x)[c] = cv::saturate_cast<uchar>(new_value);
            }
        }
    }
    return result;
}

// --- Histogram Equalization ---
cv::Mat DualImplementationProcessor::HistogramEqualization::opencv_version(const cv::Mat& input) {
    validateInput(input);
    if (input.channels() != 1) throw std::invalid_argument("Grayscale only");
    cv::Mat result;
    cv::equalizeHist(input, result);
    return result;
}

cv::Mat DualImplementationProcessor::HistogramEqualization::custom_version(const cv::Mat& input) {
    validateInput(input);
    if (input.channels() != 1) throw std::invalid_argument("Grayscale only");
    
    std::vector<int> hist(256, 0);
    for (int y = 0; y < input.rows; y++) {
        for (int x = 0; x < input.cols; x++) {
            hist[input.at<uchar>(y, x)]++;
        }
    }

    std::vector<int> cdf(256, 0);
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i-1] + hist[i];
    }

    int total_pixels = input.rows * input.cols;
    unsigned char lut[256];
    for (int i = 0; i < 256; i++) {
        lut[i] = cv::saturate_cast<uchar>( ( (cdf[i] - cdf[0]) * 255) / (total_pixels - cdf[0]) );
    }

    cv::Mat result;
    cv::LUT(input, cv::Mat(1, 256, CV_8U, lut), result);
    return result;
}