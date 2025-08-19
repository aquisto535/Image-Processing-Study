#pragma once
#include "IppImage.h"

void IppInverse(IppByteImage& img);
void IppBrightness(IppByteImage& img, int n);
void IppContrast(IppByteImage& img, int n);
void IppGammaCorrection(IppByteImage& img, float gamma);
void IppHistogram(IppByteImage &img, float histo[256]);
void IppHistogramStretching(IppByteImage& img);
void IppHistogramEqualization(IppByteImage& img);
