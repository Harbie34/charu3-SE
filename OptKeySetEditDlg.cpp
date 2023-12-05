// OptKeySetEditDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "OptKeySetEditDlg.h"
#include "key.h"
#include "resource.h"

namespace {

    //---------------------------------------------------
    //�֐���	EnumWindowTitle
    //�@�\		�E�B���h�E�̖��O���
    //---------------------------------------------------
    BOOL CALLBACK EnumWindowTitle(HWND hwnd, LPARAM lParam)
    {
        CComboBox* combo = (CComboBox*)lParam;
        if (combo) {
            TCHAR WindowName[1024] = {};
            *WindowName = (char)NULL;
            if (IsWindowVisible(hwnd)) {
                int resalt = GetWindowText(hwnd, WindowName, _countof(WindowName));
                if (resalt && _tcsclen(WindowName) > 0) {
                    combo->AddString(WindowName);
                }
            }
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

} // anonymous namespace

/////////////////////////////////////////////////////////////////////////////
// COptKeySetEditDlg �_�C�A���O

COptKeySetEditDlg::COptKeySetEditDlg(CWnd* pParent /*=NULL*/)
    : CDialog(COptKeySetEditDlg::IDD, pParent)
    , m_pasteMessage(_T(""))
    , m_copyMessage(_T(""))
    , m_keyAction(-1)
    , m_matchCombo(-1)
    , m_caption(_T(""))
    , m_copyWait(0)
    , m_pasteWait(0)
    , m_nHistoryLimit(-1)
{
}

void COptKeySetEditDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(COptKeySetEditDlg)
    //}}AFX_DATA_MAP
    if (GetDlgItem(IDC_OPT_WINCAP_COMBO)) {
        DDX_CBString(pDX, IDC_OPT_WINCAP_COMBO, m_caption);
    }
    if (GetDlgItem(IDC_OPT_MATCH_COMBO)) {
        DDX_CBIndex(pDX, IDC_OPT_MATCH_COMBO, m_matchCombo);
    }
    if (GetDlgItem(IDC_OPT_COPYKEY2)) {
        DDX_Control(pDX, IDC_OPT_COPYKEY2, m_copyKey);
    }
    if (GetDlgItem(IDC_OPT_PASTKEY2)) {
        DDX_Control(pDX, IDC_OPT_PASTKEY2, m_pasteKey);
    }
    if (GetDlgItem(IDC_OPT_COPY_WAIT2)) {
        DDX_Text(pDX, IDC_OPT_COPY_WAIT2, m_copyWait);
        // Not worked correctly
        //DDV_MinMaxInt(pDX, m_copyWait, 0, 1000);
    }
    if (GetDlgItem(IDC_OPT_PASTE_WAIT2)) {
        DDX_Text(pDX, IDC_OPT_PASTE_WAIT2, m_pasteWait);
        // Not worked correctly
        //DDV_MinMaxInt(pDX, m_pasteWait, 0, 1000);
    }
    if (GetDlgItem(IDC_OPT_HISTORY_SIZE_LIMIT)) {
        DDX_Text(pDX, IDC_OPT_HISTORY_SIZE_LIMIT, m_nHistoryLimit);
    }

    if (!pDX->m_bSaveAndValidate) {
        UINT uMod = m_key.m_sCopyPasteKey.m_uMod_Paste;
        if (m_key.m_sCopyPasteKey.m_uVK_Paste >= 0x60)
            m_pasteKey.SetHotKey(m_key.m_sCopyPasteKey.m_uVK_Paste, KeyHelper::ModToHotkey(uMod));
        else
            m_pasteKey.SetHotKey(m_key.m_sCopyPasteKey.m_uVK_Paste, KeyHelper::ModToHotkey(uMod) | HOTKEYF_EXT);

        uMod = m_key.m_sCopyPasteKey.m_uMod_Copy;
        if (m_key.m_sCopyPasteKey.m_uVK_Copy >= 0x60)
            m_copyKey.SetHotKey(m_key.m_sCopyPasteKey.m_uVK_Copy, KeyHelper::ModToHotkey(uMod));
        else
            m_copyKey.SetHotKey(m_key.m_sCopyPasteKey.m_uVK_Copy, KeyHelper::ModToHotkey(uMod) | HOTKEYF_EXT);
    }
    else {
        CHANGE_KEY key;
        WORD wMod, wVkCode;

        key.m_nHistoryLimit = m_nHistoryLimit;
        //�y�[�X�g�L�[�ϊ�
        m_pasteKey.GetHotKey(wVkCode, wMod);
        key.m_sCopyPasteKey.m_uMod_Paste = KeyHelper::HotkeyToMod(wMod);
        key.m_sCopyPasteKey.m_uVK_Paste = wVkCode;

        //�R�s�[�L�[�ϊ�
        m_copyKey.GetHotKey(wVkCode, wMod);
        key.m_sCopyPasteKey.m_uMod_Copy = KeyHelper::HotkeyToMod(wMod);
        key.m_sCopyPasteKey.m_uVK_Copy = wVkCode;

        _stscanf_s(m_pasteMessage, _T("%x,%x,%x"), &key.m_sCopyPasteKey.m_pasteMessage.Msg,
            &key.m_sCopyPasteKey.m_pasteMessage.wParam,
            &key.m_sCopyPasteKey.m_pasteMessage.lParam);

        _stscanf_s(m_copyMessage, _T("%x,%x,%x"), &key.m_sCopyPasteKey.m_copyMessage.Msg,
            &key.m_sCopyPasteKey.m_copyMessage.wParam,
            &key.m_sCopyPasteKey.m_copyMessage.lParam);

        key.m_sCopyPasteKey.m_nMessage = m_keyAction;
        key.m_nMatch = m_matchCombo;
        key.m_strTitle = m_caption;
        key.m_sCopyPasteKey.m_nCopyWait = m_copyWait;
        key.m_sCopyPasteKey.m_nPasteWait = m_pasteWait;

        m_key = key;
    }
}

BEGIN_MESSAGE_MAP(COptKeySetEditDlg, CDialog)
    //{{AFX_MSG_MAP(COptKeySetEditDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptKeySetEditDlg ���b�Z�[�W �n���h��

BOOL COptKeySetEditDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    ::EnumWindows(&EnumWindowTitle, (LPARAM)GetDlgItem(IDC_OPT_WINCAP_COMBO));
    return TRUE;
}
