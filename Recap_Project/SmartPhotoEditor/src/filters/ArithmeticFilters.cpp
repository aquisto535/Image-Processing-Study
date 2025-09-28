#include "ArithmeticFilters.h"
#include <iostream>
#include <iomanip>

// 1. 밝기 조절 구현 (add 함수 복습)
void ArithmeticFilters::adjustBrightness(const cv::Mat& src, cv::Mat& dst, int brightness) {
    std::cout << "\n=== 밝기 조절 기능 (add 함수 복습) ===" << std::endl;
    std::cout << "밝기 조절 값: " << brightness << std::endl;
    
    printImageInfo(src, "입력 이미지");
    
    // OpenCV 방법 - 성능 측정
    cv::Mat dst_opencv;
    double opencv_time = measurePerformance([&]() {
        cv::add(src, cv::Scalar(brightness, brightness, brightness), dst_opencv);
    }, "OpenCV add()");
    
    // 직접 구현 방법 - 성능 측정  
    cv::Mat dst_manual = cv::Mat::zeros(src.size(), src.type());
    double manual_time = measurePerformance([&]() {
        for (int y = 0; y < src.rows; y++) {
            for (int x = 0; x < src.cols; x++) {
                if (src.channels() == 1) {
                    // 그레이스케일
                    int val = src.at<uchar>(y, x) + brightness;
                    dst_manual.at<uchar>(y, x) = cv::saturate_cast<uchar>(val);
                } else if (src.channels() == 3) {
                    // 컬러 이미지 (BGR)
                    cv::Vec3b pixel = src.at<cv::Vec3b>(y, x);
                    for (int c = 0; c < 3; c++) {
                        int val = pixel[c] + brightness;
                        dst_manual.at<cv::Vec3b>(y, x)[c] = cv::saturate_cast<uchar>(val);
                    }
                }
            }
        }
    }, "Manual implementation");
    
    // 성능 비교 결과 출력
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Speed improvement: " << (manual_time / opencv_time) << "x" << std::endl;
    
    // 결과 비교
    compareResults(dst_opencv, dst_manual, "OpenCV", "Manual");
    
    // 기본적으로 OpenCV 결과를 반환
    dst = dst_opencv.clone();
}

// 2. 이미지 블렌딩 (addWeighted 복습)
void ArithmeticFilters::blendImages(const cv::Mat& img1, const cv::Mat& img2, cv::Mat& result, double alpha) {
    std::cout << "\n=== 이미지 블렌딩 (addWeighted 복습) ===" << std::endl;
    std::cout << "Alpha 값: " << alpha << " (이미지1:" << alpha << ", 이미지2:" << (1.0 - alpha) << ")" << std::endl;

    // 입력 이미지 유효성 검사
    if (img1.empty() || img2.empty()) {
        std::cout << "❌ 입력 이미지가 비어있습니다." << std::endl;
        return;
    }

    // 크기 맞추기
    cv::Mat img2_resized;
    if (img1.size() != img2.size()) {
        cv::resize(img2, img2_resized, img1.size());
        std::cout << "이미지 2를 " << img1.size() << " 크기로 리사이즈했습니다." << std::endl;
    } else {
        img2_resized = img2.clone();
    }
    
    printImageInfo(img1, "이미지 1");
    printImageInfo(img2_resized, "이미지 2 (리사이즈됨)");
    
    // OpenCV addWeighted 사용
    cv::Mat result_opencv;
    double opencv_time = measurePerformance([&]() {
        cv::addWeighted(img1, alpha, img2_resized, (1.0 - alpha), 0, result_opencv);
    }, "OpenCV addWeighted()");
    
    // 직접 구현
    cv::Mat result_manual = cv::Mat::zeros(img1.size(), img1.type());
    double manual_time = measurePerformance([&]() {
        for (int y = 0; y < img1.rows; y++) {
            for (int x = 0; x < img1.cols; x++) {
                if (img1.channels() == 1) {
                    double val = alpha * img1.at<uchar>(y, x) + (1.0 - alpha) * img2_resized.at<uchar>(y, x);
                    result_manual.at<uchar>(y, x) = cv::saturate_cast<uchar>(val);
                } else if (img1.channels() == 3) {
                    cv::Vec3b pixel1 = img1.at<cv::Vec3b>(y, x);
                    cv::Vec3b pixel2 = img2_resized.at<cv::Vec3b>(y, x);
                    for (int c = 0; c < 3; c++) {
                        double val = alpha * pixel1[c] + (1.0 - alpha) * pixel2[c];
                        result_manual.at<cv::Vec3b>(y, x)[c] = cv::saturate_cast<uchar>(val);
                    }
                }
            }
        }
    }, "Manual addWeighted implementation");
    
    // 성능 비교 결과 출력
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Speed improvement: " << (manual_time / opencv_time) << "x" << std::endl;
    
    // 결과 비교
    compareResults(result_opencv, result_manual, "OpenCV", "Manual");
    
    // 기본적으로 OpenCV 결과를 반환
    result = result_opencv.clone();
}

// 3. 변화 감지 (absdiff 복습)
void ArithmeticFilters::detectChanges(const cv::Mat& before, const cv::Mat& after, cv::Mat& diff, int threshold) {
    std::cout << "\n=== 변화 감지 (absdiff 복습) ===" << std::endl;
    std::cout << "임계값: " << threshold << std::endl;
    
    // 크기와 타입 맞추기
    cv::Mat after_resized;
    if (before.size() != after.size()) {
        cv::resize(after, after_resized, before.size());
        std::cout << "이미지 크기를 " << before.size() << "로 조정했습니다." << std::endl;
    } else {
        after_resized = after.clone();
    }
    
    // 그레이스케일로 변환
    cv::Mat before_gray, after_gray;
    if (before.channels() == 3) {
        cv::cvtColor(before, before_gray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(after_resized, after_gray, cv::COLOR_BGR2GRAY);
    } else {
        before_gray = before.clone();
        after_gray = after_resized.clone();
    }
    
    // absdiff를 이용한 차이 계산
    cv::Mat raw_diff;
    double absdiff_time = measurePerformance([&]() {
        cv::absdiff(before_gray, after_gray, raw_diff);
    }, "OpenCV absdiff()");
    
    // 임계값 적용하여 변화 영역만 강조
    cv::Mat binary_diff;
    cv::threshold(raw_diff, binary_diff, threshold, 255, cv::THRESH_BINARY);
    
    // 통계 정보 계산
    int changed_pixels = cv::countNonZero(binary_diff);
    int total_pixels = raw_diff.rows * raw_diff.cols;
    double change_percentage = (double)changed_pixels / total_pixels * 100.0;
    
    std::cout << "변화된 픽셀: " << changed_pixels << " / " << total_pixels << std::endl;
    std::cout << "변화 비율: " << std::fixed << std::setprecision(2) << change_percentage << "%" << std::endl;
    
    // 결과를 3채널로 변환하여 변화 영역을 빨간색으로 표시
    std::vector<cv::Mat> channels;
    channels.push_back(cv::Mat::zeros(raw_diff.size(), CV_8UC1));  // Blue
    channels.push_back(cv::Mat::zeros(raw_diff.size(), CV_8UC1));  // Green  
    channels.push_back(binary_diff);                               // Red
    cv::merge(channels, diff);
}

// 4. HDR 효과 구현 (여러 이미지의 평균)
void ArithmeticFilters::createHDREffect(const std::vector<cv::Mat>& images, cv::Mat& result) {
    std::cout << "\n=== HDR 효과 구현 (평균 연산 활용) ===" << std::endl;
    
    if (images.empty()) {
        std::cout << "입력 이미지가 없습니다." << std::endl;
        return;
    }
    
    std::cout << "입력 이미지 개수: " << images.size() << std::endl;
    
    // 첫 번째 이미지를 기준으로 크기 설정
    cv::Size target_size = images[0].size();
    std::cout << "기준 이미지 크기: " << target_size << std::endl;
    
    // 32비트 float으로 누적합 계산
    cv::Mat sum = cv::Mat::zeros(target_size, CV_32FC3);
    
    double hdr_time = measurePerformance([&]() {
        for (size_t i = 0; i < images.size(); i++) {
            cv::Mat img_resized, img_float;
            
            // 크기 맞추기
            if (images[i].size() != target_size) {
                cv::resize(images[i], img_resized, target_size);
            } else {
                img_resized = images[i].clone();
            }
            
            // float으로 변환하여 누적
            img_resized.convertTo(img_float, CV_32FC3);
            cv::add(sum, img_float, sum);
            
            std::cout << "이미지 " << (i + 1) << "/" << images.size() << " 처리 완료" << std::endl;
        }
        
        // 평균 계산
        sum = sum / (double)images.size();
        
        // 다시 8비트로 변환
        sum.convertTo(result, CV_8UC3);
    }, "HDR 효과 생성");
    
    std::cout << "HDR 효과 생성 완료!" << std::endl;
}

// 성능 측정 유틸리티
double ArithmeticFilters::measurePerformance(std::function<void()> operation, const std::string& description) {
    auto start = std::chrono::high_resolution_clock::now();
    operation();
    auto end = std::chrono::high_resolution_clock::now();
    
    double time_ms = std::chrono::duration<double, std::milli>(end - start).count();
    
    if (!description.empty()) {
        std::cout << description << ": " << std::fixed << std::setprecision(3) << time_ms << "ms" << std::endl;
    }
    
    return time_ms;
}

// 결과 비교 유틸리티
void ArithmeticFilters::compareResults(const cv::Mat& result1, const cv::Mat& result2, 
                                         const std::string& method1, const std::string& method2) {
    if (result1.size() != result2.size() || result1.type() != result2.type()) {
        std::cout << "결과 이미지의 크기나 타입이 다릅니다." << std::endl;
        return;
    }
    
    cv::Mat diff;
    cv::absdiff(result1, result2, diff);
    cv::Scalar mean_diff = cv::mean(diff);
    
    double avg_diff = (mean_diff[0] + mean_diff[1] + mean_diff[2]) / 3.0;
    
    std::cout << method1 << " vs " << method2 << " 평균 차이: " 
              << std::fixed << std::setprecision(4) << avg_diff << std::endl;
    
    if (avg_diff < 0.01) {
        std::cout << "✅ 두 방법의 결과가 거의 동일합니다!" << std::endl;
    } else if (avg_diff < 1.0) {
        std::cout << "⚠️ 두 방법의 결과에 미미한 차이가 있습니다." << std::endl;
    } else {
        std::cout << "❌ 두 방법의 결과에 상당한 차이가 있습니다." << std::endl;
    }
}

// 크기 맞추기 헬퍼 함수
void ArithmeticFilters::ensureSameSize(const cv::Mat& img1, cv::Mat& img2) {
    if (img1.size() != img2.size()) {
        cv::resize(img2, img2, img1.size());
    }
}

// 이미지 정보 출력 헬퍼 함수
void ArithmeticFilters::printImageInfo(const cv::Mat& img, const std::string& name) {
    std::cout << name << " - 크기: " << img.size() 
              << ", 채널: " << img.channels() 
              << ", 타입: " << img.type() << std::endl;
}
