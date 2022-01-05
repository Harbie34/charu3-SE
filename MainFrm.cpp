/*----------------------------------------------------------
	MainFrm�N���X
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "Charu3.h"
#include "General.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCharu3App theApp;

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
CMainFrame::CMainFrame()
{
	//���j���[�������L����
	CFrameWnd::m_bAutoMenuEnable = FALSE;
	
}

//---------------------------------------------------
//�֐���	~CMainFrame
//�@�\		�f�X�g���N�^
//---------------------------------------------------
CMainFrame::~CMainFrame()
{
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
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	//�A�C�R������
#if true
	m_hIcon = (HICON)LoadImage(theApp.m_hInstance,MAKEINTRESOURCE(IDI_RUN256),IMAGE_ICON,16,16,0);//�X���[���A�C�R��
	m_hStopIcon  = (HICON)LoadImage(theApp.m_hInstance,MAKEINTRESOURCE(IDI_STOP256),IMAGE_ICON,16,16,0);//�X���[���A�C�R��
#else
	m_hIcon = (HICON)LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_RUN), IMAGE_ICON, 16, 16, 0);//�X���[���A�C�R��
	m_hStopIcon = (HICON)LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_STOP), IMAGE_ICON, 16, 16, 0);//�X���[���A�C�R��
#endif
	//�^�X�N�g���C�A�C�R����ݒ肷��
	m_nIcon.cbSize = sizeof(NOTIFYICONDATA);
	m_nIcon.uID = 4;
	m_nIcon.hWnd = m_hWnd;
	m_nIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nIcon.hIcon = m_hStopIcon;
	m_nIcon.uCallbackMessage = WM_TASKTRAY;

	_tcscpy_s(m_nIcon.szTip,_T("Charu3"));
	Shell_NotifyIcon(NIM_ADD,&m_nIcon);

	return 0;
}

//---------------------------------------------------
//�֐���	checkTrayPos()
//�@�\		�J�[�\�����^�X�N�g���C�ɏd�Ȃ��Ă邩����
//---------------------------------------------------
bool CMainFrame::checkTrayPos()
{
	HWND hTaskBarWnd;
	HWND hTrayNotifyWnd;
	RECT rtTrayNotify;
	POINT ptCursolPos;
	bool isRet = false;

	GetCursorPos(&ptCursolPos);

	// �^�X�N�o�[�̃C���W�P�[�^�̈�̋�`�𓾂�
	hTaskBarWnd = ::FindWindow(_T(SZTASKBAR_CLASS),NULL);
	hTrayNotifyWnd = FindWindowEx(hTaskBarWnd,NULL,_T(SZTRAY_CLASS),NULL)->GetSafeHwnd();
	if (hTrayNotifyWnd == NULL)	return FALSE;

	ZeroMemory(&rtTrayNotify,sizeof(rtTrayNotify));
	::GetWindowRect(hTrayNotifyWnd ,&rtTrayNotify);

	if(ptCursolPos.x > rtTrayNotify.left && ptCursolPos.x < rtTrayNotify.right
		&& ptCursolPos.y > rtTrayNotify.top && ptCursolPos.y < rtTrayNotify.bottom)
		isRet = true;

	return isRet;
}

//---------------------------------------------------
//�֐���	setMenu()
//�@�\		���j���[������
//---------------------------------------------------
void CMainFrame::setMenu()
{
	m_PopupMenu.LoadMenu(MAKEINTRESOURCE(IDR_MAINFRAME));//���j���[�N���X�Ƀ��j���[��ǂ�
	//�}�N���v���O�C�����j���[���\�z
	if(theApp.m_ini.m_strMacroPluginName == DAT_FORMAT)
		m_PopupMenu.CheckMenuItem(IDM_MACRO_CHARU,MF_CHECKED);
	else
		m_PopupMenu.CheckMenuItem(IDM_MACRO_CHARU,MF_UNCHECKED);

	int nMax = theApp.m_pTree->m_rwPlugin.size();//�f�[�^�����擾
	for(int i = 0; i < nMax; i++) {
		m_PopupMenu.InsertMenu (IDM_MACRO_CHARU,MF_STRING,IDM_MACRO_CHARU + i + 1,theApp.m_pTree->m_rwPlugin[i].m_strSoftName);
		if(theApp.m_ini.m_strMacroPluginName == theApp.m_pTree->m_rwPlugin[i].m_strSoftName) {
			m_PopupMenu.CheckMenuItem(IDM_MACRO_CHARU + i + 1,MF_CHECKED);
		}
	}
	m_PopupMenu.InsertMenu (IDM_MACRO_CHARU,MF_SEPARATOR);
}

//---------------------------------------------------
//�֐���	PreCreateWindow(CREATESTRUCT& cs)
//�@�\		�������O����
//---------------------------------------------------
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )	return FALSE;

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
//�֐���	changeClip()
//�@�\		�N���b�v�{�[�h�̕ύX����
//---------------------------------------------------
void CMainFrame::changeClip()
{
	CString strClipBord;
	theApp.m_clipbord.getClipboardText(strClipBord);//�N���b�v�{�[�h�̓��e���擾
	theApp.changeClipBord(strClipBord);//�{�̂ɃN���b�v�{�[�h�̕ύX��ʒm

	if(!theApp.m_ini.m_etc.m_nToolTip) {
		//�^�X�N�g���C�̃c�[���q���g��ύX
		strClipBord = strClipBord.Left(1024);
		strClipBord.Replace(_T("	"),_T("�"));//�^�u��u��
		_tcscpy_s(m_nIcon.szTip,strClipBord.Left(63));
		if(!Shell_NotifyIcon(NIM_MODIFY,&m_nIcon)){
			Shell_NotifyIcon(NIM_ADD,&m_nIcon);
		}
	}

}

//---------------------------------------------------
//�֐���	changeTrayIcon(bool isStock)
//�@�\		�g���C�A�C�R���؂�ւ�
//---------------------------------------------------
void CMainFrame::changeTrayIcon(bool isStock)
{
	if(isStock) m_nIcon.hIcon = m_hIcon;
	else		m_nIcon.hIcon = m_hStopIcon;
	if(!Shell_NotifyIcon(NIM_MODIFY,&m_nIcon))	Shell_NotifyIcon(NIM_ADD,&m_nIcon);
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
	//�^�X�N�g���C����
	if(message == WM_TASKTRAY && theApp.getPhase() == PHASE_IDOL) {
		if(lParam == WM_RBUTTONUP) {//���j���[���o��
			//�}�N���v���O�C�����j���[���\�z
			int nMax = theApp.m_pTree->m_rwPlugin.size();//�f�[�^�����擾
			if(theApp.m_ini.m_strMacroPluginName == DAT_FORMAT)
				m_PopupMenu.CheckMenuItem(IDM_MACRO_CHARU,MF_CHECKED);
			else
				m_PopupMenu.CheckMenuItem(IDM_MACRO_CHARU,MF_UNCHECKED);
			for(int i = 0; i < nMax; i++) {
				if(theApp.m_ini.m_strMacroPluginName == theApp.m_pTree->m_rwPlugin[i].m_strSoftName)
					m_PopupMenu.CheckMenuItem(IDM_MACRO_CHARU + i + 1,MF_CHECKED);
				else
					m_PopupMenu.CheckMenuItem(IDM_MACRO_CHARU + i + 1,MF_UNCHECKED);
			}

			POINT point;
			GetCursorPos(&point);
			this->SetForegroundWindow();

			m_PopupMenu.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON,point.x, point.y, this);
			PostMessage(0,0,0);
			return FALSE;//�^�X�N�g���C�̃��j���[���o���Ⴄ�̂Œǉ�
		}
		//���N���b�N
		else if(lParam == WM_LBUTTONDOWN) {
			if(theApp.m_ini.m_etc.m_nIconClick == 0) {
				if(theApp.getPhase() == PHASE_IDOL) {
					theApp.popupTreeWinMC(m_hActive);//�|�b�v�A�b�v���o��
				}
			}
			else
				theApp.toggleStockMode();//�X�g�b�N���[�h�ؑ�

		}
		else if(checkTrayPos()) {//�J�[�\�����A�C�R���ɏd�Ȃ���
			m_hActive = ::GetForegroundWindow();
			if(m_hActive != theApp.m_focusInfo.m_hActiveWnd) {
				CGeneral::getFocusInfo(&theApp.m_focusInfo);
			}
		}
	}
	//�N���b�v�{�[�h�`�F�C���̕ύX�����m
  	if(message == WM_CHANGECBCHAIN) {
		//�f�o�b�O���O����
		if(theApp.m_ini.m_nDebug) {
			CString strText;
			strText.Format(_T("WM_CHANGECBCHAIN wParam:%x lParam:%x nowNext:%x\n"),wParam,lParam,theApp.m_clipbord.getNextCb());
			CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
		}

		if(theApp.m_clipbord.getNextCb() == (HWND)wParam){
			theApp.m_clipbord.setNextCb((HWND)lParam);

		}
		else if(m_hWnd != theApp.m_clipbord.getNextCb() && theApp.m_clipbord.getNextCb())
			::SendMessage(theApp.m_clipbord.getNextCb(),message,wParam,lParam);
	}
	//�N���b�v�{�[�h�̕ύX�����m
 	if(message == WM_DRAWCLIPBOARD) {
		//�f�o�b�O���O����
		if(theApp.m_ini.m_nDebug) {
			CString strText;
			strText.Format(_T("WM_DRAWCLIPBOARD wParam:%x lParam:%x nowNext:%x\n"),wParam,lParam,theApp.m_clipbord.getNextCb());
			CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
		}
		//Word�o�O���
		if(theApp.m_ini.m_etc.m_nWordBugSW)	{
			PostMessage(WM_WORD_CLIP,NULL,NULL);
		}
		else 								changeClip();
		if(m_hWnd != theApp.m_clipbord.getNextCb() && theApp.m_clipbord.getNextCb()) {//���̃`�F�C���Ƀ��b�Z�[�W��n��
			::SendMessage(theApp.m_clipbord.getNextCb(), message, wParam, lParam);
		}
	}
	//���[�h�o�O�p���b�Z�[�W�n���h��
	if(message == WM_WORD_CLIP) {
		Sleep(150);
		changeClip();
	}
	
	return CFrameWnd::WindowProc(message, wParam, lParam);
}

//---------------------------------------------------
//�֐���	OnExit()
//�@�\		�I������
//---------------------------------------------------
void CMainFrame::OnExit() 
{
	theApp.m_clipbord.delClipView(m_hWnd);
	theApp.OnExit();
	Shell_NotifyIcon(NIM_DELETE,&m_nIcon);
	CFrameWnd::DestroyWindow();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent) 
{
	if(nIDEvent == TIMER_SELF_DIAGNOSIS){
		if(!Shell_NotifyIcon(NIM_MODIFY,&m_nIcon)){
			Shell_NotifyIcon(NIM_ADD,&m_nIcon);
		}
	}	
	CFrameWnd::OnTimer(nIDEvent);
}

BOOL CMainFrame::OnQueryEndSession() 
{
	if (!CFrameWnd::OnQueryEndSession())
		return FALSE;
	
	theApp.m_clipbord.delClipView(m_hWnd);
	theApp.OnExit();
	Shell_NotifyIcon(NIM_DELETE,&m_nIcon);
	
	return TRUE;
}
