#include "TestImageGenerator.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <random>
#include <filesystem>

namespace fs = std::filesystem;

cv::Mat TestImageGenerator::createTestImage(cv::Size size, int type) {
    validateImageSize(size);
    cv::Mat test_image;
    switch (type) {
    case CV_8UC1:
        test_image = generateGradientImage(size);
        break;
    case CV_8UC3:
        test_image = generateRandomColorImage(size);
        break;
    case CV_8UC4:
        test_image = cv::Mat(size, CV_8UC3, cv::Scalar(128, 128, 128));
        cv::cvtColor(test_image, test_image, cv::COLOR_BGR2BGRA);
        break;
    default:
        test_image = generateGradientImage(size);
        break;
    }
    return test_image;
}

cv::Mat TestImageGenerator::createTestImage(cv::Size size, int type, cv::Scalar color) {
    validateImageSize(size);
    cv::Mat test_image;
    switch (type) {
    case CV_8UC1:
        test_image = cv::Mat(size, CV_8UC1, cv::Scalar(color[0]));
        break;
    case CV_8UC3:
        test_image = cv::Mat(size, CV_8UC3, color);
        break;
    case CV_8UC4:
        test_image = cv::Mat(size, CV_8UC3, color);
        cv::cvtColor(test_image, test_image, cv::COLOR_BGR2BGRA);
        break;
    default:
        test_image = cv::Mat(size, CV_8UC3, color);
        break;
    }
    return test_image;
}

cv::Mat TestImageGenerator::generateGradientImage(cv::Size size) {
    validateImageSize(size);
    cv::Mat gradient(size, CV_8UC1);
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            double normalized_x = static_cast<double>(x) / size.width;
            uchar value = static_cast<uchar>(normalized_x * 255);
            gradient.at<uchar>(y, x) = value;
        }
    }
    return gradient;
}

cv::Mat TestImageGenerator::generateNoiseImage(cv::Size size, double noise_level) {
    validateImageSize(size);
    if (noise_level < 0.0 || noise_level > 1.0) {
        throw std::invalid_argument("Noise level must be between 0.0 and 1.0");
    }
    cv::Mat noise_image(size, CV_8UC1);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            if (static_cast<double>(dis(gen)) / 255.0 < noise_level) {
                noise_image.at<uchar>(y, x) = dis(gen);
            }
            else {
                noise_image.at<uchar>(y, x) = 128;
            }
        }
    }
    return noise_image;
}

cv::Mat TestImageGenerator::generateCheckerboardImage(cv::Size size, int square_size) {
    validateImageSize(size);
    if (square_size <= 0 || square_size > std::min(size.width, size.height)) {
        throw std::invalid_argument("Invalid square size");
    }
    cv::Mat checkerboard(size, CV_8UC1);
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            int square_x = x / square_size;
            int square_y = y / square_size;
            if ((square_x + square_y) % 2 == 0) {
                checkerboard.at<uchar>(y, x) = 255;
            }
            else {
                checkerboard.at<uchar>(y, x) = 0;
            }
        }
    }
    return checkerboard;
}

cv::Mat TestImageGenerator::generateRandomColorImage(cv::Size size) {
    validateImageSize(size);
    cv::Mat color_image(size, CV_8UC3);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            cv::Vec3b& pixel = color_image.at<cv::Vec3b>(y, x);
            pixel[0] = dis(gen);
            pixel[1] = dis(gen);
            pixel[2] = dis(gen);
        }
    }
    return color_image;
}

cv::Mat TestImageGenerator::generateSyntheticImage(cv::Size size, cv::Scalar color) {
    validateImageSize(size);
    cv::Mat synthetic_image(size, CV_8UC3, color);
    return synthetic_image;
}

cv::Mat TestImageGenerator::generateHighContrastImage(cv::Size size) {
    validateImageSize(size);
    cv::Mat high_contrast(size, CV_8UC1);
    cv::Point center(size.width / 2, size.height / 2);
    int max_distance = std::max(size.width, size.height) / 2;
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            double distance = cv::norm(cv::Point(x, y) - center);
            double normalized_distance = distance / max_distance;
            uchar value = static_cast<uchar>(255 * (1.0 - normalized_distance * normalized_distance));
            high_contrast.at<uchar>(y, x) = value;
        }
    }
    return high_contrast;
}

cv::Mat TestImageGenerator::generateLowContrastImage(cv::Size size) {
    validateImageSize(size);
    cv::Mat low_contrast(size, CV_8UC1);
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            double normalized_x = static_cast<double>(x) / size.width;
            double normalized_y = static_cast<double>(y) / size.height;
            uchar value = static_cast<uchar>(100 + 55 * (normalized_x + normalized_y) / 2.0);
            low_contrast.at<uchar>(y, x) = value;
        }
    }
    return low_contrast;
}

cv::Mat TestImageGenerator::generateEdgeTestImage(cv::Size size) {
    validateImageSize(size);
    cv::Mat edge_test(size, CV_8UC1, cv::Scalar(128));
    for (int y = 0; y < size.height; y++) {
        for (int x = size.width / 4; x < size.width / 4 + 2; x++) {
            if (x < size.width) edge_test.at<uchar>(y, x) = 255;
        }
        for (int x = 3 * size.width / 4; x < 3 * size.width / 4 + 2; x++) {
            if (x < size.width) edge_test.at<uchar>(y, x) = 0;
        }
    }
    for (int x = 0; x < size.width; x++) {
        for (int y = size.height / 4; y < size.height / 4 + 2; y++) {
            if (y < size.height) edge_test.at<uchar>(y, x) = 255;
        }
        for (int y = 3 * size.height / 4; y < 3 * size.height / 4 + 2; y++) {
            if (y < size.height) edge_test.at<uchar>(y, x) = 0;
        }
    }
    return edge_test;
}

cv::Mat TestImageGenerator::generateTextureImage(cv::Size size) {
    validateImageSize(size);
    cv::Mat texture(size, CV_8UC1);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(128, 30);
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            double noise = dis(gen);
            uchar value = cv::saturate_cast<uchar>(noise);
            texture.at<uchar>(y, x) = value;
        }
    }
    cv::GaussianBlur(texture, texture, cv::Size(5, 5), 1.0);
    return texture;
}

std::vector<cv::Mat> TestImageGenerator::generateStandardTestSet() {
    std::vector<cv::Mat> test_set;
    std::vector<cv::Size> sizes = {
        cv::Size(256, 256),
        cv::Size(512, 512),
        cv::Size(1024, 1024),
        cv::Size(2048, 2048)
    };
    for (const auto& size : sizes) {
        test_set.push_back(generateGradientImage(size));
        test_set.push_back(generateCheckerboardImage(size, 32));
        test_set.push_back(generateHighContrastImage(size));
        test_set.push_back(generateLowContrastImage(size));
        test_set.push_back(generateEdgeTestImage(size));
        test_set.push_back(generateTextureImage(size));
        test_set.push_back(generateNoiseImage(size, 0.1));
        test_set.push_back(generateNoiseImage(size, 0.3));
        test_set.push_back(generateNoiseImage(size, 0.5));
    }
    cv::Size medium_size(512, 512);
    test_set.push_back(generateRandomColorImage(medium_size));
    test_set.push_back(generateSyntheticImage(medium_size, cv::Scalar(255, 0, 0)));
    test_set.push_back(generateSyntheticImage(medium_size, cv::Scalar(0, 255, 0)));
    test_set.push_back(generateSyntheticImage(medium_size, cv::Scalar(0, 0, 255)));
    return test_set;
}

std::vector<cv::Mat> TestImageGenerator::loadTestImageSet(const std::string& directory) {
    std::vector<cv::Mat> images;
    if (!fs::exists(directory)) {
        std::cerr << "Warning: Directory does not exist: " << directory << std::endl;
        std::cerr << "Generating synthetic test images instead." << std::endl;
        return generateStandardTestSet();
    }
    std::vector<std::string> extensions = { ".jpg", ".jpeg", ".png", ".bmp", ".tiff" };
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
                cv::Mat image = cv::imread(entry.path().string());
                if (!image.empty()) {
                    images.push_back(image);
                    std::cout << "Loaded: " << entry.path().filename() << std::endl;
                }
            }
        }
    }
    if (images.empty()) {
        std::cerr << "Warning: No images loaded. Generating synthetic images." << std::endl;
        return generateStandardTestSet();
    }
    return images;
}

cv::Mat TestImageGenerator::resizeToStandardSizes(const cv::Mat& src, cv::Size target_size) {
    if (src.empty()) {
        throw std::invalid_argument("Source image is empty");
    }
    cv::Mat resized;
    cv::resize(src, resized, target_size, 0, 0, cv::INTER_LINEAR);
    return resized;
}

cv::Mat TestImageGenerator::addGaussianNoise(const cv::Mat& src, double mean, double stddev) {
    if (src.empty()) {
        throw std::invalid_argument("Source image is empty");
    }
    cv::Mat noise(src.size(), src.type());
    cv::Mat noisy_image;
    cv::randn(noise, mean, stddev);
    src.convertTo(noisy_image, CV_32F);
    noise.convertTo(noise, CV_32F);
    noisy_image += noise;
    cv::Mat result;
    noisy_image.convertTo(result, src.type());
    return result;
}

cv::Mat TestImageGenerator::addSaltAndPepperNoise(const cv::Mat& src, double probability) {
    if (src.empty()) {
        throw std::invalid_argument("Source image is empty");
    }
    if (probability < 0.0 || probability > 1.0) {
        throw std::invalid_argument("Probability must be between 0.0 and 1.0");
    }
    cv::Mat noisy_image = src.clone();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    for (int y = 0; y < noisy_image.rows; y++) {
        for (int x = 0; x < noisy_image.cols; x++) {
            double rand_val = dis(gen);
            if (rand_val < probability / 2) {
                if (noisy_image.channels() == 1) {
                    noisy_image.at<uchar>(y, x) = 255;
                }
                else {
                    noisy_image.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
                }
            }
            else if (rand_val < probability) {
                if (noisy_image.channels() == 1) {
                    noisy_image.at<uchar>(y, x) = 0;
                }
                else {
                    noisy_image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                }
            }
        }
    }
    return noisy_image;
}

void TestImageGenerator::validateImageSize(cv::Size size) {
    if (size.width <= 0 || size.height <= 0) {
        throw std::invalid_argument("Image size must be positive");
    }
    if (size.width > 10000 || size.height > 10000) {
        throw std::invalid_argument("Image size too large (max 10000x10000)");
    }
}

cv::Mat TestImageGenerator::createGradientPattern(cv::Size size, bool horizontal) {
    cv::Mat pattern(size, CV_8UC1);
    if (horizontal) {
        for (int y = 0; y < size.height; y++) {
            for (int x = 0; x < size.width; x++) {
                double normalized_x = static_cast<double>(x) / size.width;
                pattern.at<uchar>(y, x) = static_cast<uchar>(normalized_x * 255);
            }
        }
    }
    else {
        for (int y = 0; y < size.height; y++) {
            for (int x = 0; x < size.width; x++) {
                double normalized_y = static_cast<double>(y) / size.height;
                pattern.at<uchar>(y, x) = static_cast<uchar>(normalized_y * 255);
            }
        }
    }
    return pattern;
}

cv::Mat TestImageGenerator::createCheckerPattern(cv::Size size, int square_size) {
    cv::Mat pattern(size, CV_8UC1);
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            int square_x = x / square_size;
            int square_y = y / square_size;
            if ((square_x + square_y) % 2 == 0) {
                pattern.at<uchar>(y, x) = 255;
            }
            else {
                pattern.at<uchar>(y, x) = 0;
            }
        }
    }
    return pattern;
}