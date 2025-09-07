#include "ColorSpaceConversion.h"
#include <stdexcept>
#include <vector>

// Note: NOMINMAX is defined in the header file.

// --- RGB -> Grayscale ---
cv::Mat ColorSpaceConversion::opencv_rgb_to_gray(const cv::Mat& src) {
    cv::Mat dst;
    cv::cvtColor(src, dst, cv::COLOR_BGR2GRAY);
    return dst;
}

cv::Mat ColorSpaceConversion::custom_rgb_to_gray(const cv::Mat& src) {
    if (src.channels() != 3) {
        throw std::invalid_argument("Input must be a 3-channel BGR image.");
    }
    cv::Mat dst(src.rows, src.cols, CV_8UC1);
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            cv::Vec3b pixel = src.at<cv::Vec3b>(i, j);
            // Using ITU-R BT.709 standard weights
            double gray = 0.114 * pixel[0] + 0.587 * pixel[1] + 0.299 * pixel[2];
            dst.at<uchar>(i, j) = cv::saturate_cast<uchar>(gray);
        }
    }
    return dst;
}

// --- RGB -> HSV ---
cv::Mat ColorSpaceConversion::opencv_rgb_to_hsv(const cv::Mat& src) {
    cv::Mat dst;
    cv::cvtColor(src, dst, cv::COLOR_BGR2HSV);
    return dst;
}

cv::Mat ColorSpaceConversion::custom_rgb_to_hsv(const cv::Mat& src) {
    if (src.channels() != 3) {
        throw std::invalid_argument("Input must be a 3-channel BGR image.");
    }
    cv::Mat dst(src.size(), CV_8UC3);
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            cv::Vec3b bgr = src.at<cv::Vec3b>(i, j);
            double blue = normalizeRGB(bgr[0]);
            double green = normalizeRGB(bgr[1]);
            double red = normalizeRGB(bgr[2]);

            double max_val = std::max({red, green, blue});
            double min_val = std::min({red, green, blue});
            double delta = max_val - min_val;

            double hue = 0;
            if (delta != 0) {
                if (max_val == red) {
                    hue = 60 * fmod(((green - blue) / delta), 6);
                } else if (max_val == green) {
                    hue = 60 * (((blue - red) / delta) + 2);
                } else {
                    hue = 60 * (((red - green) / delta) + 4);
                }
                if (hue < 0) {
                    hue += 360;
                }
            }
            
            double saturation = (max_val == 0) ? 0 : (delta / max_val);
            double value = max_val;

            dst.at<cv::Vec3b>(i, j) = cv::Vec3b(
                cv::saturate_cast<uchar>(hue / 2),
                cv::saturate_cast<uchar>(saturation * 255),
                cv::saturate_cast<uchar>(value * 255)
            );
        }
    }
    return dst;
}

// --- HSV -> RGB ---
cv::Mat ColorSpaceConversion::opencv_hsv_to_rgb(const cv::Mat& src) {
    cv::Mat dst;
    cv::cvtColor(src, dst, cv::COLOR_HSV2BGR);
    return dst;
}

cv::Mat ColorSpaceConversion::custom_hsv_to_rgb(const cv::Mat& src) {
    if (src.channels() != 3) {
        throw std::invalid_argument("Input must be a 3-channel HSV image.");
    }
    cv::Mat dst(src.size(), CV_8UC3);
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            cv::Vec3b hsv = src.at<cv::Vec3b>(i, j);
            double hue = hsv[0] * 2.0;
            double saturation = hsv[1] / 255.0;
            double value = hsv[2] / 255.0;

            double red = hsvToRgbHelper(5, hue, saturation, value);
            double green = hsvToRgbHelper(3, hue, saturation, value);
            double blue = hsvToRgbHelper(1, hue, saturation, value);

            dst.at<cv::Vec3b>(i, j) = cv::Vec3b(
                denormalizeRGB(blue),
                denormalizeRGB(green),
                denormalizeRGB(red)
            );
        }
    }
    return dst;
}

// --- Helper Functions ---
double ColorSpaceConversion::normalizeRGB(uchar value) {
    return value / 255.0;
}

uchar ColorSpaceConversion::denormalizeRGB(double value) {
    return cv::saturate_cast<uchar>(value * 255.0);
}

double ColorSpaceConversion::hsvToRgbHelper(int n, double hue, double saturation, double value) {
    double k = fmod(n + hue / 60.0, 6);
    double min_val = std::min({ k, 4.0 - k, 1.0 });
    return value - value * saturation * std::max(0.0, min_val);
}