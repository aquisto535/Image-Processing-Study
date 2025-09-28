#ifndef ARITHMETIC_FILTERS_H
#define ARITHMETIC_FILTERS_H

#include <opencv2/opencv.hpp>
#include <chrono>
#include <functional>
#include <vector>

/**
 * @brief 1주차 학습 내용 복습: 영상의 산술 연산 필터
 * 2월 1주차에 학습한 add, addWeighted, absdiff 등을 직접 구현하고
 * OpenCV 함수와 성능 비교를 통해 이해도를 높입니다.
 */
class ArithmeticFilters {
public:
    // 1. 밝기 조절 (add 함수 복습)
    static void adjustBrightness(const cv::Mat& src, cv::Mat& dst, int brightness);
    
    // 2. 이미지 블렌딩 (addWeighted 복습)
    static void blendImages(const cv::Mat& img1, const cv::Mat& img2, cv::Mat& result, double alpha);
    
    // 3. 변화 감지 (absdiff 복습)
    static void detectChanges(const cv::Mat& before, const cv::Mat& after, cv::Mat& diff, int threshold = 30);
    
    // 4. HDR 효과 구현 (여러 이미지 합성)
    static void createHDREffect(const std::vector<cv::Mat>& images, cv::Mat& result);
    
    // 직접 구현 vs OpenCV 비교 함수들
    static void manualAdd(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst);
    static void opencvAdd(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst);
    
    static void manualAddWeighted(const cv::Mat& src1, double alpha, const cv::Mat& src2, 
                                 double beta, double gamma, cv::Mat& dst);
    static void opencvAddWeighted(const cv::Mat& src1, double alpha, const cv::Mat& src2, 
                                 double beta, double gamma, cv::Mat& dst);
    
    // 성능 측정 유틸리티
    static double measurePerformance(std::function<void()> operation, const std::string& description = "");
    
    // 결과 비교 유틸리티
    static void compareResults(const cv::Mat& result1, const cv::Mat& result2, 
                              const std::string& method1, const std::string& method2);

private:
    // 헬퍼 함수들
    static void ensureSameSize(const cv::Mat& img1, cv::Mat& img2);
    static void printImageInfo(const cv::Mat& img, const std::string& name);
};

#endif // ARITHMETIC_FILTERS_H
