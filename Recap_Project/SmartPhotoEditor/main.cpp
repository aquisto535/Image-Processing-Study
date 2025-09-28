#include <opencv2/opencv.hpp>
#include "src/filters/ArithmeticFilters.h"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <clocale>

#ifdef HAVE_FREETYPE
#include <opencv2/freetype.hpp> // 한글 출력을 위해 추가
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _WIN32
// Helper function to convert UTF-8 string to system's ANSI code page for window titles
std::string utf8_to_cp(const std::string& utf8_str) {
    if (utf8_str.empty()) {
        return std::string();
    }
    int wide_len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
    if (wide_len == 0) return utf8_str;
    
    std::wstring wstr(wide_len, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], wide_len);
    
    int mb_len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (mb_len == 0) return utf8_str;

    std::string mb_str(mb_len, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &mb_str[0], mb_len, NULL, NULL);
    mb_str.resize(strlen(mb_str.c_str()));
    
    return mb_str;
}
#else
// For other OS, just return the original string
std::string utf8_to_cp(const std::string& utf8_str) {
    return utf8_str;
}
#endif

/**
 * @brief 1주차 영상 산술 연산 복습을 위한 대화형 편집기
 * 2월 1주차 학습 내용을 실제로 구현하고 테스트할 수 있는 프로그램
 * 전체 프로젝트 구조에 맞춰 ArithmeticFilters 클래스 활용
 */
class Week1Editor {
private:
    cv::Mat original_image;
    cv::Mat current_image;
    cv::Mat second_image;
    std::string window_name = "Smart Photo Editor - Week 1";
    
public:
    void run() {
        std::cout << "=====================================" << std::endl;
        std::cout << "    스마트 사진 편집기 - 1주차" << std::endl;
        std::cout << "   영상의 산술/논리 연산 복습" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        // 이미지 로드
        if (!loadImages()) {
            std::cerr << "이미지 로드에 실패했습니다." << std::endl;
            return;
        }
        
        // 메인 메뉴 루프
        while (true) {
            showMenu();
            int choice = getUserChoice();
            
            try {
                switch (choice) {
                    case 1: brightnessAdjustment(); break;
                    case 2: imageBlending(); break;
                    case 3: changeDetection(); break;
                    case 4: hdrEffect(); break;
                    case 5: bitwiseOperations(); break;
                    case 6: performanceComparison(); break;
                    case 7: resetImages(); break;
                    case 8: saveCurrentImage(); break;
                    case 0: 
                        std::cout << "프로그램을 종료합니다. 감사합니다!" << std::endl;
                        cv::destroyAllWindows();
                        return;
                    default:
                        std::cout << "❌ 잘못된 선택입니다. 다시 선택해주세요." << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "오류 발생: " << e.what() << std::endl;
            }
            
            std::cout << "\n계속하려면 Enter를 누르세요...";
            std::cin.ignore();
            std::cin.get();
        }
    }
    
private:
    bool loadImages() {
        // 메인 이미지 로드 시도
        std::cout << "이미지를 로드합니다..." << std::endl;
        
        // 여러 경로에서 이미지 찾기
        std::vector<std::string> possible_paths = {
            "../resources/sample_images/test_image1.jpg",
            "resources/sample_images/test_image1.jpg", 
            "../resources/test_image1.jpg",
            "resources/test_image1.jpg",
            "test_image.jpg",
            "lenna.png"
        };
        
        bool image_loaded = false;
        for (const auto& path : possible_paths) {
            original_image = cv::imread(path);
            if (!original_image.empty()) {
                std::cout << "✅ 이미지 로드 성공: " << path << std::endl;
                image_loaded = true;
                break;
            }
        }
        
        // 이미지를 찾지 못한 경우 테스트 이미지 생성
        if (!image_loaded) {
            std::cout << "⚠️ 이미지 파일을 찾을 수 없습니다. 테스트 이미지를 생성합니다." << std::endl;
            createTestImage(original_image);
        }
        
        current_image = original_image.clone();
        
        // 두 번째 이미지 로드 또는 생성
        second_image = cv::imread("../resources/sample_images/test_image2.jpg");
        if (second_image.empty()) {
            std::cout << "두 번째 이미지를 생성합니다." << std::endl;
            createSecondTestImage(second_image);
        }
        
        // 이미지 정보 출력
        std::cout << "\n📊 로드된 이미지 정보:" << std::endl;
        printImageInfo(original_image, "메인 이미지");
        printImageInfo(second_image, "보조 이미지");
        
        return true;
    }
    
    void createTestImage(cv::Mat& img) {
        // 512x512 테스트 이미지 생성
        img = cv::Mat::zeros(512, 512, CV_8UC3);
        
        // 그라데이션 배경
        for (int y = 0; y < img.rows; y++) {
            for (int x = 0; x < img.cols; x++) {
                img.at<cv::Vec3b>(y, x)[0] = (uchar)(x * 255 / img.cols);      // Blue
                img.at<cv::Vec3b>(y, x)[1] = (uchar)(y * 255 / img.rows);      // Green
                img.at<cv::Vec3b>(y, x)[2] = (uchar)((x + y) * 128 / (img.cols + img.rows)); // Red
            }
        }
        
        // 도형들 추가
        cv::circle(img, cv::Point(128, 128), 80, cv::Scalar(255, 255, 255), -1);
        cv::rectangle(img, cv::Rect(300, 100, 150, 100), cv::Scalar(0, 255, 255), -1);
        cv::ellipse(img, cv::Point(400, 350), cv::Size(100, 60), 45, 0, 360, cv::Scalar(255, 0, 255), -1);
        
        // 텍스트 추가
        cv::putText(img, "Test Image", cv::Point(200, 450), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(255, 255, 255), 2);
    }
    
    void createSecondTestImage(cv::Mat& img) {
        img = cv::Mat::zeros(512, 512, CV_8UC3);
        
        // 체크보드 패턴
        int block_size = 64;
        for (int y = 0; y < img.rows; y += block_size) {
            for (int x = 0; x < img.cols; x += block_size) {
                bool is_white = ((x / block_size) + (y / block_size)) % 2 == 0;
                cv::Scalar color = is_white ? cv::Scalar(255, 255, 255) : cv::Scalar(0, 0, 0);
                cv::rectangle(img, cv::Rect(x, y, block_size, block_size), color, -1);
            }
        }
        
        // 중앙에 원 추가
        cv::circle(img, cv::Point(256, 256), 150, cv::Scalar(0, 255, 0), -1);
        cv::putText(img, "Pattern", cv::Point(200, 280), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
    }
    
    void showMenu() {
        std::cout << "\n=========================================" << std::endl;
        std::cout << "               메뉴 선택" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << "1. 🔆 밝기 조절 (add 함수 복습)" << std::endl;
        std::cout << "2. 🎨 이미지 블렌딩 (addWeighted 복습)" << std::endl;
        std::cout << "3. 🔍 변화 감지 (absdiff 복습)" << std::endl;
        std::cout << "4. ✨ HDR 효과 (평균 연산 활용)" << std::endl;
        std::cout << "5. 🔲 비트 연산 (논리 연산 복습)" << std::endl;
        std::cout << "6. ⚡ 성능 비교 테스트" << std::endl;
        std::cout << "7. 🔄 이미지 초기화" << std::endl;
        std::cout << "8. 💾 현재 이미지 저장" << std::endl;
        std::cout << "0. 🚪 종료" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << "선택 (0-8): ";
    }
    
    int getUserChoice() {
        int choice;
        while (!(std::cin >> choice)) {
            std::cout << "숫자를 입력해주세요: ";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }
        return choice;
    }
    
    void brightnessAdjustment() {
        std::cout << "\n=== 밝기 조절 기능 ===" << std::endl;
        std::cout << "밝기 값을 입력하세요 (-100 ~ 100): ";
        int brightness;
        std::cin >> brightness;
        
        if (brightness < -100 || brightness > 100) {
            std::cout << "⚠️ 밝기 값이 범위를 벗어났습니다. -100~100 사이로 조정됩니다." << std::endl;
           
            // C++17에서는 std::clamp을 사용하는 것이 더 안전합니다.
            // 아래 코드를 기존 코드와 교체하세요.
            // 파일 상단에 이미 포함되어 있으면 생략 가능

            brightness = std::clamp(brightness, -100, 100);
        }
        
        cv::Mat result;
        ArithmeticFilters::adjustBrightness(current_image, result, brightness);
        
        // 결과 표시
        displayResults("밝기 조절 결과", {current_image, result}, {"변경 전", "변경 후"});
        
        // 결과 저장 여부 확인
        if (askSaveResult()) {
            current_image = result.clone();
            std::cout << "✅ 결과가 저장되었습니다." << std::endl;
        }
    }
    
    void imageBlending() {
        std::cout << "\n=== 이미지 블렌딩 기능 ===" << std::endl;
        std::cout << "블렌딩 비율을 입력하세요 (0.0 ~ 1.0): ";
        double alpha;
        std::cin >> alpha;
        
        if (alpha < 0.0 || alpha > 1.0) {
            std::cout << "⚠️ 알파 값이 범위를 벗어났습니다. 0.0~1.0 사이로 조정됩니다." << std::endl;
            //alpha = std::max(0.0, std::min(1.0, alpha));

			std::clamp(alpha, 0.0, 1.0); // C++17 이상에서 사용 가능
        }
        
        cv::Mat result;
        ArithmeticFilters::blendImages(current_image, second_image, result, alpha);
        
        // 결과 표시
        displayResults("이미지 블렌딩 결과", {current_image, second_image, result}, 
                      {"이미지 1", "이미지 2", "블렌딩 결과"});
        
        // 결과 저장 여부 확인
        if (askSaveResult()) {
            current_image = result.clone();
            std::cout << "✅ 결과가 저장되었습니다." << std::endl;
        }
    }
    
    void changeDetection() {
        std::cout << "\n=== 변화 감지 기능 ===" << std::endl;
        
        // 비교 대상 이미지 선택
        std::cout << "어떤 이미지와 비교하시겠습니까?" << std::endl;
        std::cout << "1. 원본 이미지" << std::endl;
        std::cout << "2. 보조 이미지 (Pattern)" << std::endl;
        std::cout << "선택 (1-2): ";
        int choice = getUserChoice();
        
        cv::Mat compare_target;
        std::string target_name;
        
        if (choice == 1) {
            compare_target = original_image;
            target_name = "원본";
        } else if (choice == 2) {
            compare_target = second_image;
            target_name = "보조 이미지";
        } else {
            std::cout << "잘못된 선택입니다." << std::endl;
            return;
        }

        std::cout << "임계값을 입력하세요 (0 ~ 255): ";
        int threshold;
        std::cin >> threshold;
        
        threshold =  std::clamp(threshold, 0, 255); // C++17 이상에서 사용 가능
        
        cv::Mat diff;
        // 변경: 비교 대상을 'before'로, 현재 이미지를 'after'로 전달하여 명확성 확보
        ArithmeticFilters::detectChanges(compare_target, current_image, diff, threshold);
        
        // 결과 표시
        // 변경: 비교 대상 -> 현재 이미지 -> 차이점 순서로 표시
        displayResults("변화 감지 결과", {compare_target, current_image, diff}, 
                      {target_name, "현재", "변화 감지"});
    }
    
    void hdrEffect() {
        std::cout << "\n=== HDR 효과 기능 ===" << std::endl;
        std::cout << "HDR 효과를 생성합니다..." << std::endl;
        
        // 현재 이미지에 다양한 밝기 변화를 준 이미지들 생성
        std::vector<cv::Mat> images;
        std::vector<int> brightness_values = {-50, -25, 0, 25, 50};
        
        for (int brightness : brightness_values) {
            cv::Mat bright_img;
            cv::add(current_image, cv::Scalar(brightness, brightness, brightness), bright_img);
            images.push_back(bright_img);
            std::cout << "밝기 " << brightness << " 이미지 생성 완료" << std::endl;
        }
        
        cv::Mat hdr_result;
        ArithmeticFilters::createHDREffect(images, hdr_result);
        
        // 결과 표시
        displayResults("HDR 효과 결과", {current_image, hdr_result}, {"원본", "HDR 효과"});
        
        // 결과 저장 여부 확인
        if (askSaveResult()) {
            current_image = hdr_result.clone();
            std::cout << "✅ 결과가 저장되었습니다." << std::endl;
        }
    }
    
    void bitwiseOperations() {
        std::cout << "\n=== 비트 연산 기능 ===" << std::endl;
        std::cout << "비트 연산을 선택하세요:" << std::endl;
        std::cout << "1. AND 연산" << std::endl;
        std::cout << "2. OR 연산" << std::endl;
        std::cout << "3. XOR 연산" << std::endl;
        std::cout << "4. NOT 연산" << std::endl;
        std::cout << "선택 (1-4): ";
        
        int choice;
        std::cin >> choice;
        
        cv::Mat result;
        std::string operation_name;
        std::vector<cv::Mat> display_images;
        std::vector<std::string> labels;
        
        switch (choice) {
            case 1:
                cv::bitwise_and(current_image, second_image, result);
                operation_name = "AND";
                display_images = {current_image, second_image, result};
                labels = {"이미지 1", "이미지 2", "AND 결과"};
                break;
            case 2:
                cv::bitwise_or(current_image, second_image, result);
                operation_name = "OR";
                display_images = {current_image, second_image, result};
                labels = {"이미지 1", "이미지 2", "OR 결과"};
                break;
            case 3:
                cv::bitwise_xor(current_image, second_image, result);
                operation_name = "XOR";
                display_images = {current_image, second_image, result};
                labels = {"이미지 1", "이미지 2", "XOR 결과"};
                break;
            case 4:
                cv::bitwise_not(current_image, result);
                operation_name = "NOT";
                display_images = {current_image, result};
                labels = {"원본", "NOT 결과"};
                break;
            default:
                std::cout << "잘못된 선택입니다." << std::endl;
                return;
        }
        
        std::cout << "✅ " << operation_name << " 연산 완료!" << std::endl;
        
        // 결과 표시
        displayResults("비트 " + operation_name + " 연산 결과", display_images, labels);
        
        // 결과 저장 여부 확인
        if (askSaveResult()) {
            current_image = result.clone();
            std::cout << "✅ 결과가 저장되었습니다." << std::endl;
        }
    }
    
    void performanceComparison() {
        std::cout << "\n=== 성능 비교 테스트 ===" << std::endl;
        std::cout << "다양한 이미지 크기로 성능을 측정합니다..." << std::endl;
        
        std::vector<cv::Size> test_sizes = {
            cv::Size(320, 240),   // QVGA
            cv::Size(640, 480),   // VGA
            cv::Size(1280, 720),  // HD
            cv::Size(1920, 1080)  // Full HD
        };
        
        for (const auto& size : test_sizes) {
            std::cout << "\n📏 테스트 크기: " << size.width << "x" << size.height << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            
            cv::Mat test_img1, test_img2, result;
            cv::resize(current_image, test_img1, size);
            cv::resize(second_image, test_img2, size);
            
            // 밝기 조절 성능 테스트
            std::cout << "🔆 밝기 조절 성능:" << std::endl;
            ArithmeticFilters::adjustBrightness(test_img1, result, 50);
            
            // 블렌딩 성능 테스트  
            std::cout << "🎨 이미지 블렌딩 성능:" << std::endl;
            ArithmeticFilters::blendImages(test_img1, test_img2, result, 0.5);
        }
    }
    
    void resetImages() {
        std::cout << "\n🔄 이미지를 초기 상태로 되돌립니다..." << std::endl;
        current_image = original_image.clone();
        std::cout << "✅ 초기화 완료!" << std::endl;
    }
    
    void saveCurrentImage() {
        std::cout << "\n💾 현재 이미지를 저장합니다..." << std::endl;
        std::string filename = "output_week1_" + getCurrentTimeString() + ".jpg";
        
        if (cv::imwrite(filename, current_image)) {
            std::cout << "✅ 이미지가 저장되었습니다: " << filename << std::endl;
        } else {
            std::cout << "❌ 이미지 저장에 실패했습니다." << std::endl;
        }
    }
    
    // 유틸리티 함수들
    void displayResults(const std::string& title, const std::vector<cv::Mat>& images, const std::vector<std::string>& labels) {
        if (images.empty()) return;

#ifdef HAVE_FREETYPE
        // 한글 텍스트 출력을 위한 FreeType 초기화
        cv::Ptr<cv::freetype::FreeType2> ft2;
        bool fontLoaded = false;
        try {
            ft2 = cv::freetype::createFreeType2();
            // Windows에 내장된 맑은 고딕 폰트 사용
            ft2->loadFontData("C:/Windows/Fonts/malgun.ttf", 0);
            fontLoaded = true;
        } catch (const cv::Exception&) {
            std::cout << "⚠️ 경고: 한글 폰트(malgun.ttf)를 로드할 수 없어 이미지 내 글자가 깨질 수 있습니다." << std::endl;
        }
#endif
        
        // 모든 이미지를 동일한 크기로 리사이즈
        cv::Size display_size(300, 200);
        std::vector<cv::Mat> resized_images;
        
        for (const auto& img : images) {
            cv::Mat resized;
            cv::resize(img, resized, display_size, 0, 0, cv::INTER_AREA);
            
            // 라벨 추가
            cv::Mat labeled = resized.clone();
            if (!labels.empty() && resized_images.size() < labels.size()) {
                const std::string& label_text = labels[resized_images.size()];
                bool freetype_drew = false;

#ifdef HAVE_FREETYPE
                if (fontLoaded) {
                    // FreeType으로 한글 출력
                    ft2->putText(labeled, label_text, cv::Point(10, 30), 20, cv::Scalar(0, 255, 255), -1, cv::LINE_AA, true);
                    freetype_drew = true;
                }
#endif
                if (!freetype_drew) {
                    // FreeType 모듈이 없거나 폰트 로드 실패 시 기본 함수 사용
                    cv::putText(labeled, label_text, cv::Point(10, 30), 
                               cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 255), 2);
                }
            }
            resized_images.push_back(labeled);
        }
        
        // 이미지들을 수평으로 연결
        cv::Mat display;
        cv::hconcat(resized_images, display);
        
        // UTF-8 제목을 시스템 코드페이지로 변환하여 표시
        std::string window_title = utf8_to_cp(title);
        cv::imshow(window_title, display);
        cv::waitKey(0);
        cv::destroyWindow(window_title);
    }
    
    bool askSaveResult() {
        std::cout << "결과를 저장하시겠습니까? (y/n): ";
        char save;
        std::cin >> save;
        return (save == 'y' || save == 'Y');
    }
    
    void printImageInfo(const cv::Mat& img, const std::string& name) {
        std::cout << name << " - 크기: " << img.size() 
                  << ", 채널: " << img.channels() 
                  << ", 타입: " << img.type() << std::endl;
    }
    
    std::string getCurrentTimeString() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time_t);
        
        char buffer[20];
        std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &tm);
        return std::string(buffer);
    }
};

int main() {
#ifdef _WIN32
    // Windows에서 콘솔 입출력 인코딩을 UTF-8로 설정하여 한글 깨짐 방지
    // 먼저 시스템 기본 로케일 설정
    setlocale(LC_ALL, ".utf8");

    // 콘솔 코드페이지 설정
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // UTF-8 설정이 실패하면 한국어 코드페이지로 폴백
    if (GetConsoleOutputCP() != CP_UTF8) {
        SetConsoleOutputCP(949); // Korean codepage
        SetConsoleCP(949);
        setlocale(LC_ALL, "korean");
    }

    // PowerShell이나 최신 터미널에서 UTF-8 강제 설정
    system("chcp 65001 > nul 2>&1");
#endif
    try {
        Week1Editor editor;
        editor.run();
    } catch (const std::exception& e) {
        std::cerr << "치명적 오류 발생: " << e.what() << std::endl;
        std::cout << "프로그램을 종료합니다. 아무 키나 누르세요...";
        std::cin.get();
        return -1;
    }
    
    return 0;
}
