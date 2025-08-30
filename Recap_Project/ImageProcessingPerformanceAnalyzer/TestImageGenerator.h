#ifndef TEST_IMAGE_GENERATOR_H
#define TEST_IMAGE_GENERATOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class TestImageGenerator {
public:
    // 다양한 테스트 이미지 생성 (정적 함수들)
    cv::Mat createTestImage(cv::Size size, int type);
    cv::Mat createTestImage(cv::Size size, int type, cv::Scalar color);
    static cv::Mat generateGradientImage(cv::Size size);
    static cv::Mat generateNoiseImage(cv::Size size, double noise_level);
    static cv::Mat generateCheckerboardImage(cv::Size size, int square_size);
    static cv::Mat generateRandomColorImage(cv::Size size);
    static cv::Mat generateSyntheticImage(cv::Size size, cv::Scalar color);

    // 실제 이미지 로드 및 리사이즈
    static std::vector<cv::Mat> loadTestImageSet(const std::string& directory);
    static cv::Mat resizeToStandardSizes(const cv::Mat& src, cv::Size target_size);

    // 특수 테스트 이미지 생성
    static cv::Mat generateHighContrastImage(cv::Size size);
    static cv::Mat generateLowContrastImage(cv::Size size);
    static cv::Mat generateEdgeTestImage(cv::Size size);
    static cv::Mat generateTextureImage(cv::Size size);

    // 표준 테스트 이미지 세트 생성
    static std::vector<cv::Mat> generateStandardTestSet();

    // 이미지 품질 조절
    static cv::Mat addGaussianNoise(const cv::Mat& src, double mean, double stddev);
    static cv::Mat addSaltAndPepperNoise(const cv::Mat& src, double probability);

private:
    // 내부 헬퍼 함수들
    static cv::Mat createGradientPattern(cv::Size size, bool horizontal);
    static cv::Mat createCheckerPattern(cv::Size size, int square_size);
    static void validateImageSize(cv::Size size);
};

#endif // TEST_IMAGE_GENERATOR_H