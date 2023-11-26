/*----------------------------------------------------------
    MainFrm�N���X
                                    2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <locale.h>

#include "MainFrm.h"
#include "Charu3.h"
#include "window.h"
#include "clipboard.h"
#include "log.h"
#include "resource.h"

//---------------------------------------------------
// CMainFrame
//---------------------------------------------------

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_WM_SETFOCUS()
    ON_COMMAND(IDM_EXIT, OnExit)
    ON_WM_TIMER()
    ON_WM_QUERYENDSESSION()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//---------------------------------------------------
//�֐���	CMainFrame
//�@�\		�R���X�g���N�^
//---------------------------------------------------
CMainFrame::CMainFrame() : m_hIcon(nullptr), m_hStopIcon(nullptr), m_nIcon(), m_hActive(nullptr)
{
    setlocale(LC_ALL, "");

    //���j���[�������L����
    CFrameWnd::m_bAutoMenuEnable = FALSE;

    m_PopupMenu.LoadMenu(MAKEINTRESOURCE(IDR_MAINFRAME));
}

//---------------------------------------------------
//�֐���	~CMainFrame
//�@�\		�f�X�g���N�^
//---------------------------------------------------
CMainFrame::~CMainFrame()
{
    Shell_NotifyIcon(NIM_DELETE, &m_nIcon);
}

//---------------------------------------------------
//�֐���	OnCreate(LPCREATESTRUCT lpCreateStruct)
//�@�\		������
//---------------------------------------------------
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    // �t���[���̃N���C�A���g�̈�S�̂��߂�r���[���쐬���܂��B
    if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL)) {
        TRACE0("Failed to create view window\n");
        return -1;
    }
    //�A�C�R������
#if true
    m_hIcon = (HICON)LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_RUN256), IMAGE_ICON, 16, 16, 0);//�X���[���A�C�R��
    m_hStopIcon = (HICON)LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_STOP256), IMAGE_ICON, 16, 16, 0);//�X���[���A�C�R��
#else
    m_hIcon = (HICON)LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_RUN), IMAGE_ICON, 16, 16, 0);//�X���[���A�C�R��
    m_hStopIcon = (HICON)LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_STOP), IMAGE_ICON, 16, 16, 0);//�X���[���A�C�R��
#endif
    // �ʒm�̈�A�C�R����ݒ肷��
    m_nIcon.cbSize = sizeof(NOTIFYICONDATA);
    m_nIcon.uID = 4;
    m_nIcon.hWnd = m_hWnd;
    m_nIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nIcon.hIcon = m_hStopIcon;
    m_nIcon.uCallbackMessage = WM_TASKTRAY;

    _tcscpy_s(m_nIcon.szTip, _T("Charu3"));
    Shell_NotifyIcon(NIM_ADD, &m_nIcon);

    return 0;
}

//---------------------------------------------------
//�֐���	CheckTrayPos()
//�@�\		�J�[�\�����^�X�N�g���C�ɏd�Ȃ��Ă邩����
//---------------------------------------------------
bool CMainFrame::CheckTrayPos()
{
    HWND hTaskBarWnd;
    HWND hTrayNotifyWnd;
    RECT rtTrayNotify;
    POINT ptCursolPos;
    bool isRet = false;

    GetCursorPos(&ptCursolPos);

    // �^�X�N�o�[�̃C���W�P�[�^�̈�̋�`�𓾂�
    hTaskBarWnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);
    hTrayNotifyWnd = FindWindowEx(hTaskBarWnd, NULL, _T("TrayNotifyWnd"), NULL)->GetSafeHwnd();
    if (hTrayNotifyWnd == NULL)	return FALSE;

    ZeroMemory(&rtTrayNotify, sizeof(rtTrayNotify));
    ::GetWindowRect(hTrayNotifyWnd, &rtTrayNotify);

    if (ptCursolPos.x > rtTrayNotify.left && ptCursolPos.x < rtTrayNotify.right
        && ptCursolPos.y > rtTrayNotify.top && ptCursolPos.y < rtTrayNotify.bottom)
        isRet = true;

    return isRet;
}

//---------------------------------------------------
//�֐���	PreCreateWindow(CREATESTRUCT& cs)
//�@�\		�������O����
//---------------------------------------------------
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))	return FALSE;

    cs.cx = 185;
    cs.cy = 256;
    cs.x = -500;
    cs.y = -500;

    cs.style = WS_TILED | WS_MINIMIZEBOX | WS_SYSMENU;
    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    cs.lpszClass = AfxRegisterWndClass(0);

    return TRUE;
}

//---------------------------------------------------
//�֐���	OnClipboardChanged()
//�@�\		�N���b�v�{�[�h�̕ύX����
//---------------------------------------------------
void CMainFrame::OnClipboardChanged()
{
    // Check the program that owns the currently active window if stealth program is registered.
    bool isStealth = false;
    if (theApp.m_ini.m_stealth.size() > 0) {
        const HWND hActiveWindow = ::GetForegroundWindow();
        DWORD actorBufSize = 32768;
        LPWSTR actorPathnameBuf = new WCHAR[actorBufSize];
        DWORD dwProcessId;
        ::GetWindowThreadProcessId(hActiveWindow, &dwProcessId);
        const HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
        if (::QueryFullProcessImageName(hProcess, 0, actorPathnameBuf, &actorBufSize)) {
            CString csActorPath = CString(actorPathnameBuf);
            LPWSTR p = _tcsrchr(actorPathnameBuf, _T('\\'));
            CString csActorName = p ? CString(p + 1) : csActorPath;
            for (const CString& s : theApp.m_ini.m_stealth) {
                CString csStealth = s;
                csStealth.Replace(_T('/'), _T('\\'));
                if (csStealth.Find(_T('\\')) < 0) {
                    isStealth = csActorName.CompareNoCase(csStealth) == 0;
                }
                else {
                    isStealth = csActorPath.CompareNoCase(csStealth) == 0;
                }
                if (isStealth) {
                    break;
                }
            }
        }
        delete[] actorPathnameBuf;
    }

    CString strClipboard;
    if (!isStealth) {
        if (theApp.m_ini.m_nClipboardOpenDelay > 0) {
            Sleep(theApp.m_ini.m_nClipboardOpenDelay);
        }
        theApp.m_clipboard.GetClipboardText(strClipboard, theApp.m_ini.m_nClipboardRetryTimes, theApp.m_ini.m_nClipboardRetryInterval);
        theApp.Record(strClipboard);
    }

    if (theApp.m_ini.m_bShowClipboardInTooltipOfNofifyIcon) {
        if (isStealth) {
            _tcscpy_s(m_nIcon.szTip, _T("**** Stealthed ****"));
        }
        else {
            strClipboard = strClipboard.Left(1024);
            strClipboard.Replace(_T('\t'), _T(' ')); // Replace tab with space.
            _tcscpy_s(m_nIcon.szTip, strClipboard.Left(63));
        }
        if (!Shell_NotifyIcon(NIM_MODIFY, &m_nIcon)) {
            Shell_NotifyIcon(NIM_ADD, &m_nIcon);
        }
    }

}

//---------------------------------------------------
//�֐���	SwitchNotificationAreaIcon(bool stockmode)
//�@�\		�g���C�A�C�R���؂�ւ�
//---------------------------------------------------
void CMainFrame::SwitchNotificationAreaIcon(bool stockmode)
{
    m_nIcon.hIcon = stockmode ? m_hIcon : m_hStopIcon;
    if (!Shell_NotifyIcon(NIM_MODIFY, &m_nIcon)) {
        Shell_NotifyIcon(NIM_ADD, &m_nIcon);
    }
}

//---------------------------------------------------
// CMainFrame �N���X�̐f�f
//---------------------------------------------------

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

//---------------------------------------------------
//�֐���	OnSetFocus(CWnd* pOldWnd)
//�@�\		�t�H�[�J�X��^����
//---------------------------------------------------
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
    // �r���[ �E�B���h�E�Ƀt�H�[�J�X��^���܂��B
    m_wndView.SetFocus();
}

//---------------------------------------------------
//�֐���	OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
//�@�\		�R�}���h����
//---------------------------------------------------
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // �r���[�ɍŏ��ɃR�}���h����������@���^���܂��B
    if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
        return TRUE;

    // ��������Ȃ������ꍇ�ɂ̓f�t�H���g�̏������s���܂��B
    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


//---------------------------------------------------
//�֐���	WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//�@�\		���b�Z�[�W����
//---------------------------------------------------
LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // tasktray messages
    if (WM_TASKTRAY == message && theApp.getPhase() == PHASE_IDOL) {
        if (WM_RBUTTONUP == lParam) {
            // menu
            POINT point;
            GetCursorPos(&point);
            this->SetForegroundWindow();
            m_PopupMenu.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
            PostMessage(0, 0, 0);
            return FALSE; // to suppress notification area menu
        }
        else if (WM_LBUTTONDOWN == lParam) {
            if (theApp.m_ini.m_nIconClick == 0) {
                // popup treeview
                if (theApp.getPhase() == PHASE_IDOL) {
                    theApp.popupTreeWinMC(m_hActive);
                }
            }
            else {
                // toggle stock mode
                theApp.toggleStockMode();
            }
        }
        else if (CheckTrayPos()) {
            m_hActive = ::GetForegroundWindow();
            if (m_hActive != theApp.m_focusInfo.m_hActiveWnd) {
                Window::GetFocusInfo(&theApp.m_focusInfo);
            }
        }
    }
    // clipboard update notification
    if (WM_CLIPBOARDUPDATE == message) {
        if (theApp.m_ini.m_bDebug) {
            LOG(_T("WM_CLIPBOARDUPDATE wParam:%x lParam:%x"), wParam, lParam);
        }
        OnClipboardChanged();
    }

    return CFrameWnd::WindowProc(message, wParam, lParam);
}

//---------------------------------------------------
//�֐���	OnExit()
//�@�\		�I������
//---------------------------------------------------
void CMainFrame::OnExit()
{
    theApp.OnExit();
    Shell_NotifyIcon(NIM_DELETE, &m_nIcon);
    CFrameWnd::DestroyWindow();
}

BOOL CMainFrame::OnQueryEndSession()
{
    if (!CFrameWnd::OnQueryEndSession()) {
        return FALSE;
    }
    theApp.OnExit();
    Shell_NotifyIcon(NIM_DELETE, &m_nIcon);

    return TRUE;
}
