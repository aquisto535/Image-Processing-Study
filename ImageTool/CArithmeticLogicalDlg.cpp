// CArithmeticLogicalDlg.cpp: 구현 파일
//

#include "pch.h"
#include "ImageTool.h"
#include "afxdialogex.h"
#include "CArithmeticLogicalDlg.h"
#include "ImageToolDoc.h"


// CArithmeticLogicalDlg 대화 상자

IMPLEMENT_DYNAMIC(CArithmeticLogicalDlg, CDialogEx)

CArithmeticLogicalDlg::CArithmeticLogicalDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ARITHMETIC_LOGICAL, pParent)
	, m_nFunction(0), m_pDoc1(NULL), m_pDoc2(NULL)
{

}

CArithmeticLogicalDlg::~CArithmeticLogicalDlg()
{
}

void CArithmeticLogicalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_IMAGE1, m_comboImage1);
	DDX_Control(pDX, IDC_COMBO_IMAGE2, m_comboImage2);
	DDX_Radio(pDX, IDC_FUNCTION1, m_nFunction);
}


BEGIN_MESSAGE_MAP(CArithmeticLogicalDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CArithmeticLogicalDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CArithmeticLogicalDlg 메시지 처리기

BOOL CArithmeticLogicalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	int nIndex = 0;
	CString strTitle = _T("");

	CImageToolApp* pApp = (CImageToolApp*)AfxGetApp();
	POSITION pos = pApp->m_pImageDocTemplate->GetFirstDocPosition();// 모든 문서 순회

	while (pos != NULL)
	{
		//Doc클래스에 담긴 이미지를 하나씩 순회
		CImageToolDoc* pDoc = (CImageToolDoc*)pApp->m_pImageDocTemplate->GetNextDoc(pos);

		// 8비트 그레이스케일 영상만 선택 가능
		if (pDoc->m_Dib.GetBitCount() != 8)
		{
			continue;
		}

		strTitle = pDoc->GetTitle();

		//콤보박스에 이미지 정보 삽입
		m_comboImage1.InsertString(nIndex, strTitle);
		m_comboImage2.InsertString(nIndex, strTitle);

		// 각 콤보 박스 항목에 해당하는 문서 포인터 저장
		m_comboImage1.SetItemDataPtr(nIndex, (void*)pDoc);
		m_comboImage2.SetItemDataPtr(nIndex, (void*)pDoc);

		nIndex++;
	}

	m_comboImage1.SetCurSel(0);
	m_comboImage2.SetCurSel(0);
	
	if (nIndex > 1)
	{
		m_comboImage2.SetCurSel(1);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CArithmeticLogicalDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//콤보박스에 삽입된 이미지 정보를 지닌 Doc클래스 복사
	m_pDoc1 = m_comboImage1.GetItemDataPtr(m_comboImage1.GetCurSel());
	m_pDoc2 = m_comboImage2.GetItemDataPtr(m_comboImage2.GetCurSel()); 

	CDialogEx::OnOK();
}
