// CBrightnessContrastDlg.cpp: 구현 파일
//

#include "pch.h"
#include "ImageTool.h"
#include "afxdialogex.h"
#include "CBrightnessContrastDlg.h"


// CBrightnessContrastDlg 대화 상자

IMPLEMENT_DYNAMIC(CBrightnessContrastDlg, CDialogEx)

CBrightnessContrastDlg::CBrightnessContrastDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BRIGHTNESS_CONTRAST, pParent)
	, m_nBrightness(0)
	, m_nContrast(0)
{

}

CBrightnessContrastDlg::~CBrightnessContrastDlg()
{
}

void CBrightnessContrastDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BRIGHTNESS_EDIT, m_nBrightness);
	DDV_MinMaxInt(pDX, m_nBrightness, -255, 255);
	DDX_Control(pDX, IDC_BRIGHTNESS_SLIDER, m_silderBrightness);
	DDX_Text(pDX, IDC_CONTRAST_EDIT, m_nContrast);
	DDX_Text(pDX, IDC_CONTRAST_EDIT, m_nContrast);
	DDV_MinMaxInt(pDX, m_nContrast, -100, 100);
	DDX_Control(pDX, IDC_CONTRAST_SLIDER, m_sliderContrast);
}


BEGIN_MESSAGE_MAP(CBrightnessContrastDlg, CDialogEx)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_BRIGHTNESS_EDIT, &CBrightnessContrastDlg::OnEnChangeBrightnessEdit)
	ON_EN_CHANGE(IDC_CONTRAST_EDIT, &CBrightnessContrastDlg::OnEnChangeContrastEdit)
END_MESSAGE_MAP()


// CBrightnessContrastDlg 메시지 처리기

BOOL CBrightnessContrastDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	//밝기 조절 슬라이더 컨트롤 초기화
	m_silderBrightness.SetRange(-255, 255);
	m_silderBrightness.SetTicFreq(32); // 슬라이더 눈금 간격 설정
	m_silderBrightness.SetPageSize(32); // 페이지 크기 설정

	//명암비 조절 슬라이더 컨트롤 초기화
	m_sliderContrast.SetRange(-100, 100);
	m_sliderContrast.SetTicFreq(20); // 슬라이더 눈금 간격 설정
	m_sliderContrast.SetPageSize(20); // 페이지 크기 설정

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CBrightnessContrastDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (m_silderBrightness.GetSafeHwnd() == pScrollBar->GetSafeHwnd())
	{
		// 밝기 슬라이더의 값이 변경되었을 때
		m_nBrightness = m_silderBrightness.GetPos();
		UpdateData(FALSE); // 멤버 변수에 저장되어 있는 값으로 컨트롤에 값을 업데이트
	}
	else if (m_sliderContrast.GetSafeHwnd() == pScrollBar->GetSafeHwnd())
	{
		// 명암비 슬라이더의 값이 변경되었을 때
		m_nContrast = m_sliderContrast.GetPos();
		UpdateData(FALSE); // 컨트롤에 값을 업데이트
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CBrightnessContrastDlg::OnEnChangeBrightnessEdit()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.
	// 컨트롤의 값이 변경되었을 때 호출되는 함수입니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE); // 컨트롤의 값을 멤버 변수에 저장
	m_silderBrightness.SetPos(m_nBrightness); // 슬라이더 위치 업데이트
}

void CBrightnessContrastDlg::OnEnChangeContrastEdit()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE); // 컨트롤의 값을 멤버 변수에 저장
	m_sliderContrast.SetPos(m_nContrast); // 슬라이더 위치 업데이트
}
