cmake_minimum_required(VERSION 3.15)
project(SmartPhotoEditor_Week1 VERSION 1.0.0 LANGUAGES CXX)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 컴파일 옵션
if(MSVC)
    add_compile_options(/W4 /utf-8)
    # Visual Studio에서 콘솔 창 유지
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SmartPhotoEditor_Week1)
else()
    add_compile_options(-Wall -Wextra -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# OpenCV 경로 설정 (Windows의 경우)
if(WIN32)
    # 여러 가능한 OpenCV 경로를 시도
    set(OPENCV_PATHS
        "C:/opencv/build"
        "C:/Program Files/OpenCV/build"
        "C:/msys64/mingw64"
        "C:/tools/opencv"
        "$ENV{OPENCV_DIR}"
    )

    # 경로를 찾아서 CMAKE_PREFIX_PATH에 추가
    foreach(path ${OPENCV_PATHS})
        if(EXISTS "${path}")
            set(CMAKE_PREFIX_PATH "${path}" ${CMAKE_PREFIX_PATH})
            message(STATUS "Found OpenCV at: ${path}")
            break()
        endif()
    endforeach()

    # 기본 경로 설정 (찾지 못한 경우)
    if(NOT CMAKE_PREFIX_PATH)
        set(CMAKE_PREFIX_PATH "C:/opencv/build" ${CMAKE_PREFIX_PATH})
        message(WARNING "OpenCV not found in common locations. Using default path: C:/opencv/build")
    endif()
endif()

# OpenCV 찾기 (freetype은 선택 사항으로)
find_package(OpenCV COMPONENTS core imgproc highgui freetype)

if(OpenCV_FOUND)
    # 필수 모듈 확인
    if(NOT OpenCV_core_FOUND OR NOT OpenCV_imgproc_FOUND OR NOT OpenCV_highgui_FOUND)
        message(FATAL_ERROR "Required OpenCV modules (core, imgproc, highgui) not found.")
    endif()

    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
    message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
    message(STATUS "OpenCV include: ${OpenCV_INCLUDE_DIRS}")
    include_directories(${OpenCV_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "OpenCV not found!")
endif()

# 소스 파일 설정
set(SOURCES
    main.cpp
    src/filters/ArithmeticFilters.cpp
)

set(HEADERS
    src/filters/ArithmeticFilters.h
)

# 실행 파일 생성
add_executable(SmartPhotoEditor_Week1 ${SOURCES} ${HEADERS})

# OpenCV 라이브러리 링크
target_link_libraries(SmartPhotoEditor_Week1 ${OpenCV_LIBS})

# freetype 모듈이 있는 경우 컴파일러 정의 추가
if(OpenCV_FOUND)
    # OpenCV 구성 요소 목록을 확인하여 freetype 모듈이 있는지 확인
    set(HAVE_FREETYPE FALSE)
    if(OpenCV_freetype_FOUND)
        set(HAVE_FREETYPE TRUE)
        message(STATUS "Optional OpenCV module 'freetype' found. Korean text support enabled.")
        target_compile_definitions(SmartPhotoEditor_Week1 PRIVATE HAVE_FREETYPE)
    else()
        message(WARNING "Optional OpenCV module 'freetype' not found. Korean text in images may be broken.")
    endif()
endif()

# 컴파일 옵션 설정
target_compile_features(SmartPhotoEditor_Week1 PRIVATE cxx_std_17)

# 헤더 파일 경로 설정
target_include_directories(SmartPhotoEditor_Week1 PRIVATE .)

# resources 디렉토리가 있다면 빌드 디렉토리로 복사
if(EXISTS "${CMAKE_SOURCE_DIR}/resources")
    file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
    message(STATUS "Copied resources directory to build directory")
endif()

# output 디렉토리 생성
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/output)

# Windows에서 콘솔 창 유지 (Visual Studio)
if(MSVC)
    set_target_properties(SmartPhotoEditor_Week1 PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()

# 설치 규칙 (선택사항)
install(TARGETS SmartPhotoEditor_Week1 DESTINATION bin)
if(EXISTS "${CMAKE_SOURCE_DIR}/resources")
    install(DIRECTORY resources/ DESTINATION share/SmartPhotoEditor_Week1/)
endif()

# 빌드 정보 출력
message(STATUS "=== SmartPhotoEditor Week1 Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Source files: ${SOURCES}")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "===============================================")
