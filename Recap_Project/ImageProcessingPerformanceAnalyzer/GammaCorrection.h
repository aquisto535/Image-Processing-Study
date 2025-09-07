#ifndef GAMMACORRECTION_H
#define GAMMACORRECTION_H

#include <opencv2/opencv.hpp>

/**
 * @brief 감마 보정 클래스
 *
 * OpenCV LUT 방식과 직접 구현 방식을 비교하여 성능 및 정확도를 분석합니다.
 * 감마 값에 따라 이미지의 밝기를 비선형적으로 조절합니다.
 *
 * 감마 보정 공식: output = 255 * (input/255)^(1/γ)
 * - γ > 1: 이미지가 어두워짐
 * - γ < 1: 이미지가 밝아짐
 * - γ = 1: 변화 없음
 */
class GammaCorrection {
public:
    // ============================================================================
    // 공개 인터페이스
    // ============================================================================

    /**
     * @brief OpenCV LUT를 사용한 감마 보정
     * @param src 입력 이미지 (그레이스케일 또는 컬러)
     * @param gamma 감마 값 (0.1 ~ 3.0 권장)
     * @return 감마 보정된 이미지
     * @details 룩업 테이블을 미리 생성하여 빠른 처리 속도 제공
     */
    static cv::Mat opencv_version(const cv::Mat& src, double gamma);

    /**
     * @brief 직접 구현한 픽셀별 감마 보정
     * @param src 입력 이미지 (그레이스케일 또는 컬러)
     * @param gamma 감마 값 (0.1 ~ 3.0 권장)
     * @return 감마 보정된 이미지
     * @details 각 픽셀에 대해 개별적으로 pow 연산 수행
     */
    static cv::Mat custom_version(const cv::Mat& src, double gamma);

    /**
     * @brief 최적화된 Custom 버전 (LUT 사용)
     * @param src 입력 이미지 (그레이스케일 또는 컬러)
     * @param gamma 감마 값 (0.1 ~ 3.0 권장)
     * @return 감마 보정된 이미지
     * @details Custom 구현이지만 LUT를 사용하여 성능 최적화
     */
    static cv::Mat custom_optimized_version(const cv::Mat& src, double gamma);

    // ============================================================================
    // 유틸리티 함수
    // ============================================================================

    /**
     * @brief 감마 값 유효성 검사
     * @param gamma 검사할 감마 값
     * @return 유효하면 true, 아니면 false
     */
    static bool isValidGamma(double gamma);

    /**
     * @brief 감마 보정 전후 이미지의 히스토그램 비교
     * @param original 원본 이미지
     * @param corrected 감마 보정된 이미지
     * @return 히스토그램 비교 결과 이미지
     */
    static cv::Mat compareHistograms(const cv::Mat& original, const cv::Mat& corrected);

    /**
     * @brief 여러 감마 값으로 동시에 보정하여 비교
     * @param src 입력 이미지
     * @param gammaValues 감마 값들의 벡터
     * @return 타일 형태로 배열된 비교 이미지
     */
    static cv::Mat multiGammaComparison(const cv::Mat& src, const std::vector<double>& gammaValues);

private:
    // ============================================================================
    // 내부 헬퍼 함수
    // ============================================================================

    /**
     * @brief 감마 보정 룩업 테이블 생성
     * @param gamma 감마 값
     * @return 256개 원소를 가진 LUT 매트릭스
     */
    static cv::Mat createGammaLUT(double gamma);

    /**
     * @brief 단일 픽셀 값에 감마 보정 적용
     * @param pixel 입력 픽셀 값 (0-255)
     * @param gamma 감마 값
     * @return 보정된 픽셀 값 (0-255)
     */
    static uchar applyGammaToPixel(uchar pixel, double gamma);

    /**
     * @brief 그레이스케일 이미지 감마 보정 (Custom)
     * @param src 입력 그레이스케일 이미지
     * @param gamma 감마 값
     * @return 보정된 이미지
     */
    static cv::Mat processGrayscale(const cv::Mat& src, double gamma);

    /**
     * @brief 컬러 이미지 감마 보정 (Custom)
     * @param src 입력 컬러 이미지
     * @param gamma 감마 값
     * @return 보정된 이미지
     */
    static cv::Mat processColor(const cv::Mat& src, double gamma);

    /**
     * @brief 히스토그램 이미지 생성
     * @param src 입력 이미지
     * @param color 히스토그램 색상
     * @return 히스토그램 이미지
     */
    static cv::Mat drawHistogram(const cv::Mat& src, const cv::Scalar& color);

    // ============================================================================
    // 상수 정의
    // ============================================================================

    static constexpr double MIN_GAMMA = 0.1;      ///< 최소 감마 값
    static constexpr double MAX_GAMMA = 5.0;      ///< 최대 감마 값
    static constexpr int LUT_SIZE = 256;          ///< LUT 크기
    static constexpr int HIST_WIDTH = 512;        ///< 히스토그램 이미지 너비
    static constexpr int HIST_HEIGHT = 400;       ///< 히스토그램 이미지 높이
};

#endif // GAMMACORRECTION_H