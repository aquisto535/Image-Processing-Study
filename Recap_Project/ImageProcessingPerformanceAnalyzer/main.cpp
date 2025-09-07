#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <chrono>

#include "./PerformanceAnalyzer.h"
#include "./DualImplementationProcessor.h"
#include "./TestImageGenerator.h"
// #include "Visualization/AdvancedVisualization.h"

// Note: Removed redundant includes for classes already in DualImplementationProcessor
 #include "HistogramEqualization.h"
 #include "GammaCorrection.h"
 #include "ColorSpaceConversion.h"

using namespace std;
using namespace cv;

void runBasicPerformanceComparison();
void runComprehensiveBenchmark();

int main() {
    cout << "영상 처리 성능 분석 도구 시작!" << endl;
    cout << "OpenCV 버전: " << CV_VERSION << endl;

    try {
        // 1. 기본 성능 비교 실행
        runBasicPerformanceComparison();

        //// 2. 종합 벤치마크 실행
        runComprehensiveBenchmark();

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

// A basic performance comparison based on the following philosophy:
// Step 1 (Proximate Goal): Write raw C++ code without OpenCV's high-level functions
// to understand the performance bottlenecks (e.g., memory allocation, per-pixel processing).
// Step 2 (Ultimate Goal): Apply advanced optimization techniques (e.g., LUT, direct memory access)
// to surpass the performance of general-purpose libraries like OpenCV for specific tasks.
void runBasicPerformanceComparison()
{
    cout << "\n=== Basic Performance Comparison ===" << endl;

    TestImageGenerator generator;
    Mat testImage = generator.createTestImage(Size(512, 512), CV_8UC1);
    PerformanceAnalyzer analyzer;
    int iterations = 100;

    // --- Brightness Adjustment Comparison ---
    {
        cout << "\n--- Brightness Adjustment ---" << endl;
        int brightness = 50;

        // Step 1: Compare OpenCV with the raw, unoptimized version
        cout << "Step 1: Understanding bottlenecks (OpenCV vs. Raw)" << endl;
        auto result_raw = analyzer.runComparison(
            "Brightness (Raw)",
            [&](const Mat& img) { return DualImplementationProcessor::BrightnessAdjustment::opencv_version(img, brightness); },
            [&](const Mat& img) { return DualImplementationProcessor::BrightnessAdjustment::custom_raw_version(img, brightness); },
            testImage, iterations
        );
        cout << "  OpenCV Time: " << fixed << setprecision(3) << result_raw.opencvTime << " ms" << endl;
        cout << "  Custom Raw Time: " << result_raw.customTime << " ms" << endl;
        cout << "  Speedup (OpenCV vs Raw): " << result_raw.speedupRatio << "x" << endl;

        // Step 2: Compare OpenCV with the optimized version
        cout << "\nStep 2: Surpassing general-purpose performance (OpenCV vs. Optimized)" << endl;
        auto result_optimized = analyzer.runComparison(
            "Brightness (Optimized)",
            [&](const Mat& img) { return DualImplementationProcessor::BrightnessAdjustment::opencv_version(img, brightness); },
            [&](const Mat& img) { return DualImplementationProcessor::BrightnessAdjustment::custom_version(img, brightness); },
            testImage, iterations
        );
        cout << "  OpenCV Time: " << result_optimized.opencvTime << " ms" << endl;
        cout << "  Custom Optimized Time: " << result_optimized.customTime << " ms" << endl;
        cout << "  Speedup (Optimized vs OpenCV): " << result_optimized.speedupRatio << "x" << endl;//최적화된 버전과 OpenCV의 성능을 비교하는 비율
    }

    // --- Gamma Correction Comparison ---
    {
        cout << "\n--- Gamma Correction ---" << endl;
        double gamma = 2.0;

        // Step 1: Compare OpenCV with the raw, unoptimized version
        cout << "Step 1: Understanding bottlenecks (OpenCV vs. Raw)" << endl;
        auto result_raw = analyzer.runComparison(
            "Gamma (Raw)",
            [&](const Mat& img) { return DualImplementationProcessor::GammaCorrection::opencv_version(img, gamma); },
            [&](const Mat& img) { return DualImplementationProcessor::GammaCorrection::custom_raw_version(img, gamma); },
            testImage, iterations
        );
        cout << "  OpenCV Time: " << fixed << setprecision(3) << result_raw.opencvTime << " ms" << endl;
        cout << "  Custom Raw Time: " << result_raw.customTime << " ms" << endl;
        cout << "  Speedup (OpenCV vs Raw): " << result_raw.speedupRatio << "x" << endl;

        // Step 2: Compare OpenCV with the optimized version (LUT-based)
        cout << "\nStep 2: Surpassing general-purpose performance (OpenCV vs. Optimized)" << endl;
        auto result_optimized = analyzer.runComparison(
            "Gamma (Optimized)",
            [&](const Mat& img) { return DualImplementationProcessor::GammaCorrection::opencv_version(img, gamma); },
            [&](const Mat& img) { return DualImplementationProcessor::GammaCorrection::custom_version(img, gamma); },
            testImage, iterations
        );
        cout << "  OpenCV Time: " << result_optimized.opencvTime << " ms" << endl;
        cout << "  Custom Optimized Time: " << result_optimized.customTime << " ms" << endl;
        cout << "  Speedup (Optimized vs OpenCV): " << result_optimized.speedupRatio << "x" << endl; //최적화된 버전과 OpenCV의 성능을 비교하는 비율
    }
}

void runComprehensiveBenchmark()
{
    cout << "\n=== Comprehensive Benchmark ===" << endl;

    Mat testImage = TestImageGenerator::generateGradientImage(Size(512, 512));
    cout << "Generated a test image for benchmark." << endl;
    
    Mat opencv_result = DualImplementationProcessor::HistogramEqualization::opencv_version(testImage);
    Mat custom_result = DualImplementationProcessor::HistogramEqualization::custom_version(testImage);

    // This part requires a separate HistogramEqualization class which is not fully defined
    // in the provided context. Assuming it should use methods from DualImplementationProcessor.
    // For now, we'll comment it out to fix the build.
    // If you have a separate Histogram.h/cpp, we need to review it.
    
    // auto opencv_hist = DualImplementationProcessor::HistogramEqualization::calculateHistogram(opencv_result);
    // auto custom_hist = DualImplementationProcessor::HistogramEqualization::calculateHistogram(custom_result);
    // double histogram_similarity = DualImplementationProcessor::HistogramEqualization::compareHistograms(opencv_hist, custom_hist) * 100.0;
    // cout << "Histogram Similarity: " << histogram_similarity << "%" << endl;
    
    cout << "Comprehensive benchmark completed (Histogram part is commented out)." << endl;
}
