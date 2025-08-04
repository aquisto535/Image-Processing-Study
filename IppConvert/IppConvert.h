
// IppConvert.h: IppConvert 애플리케이션의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include "IppDib.h"
#include "IppImage.h"


// CIppConvertApp:
// 이 클래스의 구현에 대해서는 IppConvert.cpp을(를) 참조하세요.
//

class CIppConvertApp : public CWinApp
{
public:
	CIppConvertApp() noexcept;


// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	void IppDibToImage(IppDib& dib, IppByteImage& img);
	void IppDibToImage(IppDib& dib, IppRgbImage& img);

	void IppImageToDib(IppByteImage& img, IppDib& dib);
	void IppImageToDib(IppFloatImage& img, IppDib& dib);
	void IppImageToDib(IppRgbImage& img, IppDib& dib);
};

extern CIppConvertApp theApp;
