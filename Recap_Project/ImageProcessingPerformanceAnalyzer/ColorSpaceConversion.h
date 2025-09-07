#ifndef COLORSPACECONVERSION_H
#define COLORSPACECONVERSION_H

// Define NOMINMAX to prevent conflicts with Windows.h min/max macros
#define NOMINMAX

#include <opencv2/opencv.hpp>

/**
 * @class ColorSpaceConversion
 * @brief Provides static methods for color space conversion.
 *
 * This class includes both OpenCV-based and custom implementations
 * for converting between RGB, Grayscale, and HSV color spaces.
 */
class ColorSpaceConversion {
public:
    // --- RGB <-> Grayscale ---
    static cv::Mat opencv_rgb_to_gray(const cv::Mat& src);
    static cv::Mat custom_rgb_to_gray(const cv::Mat& src);

    // --- RGB <-> HSV ---
    static cv::Mat opencv_rgb_to_hsv(const cv::Mat& src);
    static cv::Mat custom_rgb_to_hsv(const cv::Mat& src);
    static cv::Mat opencv_hsv_to_rgb(const cv::Mat& src);
    static cv::Mat custom_hsv_to_rgb(const cv::Mat& src);

private:
    // --- Helper Functions ---
    static double normalizeRGB(uchar value);
    static uchar denormalizeRGB(double value);
    static double hsvToRgbHelper(int n, double hue, double saturation, double value);
};

#endif // COLORSPACECONVERSION_H