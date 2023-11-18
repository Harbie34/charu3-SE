/*----------------------------------------------------------
    �r�W���A���ݒ�N���X
                                    2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <fstream>
#include "nlomann/json.hpp"

#include "OptVisual.h"
#include "Charu3.h"
#include "General.h"
#include "Color.h"
#include "resource.h"

//---------------------------------------------------
//�֐���	COptVisual
//�@�\		�R���X�g���N�^
//---------------------------------------------------
COptVisual::COptVisual(CWnd* pParent /*=NULL*/)
    : CDialog(COptVisual::IDD, pParent)
{
    //{{AFX_DATA_INIT(COptVisual)
    //}}AFX_DATA_INIT
    m_strBorderColor.Format(_T("%.6x"), theApp.m_ini.m_visual.m_nBorderColor);
    m_ctrlBorderPal.setColor((COLORREF)CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBorderColor));
    m_strBackgroundColor.Format(_T("%.6x"), theApp.m_ini.m_visual.m_nBackgroundColor);
    m_ctrlBackgroundPal.setColor((COLORREF)CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBackgroundColor));
    m_strTextColor.Format(_T("%.6x"), theApp.m_ini.m_visual.m_nTextColor);
    m_ctrlTextPal.setColor((COLORREF)CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nTextColor));
}

//---------------------------------------------------
//�֐���	DoDataExchange(CDataExchange* pDX)
//�@�\		�f�[�^�G�N�X�`�F���W
//---------------------------------------------------
void COptVisual::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    //{{AFX_DATA_MAP(COptVisual)
    if (GetDlgItem(IDC_OPT_FONT_NAME))
        DDX_Control(pDX, IDC_OPT_FONT_NAME, m_ctrlFontCombo);
    if (GetDlgItem(IDC_OPT_FONT_SIZE))
        DDX_Text(pDX, IDC_OPT_FONT_SIZE, theApp.m_ini.m_visual.m_nFontSize);
    if (GetDlgItem(IDC_OPT_ICON_FILE_NAME))
        DDX_Text(pDX, IDC_OPT_ICON_FILE_NAME, theApp.m_ini.m_visual.m_strResourceName);
    if (GetDlgItem(IDC_OPT_BORDER_COLOR))
        DDX_Text(pDX, IDC_OPT_BORDER_COLOR, m_strBorderColor);
    if (GetDlgItem(IDC_OPT_BORDER_COLOR_PAL))
        DDX_Control(pDX, IDC_OPT_BORDER_COLOR_PAL, m_ctrlBorderPal);
    if (GetDlgItem(IDC_OPT_BACKGROUND_COLOR))
        DDX_Text(pDX, IDC_OPT_BACKGROUND_COLOR, m_strBackgroundColor);
    if (GetDlgItem(IDC_OPT_BACKGROUND_COLOR_PAL))
        DDX_Control(pDX, IDC_OPT_BACKGROUND_COLOR_PAL, m_ctrlBackgroundPal);
    if (GetDlgItem(IDC_OPT_TEXT_COLOR))
        DDX_Text(pDX, IDC_OPT_TEXT_COLOR, m_strTextColor);
    if (GetDlgItem(IDC_OPT_TEXT_COLOR_PAL))
        DDX_Control(pDX, IDC_OPT_TEXT_COLOR_PAL, m_ctrlTextPal);

    if (GetDlgItem(IDC_OPT_TOUMEI_SLIDER))
        DDX_Slider(pDX, IDC_OPT_TOUMEI_SLIDER, theApp.m_ini.m_visual.m_nOpacity);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COptVisual, CDialog)
    //{{AFX_MSG_MAP(COptVisual)
    ON_BN_CLICKED(IDC_OPT_VS_BROWS, OnOptVsBrows)
    ON_EN_CHANGE(IDC_OPT_BORDER_COLOR, OnChangeOptBorderColor)
    ON_EN_CHANGE(IDC_OPT_BACKGROUND_COLOR, OnChangeOptBackColor)
    ON_EN_CHANGE(IDC_OPT_TEXT_COLOR, OnChangeOptTextColor)
    ON_BN_CLICKED(IDC_OPT_BORDER_COLOR_PAL, OnOptBorderColorPal)
    ON_BN_CLICKED(IDC_OPT_BACKGROUND_COLOR_PAL, OnOptBackgroundColorPal)
    ON_BN_CLICKED(IDC_OPT_TEXT_COLOR_PAL, OnOptTextColorPal)
    ON_BN_CLICKED(IDC_OPT_LOAD_VISUAL, OnBnClickedOptLoadStyle)
    ON_WM_SHOWWINDOW()
    ON_WM_HSCROLL()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------
// �t�H���g�񋓃R�[���o�b�N�֐�
//---------------------------------------------------
int CALLBACK EnumFontProc(ENUMLOGFONT* lpelf, NEWTEXTMETRIC* lpntm, int FontType, LPARAM lparam)
{
    (void)lpntm;
    (void)FontType;
    COptVisual* ThisClass = (COptVisual*)lparam;

    ThisClass->m_ctrlFontCombo.AddString(lpelf->elfLogFont.lfFaceName);
    if (_tcscmp(lpelf->elfLogFont.lfFaceName, LPCTSTR(theApp.m_ini.m_visual.m_strFontName)) == 0)
        ThisClass->m_ctrlFontCombo.SetCurSel(ThisClass->m_ctrlFontCombo.GetCount() - 1);

    return TRUE; // �񋓌p��
}

//---------------------------------------------------
// COptVisual ���b�Z�[�W �n���h��
//---------------------------------------------------

//---------------------------------------------------
//�֐���	PreTranslateMessage(MSG* pMsg)
//�@�\		���b�Z�[�W�O����
//---------------------------------------------------
BOOL COptVisual::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB && ::GetKeyState(VK_CONTROL) < 0) {
        ::PostMessage(::GetParent(this->m_hWnd), pMsg->message, VK_PRIOR, pMsg->lParam);
    }

    return CDialog::PreTranslateMessage(pMsg);
}

//---------------------------------------------------
//�֐���	OnInitDialog()
//�@�\		������
//---------------------------------------------------
BOOL COptVisual::OnInitDialog()
{
    CDialog::OnInitDialog();

    //�t�H���g�����
    m_ctrlFontCombo.ResetContent();
    HDC hDC;
    hDC = ::GetDC(NULL);
    EnumFontFamilies(hDC, NULL, (FONTENUMPROC)EnumFontProc, (LPARAM)this);
    ::ReleaseDC(NULL, hDC);

    SetOpacityText(theApp.m_ini.m_visual.m_nOpacity);
    return TRUE;
}

void COptVisual::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);
    if (bShow) {
        CWnd* w = GetDlgItem(IDC_OPT_FONT_NAME);
        if (w) w->SetFocus();
    }
}

void COptVisual::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    void* pOpacitySlider = GetDlgItem(IDC_OPT_TOUMEI_SLIDER);
    if (pScrollBar == pOpacitySlider) {
        SetOpacityText(static_cast<CSliderCtrl*>(pOpacitySlider)->GetPos());
    }
}

//---------------------------------------------------
//�֐���	DestroyWindow()
//�@�\		�I������
//---------------------------------------------------
BOOL COptVisual::DestroyWindow()
{
    CString strBuff;
    CEdit* edit;
    edit = (CEdit*)GetDlgItem(IDC_OPT_BACKGROUND_COLOR);
    edit->GetWindowText(strBuff);
    _stscanf_s(strBuff, _T("%x"), &theApp.m_ini.m_visual.m_nBackgroundColor);

    edit = (CEdit*)GetDlgItem(IDC_OPT_TEXT_COLOR);
    edit->GetWindowText(strBuff);
    _stscanf_s(strBuff, _T("%x"), &theApp.m_ini.m_visual.m_nTextColor);

    edit = (CEdit*)GetDlgItem(IDC_OPT_BORDER_COLOR);
    edit->GetWindowText(strBuff);
    _stscanf_s(strBuff, _T("%x"), &theApp.m_ini.m_visual.m_nBorderColor);

    int nCursel = m_ctrlFontCombo.GetCurSel();
    if (nCursel == CB_ERR) nCursel = 0;
    m_ctrlFontCombo.GetLBText(nCursel, strBuff);
    theApp.m_ini.m_visual.m_strFontName = strBuff;

    return CDialog::DestroyWindow();
}

//---------------------------------------------------
//�֐���	OnOptVsBrows()
//�@�\		�Q�ƃ{�^��
//---------------------------------------------------
void COptVisual::OnOptVsBrows()
{
    CString strRes;
    (void)strRes.LoadString(APP_INF_FILE_FILTER5);
    CFileDialog openFileDialog(TRUE, _T("bmp"), NULL, OFN_FILEMUSTEXIST, strRes);

    openFileDialog.m_ofn.lpstrInitialDir = theApp.m_ini.m_strAppPath;
    if (openFileDialog.DoModal() == IDOK)
        SetDlgItemText(IDC_OPT_ICON_FILE_NAME, openFileDialog.GetPathName());
}

//---------------------------------------------------
//�֐���	OnChangeOptBorderColor()
//�@�\		�g�F�ύX���Ƀp���b�g�`�F���W
//---------------------------------------------------
void COptVisual::OnChangeOptBorderColor()
{
    setTextToPalet((CEdit*)GetDlgItem(IDC_OPT_BORDER_COLOR), &m_ctrlBorderPal);
}

//---------------------------------------------------
//�֐���	OnChangeOptBackColor()
//�@�\		�w�i�F�ύX���Ƀp���b�g�`�F���W
//---------------------------------------------------
void COptVisual::OnChangeOptBackColor()
{
    setTextToPalet((CEdit*)GetDlgItem(IDC_OPT_BACKGROUND_COLOR), &m_ctrlBackgroundPal);
}

//---------------------------------------------------
//�֐���	OnChangeOptTextColor()
//�@�\		�e�L�X�g�F�ύX���Ƀp���b�g�`�F���W
//---------------------------------------------------
void COptVisual::OnChangeOptTextColor()
{
    setTextToPalet((CEdit*)GetDlgItem(IDC_OPT_TEXT_COLOR), &m_ctrlTextPal);
}

//---------------------------------------------------
//�֐���	OnOptBorderColorPal()
//�@�\		�g�F�p���b�g
//---------------------------------------------------
void COptVisual::OnOptBorderColorPal()
{
    setPalet((CEdit*)GetDlgItem(IDC_OPT_BORDER_COLOR), &m_ctrlBorderPal);
}

//---------------------------------------------------
//�֐���	OnOptBackColorPal()
//�@�\		�w�i�F�p���b�g
//---------------------------------------------------
void COptVisual::OnOptBackgroundColorPal()
{
    setPalet((CEdit*)GetDlgItem(IDC_OPT_BACKGROUND_COLOR), &m_ctrlBackgroundPal);
}

//---------------------------------------------------
//�֐���	OnOptTextColorPal()
//�@�\		�e�L�X�g�F�p���b�g
//---------------------------------------------------
void COptVisual::OnOptTextColorPal()
{
    setPalet((CEdit*)GetDlgItem(IDC_OPT_TEXT_COLOR), &m_ctrlTextPal);
}

void COptVisual::OnBnClickedOptLoadStyle()
{
    ReadStyleFile();
    OnInitDialog();
    InvalidateRect(NULL, false);
}

//---------------------------------------------------
//�֐���	setTextToPalet(CEdit *edit,CPaletStatic *stPal)
//�@�\		�e�L�X�g�{�b�N�X�̓��e�ɂ��������ăp���b�g�F��ύX
//---------------------------------------------------
void COptVisual::setTextToPalet(CEdit* edit, CPaletStatic* stPal)
{
    CString strBuff;
    int nColor;
    if (!edit || !stPal) return;

    edit->GetWindowText(strBuff);
    _stscanf_s(strBuff, _T("%x"), &nColor);

    stPal->setColor((COLORREF)CGeneral::RGB2BGR(nColor));
    stPal->Invalidate();
}

//---------------------------------------------------
//�֐���	setPalet(CEdit *edit,CPaletStatic *stPal)
//�@�\		�F�Z�b�g
//---------------------------------------------------
void COptVisual::setPalet(CEdit* edit, CPaletStatic* stPal)
{
    CColorDialog dlgColor;

    if (dlgColor.DoModal() == IDOK) {
        CString strBuff;
        COLORREF col;
        int nColor;
        col = dlgColor.GetColor();
        stPal->setColor(col);//�p���b�g�ɐF��ݒ�
        nColor = CGeneral::BGR2RGB((int)col);//�ϊ�
        strBuff.Format(_T("%06x"), nColor);//�e�L�X�g�ɂ���
        //�G�f�B�b�g�{�b�N�X�ɐݒ�
        edit->SetWindowText(strBuff);
        stPal->Invalidate();
    }
}

//---------------------------------------------------
//�֐���	ReadStyleFile()
//�@�\		�X�^�C���ݒ��ǂݍ���
//---------------------------------------------------
void COptVisual::ReadStyleFile()
{
    CString strFileName;
    CFileDialog* pFileDialog;
    CString strRes;
    (void)strRes.LoadString(APP_INF_FILE_FILTER_VISUAL_PREF);
    pFileDialog = new CFileDialog(TRUE, _T("json"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strRes, NULL);

    if (pFileDialog) {
        CString prevIconResourceName = theApp.m_ini.m_visual.m_strResourceName;
        pFileDialog->m_ofn.lpstrInitialDir = theApp.m_ini.m_strAppPath;
        if (IDOK == pFileDialog->DoModal()) {
            if (theApp.m_ini.m_bDebug) {
                CString strText;
                strText.Format(_T("OnVisualFile \"%s\"\n"), pFileDialog->GetPathName().GetString());
                CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
            }

            strFileName = pFileDialog->GetPathName();
            nlohmann::json j;
            try { j = nlohmann::json::parse(std::ifstream(strFileName)); }
            catch (...) {}
            theApp.m_ini.m_visual.m_nBorderColor = Color::Parse(CGeneral::getSettingString(j, "borderColor", Color::String(theApp.m_ini.m_visual.m_nBorderColor)));
            theApp.m_ini.m_visual.m_nBackgroundColor = Color::Parse(CGeneral::getSettingString(j, "backgroundColor", Color::String(theApp.m_ini.m_visual.m_nBackgroundColor)));
            theApp.m_ini.m_visual.m_nTextColor = Color::Parse(CGeneral::getSettingString(j, "textColor", Color::String(theApp.m_ini.m_visual.m_nTextColor)));
            //theApp.m_ini.m_visual.m_strFontName = CGeneral::getSettingCString(j, "fontName", theApp.m_ini.m_visual.m_strFontName);
            //theApp.m_ini.m_visual.m_nFontSize = static_cast<int>(CGeneral::getSettingNumber(j, "fontSize", theApp.m_ini.m_visual.m_nFontSize));
            //theApp.m_ini.m_visual.m_strResourceName = CGeneral::getSettingCString(j, "iconFile", theApp.m_ini.m_visual.m_strResourceName);
        }
        delete pFileDialog;

        if (theApp.m_ini.m_visual.m_strResourceName != prevIconResourceName) {
            theApp.resetTreeDialog();
        }
    }
}

void COptVisual::SetOpacityText(int value)
{
    CString strBuff;
    strBuff.Format(_T("%u%%"), value);
    GetDlgItem(IDC_OPACITY_TEXT)->SetWindowText(strBuff);
}
