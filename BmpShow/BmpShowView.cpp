
// BmpShowView.cpp: CBmpShowView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "BmpShow.h"
#endif

#include "BmpShowDoc.h"
#include "BmpShowView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DIB_HEADER_MARKER ((WORD)('M' << 8) | 'B') 
// CBmpShowView

IMPLEMENT_DYNCREATE(CBmpShowView, CView)

BEGIN_MESSAGE_MAP(CBmpShowView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CBmpShowView 생성/소멸

CBmpShowView::CBmpShowView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CBmpShowView::~CBmpShowView()
{
}

BOOL CBmpShowView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CBmpShowView 그리기

void CBmpShowView::OnDraw(CDC* /*pDC*/)
{
	CBmpShowDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CBmpShowView 인쇄

BOOL CBmpShowView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CBmpShowView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CBmpShowView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CBmpShowView 진단

#ifdef _DEBUG
void CBmpShowView::AssertValid() const
{
	CView::AssertValid();
}

void CBmpShowView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBmpShowDoc* CBmpShowView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBmpShowDoc)));
	return (CBmpShowDoc*)m_pDocument;
}
#endif //_DEBUG


// CBmpShowView 메시지 처리기

void CBmpShowView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	FILE* fp = NULL;
	fopen_s(&fp, "lenna.bmp", "rb"); // lenna.bmp 파일을 바이너리 모드로 엽니다.

	if (fp == NULL)
	{
		AfxMessageBox(_T("파일을 열 수 없습니다."));
		return;
	}

	BITMAPFILEHEADER bmfh;
	fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp); // 파일 헤더를 읽어옴

	//파일 맨 처음 2바이트가 'BM'인지 확인(BM 마커는 BMP 파일임을 나타냄)
	BITMAPINFOHEADER bmih;
	fread(&bmih, sizeof(BITMAPINFOHEADER), 1, fp);

	if (bmfh.bfType != DIB_HEADER_MARKER) // DIB_HEADER_MARKER는 'BM'을 나타냄
	{
		AfxMessageBox(_T("BMP 파일이 아닙니다."));
		fclose(fp);
		return;
	}

	LONG width = bmih.biWidth;
	LONG height = bmih.biHeight;

	// 높이가 음수이면 이미지가 뒤집혀 있으므로 높이를 양수로 변경
	if (height < 0)
	{
		height = -height;
	}
	 
	WORD bitCount = bmih.biBitCount;	

	DWORD nWidthStep = (DWORD)((width * bitCount / 8 + 3) & ~3); // 한 줄의 바이트 수
	DWORD nImageSize = nWidthStep * height; // 이미지 전체 크기

	DWORD dwSize = 0;

	if(bitCount == 24) // 트루 컬러 비트맵의 경우 biBitCount값이 0x18로 십진수 24에 해당. RGB 색상 성분 각각 8비트 사용
	{
		dwSize = nImageSize + sizeof(BITMAPINFOHEADER);
	}
	else
	{
		dwSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << bitCount) + nImageSize;
	}

	BYTE* pBuffer = new BYTE[dwSize];
	if (pBuffer == NULL)
	{
		AfxMessageBox(_T("메모리 할당 실패"));
		fclose(fp);
		return;
	}

	fseek(fp, sizeof(BITMAPFILEHEADER), SEEK_SET); // 파일 포인터를 처음으로 되돌림
	//fread(pBuffer, dwSize, dwSize, fp); // 파일에서 이미지 데이터를 읽어옴
	fread(pBuffer, 1, dwSize, fp);
	
	LPVOID pBits = NULL;

	if (bitCount == 24)// 트루 컬러 비트맵의 경우 biBitCount값이 0x18로 십진수 24에 해당. RGB 색상 성분 각각 8비트 사용
	{
		pBits = pBuffer + sizeof(BITMAPINFOHEADER);
	}
	else
	{
		pBits = pBuffer + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << bitCount);
	}

	CClientDC dc(this);
	::SetDIBitsToDevice(dc.m_hDC, point.x, point.y, width, height, 0, 0, 0, height, pBits,
		(BITMAPINFO*)(pBuffer), DIB_RGB_COLORS);

	delete[] pBuffer; // 메모리 해제
	fclose(fp); // 파일 닫기



	CView::OnLButtonDown(nFlags, point);
}
