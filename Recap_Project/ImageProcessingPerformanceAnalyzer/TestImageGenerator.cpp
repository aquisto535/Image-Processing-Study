#include "TestImageGenerator.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <random>
#include <filesystem>

namespace fs = std::filesystem;

// TestImageGenerator.cpp에 추가
cv::Mat TestImageGenerator::createTestImage(cv::Size size, int type) {
    validateImageSize(size);

    cv::Mat test_image;

    switch (type) {
    case CV_8UC1:  // 그레이스케일
        test_image = generateGradientImage(size);
        break;

    case CV_8UC3:  // 컬러
        test_image = generateRandomColorImage(size);
        break;

    case CV_8UC4:  // RGBA
        test_image = cv::Mat(size, CV_8UC3, cv::Scalar(128, 128, 128));
        cv::cvtColor(test_image, test_image, cv::COLOR_BGR2BGRA);
        break;

    default:
        // 기본값으로 그레이스케일 생성
        test_image = generateGradientImage(size);
        break;
    }

    return test_image;
}

cv::Mat TestImageGenerator::createTestImage(cv::Size size, int type, cv::Scalar color) {
    validateImageSize(size);

    cv::Mat test_image;

    switch (type) {
    case CV_8UC1:  // 그레이스케일
        test_image = cv::Mat(size, CV_8UC1, cv::Scalar(color[0]));
        break;

    case CV_8UC3:  // 컬러
        test_image = cv::Mat(size, CV_8UC3, color);
        break;

    case CV_8UC4:  // RGBA
        test_image = cv::Mat(size, CV_8UC3, color);
        cv::cvtColor(test_image, test_image, cv::COLOR_BGR2BGRA);
        break;

    default:
        // 기본값으로 지정된 색상으로 생성
        test_image = cv::Mat(size, CV_8UC3, color);
        break;
    }

    return test_image;
}

// 그라데이션 이미지 생성
cv::Mat TestImageGenerator::generateGradientImage(cv::Size size) {
    validateImageSize(size);

    cv::Mat gradient(size, CV_8UC1);

    // 수평 그라데이션 생성
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            double normalized_x = static_cast<double>(x) / size.width;
            uchar value = static_cast<uchar>(normalized_x * 255);
            gradient.at<uchar>(y, x) = value;
        }
    }

    return gradient;
}

// 노이즈 이미지 생성
cv::Mat TestImageGenerator::generateNoiseImage(cv::Size size, double noise_level) {
    validateImageSize(size);

    if (noise_level < 0.0 || noise_level > 1.0) {
        throw std::invalid_argument("노이즈 레벨은 0.0 ~ 1.0 범위여야 합니다.");
    }

    cv::Mat noise_image(size, CV_8UC1);

    // 랜덤 시드 설정
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    // 노이즈 레벨에 따른 노이즈 생성
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            if (static_cast<double>(dis(gen)) / 255.0 < noise_level) {
                noise_image.at<uchar>(y, x) = dis(gen);
            }
            else {
                noise_image.at<uchar>(y, x) = 128; // 기본값
            }
        }
    }

    return noise_image;
}

// 체커보드 패턴 이미지 생성
cv::Mat TestImageGenerator::generateCheckerboardImage(cv::Size size, int square_size) {
    validateImageSize(size);

    if (square_size <= 0 || square_size > std::min(size.width, size.height)) {
        throw std::invalid_argument("정사각형 크기가 유효하지 않습니다.");
    }

    cv::Mat checkerboard(size, CV_8UC1);

    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            int square_x = x / square_size;
            int square_y = y / square_size;

            // 체커보드 패턴: 홀수/짝수 정사각형에 따라 색상 결정
            if ((square_x + square_y) % 2 == 0) {
                checkerboard.at<uchar>(y, x) = 255; // 흰색
            }
            else {
                checkerboard.at<uchar>(y, x) = 0;   // 검은색
            }
        }
    }

    return checkerboard;
}

// 랜덤 컬러 이미지 생성
cv::Mat TestImageGenerator::generateRandomColorImage(cv::Size size) {
    validateImageSize(size);

    cv::Mat color_image(size, CV_8UC3);

    // 랜덤 시드 설정
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    // 각 픽셀에 랜덤 RGB 값 할당
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            cv::Vec3b& pixel = color_image.at<cv::Vec3b>(y, x);
            pixel[0] = dis(gen); // Blue
            pixel[1] = dis(gen); // Green
            pixel[2] = dis(gen); // Red
        }
    }

    return color_image;
}

// 합성 이미지 생성
cv::Mat TestImageGenerator::generateSyntheticImage(cv::Size size, cv::Scalar color) {
    validateImageSize(size);

    cv::Mat synthetic_image(size, CV_8UC3, color);
    return synthetic_image;
}

// 높은 명암비 이미지 생성
cv::Mat TestImageGenerator::generateHighContrastImage(cv::Size size) {
    validateImageSize(size);

    cv::Mat high_contrast(size, CV_8UC1);

    // 중앙을 기준으로 명암비가 높은 이미지 생성
    cv::Point center(size.width / 2, size.height / 2);
    int max_distance = std::max(size.width, size.height) / 2;

    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            double distance = cv::norm(cv::Point(x, y) - center);
            double normalized_distance = distance / max_distance;

            // 거리에 따른 밝기 변화 (비선형)
            uchar value = static_cast<uchar>(255 * (1.0 - normalized_distance * normalized_distance));
            high_contrast.at<uchar>(y, x) = value;
        }
    }

    return high_contrast;
}

// 낮은 명암비 이미지 생성
cv::Mat TestImageGenerator::generateLowContrastImage(cv::Size size) {
    validateImageSize(size);

    cv::Mat low_contrast(size, CV_8UC1);

    // 낮은 명암비를 위한 좁은 밝기 범위 사용
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            double normalized_x = static_cast<double>(x) / size.width;
            double normalized_y = static_cast<double>(y) / size.height;

            // 100-155 범위의 밝기 값 사용 (낮은 명암비)
            uchar value = static_cast<uchar>(100 + 55 * (normalized_x + normalized_y) / 2.0);
            low_contrast.at<uchar>(y, x) = value;
        }
    }

    return low_contrast;
}

// 에지 테스트 이미지 생성
cv::Mat TestImageGenerator::generateEdgeTestImage(cv::Size size) {
    validateImageSize(size);

    cv::Mat edge_test(size, CV_8UC1, cv::Scalar(128));

    // 수직선
    for (int y = 0; y < size.height; y++) {
        for (int x = size.width / 4; x < size.width / 4 + 2; x++) {
            if (x < size.width) edge_test.at<uchar>(y, x) = 255;
        }
        for (int x = 3 * size.width / 4; x < 3 * size.width / 4 + 2; x++) {
            if (x < size.width) edge_test.at<uchar>(y, x) = 0;
        }
    }

    // 수평선
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

// 텍스처 이미지 생성
cv::Mat TestImageGenerator::generateTextureImage(cv::Size size) {
    validateImageSize(size);

    cv::Mat texture(size, CV_8UC1);

    // 랜덤 시드 설정
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(128, 30); // 평균 128, 표준편차 30

    // 노이즈 기반 텍스처 생성
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            double noise = dis(gen);
            uchar value = cv::saturate_cast<uchar>(noise);
            texture.at<uchar>(y, x) = value;
        }
    }

    // 가우시안 블러로 부드러운 텍스처 생성
    cv::GaussianBlur(texture, texture, cv::Size(5, 5), 1.0);

    return texture;
}

// 표준 테스트 이미지 세트 생성
std::vector<cv::Mat> TestImageGenerator::generateStandardTestSet() {
    std::vector<cv::Mat> test_set;

    // 다양한 크기의 그레이스케일 이미지
    std::vector<cv::Size> sizes = {
        cv::Size(256, 256),   // small
        cv::Size(512, 512),   // medium
        cv::Size(1024, 1024), // large
        cv::Size(2048, 2048)  // xlarge
    };

    for (const auto& size : sizes) {
        // 기본 그레이스케일 이미지들
        test_set.push_back(generateGradientImage(size));
        test_set.push_back(generateCheckerboardImage(size, 32));
        test_set.push_back(generateHighContrastImage(size));
        test_set.push_back(generateLowContrastImage(size));
        test_set.push_back(generateEdgeTestImage(size));
        test_set.push_back(generateTextureImage(size));

        // 노이즈 이미지들
        test_set.push_back(generateNoiseImage(size, 0.1));
        test_set.push_back(generateNoiseImage(size, 0.3));
        test_set.push_back(generateNoiseImage(size, 0.5));
    }

    // 컬러 이미지들 (중간 크기만)
    cv::Size medium_size(512, 512);
    test_set.push_back(generateRandomColorImage(medium_size));
    test_set.push_back(generateSyntheticImage(medium_size, cv::Scalar(255, 0, 0)));   // 빨강
    test_set.push_back(generateSyntheticImage(medium_size, cv::Scalar(0, 255, 0)));   // 초록
    test_set.push_back(generateSyntheticImage(medium_size, cv::Scalar(0, 0, 255)));   // 파랑

    return test_set;
}

// 테스트 이미지 세트 로드
std::vector<cv::Mat> TestImageGenerator::loadTestImageSet(const std::string& directory) {
    std::vector<cv::Mat> images;

    if (!fs::exists(directory)) {
        std::cerr << "경고: 디렉토리가 존재하지 않습니다: " << directory << std::endl;
        std::cerr << "대신 합성 테스트 이미지를 생성합니다." << std::endl;
        return generateStandardTestSet();
    }

    // 지원하는 이미지 확장자
    std::vector<std::string> extensions = { ".jpg", ".jpeg", ".png", ".bmp", ".tiff" };

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

            if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
                cv::Mat image = cv::imread(entry.path().string());
                if (!image.empty()) {
                    images.push_back(image);
                    std::cout << "로드됨: " << entry.path().filename() << std::endl;
                }
            }
        }
    }

    if (images.empty()) {
        std::cerr << "경고: 로드된 이미지가 없습니다. 합성 이미지를 생성합니다." << std::endl;
        return generateStandardTestSet();
    }

    return images;
}

// 표준 크기로 리사이즈
cv::Mat TestImageGenerator::resizeToStandardSizes(const cv::Mat& src, cv::Size target_size) {
    if (src.empty()) {
        throw std::invalid_argument("소스 이미지가 비어있습니다.");
    }

    cv::Mat resized;
    cv::resize(src, resized, target_size, 0, 0, cv::INTER_LINEAR);
    return resized;
}

// 가우시안 노이즈 추가
cv::Mat TestImageGenerator::addGaussianNoise(const cv::Mat& src, double mean, double stddev) {
    if (src.empty()) {
        throw std::invalid_argument("소스 이미지가 비어있습니다.");
    }

    cv::Mat noise(src.size(), src.type());
    cv::Mat noisy_image;

    // 노이즈 생성
    cv::randn(noise, mean, stddev);

    // 노이즈 추가
    src.convertTo(noisy_image, CV_32F);
    noise.convertTo(noise, CV_32F);
    noisy_image += noise;

    // 0-255 범위로 클리핑
    cv::Mat result;
    noisy_image.convertTo(result, src.type());

    return result;
}

// 솔트 앤 페퍼 노이즈 추가
cv::Mat TestImageGenerator::addSaltAndPepperNoise(const cv::Mat& src, double probability) {
    if (src.empty()) {
        throw std::invalid_argument("소스 이미지가 비어있습니다.");
    }

    if (probability < 0.0 || probability > 1.0) {
        throw std::invalid_argument("확률은 0.0 ~ 1.0 범위여야 합니다.");
    }

    cv::Mat noisy_image = src.clone();

    // 랜덤 시드 설정
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // 솔트 앤 페퍼 노이즈 추가
    for (int y = 0; y < noisy_image.rows; y++) {
        for (int x = 0; x < noisy_image.cols; x++) {
            double rand_val = dis(gen);

            if (rand_val < probability / 2) {
                // 솔트 노이즈 (흰색)
                if (noisy_image.channels() == 1) {
                    noisy_image.at<uchar>(y, x) = 255;
                }
                else {
                    noisy_image.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
                }
            }
            else if (rand_val < probability) {
                // 페퍼 노이즈 (검은색)
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

// 이미지 크기 검증
void TestImageGenerator::validateImageSize(cv::Size size) {
    if (size.width <= 0 || size.height <= 0) {
        throw std::invalid_argument("이미지 크기는 양수여야 합니다.");
    }

    if (size.width > 10000 || size.height > 10000) {
        throw std::invalid_argument("이미지 크기가 너무 큽니다 (최대 10000x10000).");
    }
}

// 내부 헬퍼 함수들
cv::Mat TestImageGenerator::createGradientPattern(cv::Size size, bool horizontal) {
    cv::Mat pattern(size, CV_8UC1);

    if (horizontal) {
        // 수평 그라데이션
        for (int y = 0; y < size.height; y++) {
            for (int x = 0; x < size.width; x++) {
                double normalized_x = static_cast<double>(x) / size.width;
                pattern.at<uchar>(y, x) = static_cast<uchar>(normalized_x * 255);
            }
        }
    }
    else {
        // 수직 그라데이션
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