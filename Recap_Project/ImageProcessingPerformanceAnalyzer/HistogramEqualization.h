#ifndef HISTOGRAMEQUALIZATION_H
#define HISTOGRAMEQUALIZATION_H

#include <opencv2/opencv.hpp>
#include <vector>

/**
 //* @brief 히스토그램 평활화 클래스
 //*
// * OpenCV equalizeHist 함수와 직접 구현한 CDF 기반 평활화를 비교하여
// * 성능 및 정확도를 분석합니다.
// *
// * 히스토그램 평활화는 이미지의 명암 대비를 개선하여 세부사항을 더 잘 보이게 합니다.
// * 누적분포함수(CDF)를 사용하여 픽셀 값을 재분배합니다.
// */
class HistogramEqualization {
public:
    // ============================================================================
    // 공개 인터페이스
    // ============================================================================

    /**
     //* @brief OpenCV equalizeHist를 사용한 히스토그램 평활화
     //* @param src 입력 이미지 (그레이스케일 또는 컬러)
     //* @return 평활화된 이미지
     //* @details 컬러 이미지의 경우 YUV 색공간으로 변환 후 Y 채널만 평활화
     //*/
    static cv::Mat opencv_version(const cv::Mat& src);

    ///**
    // * @brief 직접 구현한 CDF 기반 히스토그램 평활화
    // * @param src 입력 이미지 (그레이스케일 또는 컬러)
    // * @return 평활화된 이미지
    // * @details 누적분포함수를 직접 계산하여 픽셀 값 재매핑
    // **/
    static cv::Mat custom_version(const cv::Mat& src);

    /**
    // * @brief 적응적 히스토그램 평활화 (CLAHE 방식)
    // * @param src 입력 이미지
    // * @param clipLimit 클리핑 한계값 (기본: 2.0)
    // * @param tileGridSize 타일 그리드 크기 (기본: 8x8)
    // * @return 적응적으로 평활화된 이미지
    // */
    static cv::Mat adaptive_version(const cv::Mat& src, double clipLimit = 2.0,
        cv::Size tileGridSize = cv::Size(8, 8));

    // ============================================================================
    // 히스토그램 계산 및 분석
    // ============================================================================

    /**
     //* @brief 히스토그램 계산 (단일 채널)
    // * @param src 입력 그레이스케일 이미지
    // * @return 256개 원소의 히스토그램 벡터
    // */
    static std::vector<int> calculateHistogram(const cv::Mat& src);

    /**
    // * @brief 누적분포함수(CDF) 계산
     //* @param histogram 입력 히스토그램
    // * @return 정규화된 CDF 벡터 (0-255 범위)
    // */
    static std::vector<float> calculateCDF(const std::vector<int>& histogram);

    /**
   //  * @brief 히스토그램 균등성 측정
    // * @param histogram 입력 히스토그램
    // * @return 균등성 점수 (0.0-1.0, 1.0이 완전 균등)
    // */
    static double measureUniformity(const std::vector<int>& histogram);

    /**
   //  * @brief 두 히스토그램의 유사도 계산
   //  * @param hist1 첫 번째 히스토그램
   //  * @param hist2 두 번째 히스토그램
   //  * @return 유사도 점수 (0.0-1.0, 1.0이 완전 일치)
   //  */
    static double compareHistograms(const std::vector<int>& hist1,
        const std::vector<int>& hist2);

    // ============================================================================
    // 시각화 및 검증
    // ============================================================================

    /**
 //    * @brief 히스토그램 평활화 전후 비교 이미지 생성
   //  * @param original 원본 이미지
   //  * @param equalized 평활화된 이미지
   //  * @return 4분할 비교 이미지 (원본, 평활화, 원본 히스토그램, 평활화 히스토그램)
   //  */
    static cv::Mat createComparisonImage(const cv::Mat& original, const cv::Mat& equalized);

    /**
    // * @brief 히스토그램 이미지 생성
    // * @param src 입력 이미지
    // * @param color 히스토그램 색상
    // * @param showCDF CDF도 함께 표시할지 여부
    // * @return 히스토그램 이미지
   //  */
    static cv::Mat drawHistogram(const cv::Mat& src, const cv::Scalar& color,
        bool showCDF = false);

    /**
   //  * @brief 평활화 품질 분석
     //* @param original 원본 이미지
    // * @param equalized 평활화된 이미지
    // * @return 분석 결과 구조체
    // */
    struct QualityMetrics {
        double uniformityImprovement;    ///< 균등성 개선도
        double contrastEnhancement;      ///< 대비 향상도
        double informationContent;       ///< 정보량 변화
        double mse;                      ///< 평균제곱오차
        double psnr;                     ///< 신호대잡음비
    };

    static QualityMetrics analyzeQuality(const cv::Mat& original, const cv::Mat& equalized);

    /**
     * @brief 히스토그램 평활화 정확도 검증
     * @param testImage 테스트 이미지
     * @return 검증 성공 여부
     */
    static bool validateHistogramEqualization(const cv::Mat& testImage);

private:
    // ============================================================================
    // 내부 헬퍼 함수
    // ============================================================================

    /**
     //* @brief 그레이스케일 이미지 평활화 (Custom)
     //* @param src 입력 그레이스케일 이미지
     //* @return 평활화된 이미지
     //*/
    static cv::Mat processGrayscale(const cv::Mat& src);

    /**
     //* @brief 컬러 이미지 평활화 (Custom)
     //* @param src 입력 컬러 이미지
     //* @return 평활화된 이미지
     //*/
    static cv::Mat processColor(const cv::Mat& src);

    /**
     //* @brief CDF를 사용하여 픽셀 값 재매핑
     //* @param src 입력 이미지
     //* @param cdf 누적분포함수
     //* @return 재매핑된 이미지
     //*/
    static cv::Mat remapPixels(const cv::Mat& src, const std::vector<float>& cdf);

    /**
     //* @brief 이미지 엔트로피 계산
     //* @param src 입력 이미지
     //* @return 엔트로피 값
     //*/
    static double calculateEntropy(const cv::Mat& src);

    /**
     //* @brief 이미지 대비 측정
     //* @param src 입력 이미지
     //* @return 대비 값 (표준편차 기반)
     //*/
    static double measureContrast(const cv::Mat& src);

    /**
    // * @brief 히스토그램 그리기 (내부용)
    // * @param histogram 히스토그램 데이터
    // * @param color 색상
    // * @param width 이미지 너비
    // * @param height 이미지 높이
    // * @return 히스토그램 이미지
    // */
    static cv::Mat drawHistogramInternal(const std::vector<int>& histogram,
        const cv::Scalar& color,
        int width, int height);

    // ============================================================================
    // 상수 정의
    // ============================================================================

    static constexpr int HIST_SIZE = 256;           ///< 히스토그램 크기
    static constexpr int HIST_WIDTH = 512;          ///< 히스토그램 이미지 너비
    static constexpr int HIST_HEIGHT = 400;         ///< 히스토그램 이미지 높이
    static constexpr double SIMILARITY_THRESHOLD = 0.95;  ///< 유사도 임계값
    static constexpr double MIN_UNIFORMITY_IMPROVEMENT = 0.1;  ///< 최소 균등성 개선도
};

#endif // HISTOGRAMEQUALIZATION_H