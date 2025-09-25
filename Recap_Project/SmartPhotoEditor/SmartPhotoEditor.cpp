// SmartPhotoEditor.cpp : 애플리케이션의 진입점을 정의합니다.
//
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "SmartPhotoEditor.h"

using namespace std;

int main()
{
    std::cout << "=== 스마트 사진 편집기 v1.0 ===" << std::endl;

    // OpenCV 버전 확인
    std::cout << "OpenCV 버전: " << CV_VERSION << std::endl;
    std::cout << "CMake + OpenCV 프로젝트가 성공적으로 실행되었습니다!" << std::endl;

    // 간단한 테스트 이미지 생성 (검은색 배경에 빨간 원)
    cv::Mat testImage = cv::Mat::zeros(400, 400, CV_8UC3);
    cv::circle(testImage, cv::Point(200, 200), 100, cv::Scalar(0, 0, 255), -1);
    cv::putText(testImage, "OpenCV Test", cv::Point(100, 350),
        cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

    // 이미지 표시
    cv::imshow("OpenCV 테스트 이미지", testImage);
    std::cout << "테스트 이미지가 표시되었습니다. 아무 키나 누르면 계속됩니다." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    // 간단한 메뉴 표시
    std::cout << "\n[메뉴]" << std::endl;
    std::cout << "1. 이미지 로드하기" << std::endl;
    std::cout << "2. 필터 적용하기" << std::endl;
    std::cout << "3. 이미지 저장하기" << std::endl;
    std::cout << "4. 종료" << std::endl;

    int choice;
    std::cout << "\n선택하세요 (1-4): ";
    std::cin >> choice;

    switch (choice) {
    case 1:
        std::cout << "이미지 로드 기능 (준비 중...)" << std::endl;
        break;
    case 2:
        std::cout << "필터 적용 기능 (준비 중...)" << std::endl;
        break;
    case 3:
        std::cout << "이미지 저장 기능 (준비 중...)" << std::endl;
        break;
    case 4:
        std::cout << "프로그램을 종료합니다." << std::endl;
        break;
    default:
        std::cout << "잘못된 선택입니다." << std::endl;
    }

    return 0;
}
