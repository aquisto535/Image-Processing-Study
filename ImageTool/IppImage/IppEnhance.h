#pragma once
#include "IppImage.h"

// 영상 화질 향상 함수
void IppInverse(IppByteImage& img);
void IppBrightness(IppByteImage& img, int n);
void IppContrast(IppByteImage& img, int n);
void IppGammaCorrection(IppByteImage& img, float gamma);

// 히스토그램 관련 함수
void IppHistogram(IppByteImage &img, float histo[256]);
void IppHistogramStretching(IppByteImage& img);
void IppHistogramEqualization(IppByteImage& img);

// 영상 산술 연산 함수
bool IppAdd(IppByteImage& img1, IppByteImage& img2, IppByteImage& img3);
bool IppSub(IppByteImage& img1, IppByteImage& img2, IppByteImage& img3);
bool IppAve(IppByteImage& img1, IppByteImage& img2, IppByteImage& img3);
bool IppDiff(IppByteImage& img1, IppByteImage& img2, IppByteImage& img3);

// 영상 논리 연산 함수
bool IppAND(IppByteImage& img1, IppByteImage& img2, IppByteImage& img3);
bool IppOR(IppByteImage& img1, IppByteImage& img2, IppByteImage& img3);
