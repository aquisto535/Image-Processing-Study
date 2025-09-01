// CGaussianDlg.cpp: 구현 파일
//

#include "pch.h"
#include "ImageTool.h"
#include "afxdialogex.h"
#include "CGaussianDlg.h"


// CGaussianDlg 대화 상자

IMPLEMENT_DYNAMIC(CGaussianDlg, CDialogEx)

CGaussianDlg::CGaussianDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GAUSSIAN, pParent)
	, m_fSigma(0)
{

}

CGaussianDlg::~CGaussianDlg()
{
}

void CGaussianDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SIGMA_SLIDER, m_sliderSigma);
	DDX_Text(pDX, IDC_SIGMA_EDIT, m_fSigma);
	DDV_MinMaxFloat(pDX, m_fSigma, 0.20, 5.00);
}


BEGIN_MESSAGE_MAP(CGaussianDlg, CDialogEx)
END_MESSAGE_MAP()


// CGaussianDlg 메시지 처리기
