/*----------------------------------------------------------
    �ݒ胁�C���N���X
                                    2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "OptMainDialog.h"
#include "Charu3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCharu3App theApp;

//---------------------------------------------------
//�֐���	COptMainDialog
//�@�\		�R���X�g���N�^
//---------------------------------------------------
COptMainDialog::COptMainDialog(CWnd* pParent /*=NULL*/, int nPage)
    : CDialog(COptMainDialog::IDD, pParent)
    , m_OptionPage{ &m_EtcPage, &m_VisualPage, &m_PopupPage, &m_FifoPage, &m_KeysetPage, &m_AdvancedPage }
    , m_nPage(nPage)
{
}

//---------------------------------------------------
//�֐���	DoDataExchange(CDataExchange* pDX)
//�@�\		�f�[�^�G�N�X�`�F���W
//---------------------------------------------------
void COptMainDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(COptMainDialog)
    if (GetDlgItem(IDC_OPT_TAB))
        DDX_Control(pDX, IDC_OPT_TAB, m_ctrlTab);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COptMainDialog, CDialog)
    //{{AFX_MSG_MAP(COptMainDialog)
    ON_NOTIFY(TCN_SELCHANGE, IDC_OPT_TAB, OnSelchangeOptTab)
    ON_WM_SHOWWINDOW()
    ON_NOTIFY(TCN_KEYDOWN, IDC_OPT_TAB, OnKeydownOptTab)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------
// COptMainDialog ���b�Z�[�W �n���h��
//---------------------------------------------------

//---------------------------------------------------
//�֐���	OnInitDialog()
//�@�\		������
//---------------------------------------------------
BOOL COptMainDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    for (int i = 0; i <= MAX_OPT_PAGE; i++) {
        CString strTabText;
        (void)strTabText.LoadString(APP_INF_TABNAME_01 + i);
        TC_ITEM item = {};
        item.mask = TCIF_TEXT;
        item.pszText = (TCHAR*)LPCTSTR(strTabText);
        m_ctrlTab.InsertItem(i, &item);
    }

    CRect rect;
    m_ctrlTab.GetWindowRect(&rect);
    m_ctrlTab.AdjustRect(FALSE, &rect);
    ScreenToClient(&rect);

    const int nDialogID[] = { IDD_SETTINGS_01_GENERAL, IDD_SETTINGS_02_STYLE, IDD_SETTINGS_03_DATATREE, IDD_SETTINGS_04_STOCKMODE, IDD_SETTINGS_05_KEYS, IDD_SETTINGS_06_ADVANCED };
    for (int i = 0; i <= MAX_OPT_PAGE; i++) {
        m_OptionPage[i]->Create(nDialogID[i], this);
        m_OptionPage[i]->MoveWindow(&rect);
        m_OptionPage[i]->ShowWindow(SW_HIDE);
    }
    m_nPage = theApp.m_ini.m_nOptionPage;
    m_OptionPage[m_nPage]->ShowWindow(SW_SHOW);
    m_ctrlTab.SetCurFocus(m_nPage);

    return FALSE; // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
                  // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

//---------------------------------------------------
//�֐���	OnSelchangeOptTab(NMHDR* pNMHDR, LRESULT* pResult)
//�@�\		�^�u�؂�ւ�����
//---------------------------------------------------
void COptMainDialog::OnSelchangeOptTab(NMHDR* pNMHDR, LRESULT* pResult)
{
    for (int i = 0; i <= MAX_OPT_PAGE; i++)
        m_OptionPage[i]->ShowWindow(SW_HIDE);

    m_nPage = m_ctrlTab.GetCurSel();
    m_OptionPage[m_nPage]->ShowWindow(SW_SHOW);

    *pResult = 0;
}

//---------------------------------------------------
//�֐���	DestroyWindow()
//�@�\		�E�B���h�E�j������
//---------------------------------------------------
BOOL COptMainDialog::DestroyWindow()
{
    if (theApp.m_pTreeDlg->IsWindowVisible()) {
        theApp.m_pTreeDlg->RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
    }

    for (int i = 0; i <= MAX_OPT_PAGE; i++) {
        m_OptionPage[i]->UpdateData();
        m_OptionPage[i]->DestroyWindow();
    }
    theApp.m_ini.m_nOptionPage = m_nPage;

    return CDialog::DestroyWindow();
}

void COptMainDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

//---------------------------------------------------
// ���b�Z�[�W�O����
//---------------------------------------------------
BOOL COptMainDialog::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN) {
        if (pMsg->wParam == VK_PRIOR) {
            if (m_nPage < MAX_OPT_PAGE) m_nPage++;
            else			m_nPage = 0;
            m_ctrlTab.SetCurFocus(m_nPage);
        }
        else if (pMsg->wParam == VK_NEXT) {
            if (m_nPage > 0) m_nPage--;
            else			m_nPage = MAX_OPT_PAGE;
            m_ctrlTab.SetCurFocus(m_nPage);
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void COptMainDialog::OnKeydownOptTab(NMHDR* pNMHDR, LRESULT* pResult)
{
    TC_KEYDOWN* pTCKeyDown = (TC_KEYDOWN*)pNMHDR;
    if (pTCKeyDown->wVKey == VK_TAB && ::GetKeyState(VK_CONTROL) < 0) {
        if (m_nPage < 4) m_nPage++;
        else			m_nPage = 0;
        m_ctrlTab.SetCurFocus(m_nPage);
    }
    *pResult = 0;
}
