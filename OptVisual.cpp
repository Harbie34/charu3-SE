/*----------------------------------------------------------
	�r�W���A���ݒ�N���X
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "Charu3.h"
#include "OptVisual.h"
#include "General.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCharu3App theApp;

//---------------------------------------------------
//�֐���	COptVisual
//�@�\		�R���X�g���N�^
//---------------------------------------------------
COptVisual::COptVisual(CWnd* pParent /*=NULL*/)
	: CDialog(COptVisual::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptVisual)
	m_strBackColor = _T("");
	m_strTextColor = _T("");
	m_strBorderColor = _T("");
	m_n = 0;
	//}}AFX_DATA_INIT
	m_nMaxFont = 0;
	m_nScrollH = 0;
	m_nScrollV = 0;

	if(theApp.m_ini.m_visual.m_nScrollbar == SB_VERT || theApp.m_ini.m_visual.m_nScrollbar == SB_BOTH)
		m_nScrollV = 1;
	if(theApp.m_ini.m_visual.m_nScrollbar == SB_HORZ || theApp.m_ini.m_visual.m_nScrollbar == SB_BOTH)
		m_nScrollH = 1;
}

//---------------------------------------------------
//�֐���	DoDataExchange(CDataExchange* pDX)
//�@�\		�f�[�^�G�N�X�`�F���W
//---------------------------------------------------
void COptVisual::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptVisual)
	if(GetDlgItem(IDC_OPT_BORDER_COLOR_PAL))
		DDX_Control(pDX, IDC_OPT_BORDER_COLOR_PAL, m_ctrlBorderPal);
	if(GetDlgItem(IDC_OPT_TEXT_COLOR_PAL))
		DDX_Control(pDX, IDC_OPT_TEXT_COLOR_PAL, m_ctrlTextPal);
	if(GetDlgItem(IDC_OPT__BACK_COLOR_PAL))
		DDX_Control(pDX, IDC_OPT__BACK_COLOR_PAL, m_ctrlBackPal);
	if(GetDlgItem(IDC_OPT_FONT_NAME))
		DDX_Control(pDX, IDC_OPT_FONT_NAME, m_ctrlFontCombo);
	if(GetDlgItem(IDC_OPT__BACK_COLOR))
		DDX_Text(pDX, IDC_OPT__BACK_COLOR, m_strBackColor);
	if(GetDlgItem(IDC_OPT_TEXT_COLOR))
		DDX_Text(pDX, IDC_OPT_TEXT_COLOR, m_strTextColor);
	if(GetDlgItem(IDC_OPT_BORDER_COLOR))
		DDX_Text(pDX, IDC_OPT_BORDER_COLOR, m_strBorderColor);
	//}}AFX_DATA_MAP
	if(GetDlgItem(IDC_OPT_ICON_FILE_NAME))
		DDX_Text(pDX, IDC_OPT_ICON_FILE_NAME, theApp.m_ini.m_visual.m_strResourceName);
	if(GetDlgItem(IDC_OPT_FONT_SIZE))
		DDX_Text(pDX, IDC_OPT_FONT_SIZE, theApp.m_ini.m_visual.m_nFontSize);
	if(GetDlgItem(IDC_OPT_TOUMEI_CHECK))
		DDX_Check(pDX,IDC_OPT_TOUMEI_CHECK,theApp.m_ini.m_visual.m_nTransparentSW);
	if(GetDlgItem(IDC_OPT_RESET_TREE))
		DDX_Check(pDX,IDC_OPT_RESET_TREE,theApp.m_ini.m_visual.m_nResetTree);
	if(GetDlgItem(IDC_OPT_TOUMEI_SLIDER))
		DDX_Slider(pDX,IDC_OPT_TOUMEI_SLIDER,theApp.m_ini.m_visual.m_nSemitransparent);
	if(GetDlgItem(IDC_OPT_SCROLBAR1))
		DDX_Check(pDX,IDC_OPT_SCROLBAR1,m_nScrollV);
	if(GetDlgItem(IDC_OPT_SCROLBAR2))
		DDX_Check(pDX,IDC_OPT_SCROLBAR2,m_nScrollH);
	if(GetDlgItem(IDC_OPT_TOOLTIP_01))
		DDX_Radio(pDX, IDC_OPT_TOOLTIP_01, theApp.m_ini.m_visual.m_nToolTip);
}


BEGIN_MESSAGE_MAP(COptVisual, CDialog)
	//{{AFX_MSG_MAP(COptVisual)
	ON_BN_CLICKED(IDC_OPT__BACK_COLOR_PAL, OnOptBackColorPal)
	ON_BN_CLICKED(IDC_OPT_BORDER_COLOR_PAL, OnOptBorderColorPal)
	ON_BN_CLICKED(IDC_OPT_TEXT_COLOR_PAL, OnOptTextColorPal)
	ON_BN_CLICKED(IDC_OPT_VS_BROWS, OnOptVsBrows)
	ON_EN_CHANGE(IDC_OPT_TEXT_COLOR, OnChangeOptTextColor)
	ON_EN_CHANGE(IDC_OPT_BORDER_COLOR, OnChangeOptBorderColor)
	ON_EN_CHANGE(IDC_OPT__BACK_COLOR, OnChangeOptBackColor)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_OPT_LOAD_VISUAL, &COptVisual::OnBnClickedOptLoadVisual)
END_MESSAGE_MAP()


//---------------------------------------------------
// �t�H���g�񋓃R�[���o�b�N�֐�
//---------------------------------------------------
int CALLBACK EnumFontProc(ENUMLOGFONT *lpelf,NEWTEXTMETRIC *lpntm,int FontType,LPARAM lparam)
{
	(void)lpntm;
	(void)FontType;
	COptVisual* ThisClass = (COptVisual*)lparam;

	ThisClass->m_ctrlFontCombo.AddString(lpelf->elfLogFont.lfFaceName);
	if(_tcscmp(lpelf->elfLogFont.lfFaceName,LPCTSTR(theApp.m_ini.m_visual.m_strFontName)) == 0)
		ThisClass->m_ctrlFontCombo.SetCurSel(ThisClass->m_nMaxFont);
	ThisClass->m_nMaxFont++;
	if(ThisClass->m_nMaxFont >= MAX_FONT) 	return FALSE; // �񋓒��~

	return TRUE; // �񋓌p��
}

//---------------------------------------------------
// COptVisual ���b�Z�[�W �n���h��
//---------------------------------------------------

//---------------------------------------------------
//�֐���	OnInitDialog()
//�@�\		������
//---------------------------------------------------
BOOL COptVisual::OnInitDialog() 
{
	m_strBackColor.Format(_T("%06x"),theApp.m_ini.m_visual.m_nBackColor);
	m_strBorderColor.Format(_T("%06x"),theApp.m_ini.m_visual.m_nBorderColor);
	m_strTextColor.Format(_T("%06x"),theApp.m_ini.m_visual.m_nTextColor);
	m_ctrlBackPal.setColor((COLORREF)CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBackColor));
	m_ctrlBorderPal.setColor((COLORREF)CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBorderColor));
	m_ctrlTextPal.setColor((COLORREF)CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nTextColor));

	CDialog::OnInitDialog();

	//�t�H���g�����
	HDC hDC;
	hDC = ::GetDC(NULL);
	EnumFontFamilies(hDC,NULL,(FONTENUMPROC)EnumFontProc,(LPARAM)this);
	::ReleaseDC(NULL,hDC);
	
	return TRUE;
}

//---------------------------------------------------
//�֐���	DestroyWindow()
//�@�\		�I������
//---------------------------------------------------
BOOL COptVisual::DestroyWindow() 
{
	CString strBuff;
	CEdit *edit;
	edit = (CEdit*)GetDlgItem(IDC_OPT__BACK_COLOR);
	edit->GetWindowText(strBuff);
	_stscanf_s(strBuff,_T("%x"),&theApp.m_ini.m_visual.m_nBackColor);
	
	edit = (CEdit*)GetDlgItem(IDC_OPT_TEXT_COLOR);
	edit->GetWindowText(strBuff);
	_stscanf_s(strBuff,_T("%x"),&theApp.m_ini.m_visual.m_nTextColor);

	edit = (CEdit*)GetDlgItem(IDC_OPT_BORDER_COLOR);
	edit->GetWindowText(strBuff);
	_stscanf_s(strBuff,_T("%x"),&theApp.m_ini.m_visual.m_nBorderColor);

	int nCursel = m_ctrlFontCombo.GetCurSel();
	if(nCursel == CB_ERR) nCursel = 0; 
	m_ctrlFontCombo.GetLBText(nCursel,strBuff);
	theApp.m_ini.m_visual.m_strFontName = strBuff;


	CButton* pScroll1 = (CButton*)GetDlgItem(IDC_OPT_SCROLBAR1);
	CButton* pScroll2 = (CButton*)GetDlgItem(IDC_OPT_SCROLBAR2);
	if(pScroll1 && pScroll2) {
		if(pScroll1->GetCheck() && pScroll2->GetCheck()) theApp.m_ini.m_visual.m_nScrollbar = SB_BOTH;
		else if(pScroll1->GetCheck()) theApp.m_ini.m_visual.m_nScrollbar = SB_VERT;
		else if(pScroll2->GetCheck()) theApp.m_ini.m_visual.m_nScrollbar = SB_HORZ;
		else theApp.m_ini.m_visual.m_nScrollbar = SB_CTL;
	}

	return CDialog::DestroyWindow();
}

//---------------------------------------------------
//�֐���	OnOptBackColorPal()
//�@�\		�w�i�F�p���b�g
//---------------------------------------------------
void COptVisual::OnOptBackColorPal() 
{
	setPalet((CEdit*)GetDlgItem(IDC_OPT__BACK_COLOR),&m_ctrlBackPal);
}

//---------------------------------------------------
//�֐���	OnOptBorderColorPal()
//�@�\		�g�F�p���b�g
//---------------------------------------------------
void COptVisual::OnOptBorderColorPal() 
{
	setPalet((CEdit*)GetDlgItem(IDC_OPT_BORDER_COLOR),&m_ctrlBorderPal);
}

//---------------------------------------------------
//�֐���	OnOptTextColorPal()
//�@�\		�e�L�X�g�F�p���b�g
//---------------------------------------------------
void COptVisual::OnOptTextColorPal() 
{
	setPalet((CEdit*)GetDlgItem(IDC_OPT_TEXT_COLOR),&m_ctrlTextPal);
}

//---------------------------------------------------
//�֐���	setPalet(CEdit *edit,CPaletStatic *stPal)
//�@�\		�F�Z�b�g
//---------------------------------------------------
void COptVisual::setPalet(CEdit *edit,CPaletStatic *stPal)
{
	CColorDialog dlgColor;

	if(dlgColor.DoModal() == IDOK) {
		CString strBuff;
		COLORREF col;
		int nColor;
		col= dlgColor.GetColor();
		stPal->setColor(col);//�p���b�g�ɐF��ݒ�
		nColor = CGeneral::BGR2RGB((int)col);//�ϊ�
		strBuff.Format(_T("%06x"),nColor);//�e�L�X�g�ɂ���
		//�G�f�B�b�g�{�b�N�X�ɐݒ�
		edit->SetWindowText(strBuff);
		stPal->Invalidate();
	}	
}

//---------------------------------------------------
//�֐���	OnOptVsBrows()
//�@�\		�Q�ƃ{�^��
//---------------------------------------------------
void COptVisual::OnOptVsBrows() 
{
	CString strRes;
	strRes.LoadString(APP_INF_FILE_FILTER5);
	CFileDialog openFileDialog(TRUE,_T("bmp"),NULL,OFN_FILEMUSTEXIST,strRes);

	openFileDialog.m_ofn.lpstrInitialDir = theApp.m_ini.m_strAppPath;
	if(openFileDialog.DoModal() == IDOK)
		SetDlgItemText(IDC_OPT_ICON_FILE_NAME,openFileDialog.GetPathName());		
}

//---------------------------------------------------
//�֐���	OnChangeOptTextColor() 
//�@�\		�e�L�X�g�F�ύX���Ƀp���b�g�`�F���W
//---------------------------------------------------
void COptVisual::OnChangeOptTextColor() 
{
	setTextToPalet((CEdit*)GetDlgItem(IDC_OPT_TEXT_COLOR),&m_ctrlTextPal);
}

//---------------------------------------------------
//�֐���	OnChangeOptBorderColor() 
//�@�\		�g�F�ύX���Ƀp���b�g�`�F���W
//---------------------------------------------------
void COptVisual::OnChangeOptBorderColor() 
{
	setTextToPalet((CEdit*)GetDlgItem(IDC_OPT_BORDER_COLOR),&m_ctrlBorderPal);
}

//---------------------------------------------------
//�֐���	OnChangeOptBackColor() 
//�@�\		�w�i�F�ύX���Ƀp���b�g�`�F���W
//---------------------------------------------------
void COptVisual::OnChangeOptBackColor() 
{
	setTextToPalet((CEdit*)GetDlgItem(IDC_OPT__BACK_COLOR),&m_ctrlBackPal);
}

//---------------------------------------------------
//�֐���	setTextToPalet(CEdit *edit,CPaletStatic *stPal)
//�@�\		�e�L�X�g�{�b�N�X�̓��e�ɂ��������ăp���b�g�F��ύX
//---------------------------------------------------
void COptVisual::setTextToPalet(CEdit *edit,CPaletStatic *stPal)
{
	CString strBuff;
	int nColor;
	if(!edit || !stPal) return;
	
	edit->GetWindowText(strBuff);
	_stscanf_s(strBuff,_T("%x"),&nColor);

	stPal->setColor((COLORREF)CGeneral::RGB2BGR(nColor));
	stPal->Invalidate();
}


BOOL COptVisual::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB && ::GetKeyState(VK_CONTROL) < 0){
		::PostMessage(::GetParent(this->m_hWnd),pMsg->message,VK_PRIOR,pMsg->lParam);
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void COptVisual::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	if(bShow)	GetDlgItem(IDC_OPT_TOUMEI_CHECK)->SetFocus();	
}


void COptVisual::OnBnClickedOptLoadVisual()
{
	theApp.OnVisualFile();
	OnInitDialog();
	InvalidateRect(NULL, false);
}
