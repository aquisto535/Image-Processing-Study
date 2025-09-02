#ifndef TEST_IMAGE_GENERATOR_H
#define TEST_IMAGE_GENERATOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class TestImageGenerator {
public:
    // Basic test image generation functions
    static cv::Mat createTestImage(cv::Size size, int type);
    static cv::Mat createTestImage(cv::Size size, int type, cv::Scalar color);
    static cv::Mat generateGradientImage(cv::Size size);
    static cv::Mat generateNoiseImage(cv::Size size, double noise_level);
    static cv::Mat generateCheckerboardImage(cv::Size size, int square_size);
    static cv::Mat generateRandomColorImage(cv::Size size);
    static cv::Mat generateSyntheticImage(cv::Size size, cv::Scalar color);

    // Image loading and processing functions
    static std::vector<cv::Mat> loadTestImageSet(const std::string& directory);
    static cv::Mat resizeToStandardSizes(const cv::Mat& src, cv::Size target_size);

    // Specific test image generation functions
    static cv::Mat generateHighContrastImage(cv::Size size);
    static cv::Mat generateLowContrastImage(cv::Size size);
    static cv::Mat generateEdgeTestImage(cv::Size size);
    static cv::Mat generateTextureImage(cv::Size size);

    // Standard test image set generation
    static std::vector<cv::Mat> generateStandardTestSet();

    // Image quality degradation functions
    static cv::Mat addGaussianNoise(const cv::Mat& src, double mean, double stddev);
    static cv::Mat addSaltAndPepperNoise(const cv::Mat& src, double probability);

private:
    // Internal helper functions
    static cv::Mat createGradientPattern(cv::Size size, bool horizontal);
    static cv::Mat createCheckerPattern(cv::Size size, int square_size);
    static void validateImageSize(cv::Size size);
};

#endif // TEST_IMAGE_GENERATOR_H