#pragma once

#include "IppImage.h"

class IppFourier
{
public:
	int width;
	int height;
	IppDoubleImage real; // 실수부
	IppDoubleImage imag; // 허수부

public:
	IppFourier();

	void SetImage(IppByteImage& img); // 입력 영상을 real 영상에 복사
	void GetImage(IppByteImage& img); // real 영상을 IppByteImage 타입의 영상 img로 복사
	void GetSpectrumImage(IppByteImage& img); //푸리에 변환 후 호출되어, 푸리에 스펙트럼 영상을 생성
	void GetPhaseImage(IppByteImage& img); // 푸리에 변환 후 호출되어, 위상값 영상을 생성

	// 영상의 푸리에 변환 함수
	void DFT(int dir); // 영상의 이산 푸리에 변환 또는 역변환을 수행 (dir = 1: 정방향, dir = -1: 역방향)
	void DFTRC(int dir); // 영상의 행과 열을 분리하여 영상의 이산	 푸리에 변환 또는 역변환을 수행 (dir = 1: 정방향, dir = -1: 역방향)
	void FFT(int dir); // 영상의 고속 푸리에 변환 또는 역변환을 수행 (dir = 1: 정방향, dir = -1: 역방향)

	// 주파수 공간에서의 필터링 함수
	void LowPassIdeal(int cutoff); // 주파수 공간에서 이상적 저역 통과 필터 수행
	void HighPassIdeal(int cutoff); // 주파수 공간에서 이상적 고역 통과 필터 수행
	void LowPassGaussian(int cutoff); // 주파수 공간에서 가우시안 저역 통과 필터 수행
	void HighPassGaussian(int cutoff); // 주파수 공간에서 가우시안 고역 통과 필터 수행
};

// 전역 함수 선언
void DFT1d(double* re, double* im, int N, int dir); // 1차원 이산 푸리에 변환 또는 역변환 수행
void FFT1d(double* re, double* im, int N, int dir); // 1차원 고속 푸리에 변환 또는 역변환 수행
bool IsPowerOf2(int n); // n이 2의 거듭제곱인지 검사
