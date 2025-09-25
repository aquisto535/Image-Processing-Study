cmake_minimum_required(VERSION 3.15)
project(SmartPhotoEditor VERSION 1.0.0 LANGUAGES CXX)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 컴파일 옵션
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -O2)
endif()

# OpenCV 경로 설정
set(CMAKE_PREFIX_PATH "C:/opencv/build" ${CMAKE_PREFIX_PATH})

# OpenCV 찾기
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
    message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
    message(STATUS "OpenCV include: ${OpenCV_INCLUDE_DIRS}")
    include_directories(${OpenCV_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "OpenCV not found!")
endif()

# 간단한 실행 파일 생성
add_executable(SmartPhotoEditor SmartPhotoEditor.cpp)

# OpenCV 라이브러리 링크
target_link_libraries(SmartPhotoEditor ${OpenCV_LIBS})
