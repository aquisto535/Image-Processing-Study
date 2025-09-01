#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <chrono>

#include "./PerformanceAnalyzer.h"
#include "./DualImplementationProcessor.h"
#include "./TestImageGenerator.h"
//#include "Visualization/AdvancedVisualization.h"

using namespace std;
using namespace cv;

void runBasicPerformanceComparison();

int main() {
    cout << "영상 처리 성능 분석 도구 시작!" << endl;
    cout << "OpenCV 버전: " << CV_VERSION << endl;

    try {
        // 1. 기본 성능 비교 실행
        runBasicPerformanceComparison();

        //// 2. 종합 벤치마크 실행
        //runComprehensiveBenchmark();

        //// 3. 실시간 모니터링 데모
        //runRealTimeDemo();

        //// 4. 리포트 생성
        //generateAnalysisReport();

    }
    catch (const exception& e) {
        cerr << "오류 발생: " << e.what() << endl;
        return -1;
    }

    cout << "분석 완료! 결과를 확인하세요." << endl;
    waitKey(0);
    return 0;
}

//기본 성능 비교 함수
void runBasicPerformanceComparison() 
{
    cout << "\n=== 기본 성능 비교 ===" << endl;

    // 테스트 이미지 생성
    TestImageGenerator generator;
    Mat testImage = generator.createTestImage(Size(512, 512), CV_8UC1);

    PerformanceAnalyzer analyzer;

    // 밝기 조절 비교
    {
        int brightness = 50;
        auto opencv_func = [brightness](const Mat& img) {
            return DualImplementationProcessor::BrightnessAdjustment::opencv_version(img, brightness);
            };
        auto custom_func = [brightness](const Mat& img) {
            return DualImplementationProcessor::BrightnessAdjustment::custom_version(img, brightness);
            };

		auto result = analyzer.runComparison("Brightness_Adjustment", opencv_func, custom_func, testImage, 100); // 각 함수를 100회씩 실행한 결과를 반환

        cout << "   밝기 조절 결과:" << endl;
        cout << "   OpenCV: " << fixed << setprecision(2) << result.opencvTime << " ms" << endl;
        cout << "   Custom: " << result.customTime << " ms" << endl;
        cout << "   성능 비율: " << result.speedupRatio << "x" << endl;
        cout << "   정확도 차이: " << result.accuracy << "% (Custom - OpenCV)" << endl;
    }

    // 감마 보정 비교
    {
        double gamma = 2.0;
        auto opencv_func = [gamma](const Mat& img) {
            return DualImplementationProcessor::GammaCorrection::opencv_version(img, gamma);
            };
        auto custom_func = [gamma](const Mat& img) {
            return DualImplementationProcessor::GammaCorrection::custom_version(img, gamma);
            };

        auto result = analyzer.runComparison("Gamma_Correction", opencv_func, custom_func, testImage, 100);

        cout << "\n 감마 보정 결과:" << endl;
        cout << "   OpenCV: " << result.opencvTime << " ms" << endl;
        cout << "   Custom: " << result.customTime << " ms" << endl;
        cout << "   성능 비율: " << result.speedupRatio << "x" << endl;
        cout << "   정확도 차이: " << result.accuracy << "% (Custom - OpenCV)" << endl;
    }
}
