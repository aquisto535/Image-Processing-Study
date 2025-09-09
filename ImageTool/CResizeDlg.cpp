// CResizeDlg.cpp: 구현 파일
//

#include "pch.h"
#include "ImageTool.h"
#include "afxdialogex.h"
#include "CResizeDlg.h"


// CResizeDlg 대화 상자

IMPLEMENT_DYNAMIC(CResizeDlg, CDialogEx)

CResizeDlg::CResizeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RESIZE, pParent)
	, m_nOldWidth(0)
	, m_nOldHeight(0)
	, m_nNewWidth(0)
	, m_nHeight(0)
	, m_bAspectRatio(FALSE)
	, m_nInterpolation(0)
{

}

CResizeDlg::~CResizeDlg()
{
}

void CResizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_OLD_WIDTH, m_nOldWidth);
	DDX_Text(pDX, IDC_OLD_HEIGHT, m_nOldHeight);
	DDX_Text(pDX, IDC_NEW_WIDTH, m_nNewWidth);
	DDV_MinMaxInt(pDX, m_nNewWidth, 1, 4096);
	DDX_Text(pDX, IDC_NEW_HEIGHT, m_nHeight);
	DDV_MinMaxInt(pDX, m_nHeight, 1, 4096);
	DDX_Check(pDX, IDC_ASPECT_RATIO, m_bAspectRatio);
	DDX_CBIndex(pDX, IDC_INTERPORATION, m_nInterpolation);
}


BEGIN_MESSAGE_MAP(CResizeDlg, CDialogEx)
	ON_EN_CHANGE(IDC_NEW_WIDTH, &CResizeDlg::OnEnChangeNewWidth)
	ON_EN_CHANGE(IDC_NEW_HEIGHT, &CResizeDlg::OnEnChangeNewHeight)
	ON_BN_CLICKED(IDC_ASPECT_RATIO, &CResizeDlg::OnBnClickedAspectRatio)
END_MESSAGE_MAP()


// CResizeDlg 메시지 처리기

BOOL CResizeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WIDTH))->SetRange(1, 4096);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_HEIGHT))->SetRange(1, 4096);

	m_nNewWidth = m_nOldWidth;
	m_nHeight = m_nOldHeight;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CResizeDlg::OnEnChangeNewWidth()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	if (GetDlgItem(IDC_INTERPORATION) == NULL)
		return;

	if (m_bAspectRatio)
	{
		UpdateData(TRUE);
		m_nHeight = m_nNewWidth * m_nOldHeight / m_nOldWidth;
		UpdateData(FALSE);
	}
}

void CResizeDlg::OnEnChangeNewHeight()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	if (GetDlgItem(IDC_INTERPORATION) == NULL)
		return;

	if (m_bAspectRatio)
	{
		UpdateData(TRUE);
		m_nNewWidth = m_nHeight * m_nOldWidth / m_nOldHeight;
		UpdateData(FALSE);
	}
}

void CResizeDlg::OnBnClickedAspectRatio()
{
	UpdateData(TRUE);

	if (m_bAspectRatio)
	{
		m_nHeight = m_nNewWidth * m_nOldHeight / m_nOldWidth;
		UpdateData(FALSE);
	}
}
