#pragma once
#include "IppImage.h"

// ���� ȭ�� ��� �Լ�
void IppInverse(IppByteImage& img);
void IppBrightness(IppByteImage& img, int n);
void IppContrast(IppByteImage& img, int n);
void IppGammaCorrection(IppByteImage& img, float gamma);

// ������׷� ���� �Լ�
void IppHistogram(IppByteImage &img, float histo[256]);
void IppHistogramStretching(IppByteImage& img);
void IppHistogramEqualization(IppByteImage& img);

// ���� ��� ���� �Լ�
bool IppAdd(IppByteImage& img1, IppByteImage& img2, IppByteImage& img3);
bool IppSub(IppByteImage& img1, IppByteImage& img2, IppByteImage& img3);
bool IppAve(IppByteImage& img1, IppByteImage& img2, IppByteImage& img3);
bool IppDiff(IppByteImage& img1, IppByteImage& img2, IppByteImage& img3);

// ���� �� ���� �Լ�
bool IppAND(IppByteImage& img1, IppByteImage& img2, IppByteImage& img3);
bool IppOR(IppByteImage& img1, IppByteImage& img2, IppByteImage& img3);
