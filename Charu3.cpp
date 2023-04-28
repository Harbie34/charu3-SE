/*----------------------------------------------------------
	Charu3
	�A�v���P�[�V�����p�N���X�̋@�\��`	'2002.11.15 (c)C+ Factory
----------------------------------------------------------*/
#include "stdafx.h"
#include "Charu3.h"
#include "MainFrm.h"
#include "AddDialog.h"
#include "OptMainDialog.h"
#include "MyFileDialog.h"
#include "StringWork.h"
#include "General.h"
#include "Color.h"
#include "nlomann/json.hpp"
#include <fstream>
#if false
// TODO: Did not handle this well with Visual Studio 2019.
#include <MULTIMON.H>
#endif
#include <dwmapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _ME_NAME_
#undef _ME_NAME_
#endif
#define _ME_NAME_ _T("Charu3.cpp")

//---------------------------------------------------
//�֐���	MonitorEnumFunc
//�@�\		���j�^�[�񋓃R�[���o�b�N
//---------------------------------------------------
BOOL CALLBACK MonitorEnumFunc(HMONITOR hMonitor,HDC hdc,LPRECT rect,LPARAM lParam)
{
    MONITORINFOEX MonitorInfoEx;
    static int count=0;

	CArray<RECT,RECT> *pArray;
	pArray = (CArray<RECT,RECT>*)lParam;
    count++;
    MonitorInfoEx.cbSize=sizeof(MonitorInfoEx);
    if (!GetMonitorInfo(hMonitor,&MonitorInfoEx)) {
         return FALSE;
    }
	pArray->Add(*rect);
    return TRUE;
}

//---------------------------------------------------
// CCharu3App
//---------------------------------------------------
BEGIN_MESSAGE_MAP(CCharu3App, CWinApp)
	//{{AFX_MSG_MAP(CCharu3App)
	ON_COMMAND(IDM_ABOUT, OnAbout)
	ON_COMMAND(IDM_HELP, OnHelp)
	ON_COMMAND(IDM_EXIT, OnExit)
	ON_COMMAND(IDM_OPTION, OnOption)
	ON_COMMAND(IDM_DATA_SAVE, OnDataSave)
	ON_COMMAND(IDM_ONETIME_CLEAR, OnOnetimeClear)
	ON_COMMAND(IDM_ALL_LOCK, OnAllLock)
	ON_COMMAND(IDM_CHANG_DATA, OnChangData)
	ON_COMMAND(IDM_ADD_DATA, OnAddData)
	ON_COMMAND(IDM_ICON_DECIDE, OnIconDecide)
	ON_COMMAND(IDM_BBS_OPEN, OnBbsOpen)
	ON_COMMAND(IDM_STOCK_STOP, OnStockStop)
	ON_COMMAND(IDM_RESET_TREE, OnResetTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------
//�֐���	CCharu3App()
//�@�\		CCharu3App�N���X�̃R���X�g���N�^
//---------------------------------------------------
CCharu3App::CCharu3App()// : m_treeDlg(&m_tree)
{
	m_isCloseKey = false;
	m_isStockMode = false;
	m_isCornerPopup = false;
	m_isWordBugClipCheck = false;
	m_nPhase = PHASE_START;
	m_dwDoubleKeyPopTime = 0;
	m_dwDoubleKeyFifoTime = 0;
	m_hSelectItemBkup = NULL;
	m_focusInfo.m_hActiveWnd  = NULL;
	m_focusInfo.m_hFocusWnd = NULL;

	int i,nCount = 0,nSize;

	int stKeycode[] = {	0x08,0x09,0x0c,0x0d,0x10,0x11,0x12,0x13,
						0x14,0x1B,0x20,0x21,0x22,0x23,0x24,0x25,
						0x26,0x27,0x28,0x29,0x2b,0x2c,0x2d,0x2e,
						0x2f,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x90,
						0x91,0x92,0xBB,0xBC,};

	const char *stKeyName[] = {"BS","TAB","CLEAR","ENTER","SHIFT","CTRL","ALT","PAUSE",
						"CAPSLOCK","ESC","SPACE","PAGEUP","PAGEDOWN","END","HOME","LEFT",
						"UP","RIGHT","DOWN","SELECT","EXECUTE","PRINTSCREEN","INSERT","DEL",
						"HELP","NUM*","NUM+","NUMSEP","NUM-","NUM.","NUM/","NUMLOCK",
						"SCROLLLOCK","NUM=","SEMICOLON","COMMA",};

	//�e���L�[�̐���
	for(i = 0x60; i <= 0x69; i++,nCount++) {
		m_keyStruct[nCount].nKeyCode = i;
		m_keyStruct[nCount].strName.Format(_T("NUM%d"),i - 0x60);
	}
	//�t�@���N�V�����L�[
	for(i = 0x70; i <= 0x87; i++,nCount++) {
		m_keyStruct[nCount].nKeyCode = i;
		m_keyStruct[nCount].strName.Format(_T("F%d"),i - 0x6f);
	}

	nSize = _countof(stKeycode);
	//�g���L�[
	for(i = 0; i < nSize; i++,nCount++) {
		m_keyStruct[nCount].nKeyCode = stKeycode[i];
		m_keyStruct[nCount].strName = stKeyName[i];
	}
	m_keyStruct[nCount].nKeyCode = -1;

	m_pTreeDlg = new CMyTreeDialog;
	m_pTree = new CCharu3Tree;
}
//---------------------------------------------------
//�֐���	~CCharu3App()
//�@�\		CCharu3App�N���X�̃f�X�g���N�^
//---------------------------------------------------
CCharu3App::~CCharu3App()
{
	if(m_pTreeDlg)	delete m_pTreeDlg;
	if(m_pTree)		delete m_pTree;
	if(m_hMutex)	CloseHandle(m_hMutex);
}

//---------------------------------------------------
// �B��� CCharu3App �I�u�W�F�N�g
//---------------------------------------------------
CCharu3App theApp;

//---------------------------------------------------
//�֐���	InitInstance()
//�@�\		CCharu3App �N���X�̏�����
//---------------------------------------------------
BOOL CCharu3App::InitInstance()
{
	//�d���N���j�~
	LPCTSTR pszMutexObjectName = _T("Charu3");			//�d���N���h�~��
	m_hMutex = OpenMutex( MUTEX_ALL_ACCESS, FALSE, pszMutexObjectName);
//	HWND hActiveWnd = ::GetForegroundWindow();
	CGeneral::getFocusInfo(&m_focusInfo);

	if(m_hMutex) {
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		return FALSE;
	}
	m_hMutex = CreateMutex( FALSE, 0, pszMutexObjectName);

#if false
// no longer needed (obsolete)
#ifdef _AFXDLL
	Enable3dControls();		// ���L DLL �̒��� MFC ���g�p����ꍇ�ɂ͂������Ăяo���Ă��������B
#else
	Enable3dControlsStatic();	// MFC �ƐÓI�Ƀ����N���Ă���ꍇ�ɂ͂������Ăяo���Ă��������B
#endif
#endif

	//���ꃊ�\�[�X��ݒ�
	m_hLangDll = LoadLibrary(_T("c3language.dll"));
	if(m_hLangDll) {
		AfxSetResourceHandle(m_hLangDll);
	}

	//���C���t�H�[���̍쐬
	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;
	// �t���[�������\�[�X���烍�[�h���č쐬���܂�
	pFrame->LoadFrame(IDR_MAINFRAME,WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,	NULL);

	// ���C�� �E�B���h�E�͍ŏ������ĉB���Ă����܂�
	pFrame->ShowWindow(SW_MINIMIZE);
	pFrame->ShowWindow(SW_HIDE);
	pFrame->UpdateWindow();
	m_pMainFrame = pFrame;

	HWND hWndTop;
	m_hSelfWnd = CWnd::GetSafeOwner_(NULL, &hWndTop);

	m_pTreeDlg->setTree(m_pTree);
    m_pTreeDlg->Create(IDD_POPUP_DIALOG,this->m_pMainWnd);
//	::SetForegroundWindow(hActiveWnd);
	DWMNCRENDERINGPOLICY policy = DWMNCRP_DISABLED;
	DwmSetWindowAttribute(m_pTreeDlg->m_hWnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof policy);

	pFrame->setMenu();
	init();//������
	CGeneral::setFocusInfo(&m_focusInfo);
//	setAppendKeyInit(m_focusInfo.m_hActiveWnd,&m_keySet);//�L�[�ݒ��ύX

	m_nPhase = PHASE_IDOL;

	pFrame->ShowWindow(SW_MINIMIZE);
	pFrame->ShowWindow(SW_HIDE);
	return TRUE;
}

int CCharu3App::ExitInstance() 
{
	if(m_hLangDll) {
		FreeLibrary(m_hLangDll);
	}
	return CWinApp::ExitInstance();
}
//---------------------------------------------------
// �A�v���P�[�V�����̃o�[�W�������Ŏg���� CAboutDlg �_�C�A���O
//---------------------------------------------------
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strVersion = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	TCHAR fileName[MAX_PATH + 1];
	::GetModuleFileName(NULL, fileName, sizeof(fileName));
	DWORD bufSize = ::GetFileVersionInfoSize(fileName, NULL);
	BYTE* pVersion = new BYTE[bufSize];
	::GetFileVersionInfo(fileName, NULL, bufSize, pVersion);
	VS_FIXEDFILEINFO* pFileInfo;
	UINT queryLen;
	::VerQueryValue(pVersion, _T("\\"), (void**)&pFileInfo, &queryLen);
	m_strVersion.Format(_T("%s Version %u.%u (%u.%u.%u.%u)"),
		ABOUT_NAME,
		LOWORD(pFileInfo->dwFileVersionMS),
		LOWORD(pFileInfo->dwFileVersionLS),
		HIWORD(pFileInfo->dwFileVersionMS),
		LOWORD(pFileInfo->dwFileVersionMS),
		HIWORD(pFileInfo->dwFileVersionLS),
		LOWORD(pFileInfo->dwFileVersionLS));
	delete[] pVersion;
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_VERSION_NAME, m_strVersion);
	//}}AFX_DATA_MAP
}

//---------------------------------------------------
//�֐���	init()
//�@�\		������
//---------------------------------------------------
void CCharu3App::init()
{
	TCHAR wcAppDataPath[MAX_PATH];
	SHGetSpecialFolderPath(NULL, wcAppDataPath, CSIDL_APPDATA, 0);
	CString strAppDataPath = CString(wcAppDataPath) + "\\" + NAME;
	if (!FILEEXIST(strAppDataPath)) {
		CreateDirectory(strAppDataPath, NULL);
	}
	CString strUserIniFile = strAppDataPath + "\\" + INI_FILE;

	//�ݒ�N���X��������
	m_ini.initialize();

	// Read .ini file
	m_ini.setIniFileName(strUserIniFile);
	m_ini.readAllInitData();

	m_pTree->setImageList(theApp.m_ini.m_IconSize,theApp.m_ini.m_visual.m_strResourceName,m_ini.m_strAppPath);
	m_pTree->setInitInfo(&m_ini.m_nTreeID,&m_ini.m_nSelectID,&m_ini.m_nRecNumber);//ID�����l��ݒ�

	//�f�o�b�O���O����
	m_ini.m_strDebugLog = strAppDataPath + "\\" + m_ini.m_strDebugLog;
	if(m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("start \"%s\"\n"),ABOUT_NAME);
		CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
		strText.Format(_T("ini file name \"%s\"\n"), strUserIniFile.GetString());
		CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}

	//�f�[�^�݊��v���O�C����������
	if(!m_pTree->setPlugin(m_ini.m_strRwPluginFolder)){
		m_ini.m_strRwPluginFolder = m_ini.m_strAppPath + "rw_plugin";
		m_pTree->setPlugin(m_ini.m_strRwPluginFolder);
		m_ini.writeEnvInitData();
	}

	//�f�[�^�ǂݍ���
	(void)m_pTree->loadDataFileDef(m_ini.m_strDataFile, m_ini.m_strPluginName);

	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("read data file\"%s\" %s\n"),m_ini.m_strDataFile.GetString(),m_ini.m_strPluginName.GetString());
		CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}

	//�N���b�v�{�[�h�N���X�̏����� �ύX���m��ݒ�(���C���t���[���Ń��b�Z�[�W���������Ă܂�)
	m_clipbord.getClipboardText(m_strlClipBackup);
	m_clipbord.setParent(this->m_pMainWnd->m_hWnd);
	m_keySet = m_ini.m_key.m_defKeySet;

	TCHAR strKeyLayoutName[256];
	GetKeyboardLayoutName(strKeyLayoutName);
	m_ini.m_keyLayout = LoadKeyboardLayout(strKeyLayoutName,KLF_REPLACELANG);
	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("KeyLayoutName \"%s\"\n"),strKeyLayoutName);
		CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}

	//���Ȑf�f�^�C�}�[�Z�b�g
	if(m_ini.m_etc.m_nSelfDiagnosisTime)
		SetTimer(m_pMainWnd->m_hWnd,TIMER_SELF_DIAGNOSIS,m_ini.m_etc.m_nSelfDiagnosisTime,NULL);

	//�}�E�X�Ď��^�C�}�[
	if(m_ini.m_pop.m_nCornerPopup)
		SetTimer(m_pMainWnd->m_hWnd,TIMER_MOUSE,m_ini.m_pop.m_nCornerPopupTime,NULL);

	m_ini.setHookKey(m_hSelfWnd);

	//�z�b�g�L�[��ݒ�
	registerHotkey();
	setAppendHotKey();
}

//---------------------------------------------------
//�֐���	registerHotkey()
//�@�\		�z�b�g�L�[��ݒ肷��
//---------------------------------------------------
void CCharu3App::registerHotkey()
{
	UINT uMod = 0,uVK = 0;
	switch(m_ini.m_pop.m_nDoubleKeyPOP) {
		case 1:	uMod = MOD_SHIFT;break;
		case 2:	uMod = MOD_CONTROL;break;
		case 3:	uMod = MOD_ALT;break;
		default:
		case 0:
			uMod = m_ini.m_pop.m_uMod_Pouup;
			uVK  = m_ini.m_pop.m_uVK_Pouup;
			break;
	}
	if(!RegisterHotKey(NULL,HOTKEY_POPUP,uMod,uVK)) {//�|�b�v�A�b�v�L�[
		CString strRes;
		strRes.LoadString(APP_MES_FAILURE_HOTKEY);
		//�f�o�b�O���O����
		if(m_ini.m_nDebug) {
			CGeneral::writeLog(m_ini.m_strDebugLog,_T("False hotkey setting popup\n"),_ME_NAME_,__LINE__);
		}
//		AfxMessageBox(strRes + _T("Popup Key"),MB_ICONEXCLAMATION,0);
	}

	uMod = 0,uVK = 0;
	switch(m_ini.m_pop.m_nDoubleKeyFIFO) {
		case 1:	uMod = MOD_SHIFT;break;
		case 2:	uMod = MOD_CONTROL;break;
		case 3:	uMod = MOD_ALT;break;
		default:
		case 0:
			uMod = m_ini.m_pop.m_uMod_Fifo;
			uVK  = m_ini.m_pop.m_uVK_Fifo;
			break;
	}
	if(!RegisterHotKey(NULL,HOTKEY_FIFO,uMod,uVK)) {//����FIFO�L�[
		CString strRes;
		strRes.LoadString(APP_MES_FAILURE_HOTKEY);
		//�f�o�b�O���O����
		if(m_ini.m_nDebug) {
			CGeneral::writeLog(m_ini.m_strDebugLog,_T("False hotkey setting stuckmode\n"),_ME_NAME_,__LINE__);
		}
//		AfxMessageBox(strRes + _T("Stock mode Key"),MB_ICONEXCLAMATION,0);
	}
	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CGeneral::writeLog(m_ini.m_strDebugLog,_T("registerHotkey\n"),_ME_NAME_,__LINE__);
	}
}

//---------------------------------------------------
//�֐���	stopHotkey()
//�@�\		�z�b�g�L�[���~�߂�
//---------------------------------------------------
void CCharu3App::stopHotkey()
{
	UnregisterHotKey(NULL,HOTKEY_POPUP);
	UnregisterHotKey(NULL,HOTKEY_FIFO);
	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CGeneral::writeLog(m_ini.m_strDebugLog,_T("stopHotkey\n"),_ME_NAME_,__LINE__);
	}
}

//---------------------------------------------------
//�֐���	setAppendHotKey()
//�@�\		�ǉ��z�b�g�L�[��ݒ�
//---------------------------------------------------
void CCharu3App::setAppendHotKey() 
{
	HOT_KEY_CODE keyData;
	list<STRING_DATA>::iterator it;

	m_hotkeyVector.clear();

	CString strKey;
	STRING_DATA data;
	HTREEITEM hRet = NULL;

	if(m_isStockMode)
		RegisterHotKey(NULL,HOTKEY_PASTE,m_keySet.m_uMod_Paste,m_keySet.m_uVK_Paste);//�y�[�X�g�L�[

	int nSize = m_pTree->m_MyStringList.size();
	HTREEITEM hTreeItem = m_pTree->GetRootItem();
	for(int i = 0; i < nSize && hTreeItem; hTreeItem = m_pTree->getTrueNextItem(hTreeItem),i++) {
		if(hTreeItem) {
			data = m_pTree->getData(hTreeItem);
			strKey = m_pTree->getDataOptionStr(data.m_strMacro,EXMACRO_HOT_KEY);
			if(strKey != "") {
				//�z�b�g�L�[�ݒ��ϊ�
				convHotKeyConf(strKey,&keyData.m_uModKey,&keyData.m_uVkCode,&keyData.m_isDoubleClick);
				keyData.m_strMacroName = EXMACRO_HOT_KEY;
				keyData.m_nDataID = data.m_nMyID;
				keyData.m_hItem = hTreeItem;
				keyData.m_dwDoubleKeyTime = 0;
				m_hotkeyVector.insert(m_hotkeyVector.end(),keyData);//�ݒ�A���C�ɒǉ�
				int nret = RegisterHotKey(NULL,HOT_ITEM_KEY+m_hotkeyVector.size()-1,keyData.m_uModKey,keyData.m_uVkCode);//�z�b�g�L�[�����W�X�g
				//�f�o�b�O���O����
				if(m_ini.m_nDebug) {
					CString strText;
					strText.Format(_T("setAppendHotKey hotkey \"%s\" %d\n"),strKey.GetString(),nret);
					CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
				}
			}
			strKey = m_pTree->getDataOptionStr(data.m_strMacro,EXMACRO_DIRECT_COPY);
			if(strKey != "") {
				//�z�b�g�L�[�ݒ��ϊ�
				convHotKeyConf(strKey,&keyData.m_uModKey,&keyData.m_uVkCode,&keyData.m_isDoubleClick);
				keyData.m_strMacroName = EXMACRO_DIRECT_COPY;
				keyData.m_nDataID = data.m_nMyID;
				keyData.m_hItem = hTreeItem;
				keyData.m_dwDoubleKeyTime = 0;
				m_hotkeyVector.insert(m_hotkeyVector.end(),keyData);//�ݒ�A���C�ɒǉ�
				int nret = RegisterHotKey(NULL,HOT_ITEM_KEY+m_hotkeyVector.size()-1,keyData.m_uModKey,keyData.m_uVkCode);//�z�b�g�L�[�����W�X�g
				//�f�o�b�O���O����
				if(m_ini.m_nDebug) {
					CString strText;
					strText.Format(_T("setAppendHotKey directcopy \"%s\" %d\n"),strKey.GetString(),nret);
					CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
				}
			}
		}
	}
}
//---------------------------------------------------
//�֐���	stopAppendHotKey()
//�@�\		�ǉ��z�b�g�L�[�����ׂĎ~�߂�
//---------------------------------------------------
void CCharu3App::stopAppendHotKey() 
{
	UnregisterHotKey(NULL,HOTKEY_PASTE);
	int nSize = m_hotkeyVector.size();
	for(int i = 0; i < nSize; i++) {
		UnregisterHotKey(NULL,HOT_ITEM_KEY+i);
	}
	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CGeneral::writeLog(m_ini.m_strDebugLog,_T("stopAppendHotKey\n"),_ME_NAME_,__LINE__);
	}
}

//---------------------------------------------------
//�֐���	convHotKeyConf()
//�@�\		�����񂩂�z�b�g�L�[�̐ݒ�����
//---------------------------------------------------
void CCharu3App::convHotKeyConf(CString strKey,UINT *pMod,UINT *pVK,bool *isDoubleClick)
{
	int nFound;
	CString strVK,strMod;
	strKey.MakeUpper();
	nFound = strKey.ReverseFind('+');
	*isDoubleClick = false;

	if(nFound < 0) {//+�ŋ�؂��ĂȂ��ꍇ
		*pMod = NULL;
		strVK = strKey;
		strVK.TrimLeft();
		strVK.TrimRight();
		if(strVK == "SHIFT*2") {
			*pMod = MOD_SHIFT;
		}
		else if(strVK == "CTRL*2") {
			*pMod = MOD_CONTROL;
		}
		else if(strVK == "ALT*2") {
			*pMod = MOD_ALT;
		}
		if(*pMod) {
			*isDoubleClick = true;
			*pVK = NULL;
			return;
		}
	}
	else {//+�ŋ�؂��Ă�ꍇ
		strVK  = strKey.Right(strKey.GetLength() - nFound - 1);
		strVK.TrimLeft();
		strVK.TrimRight();

		*pMod = 0;
		if(strKey.Find(_T("SHIFT")) >= 0) {
			*pMod = *pMod | MOD_SHIFT;
		}
		if(strKey.Find(_T("CTRL")) >= 0) {
			*pMod = *pMod | MOD_CONTROL;
		}
		if(strKey.Find(_T("ALT")) >= 0) {
			*pMod = *pMod | MOD_ALT;
		}
	}
	*pVK = getKeycode(strVK.GetBuffer(strVK.GetLength()));
}
//---------------------------------------------------
//�֐���	getKeycode(char *szKeyName)
//�@�\		�L�[������R�[�h�𓾂�
//---------------------------------------------------
int CCharu3App::getKeycode(TCHAR *szKeyName)
{
	CString strTmp;
	strTmp = szKeyName;
	strTmp.MakeUpper();

	int i,nRet = 0;

	if(strTmp.Find(_T("0X")) >= 0) 
		_stscanf_s(szKeyName,_T("0x%2x"),&nRet);
	
	if(!nRet) {
		if(_tcsclen(szKeyName) == 1) {
			SHORT code = VkKeyScanEx(*szKeyName,m_ini.m_keyLayout);
			nRet = code & 0xff;
		}
		else {
			for(i = 0; m_keyStruct[i].nKeyCode > 0 && i < 256; i++) {
				if(m_keyStruct[i].strName == strTmp) {
					nRet = m_keyStruct[i].nKeyCode;
					break;
				}
			}
		}
	}
	return nRet;
}

//---------------------------------------------------
//�֐���	setAppendKeyInit(HWND hTopWindow)
//�@�\		�ǉ��L�[�ݒ�
//---------------------------------------------------
bool CCharu3App::setAppendKeyInit(HWND hTopWindow,COPYPASTE_KEY *keySet)
{
	TCHAR strWindowName[1024];
	*strWindowName = (TCHAR)NULL;
	CString strWinName;
	bool isRet = false;

	if(!hTopWindow || m_hActiveKeyWnd == hTopWindow) return isRet;//�O�Ɠ����n���h���Ȃ�A��
	if(!GetWindowText(hTopWindow,strWindowName,_countof(strWindowName))) return isRet;//�L���v�V�������擾

	strWinName = strWindowName;
	*keySet = m_ini.getAppendKeyInit(strWinName);
	if(keySet->m_nMessage <= -1)	*keySet = m_ini.m_key.m_defKeySet;
	m_hActiveKeyWnd = hTopWindow;

	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("setAppendKeyInit %s %d %d %d %d %d\n"),strWinName.GetString(),keySet->m_uMod_Copy,keySet->m_uVK_Copy,keySet->m_uMod_Paste,keySet->m_uVK_Paste,keySet->m_nMessage);
		CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}
	
	isRet = true;
	return isRet;
}

//---------------------------------------------------
//�֐���	popupTreeWindow(POINT pos)
//�@�\		�|�b�v�A�b�v��\��
//---------------------------------------------------
void CCharu3App::popupTreeWindow(POINT pos,int nSelect,HTREEITEM hOpenItem)
{
	if(m_nPhase != PHASE_IDOL) return;
	m_nPhase = PHASE_POPUP;
	m_ini.unHookKey();
	stopAppendHotKey();//�ǉ��z�b�g�L�[���~

//	CGeneral::getFocusInfo(&m_focusInfo);

	if(m_focusInfo.m_hActiveWnd == this->m_pMainFrame->m_hWnd) {
		m_nPhase = PHASE_IDOL;
		return;
	}

	if(m_isStockMode)	KillTimer(m_pMainWnd->m_hWnd,TIMER_ACTIVE);//�Ď��^�C�}�[��~
	//IME��OFF�ɂ���
	if(m_ini.m_pop.m_nAutoImeOff) {
		HIMC hIMC = ImmGetContext(m_pTreeDlg->m_hWnd);
		m_isImeStatus = ImmGetOpenStatus(hIMC);
		if(m_isImeStatus) ImmSetOpenStatus(hIMC,FALSE);
	}
	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("popupTreeWindow %d %d %d\n"),pos.x,pos.y,nSelect);
		CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}

	CGeneral::setAbsoluteForegroundWindow(m_pMainWnd->m_hWnd);//�������A�N�e�B�u�ɐݒ�
	if(nSelect)
		m_pTreeDlg->showWindowPos(pos,m_ini.m_DialogSize,SW_SHOW,true,hOpenItem);//�_�C�A���O��\��
	else
		m_pTreeDlg->showWindowPos(pos,m_ini.m_DialogSize,SW_SHOW,false,hOpenItem);//�_�C�A���O��\��
}

//---------------------------------------------------
//�֐���	popupTreeWinMC(HWND hForeground)
//�@�\		�|�b�v�A�b�v���}�E�X�J�[�\���ʒu�ɕ\��
//---------------------------------------------------
void CCharu3App::popupTreeWinMC(HWND hForeground)
{
	POINT pos;
	GetCursorPos(&pos);
	pos.x -=  m_ini.m_DialogSize.x;
	pos.y -=  m_ini.m_DialogSize.y;
	if(m_ini.m_pop.m_nOutRevice)	reviseWindowPos(&pos);
//	CGeneral::getFocusInfo(&theApp.m_focusInfo,hForeground);
	popupTreeWindow(pos,m_ini.m_pop.m_nSelectSW);//�|�b�v�A�b�v
}
//---------------------------------------------------
//�֐���	reviseWindowPos(POINT pos)
//�@�\		�|�b�v�A�b�v�ʒu��␳
//---------------------------------------------------
void CCharu3App::reviseWindowPos(POINT *pos)
{
	RECT DeskTopSize;
	int nMonCount = GetSystemMetrics(SM_CMONITORS);

	if(nMonCount <= 1) {
		//�f�X�N�g�b�v�̃T�C�Y���擾
		int nDektopWidth = GetSystemMetrics( SM_CXSCREEN);
		int nDesktopHeight = GetSystemMetrics( SM_CYFULLSCREEN );
		//�𑜓x�̎擾
		int nScreenWidth = GetSystemMetrics( SM_CXSCREEN );
		int nScreenHeight = GetSystemMetrics( SM_CYSCREEN );
		//�f�o�b�O���O����
		if(m_ini.m_nDebug) {
			CString strText;
			strText.Format(_T("reviseWindowPos %d %d %d %d\n"),nDektopWidth,nDesktopHeight,nScreenWidth,nScreenHeight);
			CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
		}
		HWND hDeskTop = GetDesktopWindow();
		if(hDeskTop) ::GetWindowRect(hDeskTop,&DeskTopSize);
		else return;
	}
	else {
		CArray<RECT,RECT> arrayRect;
		arrayRect.RemoveAll();
		EnumDisplayMonitors(NULL,NULL,(MONITORENUMPROC)MonitorEnumFunc,(LPARAM)&arrayRect);

		int nSize = arrayRect.GetSize(),nCurrentMon = 0,nWidth,nHeight;
		CString strBuff;
		double dDistance,dMinDistance = -1.0f;
		for(int i = 0; i < nSize; i++) {
			strBuff.Format(_T("left:%d top:%d right:%d bottom:%d"),arrayRect[i].left,arrayRect[i].top,arrayRect[i].right,arrayRect[i].bottom);
			nWidth = abs((arrayRect[i].left + arrayRect[i].right) / 2 - pos->x);
			nHeight = abs((arrayRect[i].top + arrayRect[i].bottom) / 2 - pos->y);

			dDistance = sqrt((nWidth * nWidth) + (nHeight * nHeight));
			if(dDistance < dMinDistance || i == 0) {
				dMinDistance = dDistance;
				nCurrentMon = i;
			}
		}
		DeskTopSize = arrayRect[nCurrentMon];
	}
	//�E�B���h�E�ʒu��␳
	if(pos->y + m_ini.m_DialogSize.y > DeskTopSize.bottom)
		pos->y -= (pos->y + m_ini.m_DialogSize.y) - DeskTopSize.bottom;
	if(pos->x + m_ini.m_DialogSize.x > DeskTopSize.right)
		pos->x -= (pos->x + m_ini.m_DialogSize.x) - DeskTopSize.right;

	if(pos->y < DeskTopSize.top)
		pos->y = DeskTopSize.top;
	if(pos->x < DeskTopSize.left)
		pos->x = DeskTopSize.left;


}

//---------------------------------------------------
//�֐���	closeTreeWindow(int nRet)
//�@�\		�|�b�v�A�b�v�B������
//---------------------------------------------------
void CCharu3App::closeTreeWindow(int nRet)
{
	m_pTreeDlg->ShowWindow(SW_HIDE);
	STRING_DATA data;
	bool isPaste = true;
	if(::GetAsyncKeyState(VK_SHIFT) < 0) isPaste = false;

	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("closeTreeWindow %d\n"),nRet);
		CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}

	//�A�N�e�B�u�E�B���h�E�𕜋A
	if(nRet == IDOK)  {
		CString strClip,strSelect;
		m_clipbord.getClipboardText(strClip);//�N���b�v�{�[�h��ۑ�

		setAppendKeyInit(m_focusInfo.m_hActiveWnd,&m_keySet);//�L�[�ݒ��ύX
		//�L�[���������̂�҂�
		while(::GetAsyncKeyState(VK_MENU) < 0 || ::GetAsyncKeyState(VK_CONTROL) < 0 ||
			::GetAsyncKeyState(VK_SHIFT) < 0 || ::GetAsyncKeyState(VK_RETURN) < 0) Sleep(50);
		CGeneral::setFocusInfo(&m_focusInfo);//�^�[�Q�b�g���t�H�[�J�X
		//�f�o�b�O���O����
		if(m_ini.m_nDebug) {
			CString strText;
			strText.Format(_T("setFocusInfo %x %x\n"),m_focusInfo.m_hActiveWnd,m_focusInfo.m_hActiveWnd);
			CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
		}
		//�\��t������
		if(m_pTree->m_ltCheckItems.size() > 0) {//�����I���f�[�^������
			strSelect = getSelectString(m_keySet,m_focusInfo.m_hFocusWnd);//�I���e�L�X�g�擾
			//�f�o�b�O���O����
			if(m_ini.m_nDebug) {
				CString strText;
				strText.Format(_T("closeTreeWindow sel:%s clip:%s\n"),strSelect.GetString(),strClip.GetString());
				CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
			}

			list<HTREEITEM>::iterator it;
			for(it = m_pTree->m_ltCheckItems.begin(); it != m_pTree->m_ltCheckItems.end(); it++) {
				if(m_pTree->GetItemState(*it,TVIF_HANDLE)) {
					data = m_pTree->getData(*it);
					//�f�o�b�O���O����
					if(m_ini.m_nDebug) {
						CString strText;
						strText.Format(_T("closeTreeWindow check paste %s\n"),data.m_strTitle.GetString());
						CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
					}
					playData(data,strClip,strSelect,isPaste,false);
				}
			}
			//�ꎞ���ڂ͏���
			for(it = m_pTree->m_ltCheckItems.begin(); it != m_pTree->m_ltCheckItems.end(); it++) {
				if(m_pTree->GetItemState(*it,TVIF_HANDLE)) {
					HTREEITEM hItem = *it;
					*it = NULL;
					data = m_pTree->getData(hItem);
					if(data.m_cKind & KIND_ONETIME) {
						m_hSelectItemBkup = NULL;
					}
				}
			}
		}
		else if(m_pTreeDlg->m_selectDataPtr != nullptr) {//�ʏ�I���f�[�^
			strSelect = getSelectString(m_keySet,m_focusInfo.m_hFocusWnd);//�I���e�L�X�g�擾
			data = *(m_pTreeDlg->m_selectDataPtr);
			//�f�o�b�O���O����
			if(m_ini.m_nDebug) {
				CString strText;
				strText.Format(_T("closeTreeWindow sel:%s clip:%s title:\n"),strSelect.GetString(),strClip,data.m_strTitle.GetString());
				CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
			}
			playData(data,strClip,strSelect,isPaste);
		}
	}
	else if(::GetForegroundWindow() == m_focusInfo.m_hActiveWnd){
		CGeneral::setFocusInfo(&m_focusInfo);
	}
	if(m_ini.m_fifo.m_nAllClearOff && !m_pTree->getOneTimeText(m_ini.m_fifo.m_nFifo) && m_isStockMode) toggleStockMode();
	if(m_hSelectItemBkup) {
		m_pTree->SelectItem(m_hSelectItemBkup);
		m_hSelectItemBkup = NULL;
	}

	
	//�f�[�^��ۑ�
	if (!m_ini.m_strDataFile.IsEmpty()) {
		if (!m_pTree->saveDataFile(m_ini.m_strDataFile, m_ini.m_strPluginName, NULL)) {
			CString strRes;
			strRes.LoadString(APP_MES_FAILURE_DATA_SAVE);
			AfxMessageBox(strRes, MB_ICONEXCLAMATION, 0);
		}
	}
	m_ini.writeEnvInitData();//���ݒ��ۑ�

	//�Ď��^�C�}�[�Z�b�g
	if(m_isStockMode && m_ini.m_etc.m_nActiveTime)
		SetTimer(m_pMainWnd->m_hWnd,TIMER_ACTIVE,m_ini.m_etc.m_nActiveTime,NULL);
	
	setAppendHotKey();//�ǉ��z�b�g�L�[��ݒ�
	m_ini.setHookKey(m_hSelfWnd);

	//�`�F�b�N�{�b�N�X�t���������ꍇ�̓`�F�b�N���N���A
	if(m_pTree->GetStyle() & TVS_CHECKBOXES) {
		if(m_ini.m_visual.m_nResetTree) {
			resetTreeDialog();
			CGeneral::setFocusInfo(&m_focusInfo);//�^�[�Q�b�g���t�H�[�J�X
		}
		else {
			m_pTree->removeCheck();
		}
	}
	//IME��ON�ɂ���
	if(m_ini.m_pop.m_nAutoImeOff) {
		HIMC hIMC = ImmGetContext(m_pTreeDlg->m_hWnd);
		if(m_isImeStatus)	ImmSetOpenStatus(hIMC,m_isImeStatus);
	}
	if(m_ini.m_etc.m_nMinimization) {
		m_pMainFrame->ShowWindow(SW_HIDE);
		m_pMainFrame->ShowWindow(SW_MINIMIZE);
		m_pMainFrame->ShowWindow(SW_HIDE);
	}
	ASSERT(m_nPhase == PHASE_POPUP);
	m_nPhase = PHASE_IDOL;
}

//---------------------------------------------------
//�֐���	playData(STRING_DATA data,CString strClip)
//�@�\		�f�[�^�̓W�J����������
//---------------------------------------------------
void CCharu3App::playData(STRING_DATA data,CString strClip,CString strSelect,bool isPaste,bool isChange)
{
	CString strMacro,strPaste;
	int nIsMove;

	//�}�N������
	HTREEITEM hSelectItem = m_pTree->GetSelectedItem();
	HTREEITEM hMacroItem = NULL;
	if(hSelectItem) hMacroItem = m_pTree->serchParentOption(hSelectItem,_T("macro"));//��ԋ߂��e�������̃}�N���𒲂ׂ�
	if(hMacroItem) {
		STRING_DATA *macroDataPtr = m_pTree->getDataPtr(hMacroItem);
		strMacro = m_pTree->getDataOptionStr(macroDataPtr->m_strMacro,_T("macro"));
	}
	else strMacro = m_ini.m_strMacroPluginName;
	strPaste = convertMacro(&data,strSelect,strClip,strMacro);

	//�e�L�X�g��\��t������
	if(isPaste) {
		//�f�o�b�O���O����
		if(m_ini.m_nDebug) {
			CString strText;
			strText.Format(_T("playData active:%x focus:%x\n"),m_focusInfo.m_hActiveWnd,m_focusInfo.m_hFocusWnd);
			CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
		}
		execData(strPaste,m_keySet,hSelectItem,m_focusInfo.m_hFocusWnd);

		//�N���b�v�{�[�h���A
		if(!m_ini.m_etc.m_nToClip) {
			m_strlClipBackup = strClip;
			m_clipbord.setClipboardText(strClip);
		}
		if(isChange) {
			//�\��t���f�[�^���t�H���_�̐擪�Ɉړ�
			if(hSelectItem)	hMacroItem  = m_pTree->serchParentOption(hSelectItem,_T("move"));
			else			hMacroItem = NULL;
			if(hMacroItem) {
				STRING_DATA* macroDataPtr = m_pTree->getDataPtr(hMacroItem);
				nIsMove = m_pTree->getDataOption(macroDataPtr->m_strMacro,_T("move"));
				if(nIsMove) { 
					hSelectItem = m_pTree->moveFolderTop(hSelectItem);
					if(hSelectItem) m_pTree->SelectItem(hSelectItem);
				}
			}
			//�ꎞ���ڂ͏���
			if(m_pTree->getDatakind(hSelectItem) & KIND_ONETIME) {
				m_pTree->deleteData(hSelectItem);
				m_hSelectItemBkup = NULL;
			}
		}
	}
	else {
		m_clipbord.setClipboardText(strPaste);
	}
}

//---------------------------------------------------
//�֐���	playHotItem(nTarget)
//�@�\		�z�b�g�A�C�e������
//---------------------------------------------------
void CCharu3App::playHotItem(int nTarget)
{
	if(nTarget >= 0 && static_cast<size_t>(nTarget) < m_hotkeyVector.size()) {
		HOT_KEY_CODE keyData;
		keyData = m_hotkeyVector.at(nTarget);
		if(!keyData.m_uVkCode) {
			DWORD dwTime = timeGetTime();
			if(static_cast<int>(dwTime - keyData.m_dwDoubleKeyTime) >  m_ini.m_pop.m_nDCKeyPopTime) {
				m_hotkeyVector[nTarget].m_dwDoubleKeyTime = dwTime;
				return;
			}
		}
		STRING_DATA data;//�e�L�X�g�f�[�^

		if(keyData.m_hItem) {
			data = m_pTree->getData(keyData.m_hItem);
			CString strMacroData;
			CString strSelect,strMacro;
			//�t�H���_�̏ꍇ
			if(data.m_cKind & KIND_FOLDER_ALL) {
				POINT pos;
				//�_�C���N�g�R�s�[
				if(keyData.m_strMacroName == EXMACRO_DIRECT_COPY) {
					m_nPhase = PHASE_LOCK;
					stopAppendHotKey();//�ǉ��z�b�g�L�[���~
					STRING_DATA dataChild;
					CGeneral::getCaretPos(&pos,&m_focusInfo);//�L�����b�g�ʒu���擾(�n���h������邽��)

					//�L�[���������̂�҂�
					while(::GetAsyncKeyState(VK_MENU) < 0 || ::GetAsyncKeyState(VK_CONTROL) < 0 ||
						::GetAsyncKeyState(VK_SHIFT) < 0 || ::GetAsyncKeyState(keyData.m_uVkCode) < 0) Sleep(50);
					keyUpDown(keyData.m_uModKey,keyData.m_uVkCode,KEY_UP);//�L�[�𗣂������i���ꂪ�����ƑI���e�L�X�g�擾�Ŏ��s�j

					setAppendKeyInit(m_focusInfo.m_hActiveWnd,&m_keySet);//�L�[�ݒ��ύX
					strSelect = getSelectString(m_keySet,m_focusInfo.m_hFocusWnd);//�I�𕶎��擾
					if(strSelect != "") {
						dataChild.m_cKind = KIND_LOCK;
						dataChild.m_nParentID = data.m_nMyID;
						dataChild.m_strData = strSelect;
						int nTitleLength = m_pTree->getDataOption(data.m_strMacro,_T("titlelen"));//�^�C�g���̕�����
						if(nTitleLength < 1 || nTitleLength > 256) nTitleLength = 32;
						dataChild.m_strTitle = m_pTree->makeTitle(strSelect,nTitleLength);
						//�f�o�b�O���O����
						if(m_ini.m_nDebug) {
							CString strText;
							strText.Format(_T("Direct copy folder %s\n"),dataChild.m_strTitle.GetString());
							CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
						}
						m_pTree->addData(keyData.m_hItem,dataChild,true,true);
					}
					setAppendHotKey();//�ǉ��z�b�g�L�[��ݒ�
					m_nPhase = PHASE_IDOL;
				}
				//�z�b�g�L�[
				else if(keyData.m_strMacroName == EXMACRO_HOT_KEY) {
					m_hSelectItemBkup = m_pTree->GetSelectedItem();

					m_pTree->SelectItem(keyData.m_hItem);
					getPopupPos(&pos,m_ini.m_pop.m_nPopupPos);//�|�b�v�A�b�v�ʒu���擾
					if(m_ini.m_pop.m_nOutRevice)	reviseWindowPos(&pos);
					popupTreeWindow(pos,true,keyData.m_hItem);//�|�b�v�A�b�v
				}
			}
			else {//��`���͈ꔭ�\��t��
				m_nPhase = PHASE_LOCK;
				stopAppendHotKey();//�ǉ��z�b�g�L�[���~

				CString strClip,strPaste;
				strPaste = data.m_strData;
				m_clipbord.getClipboardText(strClip);//�N���b�v�{�[�h��ۑ�

				POINT pos;

				//�L�[���������̂�҂�
				while(::GetAsyncKeyState(VK_MENU) < 0 || ::GetAsyncKeyState(VK_CONTROL) < 0 ||
					::GetAsyncKeyState(VK_SHIFT) < 0 || ::GetAsyncKeyState(keyData.m_uVkCode) < 0) Sleep(50);
				keyUpDown(keyData.m_uModKey,keyData.m_uVkCode,KEY_UP);//�L�[�𗣂������i���ꂪ�����ƑI���e�L�X�g�擾�Ŏ��s�j

				HTREEITEM hMacroItem;
				int nIsMove;

				//�_�C���N�g�R�s�[
				if(keyData.m_strMacroName == EXMACRO_DIRECT_COPY) {
					CGeneral::getCaretPos(&pos,&m_focusInfo);//�L�����b�g�ʒu���擾
					setAppendKeyInit(m_focusInfo.m_hActiveWnd,&m_keySet);//�L�[�ݒ��ύX
					strSelect = getSelectString(m_keySet,m_focusInfo.m_hFocusWnd);//�I�𕶎��擾
					data.m_strData = strSelect;
					m_pTree->editData(keyData.m_hItem,data);
					//�f�o�b�O���O����
					if(m_ini.m_nDebug) {
						CString strText;
						strText.Format(_T("Direct copy data %s\n"),data.m_strTitle.GetString());
						CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
					}
				}
				//�z�b�g�L�[
				else if(keyData.m_strMacroName == EXMACRO_HOT_KEY) {
					//�}�N������
					hMacroItem = m_pTree->serchParentOption(keyData.m_hItem,_T("macro"));//��ԋ߂��e�������̃}�N���𒲂ׂ�
					if(hMacroItem) {
						STRING_DATA *macroDataPtr = m_pTree->getDataPtr(hMacroItem);
						strMacro = m_pTree->getDataOptionStr(macroDataPtr->m_strMacro,_T("macro"));
					}
					else strMacro = m_ini.m_strMacroPluginName;

					CGeneral::getCaretPos(&pos,&m_focusInfo);//�L�����b�g�ʒu���擾
					setAppendKeyInit(m_focusInfo.m_hActiveWnd,&m_keySet);//�L�[�ݒ��ύX

					strSelect = getSelectString(m_keySet,m_focusInfo.m_hFocusWnd);//�I�𕶎��擾
					strPaste = convertMacro(&data,strSelect,strClip,strMacro);//�}�N���ϊ�
					
					//�f�o�b�O���O����
					if(m_ini.m_nDebug) {
						CString strText;
						strText.Format(_T("Direct paste data %s active:%x focus:%x\n"),strPaste.GetString(),m_focusInfo.m_hActiveWnd,m_focusInfo.m_hFocusWnd);
						CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
					}

					//�e�L�X�g��\��t������
					execData(strPaste,m_keySet,keyData.m_hItem,m_focusInfo.m_hFocusWnd);

					//�\��t���f�[�^���t�H���_�̐擪�Ɉړ�
					hMacroItem  = m_pTree->serchParentOption(keyData.m_hItem,_T("move"));
					if(hMacroItem) {
						STRING_DATA* macroDataPtr = m_pTree->getDataPtr(hMacroItem);
						nIsMove = m_pTree->getDataOption(macroDataPtr->m_strMacro,_T("move"));
						if(nIsMove) {
							keyData.m_hItem = m_pTree->moveFolderTop(keyData.m_hItem);
						}
					}
					//�N���b�v�{�[�h���A
					if(!m_ini.m_etc.m_nToClip) {
						m_clipbord.setClipboardText(strClip);
					}
					//�ꎞ���ڂ͏���
					if(m_pTree->getDatakind(keyData.m_hItem) & KIND_ONETIME) {
						m_pTree->deleteData(keyData.m_hItem);
					}
				}
				setAppendHotKey();//�ǉ��z�b�g�L�[��ݒ�
				m_nPhase = PHASE_IDOL;
			}
		}
	}
	return;
}

//---------------------------------------------------
//�֐���	getSelectString()
//�@�\		�I�𕶎�������
//---------------------------------------------------
CString CCharu3App::getSelectString(COPYPASTE_KEY key,HWND hWnd)
{
	m_clipbord.setClipboardText(_T(""));
	if(!hWnd)	hWnd = m_focusInfo.m_hFocusWnd;
	CString strSelect;

	TCHAR strWindowName[1024];
	*strWindowName = (TCHAR)NULL;
	CString strWinName;
	int i;
	COPYPASTE_KEY keySet;

	GetWindowText(m_focusInfo.m_hActiveWnd,strWindowName,_countof(strWindowName));
	strWinName = strWindowName;

	keySet.m_nMessage = 0;

	for(i = 0; keySet.m_nMessage > -1 ;i++) {
		keySet = m_ini.getAppendKeyInit(strWinName,i);
		if(keySet.m_nMessage <= -1 && i == 0)	keySet = key;
		if(keySet.m_nMessage > -1) {
			if(keySet.m_nMessage == 0) {//�C�x���g����
				keyUpDown(keySet.m_uMod_Copy,keySet.m_uVK_Copy,KEY_DOWN);
				keyUpDown(keySet.m_uMod_Copy,keySet.m_uVK_Copy,KEY_UP);
				Sleep(keySet.m_nCopyWait);//�E�F�C�g
				keyUpDownMessage(keySet.m_uMod_Copy,keySet.m_uVK_Copy,KEY_UP,hWnd);
			}
			else if(keySet.m_nMessage == 1) {//Charu2Pro����
				keyUpDownC2(keySet.m_uMod_Copy,keySet.m_uVK_Copy,KEY_DOWN);
				keyUpDownC2(keySet.m_uMod_Copy,keySet.m_uVK_Copy,KEY_UP);
				Sleep(keySet.m_nCopyWait);//�E�F�C�g
				keyUpDownMessage(keySet.m_uMod_Copy,keySet.m_uVK_Copy,KEY_DOWN,hWnd);
				Sleep(keySet.m_nCopyWait/4);//�E�F�C�g
				keyUpDownMessage(keySet.m_uMod_Copy,keySet.m_uVK_Copy,KEY_DOWN,hWnd);
				keyUpDownMessage(keySet.m_uMod_Copy,keySet.m_uVK_Copy,KEY_UP,hWnd);
				Sleep(keySet.m_nCopyWait);//�E�F�C�g
			}
			else if(keySet.m_nMessage == 2) {
				::PostMessage(hWnd,key.m_copyMessage.Msg,key.m_copyMessage.wParam,key.m_copyMessage.lParam);//���b�Z�[�W����
//				::SendMessage(hWnd,WM_COPY,NULL,NULL);//���b�Z�[�W����
			}
			//�f�o�b�O���O����
			if(m_ini.m_nDebug) {
				CString strText;
				strText.Format(_T("getSelectString %d %s\n"),keySet.m_nMessage,strSelect.GetString());
				CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
			}
		}
	}
	for(i = 0; i <= m_ini.m_etc.m_nCopyWaitCnt; i++) {
		if(m_clipbord.getClipboardText(strSelect)) break;
		Sleep(key.m_nCopyWait);//�E�F�C�g
	}
	return strSelect;
}

//---------------------------------------------------
//�֐���	execKeyMacro(CString strKeyMacro)
//�@�\		�L�[�{�[�h�}�N�������s
//---------------------------------------------------
void CCharu3App::execKeyMacro(CString strKeyMacro)
{
	TCHAR *szKeyMacro,strKeyCode[256],*szSplit;
	int nCount,nSleep,nKey[16],i,nKeyCount;
	szKeyMacro  = strKeyMacro.GetBuffer(strKeyMacro.GetLength());

	//�\��t����̃X���[�v���擾
	szSplit = UStringWork::awk(szKeyMacro,strKeyCode,_countof(strKeyCode),3,_T(';'));
	if(szSplit)  {
		nSleep = _ttoi(strKeyCode);
		if(nSleep > 10000) nSleep = 10000;
	}
	else         nSleep = 0;

	//�\��t���񐔂��擾
	szSplit = UStringWork::awk(szKeyMacro,strKeyCode,_countof(strKeyCode),2,_T(';'));
	if(szSplit)  {
		nCount = _ttoi(strKeyCode);
		if(nCount > 256) nCount = 256;
		*szSplit = NULL;
	}
	else         nCount = 1;

	//�L�[�z����擾�@�ő��16�L�[
	for(i = 0; i <= 15; i++) {
		UStringWork::awk(szKeyMacro,strKeyCode,_countof(strKeyCode),i+1,_T(','));
		if(_tcsclen(strKeyCode) > 0) nKey[i] = getKeycode(strKeyCode);
		else break;
	}
	nKeyCount = i - 1;//�L�[�̌�

	//�L�[������
	for(;nCount > 0; nCount--) {
		for(i = 0;i <= nKeyCount; i++) {
			keybd_event(nKey[i],0,KEYEVENTF_EXTENDEDKEY,0);//����
		}
	}
	for(i = 0;i <= nKeyCount; i++) {
		keybd_event(nKey[i],0,KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);//�L�[�𗣂�
	}
	Sleep(nSleep);
}

//---------------------------------------------------
//�֐���	execData()
//�@�\		�L�[�{�[�h�}�N�������s���Ȃ���f�[�^��\��t����
//---------------------------------------------------
void CCharu3App::execData(CString strPaste,COPYPASTE_KEY key,HTREEITEM hTargetItem,HWND hWnd)
{
	HTREEITEM hMacroItem;
	CString strCut,strKeyMacro;
	int nStart = 0,nFoundStart,nFoundEnd,nMacroLenS,nMacroLenE;

	//�r�t�H�A�L�[����
	if(hTargetItem)	hMacroItem  = m_pTree->serchParentOption(hTargetItem,_T("beforkey"));
	else			hMacroItem = NULL;
	if(hMacroItem) {
		STRING_DATA *macroDataPtr = m_pTree->getDataPtr(hMacroItem);
		strKeyMacro = m_pTree->getDataOptionStr(macroDataPtr->m_strMacro,_T("beforkey"));
		if(strKeyMacro != "") execKeyMacro(strKeyMacro);
	}
	
	//�}�N����u��
	strPaste.Replace(_T("<charu2MACRO_KEY>"),MACRO_START_KEY);
	strPaste.Replace(_T("</charu2MACRO_KEY>"),MACRO_END_KEY);

	nMacroLenS = _tcsclen(MACRO_START_KEY);
	nMacroLenE = _tcsclen(MACRO_END_KEY);

	do {
		nFoundStart = strPaste.Find(MACRO_START_KEY,nStart);
		strKeyMacro = "";
		//�L�[�}�N�����܂�
		if(nFoundStart >= 0) {
			nFoundEnd = strPaste.Find(MACRO_END_KEY,nFoundStart);
			strCut = strPaste.Mid(nStart,nFoundStart - nStart);
			if(nFoundEnd > nFoundStart)	{
				nStart = nFoundEnd + nMacroLenE;
					strKeyMacro = strPaste.Mid(nFoundStart + nMacroLenS,nFoundEnd - nFoundStart - nMacroLenS);
			}
			else	nStart = nFoundStart + nMacroLenS;
		}
		else 	strCut = strPaste.Mid(nStart,strPaste.GetLength() - nStart);
		//�ʂɓ\��t������
		if(strCut != _T("")) {
//			pasteData(strCut,key,hWnd);//�\��t��
//			m_strlClipBackup = strCut;
			
			TCHAR strWindowName[1024];
			*strWindowName = (char)NULL;
			CString strWinName;
			int i;
			COPYPASTE_KEY keySet;

			GetWindowText(m_focusInfo.m_hActiveWnd,strWindowName,_countof(strWindowName));
			strWinName = strWindowName;

			keySet.m_nMessage = 0;
			for(i = 0; keySet.m_nMessage > -1 ;i++) {
				keySet = m_ini.getAppendKeyInit(strWinName,i);
				if(keySet.m_nMessage <= -1 && i == 0)	keySet = key;
				if(keySet.m_nMessage > -1) {
					pasteData(strCut,keySet,hWnd);//�\��t��
				}
			}
			m_strlClipBackup = strCut;
		}
		//�L�[�}�N�������s
		if(strKeyMacro != _T("")) {
			execKeyMacro(strKeyMacro);
		}
	}while(nFoundStart >= 0);

	//�A�t�^�[�L�[����
	if(hTargetItem)	hMacroItem  = m_pTree->serchParentOption(hTargetItem,_T("afterkey"));
	else			hMacroItem = NULL;
	if(hMacroItem) {
		STRING_DATA *macroDataPtr = m_pTree->getDataPtr(hMacroItem);
		strKeyMacro = m_pTree->getDataOptionStr(macroDataPtr->m_strMacro,_T("afterkey"));
		if(strKeyMacro != _T("")) execKeyMacro(strKeyMacro);
	}
}

//---------------------------------------------------
//�֐���	pasteData()
//�@�\		�f�[�^��\��t����
//---------------------------------------------------
void CCharu3App::pasteData(CString strPaste,COPYPASTE_KEY key,HWND hWnd)
{
	if(m_isStockMode) m_strlClipBackup = strPaste;
	m_clipbord.setClipboardText(strPaste);

	if(key.m_nMessage == 0) {//�C�x���g����
		keyUpDown(key.m_uMod_Paste,key.m_uVK_Paste,KEY_DOWN);
		keyUpDown(key.m_uMod_Paste,key.m_uVK_Paste,KEY_UP);
		Sleep(key.m_nPasteWait);
	}
	else if(key.m_nMessage == 1) {//Charu2Pro����
		keyUpDownC2(key.m_uMod_Paste,key.m_uVK_Paste,KEY_DOWN);
		keyUpDownC2(key.m_uMod_Paste,key.m_uVK_Paste,KEY_UP);
		Sleep(key.m_nPasteWait);
	}
	else if(key.m_nMessage == 2) {
		::PostMessage(hWnd,key.m_pasteMessage.Msg,key.m_pasteMessage.wParam,key.m_pasteMessage.lParam);//���b�Z�[�W����
//		::PostMessage(hWnd,WM_PASTE,NULL,NULL);//���b�Z�[�W����
	}
	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("pasteData %d %s %x %x active:%x focus:%x\n"),key.m_nMessage,strPaste.GetString(),key.m_uMod_Paste,key.m_uVK_Paste,m_focusInfo.m_hActiveWnd,m_focusInfo.m_hFocusWnd);
		CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}
}

//---------------------------------------------------
//�֐���	keyUpDown()
//�@�\		���z�L�[������
//---------------------------------------------------
void CCharu3App::keyUpDown(UINT uMod,UINT uVKCode,int nFlag)
{
	if(nFlag & KEY_DOWN) {
		if(uMod & MOD_ALT)
			keybd_event(VK_MENU, (BYTE)MapVirtualKey(VK_MENU, 0), KEYEVENTF_EXTENDEDKEY, 0);
		if(uMod & MOD_CONTROL)
			keybd_event(VK_CONTROL, (BYTE)MapVirtualKey(VK_CONTROL, 0), KEYEVENTF_EXTENDEDKEY, 0);
		if(uMod & MOD_SHIFT)
			keybd_event(VK_SHIFT, (BYTE)MapVirtualKey(VK_SHIFT, 0), KEYEVENTF_EXTENDEDKEY, 0);
		
		keybd_event(uVKCode,0,KEYEVENTF_EXTENDEDKEY,0);
	}
	if(nFlag & KEY_UP) {
		keybd_event(uVKCode,0,KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);//�L�[�𗣂�

		if(uMod & MOD_ALT)
			keybd_event(VK_MENU, (BYTE)MapVirtualKey(VK_MENU, 0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		if(uMod & MOD_CONTROL)
			keybd_event(VK_CONTROL, (BYTE)MapVirtualKey(VK_CONTROL, 0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		if(uMod & MOD_SHIFT)
			keybd_event(VK_SHIFT, (BYTE)MapVirtualKey(VK_SHIFT, 0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	}
}

//---------------------------------------------------
//�֐���	keyUpDown()
//�@�\		���z�L�[������
//---------------------------------------------------
void CCharu3App::keyUpDownC2(UINT uMod,UINT uVKCode,int nFlag)
{
	if(nFlag & KEY_DOWN) {
		if(uMod & MOD_ALT)		keybd_event(VK_MENU,0,NULL,0);
		if(uMod & MOD_CONTROL)	keybd_event(VK_CONTROL,0,NULL,0);
		if(uMod & MOD_SHIFT)	keybd_event(VK_SHIFT,0,NULL,0);
		
		keybd_event(uVKCode,0,NULL,0);
	}
	else if(nFlag & KEY_UP) {
		keybd_event(uVKCode,0,KEYEVENTF_KEYUP,0);//�L�[�𗣂�

		if(uMod & MOD_ALT)		keybd_event(VK_MENU,0,KEYEVENTF_KEYUP,0);
		if(uMod & MOD_CONTROL)	keybd_event(VK_CONTROL,0,KEYEVENTF_KEYUP,0);
		if(uMod & MOD_SHIFT)	keybd_event(VK_SHIFT,0,KEYEVENTF_KEYUP,0);
	}
}

//---------------------------------------------------
//�֐���	keyUpDownMessage
//�@�\		���z�L�[������
//---------------------------------------------------
void CCharu3App::keyUpDownMessage(UINT uMod,UINT uVKCode,int nFlag,HWND hWnd)
{
	if(nFlag & KEY_DOWN) {
		if(uMod & MOD_ALT)		::SendMessage(hWnd,WM_KEYDOWN,VK_MENU,1);
		if(uMod & MOD_CONTROL)	::SendMessage(hWnd,WM_KEYDOWN,VK_CONTROL,1);
		if(uMod & MOD_SHIFT)	::SendMessage(hWnd,WM_KEYDOWN,VK_SHIFT,1);

		::SendMessage(hWnd,WM_KEYDOWN,uVKCode,1);
	}
	if(nFlag & KEY_UP) {
		::SendMessage(hWnd,WM_KEYUP,uVKCode,1);

		if(uMod & MOD_ALT)		::SendMessage(hWnd,WM_KEYUP,VK_MENU,1);
		if(uMod & MOD_CONTROL)	::SendMessage(hWnd,WM_KEYUP,VK_CONTROL,1);
		if(uMod & MOD_SHIFT)	::SendMessage(hWnd,WM_KEYUP,VK_SHIFT,1);
	}
}
//---------------------------------------------------
//�֐���	convertMacro(CString strSourceData)
//�@�\		�}�N���������u��
//---------------------------------------------------
CString CCharu3App::convertMacro(STRING_DATA *SourceData,CString strSelect,CString strClip,CString strSoftName)
{
	SetCurrentDirectory(m_ini.m_strAppPath);

	if(strSoftName == _T("no")) return SourceData->m_strData;

	CString strSourceData;
	if(strSoftName != DAT_FORMAT) {
		if(m_pTree->convertMacroPlugin(SourceData,&strSourceData,strSelect,strClip,strSoftName))
			return strSourceData;
	}
	
	int nStart,nEnd,nMove,nTagStart,nTagEnd;

	CString strTag,strTagEnd,strBuff,strBuff2,strTime;
	CString strShellStart = _T("$SHELL<"),strShellEnd = _T(">$SHELL");
	CString strRelateStart = _T("$RELATE<"),strRelateEnd = _T(">$RELATE");
	CString strPlugStart = _T("$PLUG-IN<"),strPlugEnd = _T(">$PLUG-IN");

	//�}�N���돈���΍�
	TCHAR strMark[2];
	strMark[0] = 0x7f;
	strMark[1] = NULL;
	bool isSelect = false,isClip = false;

	CString strRepBuff1,strRepBuff2,strRepBuff3,strRepBuff4,strRepBuff5;
	strRepBuff1 = _T("<charuMACR@c3mO@>");
	strRepBuff2 = _T("</charuMACR@c3mO@>");
	strRepBuff3 = _T("@c3m$@PLUG-IN<");
	strRepBuff4 = _T(">@c3m$@PLUG-IN");
	strRepBuff5 = _T("@c3m$@CLIP");

	if(SourceData->m_strData.Find(_T("$SEL")) != -1) {
		strSelect.Replace(MACRO_START,strRepBuff1);
		strSelect.Replace(MACRO_END,strRepBuff2);
		strSelect.Replace(strPlugStart,strRepBuff3);
		strSelect.Replace(strPlugEnd,strRepBuff4);
		strSelect.Replace(_T("$CLIP"),strRepBuff5);
		isSelect = true;
	}
	if(SourceData->m_strData.Find(_T("$CLIP")) != -1) {
		strClip.Replace(MACRO_START,strRepBuff1);
		strClip.Replace(MACRO_END,strRepBuff2);
		strClip.Replace(strPlugStart,strRepBuff3);
		strClip.Replace(strPlugEnd,strRepBuff4);
		isClip = true;
	}

	CTime cTime = CTime::GetCurrentTime();
	strSourceData = SourceData->m_strData;

	strSourceData.Replace(_T("<charu2MACRO>"),MACRO_START);
	strSourceData.Replace(_T("</charu2MACRO>"),MACRO_END);
	strTag = MACRO_START;
	strTagEnd = MACRO_END;
	nEnd = 0;
	while(1) {
 		nStart = strSourceData.Find(strTag,nEnd);//�^�O�̐擪���擾
		if(nStart == -1) break;
		nEnd = strSourceData.Find(strTagEnd,nStart);//�^�O�̏I�����擾
		if(nEnd == -1) nEnd = strSourceData.GetLength();
		if(nStart > nEnd) break;
		nEnd += strTagEnd.GetLength();

		strBuff = strSourceData.Mid(nStart,nEnd-nStart);//�^�O�����؂�o��
		strBuff2 = strBuff;
		strBuff2.Replace(strTag,_T(""));
		strBuff2.Replace(strTagEnd,_T(""));

		if(strBuff2.Find(_T("$Y")) != -1) {//2000 ����4��
			strTime.Format(_T("%04d"),cTime.GetYear());
			strBuff2.Replace(_T("$Y"),strTime);
		}
		if(strBuff2.Find(_T("$y")) != -1) {//00 ����2��
			strTime.Format(_T("%02d"),cTime.GetYear() - 2000);
			strBuff2.Replace(_T("$y"),strTime);
		}
		if(strBuff2.Find(_T("$WH")) != -1) {//00 �a��2��
			strTime.Format(_T("%02d"),cTime.GetYear() - 1988);
			strBuff2.Replace(_T("$WH"),strTime);
		}

		if(strBuff2.Find(_T("$MM")) != -1) {//May ���p��\�L
			CString strMonth[] = {_T("Jan"),_T("Feb"),_T("Mar"),_T("Apr"),_T("May"),_T("Jun"),_T("Jul"),_T("Aug"),_T("Sep"),_T("Oct"),_T("Nov"),_T("Dec")};
			strBuff2.Replace(_T("$MM"),strMonth[cTime.GetMonth() - 1]);
		}
		if(strBuff2.Find(_T("$M")) != -1) {//05 ��2��
			strTime.Format(_T("%02d"),cTime.GetMonth());
			strBuff2.Replace(_T("$M"),strTime);
		}

		if(strBuff2.Find(_T("$D")) != -1) {//06 ���t����2��
			strTime.Format(_T("%02d"),cTime.GetDay());
			strBuff2.Replace(_T("$D"),strTime);
		}
		if(strBuff2.Find(_T("$ddd")) != -1) {//Saturday �j���p��
			CString strYoubi[] = {_T("Sunday"),_T("Monday"),_T("Tuesday"),_T("Wednesday"),_T("Thursday"),_T("Friday"),_T("Saturday")};
			strBuff2.Replace(_T("$ddd"),strYoubi[cTime.GetDayOfWeek() - 1]);
		}
		if(strBuff2.Find(_T("$dd")) != -1) {//Sat �j���p��ȈՕ\�L
			CString strYoubi[] = {_T("Sun"),_T("Mon"),_T("Tue"),_T("Wed"),_T("Thu"),_T("Fri"),_T("Sat")};
			strBuff2.Replace(_T("$dd"),strYoubi[cTime.GetDayOfWeek() - 1]);
		}
		if(strBuff2.Find(_T("$d")) != -1) {//�y�j �j��
			CString strYoubi[] = {_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("�y")};
			strBuff2.Replace(_T("$d"),strYoubi[cTime.GetDayOfWeek() - 1]);
		}

		if(strBuff2.Find(_T("$H")) != -1) {//21 ����24���ԕ\�L
			strTime.Format(_T("%02d"),cTime.GetHour());
			strBuff2.Replace(_T("$H"),strTime);
		}
		if(strBuff2.Find(_T("$h")) != -1) {//09 ����
			int nHours = cTime.GetHour();
			if(nHours > 11) nHours -= 12;
			strTime.Format(_T("%02d"),nHours);
			strBuff2.Replace(_T("$h"),strTime);
		}
		if(strBuff2.Find(_T("$m")) != -1) {//46 ��
			strTime.Format(_T("%02d"),cTime.GetMinute());
			strBuff2.Replace(_T("$m"),strTime);
		}
		if(strBuff2.Find(_T("$s")) != -1) {//02 �b
			strTime.Format(_T("%02d"),cTime.GetSecond());
			strBuff2.Replace(_T("$s"),strTime);
		}

		if(strBuff2.Find(_T("$ampm")) != -1) {//�ߌ�
			int nHours = cTime.GetHour();
			if(nHours > 11) strTime = _T("�ߌ�");
			else			strTime = _T("�ߑO");
			strBuff2.Replace(_T("$ampm"),strTime);
		}
		if(strBuff2.Find(_T("$AMPM")) != -1) {//PM
			int nHours = cTime.GetHour();
			if(nHours > 11) strTime = _T("PM");
			else			strTime = _T("AM");
			strBuff2.Replace(_T("$AMPM"),strTime);
		}
		if(strBuff2.Find(_T("$SEL")) != -1) {//�I�𕔕�
			strBuff2.Replace(_T("$SEL"),strSelect);
		}
		if(strBuff2.Find(_T("$CLIP")) != -1) {//�I�𕔕�
			strBuff2.Replace(_T("$CLIP"),strClip);
		}
		if(strBuff2.Find(strPlugStart) != -1) {//�v���O�C��
			int nPlugLength = strPlugStart.GetLength();
			int nPlugEndLength = strPlugEnd.GetLength();

			nTagStart = strBuff2.Find(strPlugStart);
			nTagEnd = strBuff2.Find(strPlugEnd);

			if(nTagEnd < nTagStart) {
				strBuff2.Delete(nTagStart,nPlugLength);
			}
			else {

				HMODULE hDLL;
				CharuPlugIn pPlugIn;

				while(strBuff2.Find(strPlugStart) != -1) {
					int nPlugStart = strBuff2.Find(strPlugStart) + nPlugLength;//�J�n�ʒu�擾
					int nPlugEnd = strBuff2.Find(strPlugEnd,nPlugStart);//�I���ʒu�擾
					if(nPlugEnd == -1) nPlugEnd = strBuff2.GetLength();

					CString strPlug,strDllName,strTmp;
					TCHAR *strRet;
					strPlug = strBuff2.Mid(nPlugStart,nPlugEnd - nPlugStart);
					int nComma = strPlug.Find(_T(","),0);
					if(nComma >= 0){
						strDllName = strPlug.Left(nComma);
						strTmp = strPlug.Right(strPlug.GetLength() - nComma - 1);
					}
					else{
						strDllName = _T("");
						strTmp = strPlug;
					}

					//DLL�����[�h
					hDLL = LoadLibrary(strDllName);
					if(hDLL){
						pPlugIn = (CharuPlugIn)GetProcAddress(hDLL,"CharuPlugIn");
						if(pPlugIn) {
							int nSize = strTmp.GetLength() * 5;
							if(nSize < 10240) nSize = 10240;
							strRet = new TCHAR[nSize];
							if(strRet) {
								CString strPluginRet;
								int nTmpLen = strTmp.GetLength()+1;
								if(strTmp.Find(_T("@c3m")) >= 0) {
									strTmp.Replace(_T("@c3mO@"),_T("O"));
									strTmp.Replace(_T("@c3m$@"),_T("$"));
								}
//								int nBuffSize = nSize * sizeof(TCHAR);

								pPlugIn(strTmp.GetBuffer(strTmp.GetLength()+1),strRet,nSize,SourceData,(void*)&m_focusInfo);
								strPluginRet = strRet;
								strPluginRet.Replace(MACRO_START,strRepBuff1);
								strPluginRet.Replace(MACRO_END,strRepBuff2);
								strPluginRet.Replace(strPlugStart,strRepBuff3);
								strPluginRet.Replace(strPlugEnd,strRepBuff4);

								strBuff2.Delete(nPlugStart,strDllName.GetLength()+nTmpLen);
								strBuff2.Insert(nPlugStart,strPluginRet);//�u��
								delete []strRet;
							}
						}
						else{
							CString strRes;
							strRes.LoadString(APP_MES_NO_ENTRY_FUNC);
							AfxMessageBox(strRes,MB_ICONEXCLAMATION | MB_SYSTEMMODAL,0);
						}
						FreeLibrary(hDLL);
					}
					else{
						CString strMessage,strRes;
						strRes.LoadString(APP_MES_NOT_FOUND);
						strMessage.Format(strRes,strDllName);
						AfxMessageBox(strMessage,MB_ICONEXCLAMATION | MB_SYSTEMMODAL,0);
					}
					strBuff2.Delete(strBuff2.Find(strPlugStart),nPlugLength);
					strBuff2.Delete(strBuff2.Find(strPlugEnd),nPlugEndLength);
				}
			}
		}
		if(strBuff2.Find(strRelateStart) != -1) {//�֘A�t��
			int nRelateLength = strRelateStart.GetLength();
			nTagStart = strBuff2.Find(strRelateStart);
			nTagEnd = strBuff2.Find(strRelateEnd);

			if(nTagEnd < nTagStart) {
				strBuff2.Delete(nTagStart,nRelateLength);
			}
			else {
				while(strBuff2.Find(strRelateStart) != -1) {
					int nMacroStart = strBuff2.Find(strRelateStart);
					int nRelateStart = nMacroStart + nRelateLength;//�J�n�ʒu�擾
					int nRelateEnd = strBuff2.Find(strRelateEnd,nRelateStart);//�I���ʒu�擾
					if(nRelateEnd == -1) nRelateEnd = strBuff2.GetLength();

					CString strRelate;
					strRelate = strBuff2.Mid(nRelateStart,nRelateEnd - nRelateStart);//�R�}���h���C���؂�o��

					ShellExecute(NULL,NULL,strRelate,NULL,NULL,SW_SHOWNORMAL);
					strBuff2.Delete(nMacroStart,nRelateLength + nRelateEnd  - nMacroStart);//�^�O���폜
				}
			}
		}
		if(strBuff2.Find(strShellStart) != -1) {//�V�F��
			STARTUPINFO sinf;
			PROCESS_INFORMATION pri;
			ZeroMemory(&sinf,sizeof(sinf));
			int nShellLength = strShellStart.GetLength();
			int nRelateLength = strRelateStart.GetLength();

			nTagStart = strBuff2.Find(strShellStart);
			nTagEnd = strBuff2.Find(strShellEnd);

			if(nTagEnd < nTagStart) {
				strBuff2.Delete(nTagStart,nShellLength);
			}
			else {
				while(strBuff2.Find(strShellStart) != -1) {
					int nMacroStart = strBuff2.Find(strShellStart);
					int nShellStart = nMacroStart + nShellLength;//�J�n�ʒu�擾
					int nShellEnd = strBuff2.Find(strShellEnd,nShellStart);//�I���ʒu�擾
					if(nShellEnd == -1) nShellEnd = strBuff2.GetLength();

					CString strShell;
					strShell = strBuff2.Mid(nShellStart,nShellEnd - nShellStart);//�R�}���h���C���؂�o��

					CreateProcess(NULL,strShell.GetBuffer(strShell.GetLength()),NULL,NULL,FALSE,0,NULL,NULL,&sinf,&pri);//�N��
					WaitForInputIdle(pri.hProcess,INFINITE);
					strBuff2.Delete(nMacroStart,nShellLength + nShellEnd  - nMacroStart);//�^�O���폜
				}
			}
		}

		
		nMove = strBuff2.GetLength();// - strBuff.GetLength();
		strSourceData.Delete(nStart,strBuff.GetLength());
		strSourceData.Insert(nStart,strBuff2);
		nEnd = nStart + nMove;
	}
	if(strSourceData.Find(_T("@c3m")) >= 0) {
		strSourceData.Replace(_T("@c3mO@"),_T("O"));
		strSourceData.Replace(_T("@c3m$@"),_T("$"));
	}
/*	strSourceData.Replace(strRepBuff2,MACRO_END);
	strSourceData.Replace(strRepBuff3,strPlugStart);
	strSourceData.Replace(strRepBuff4,strPlugEnd);*/

	return strSourceData;
}

//---------------------------------------------------
//�֐���	changeClipBord()
//�@�\		���C���t���[���ŃN���b�v�{�[�h�̕ύX�����m�A
//          �N���b�v�{�[�h�̓��e�����X�g�ɒǉ�����
//---------------------------------------------------
void CCharu3App::changeClipBord(CString strClipBord)
{ 
	if(m_isWordBugClipCheck) {
		m_isWordBugClipCheck = false;
		return;
	}
	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("changeClipBord \"%s\"\n"),strClipBord.GetString());
		CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}
	//�A���ŋ�̃N���b�v�{�[�h�X�V�C�x���g���N����̂ő΍�
	//2007/10/27-20:20:19-------------------
	static int nEmptyCnt = 0;
	if(strClipBord == "")  {
/*		if(nEmptyCnt > 5) {
			Sleep(3000);
			nEmptyCnt = 0;
			//�f�o�b�O���O����
			if(m_ini.m_nDebug) {
				CString strText;
				strText.Format(_T("changeClipBord Sleep \"%s\"\n"),strClipBord);
				CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
			}
		}
		else {
			nEmptyCnt++;
		}
*/		return;
	}
	nEmptyCnt = 0;
	CString strText;

	//�T�C�Y�m�F
	int nLimit;
	CString strTitle;
	nLimit = m_ini.m_key.m_nHistoryLimit;

	//�^�C�g���ʂɏ������Ă݂�
	strTitle = CGeneral::getWindowTitle(::GetForegroundWindow());
	if(strTitle != "") {
		CHANGE_KEY key;
		key = m_ini.getAppendKeyInit2(strTitle);
		if(key.m_nMatch >= 0) nLimit = key.m_nHistoryLimit;
	}
	nLimit = nLimit * 1024;//KB�Ȃ̂�1024�{
	if(nLimit >= 0 && nLimit < strClipBord.GetLength()) return;//������傫�������甲����


	if(m_nPhase == PHASE_IDOL && /*strClipBord != m_strlClipBackup &&*/ strClipBord != "") {
		m_nPhase = PHASE_LOCK;
		STRING_DATA data;
		m_pTree->initStringData(&data);

		data.m_cKind    = KIND_ONETIME;
		data.m_strData  = strClipBord;
		data.m_strTitle = "";
		data.m_cIcon = KIND_DEFAULT;
		//�^�C�}�[��~
		KillTimer(m_pMainWnd->m_hWnd,TIMER_ACTIVE);
		KillTimer(m_pMainWnd->m_hWnd,TIMER_SELF_DIAGNOSIS);
		//�f�o�b�O���O����
		if(m_ini.m_nDebug) {
			CString strText;
			strText.Format(_T("clipboard record \"%s\"\n"),strClipBord.GetString());
			CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
		}
		//�X�g�b�N���[�h����
		if(m_isStockMode) {
			if((m_ini.m_fifo.m_nDuplication && strClipBord != m_strlClipBackup) || !m_ini.m_fifo.m_nDuplication) {
				if(m_ini.m_fifo.m_nCopySound) PlaySound(m_ini.m_fifo.m_strWaveFile, NULL, SND_ASYNC|SND_FILENAME);
				m_pTree->addData(NULL,data);//���X�g�ɒǉ�
			}
		}
		//�����t�H���_����
		m_pTree->addDataToRecordFolder(data,m_strlClipBackup);
		if(m_isStockMode)
			SetTimer(m_pMainWnd->m_hWnd,TIMER_ACTIVE,m_ini.m_etc.m_nActiveTime,NULL);
		//���Ȑf�f�^�C�}�[�Z�b�g
		if(m_ini.m_etc.m_nSelfDiagnosisTime)
			SetTimer(m_pMainWnd->m_hWnd,TIMER_SELF_DIAGNOSIS,m_ini.m_etc.m_nSelfDiagnosisTime,NULL);
	
		m_nPhase = PHASE_IDOL;
	}
	m_strlClipBackup = strClipBord;
}

//---------------------------------------------------
//�֐���	fifoClipbord()
//�@�\		�X�g�b�N���[�h����
//---------------------------------------------------
void CCharu3App::fifoClipbord()
{
	if(!m_isStockMode || m_nPhase != PHASE_IDOL) return;
	UnregisterHotKey(NULL,HOTKEY_PASTE);//�z�b�g�L�[����
	STRING_DATA data;
	HTREEITEM hTarget;

	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CGeneral::writeLog(m_ini.m_strDebugLog,_T("fifoClipbord\n"),_ME_NAME_,__LINE__);
	}
	m_nPhase = PHASE_LOCK;
	//����ꏈ��
	if(m_ini.m_fifo.m_nFifo) {
		hTarget = m_pTree->getOneTimeText(m_ini.m_fifo.m_nFifo);
		if(hTarget)	{
			data = m_pTree->getData(hTarget);
			m_pTree->deleteData(hTarget);
		}
		if(data.m_strData != "" || !m_ini.m_fifo.m_nNoClear) {
			for(int i = 0; i <= m_ini.m_etc.m_nPasteWaitCnt; i++) {
				//�f�o�b�O���O����
				if(m_ini.m_nDebug) {
					CString strText;
					strText.Format(_T("fifoClipbord text:%s\n"),data.m_strData.GetString());
					CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
				}
				if(m_clipbord.setClipboardText(data.m_strData))	break;//�N���b�v�{�[�h�ɃZ�b�g
				Sleep(50);//�E�F�C�g
			}
			m_strlClipBackup = data.m_strData;		//��r�p������ɃZ�b�g
		}
	}
	if(m_ini.m_fifo.m_nPasteSound) {//���ʉ���炷
		PlaySound(m_ini.m_fifo.m_strWaveFile, NULL, SND_ASYNC|SND_FILENAME);
	}
	//���z�I��Ctrl+V������
	UINT uVkCode = NULL;
	if(m_keySet.m_nMessage == 1) {//Charu2Pro�����C�x���g����
		keyUpDownC2(m_keySet.m_uMod_Paste,m_keySet.m_uVK_Paste,KEY_DOWN);

	}
	else {
		keyUpDown(m_keySet.m_uMod_Paste,m_keySet.m_uVK_Paste,KEY_DOWN);
	}
	//�L�[�𗣂�����������
	if(::GetAsyncKeyState(VK_RCONTROL) < 0 || ::GetAsyncKeyState(VK_LCONTROL) < 0 || ::GetAsyncKeyState(VK_CONTROL) < 0)
		uVkCode |= VK_CONTROL;
	if(::GetAsyncKeyState(VK_RSHIFT) < 0 || ::GetAsyncKeyState(VK_LSHIFT) < 0 || ::GetAsyncKeyState(VK_SHIFT) < 0)
		uVkCode |= VK_SHIFT;
	if(::GetAsyncKeyState(VK_RMENU) < 0 || ::GetAsyncKeyState(VK_LMENU) < 0 || ::GetAsyncKeyState(VK_MENU) < 0)
		uVkCode |= VK_MENU;

	keybd_event(uVkCode, (BYTE)MapVirtualKey(uVkCode, 0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

	RegisterHotKey(NULL,HOTKEY_PASTE,m_keySet.m_uMod_Paste,m_keySet.m_uVK_Paste);//�y�[�X�g�L�[
	//�ꎞ���ڂ�����������X�g�b�N���[�hOFF
	if(m_ini.m_fifo.m_nAllClearOff && !m_pTree->getOneTimeText(m_ini.m_fifo.m_nFifo)) toggleStockMode();
	m_nPhase = PHASE_IDOL;
}

//---------------------------------------------------
//�֐���	toggleStockMode()
//�@�\		�X�g�b�N���[�h�ؑ�
//---------------------------------------------------
void CCharu3App::toggleStockMode()
{
	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("toggleStockMode %d\n"),!m_isStockMode);
		CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}

	if(m_isStockMode) {
		m_isStockMode = false;
		KillTimer(m_pMainWnd->m_hWnd,TIMER_ACTIVE);
		UnregisterHotKey(NULL,HOTKEY_PASTE);
		m_pMainFrame->changeTrayIcon(m_isStockMode);
		if(m_ini.m_fifo.m_nOffClear)	m_pTree->clearOneTime(m_pTree->GetRootItem());//�ꎞ���ڃN���A
	}
	else {
		m_isStockMode = true;
		m_strlClipBackup = "";
		setAppendKeyInit(::GetForegroundWindow(),&m_keySet);
		RegisterHotKey(NULL,HOTKEY_PASTE,theApp.m_keySet.m_uMod_Paste,theApp.m_keySet.m_uVK_Paste);//�y�[�X�g�L�[
		//�Ď��^�C�}�[�Z�b�g
		if(m_ini.m_etc.m_nActiveTime)
			SetTimer(m_pMainWnd->m_hWnd,TIMER_ACTIVE,m_ini.m_etc.m_nActiveTime,NULL);
		m_pMainFrame->changeTrayIcon(m_isStockMode);
	}
}

//---------------------------------------------------
//�֐���	getPopupPos(POINT *pPos,int nPodType,HWND hTarget)
//�@�\		�ݒ�ɉ������|�b�v�A�b�v�ʒu���擾
//---------------------------------------------------
void CCharu3App::getPopupPos(POINT *pPos,int nPosType)
{
	RECT rectDeskTop;

	//�^�[�Q�b�g�̃n���h������邽�߂Ɏ��s
	CGeneral::getCaretPos(pPos,&m_focusInfo);//�L�����b�g�ʒu���擾

	//�f�X�N�g�b�v�̃T�C�Y���擾
	if(nPosType > POPUP_POS_MOUSE) {
		HWND hDeskTop = GetDesktopWindow();
		::GetWindowRect(hDeskTop,&rectDeskTop);
	}
	switch(nPosType) {
	case POPUP_POS_MOUSE:
		GetCursorPos(pPos);
		break;
	case POPUP_POS_LEFT_U:
		pPos->x = 0;
		pPos->y = 0;
		break;
	case POPUP_POS_RIGHT_U:
		pPos->x = rectDeskTop.right - m_ini.m_DialogSize.x;
		pPos->y = 0;
		break;
	case POPUP_POS_CENTOR:
		pPos->x = rectDeskTop.right / 2 - m_ini.m_DialogSize.x / 2;
		pPos->y = rectDeskTop.bottom / 2 - m_ini.m_DialogSize.y / 2;
		break;
	case POPUP_POS_LEFT_D:
		pPos->x = 0;
		pPos->y = rectDeskTop.bottom - m_ini.m_DialogSize.y;
		break;
	case POPUP_POS_RIGHT_D:
		pPos->x = rectDeskTop.right - m_ini.m_DialogSize.x;
		pPos->y = rectDeskTop.bottom - m_ini.m_DialogSize.y;
		break;
	case POPUP_POS_CARET:
		pPos->x += m_ini.m_pop.m_posCaretHosei.x;
		pPos->y += m_ini.m_pop.m_posCaretHosei.y;
		break;
	}
}

//---------------------------------------------------
//�֐���	cornerPopup()
//�@�\		�f�X�N�g�b�v���ł̃|�b�v�A�b�v����
//---------------------------------------------------
bool CCharu3App::cornerPopup()
{
	//�f�X�N�g�b�v�̃T�C�Y���擾
	RECT rectDeskTop;
	HWND hDeskTop = GetDesktopWindow();
	::GetWindowRect(hDeskTop,&rectDeskTop);
	POINT pos;
	GetCursorPos(&pos);
	int nPosType = 0;
	if(m_ini.m_pop.m_nCornerPopup & 0x02) //����
		if(pos.x  <= m_ini.m_pop.m_nCornerPopupPix && pos.y <= m_ini.m_pop.m_nCornerPopupPix) nPosType = POPUP_POS_LEFT_U;

	if(m_ini.m_pop.m_nCornerPopup & 0x04) //�E��
		if(pos.x >= rectDeskTop.right - m_ini.m_pop.m_nCornerPopupPix && pos.y <= m_ini.m_pop.m_nCornerPopupPix) nPosType = POPUP_POS_RIGHT_U;
		
	if(m_ini.m_pop.m_nCornerPopup & 0x08) //����
		if(pos.x  <= m_ini.m_pop.m_nCornerPopupPix && pos.y >= rectDeskTop.bottom - m_ini.m_pop.m_nCornerPopupPix) nPosType = POPUP_POS_LEFT_D;
		
	if(m_ini.m_pop.m_nCornerPopup & 0x10) //�E��
		if(pos.x >= rectDeskTop.right - m_ini.m_pop.m_nCornerPopupPix && pos.y >= rectDeskTop.bottom - m_ini.m_pop.m_nCornerPopupPix) nPosType = POPUP_POS_RIGHT_D;
		
	if(nPosType) {
		if(!m_isCornerPopup) {
			m_isCornerPopup = true;
			return false;
		}
		getPopupPos(&pos,nPosType);
		popupTreeWindow(pos,m_ini.m_pop.m_nSelectSW);//�|�b�v�A�b�v
		m_isCornerPopup = false;
		return true;
	}
	else m_isCornerPopup = false;
	return false;
}

//---------------------------------------------------
//�֐���	resetTreeDialog()
//�@�\		�c���[�_�C�A���O���č\�z
//---------------------------------------------------
void  CCharu3App::resetTreeDialog()
{
	list<STRING_DATA> stringList;//�f�[�^���X�g

	//�f�[�^�̏��Ԃ𐳋K�����ăR�s�[
	HTREEITEM hStartItem = m_pTree->GetRootItem();
	m_pTree->tree2List(hStartItem,&stringList,true);

	if(m_pTree)		delete m_pTree;
	if(m_pTreeDlg)	delete m_pTreeDlg;

	m_pTreeDlg = NULL;
	m_pTree = NULL;

	m_pTreeDlg = new CMyTreeDialog;
	m_pTree = new CCharu3Tree;
	if(m_pTree) {
		m_pTreeDlg->setTree(m_pTree);
		m_pTreeDlg->Create(IDD_POPUP_DIALOG,this->m_pMainWnd);

		m_pTree->setImageList(m_ini.m_IconSize,theApp.m_ini.m_visual.m_strResourceName,m_ini.m_strAppPath);
		m_pTree->setInitInfo(&m_ini.m_nTreeID,&m_ini.m_nSelectID,&m_ini.m_nRecNumber);//ID�����l��ݒ�

		//�f�[�^�݊��v���O�C����������
		if(!m_pTree->setPlugin(m_ini.m_strRwPluginFolder)){
			m_ini.m_strRwPluginFolder = m_ini.m_strAppPath + "rw_plugin";
			m_pTree->setPlugin(m_ini.m_strRwPluginFolder);
			m_ini.writeEnvInitData();
		}
		m_pTree->m_MyStringList = stringList;
		m_pTree->CWnd::LockWindowUpdate();
		m_pTree->copyData(ROOT,TVI_ROOT,&m_pTree->m_MyStringList);//�c���[�Ƀf�[�^���Z�b�g
		m_pTree->CWnd::UnlockWindowUpdate();

		setAppendHotKey();
	}
}

//---------------------------------------------------
// CCharu3App ���b�Z�[�W �n���h��
//---------------------------------------------------

//---------------------------------------------------
//�֐���	PreTranslateMessage(MSG* pMsg)
//�@�\		���b�Z�[�W�O����
//---------------------------------------------------
BOOL CCharu3App::PreTranslateMessage(MSG* pMsg) 
{
	//�L�[�t�b�N����
	if(pMsg->message == WM_KEY_HOOK) {
		//�f�o�b�O���O����
		if(m_ini.m_nDebug) {
			CString strText;
			strText.Format(_T("WM_KEY_HOOK %d %d\n"),pMsg->wParam,pMsg->lParam);
			CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
		}
		UINT uMod = 0;
		if(m_ini.m_pop.m_nDoubleKeyPOP) {
			switch(m_ini.m_pop.m_nDoubleKeyPOP) {
				case 1:	uMod = MOD_SHIFT;break;
				case 2:	uMod = MOD_CONTROL;break;
				case 3:	uMod = MOD_ALT;break;
			}
			if(CGeneral::mod2VK(uMod) == pMsg->wParam) {
				pMsg->message = WM_HOTKEY;
				pMsg->wParam  = HOTKEY_POPUP;
			}
		}
		if(m_ini.m_pop.m_nDoubleKeyFIFO) {
			switch(m_ini.m_pop.m_nDoubleKeyFIFO) {
				case 1:	uMod = MOD_SHIFT;break;
				case 2:	uMod = MOD_CONTROL;break;
				case 3:	uMod = MOD_ALT;break;
			}
			if(CGeneral::mod2VK(uMod) == pMsg->wParam) {
				pMsg->message = WM_HOTKEY;
				pMsg->wParam  = HOTKEY_FIFO;
			}
		}
		//�t�H���_�z�b�g�L�[����
		int nSize = m_hotkeyVector.size();
		for(int i = 0; i < nSize; i++) {
			if(m_hotkeyVector[i].m_isDoubleClick) {
				if(CGeneral::mod2VK(m_hotkeyVector[i].m_uModKey) == pMsg->wParam) {
					pMsg->message = WM_HOTKEY;
					pMsg->wParam  = HOT_ITEM_KEY + i;
					break;
				}
			}
		}
	}
	
	if(m_nPhase == PHASE_IDOL && pMsg->message == WM_TIMER) {
		//�Ď��^�C�}�[����
		if(pMsg->wParam == TIMER_ACTIVE && m_isStockMode) {
			if(setAppendKeyInit(::GetForegroundWindow(),&m_keySet)) {
				UnregisterHotKey(NULL,HOTKEY_PASTE);
				RegisterHotKey(NULL,HOTKEY_PASTE,theApp.m_keySet.m_uMod_Paste,theApp.m_keySet.m_uVK_Paste);
			}
		}
		//���Ȑf�f�^�C�}�[
		else if(pMsg->wParam == TIMER_SELF_DIAGNOSIS) {
			m_clipbord.resetClipView(this->m_pMainWnd->m_hWnd);
			//�f�o�b�O���O����
			if(m_ini.m_nDebug) {
				CGeneral::writeLog(m_ini.m_strDebugLog,_T("TIMER_SELF_DIAGNOSIS\n"),_ME_NAME_,__LINE__);
			}
		}
		//�}�E�X�Ď��^�C�}�[
		else if(pMsg->wParam == TIMER_MOUSE) {
			if(cornerPopup()) return FALSE;
		}
	}
	//�z�b�g�L�[����
	if(pMsg->message == WM_HOTKEY) {
		switch(pMsg->wParam) {
		case HOTKEY_POPUP://�|�b�v�A�b�v
			//�f�o�b�O���O����
			if(m_ini.m_nDebug) {
				CGeneral::writeLog(m_ini.m_strDebugLog,_T("HOTKEY_POPUP\n"),_ME_NAME_,__LINE__);
			}
			if(m_ini.m_pop.m_nDoubleKeyPOP) {//�_�u���L�[�N���b�N
				DWORD dwTime = timeGetTime();
				if(static_cast<int>(dwTime - m_dwDoubleKeyPopTime) > m_ini.m_pop.m_nDCKeyPopTime) {
					m_dwDoubleKeyPopTime = dwTime;
					return FALSE;
				}
			}
			if(m_nPhase == PHASE_IDOL) {
				POINT pos;
				getPopupPos(&pos,m_ini.m_pop.m_nPopupPos);//�|�b�v�A�b�v�ʒu���擾
				if(m_ini.m_nDebug) {
					CString strText;
					strText.Format(_T("getPopupPos %d %d\n"),pos.x,pos.y);
					CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
				}
				if(m_ini.m_pop.m_nOutRevice)	reviseWindowPos(&pos);
				if(m_ini.m_nDebug) {
					CString strText;
					strText.Format(_T("reviseWindowPos %d %d\n"),pos.x,pos.y);
					CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
				}
				popupTreeWindow(pos,m_ini.m_pop.m_nSelectSW);//�|�b�v�A�b�v
			}
			else	m_isCloseKey = true;//����X�C�b�`�����Ă���
			return FALSE;
			break;
		case HOTKEY_FIFO://�X�g�b�N���[�h�؂�ւ�
			//�f�o�b�O���O����
			if(m_ini.m_nDebug) {
				CGeneral::writeLog(m_ini.m_strDebugLog,_T("HOTKEY_FIFO\n"),_ME_NAME_,__LINE__);
			}
			if(m_ini.m_pop.m_nDoubleKeyFIFO) {//�_�u���L�[�N���b�N
				DWORD dwTime = timeGetTime();
				if(static_cast<int>(dwTime - m_dwDoubleKeyFifoTime) > m_ini.m_pop.m_nDCKeyFifoTime) {
					m_dwDoubleKeyFifoTime = dwTime;
					return FALSE;
				}
			}
			toggleStockMode();
			return FALSE;
			break;
		case HOTKEY_PASTE://����FIFO����
			//�f�o�b�O���O����
			if(m_ini.m_nDebug) {
				CGeneral::writeLog(m_ini.m_strDebugLog,_T("HOTKEY_PASTE\n"),_ME_NAME_,__LINE__);
			}
			if(m_ini.m_etc.m_nWordBugSW && !m_ini.m_fifo.m_nDuplication) m_isWordBugClipCheck = true;

			fifoClipbord();
			return FALSE;
			break;
		//�z�b�g�A�C�e��
		default:
			if(m_nPhase == PHASE_IDOL) {
				int nTarget = pMsg->wParam - HOT_ITEM_KEY;
				playHotItem(nTarget);
				return FALSE;
			}
			break;
		}
	}
	//����X�C�b�`�������ĂāA�L�[�A�b�v�̎��Ƀ|�b�v�A�b�v�����
	//(ALT�̎��ɑΏۃA�v���̃��j���[���J�����Ⴄ�̂�h��)
	else if((pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP) && m_isCloseKey) {
		m_pTreeDlg->PostMessage(WM_TREE_CLOSE,IDCANCEL,NULL);
		m_isCloseKey = false;
	}
	//�|�b�v�A�b�v������閽�߂������̂ŕ���
	if(pMsg->message == WM_TREE_CLOSE && m_nPhase == PHASE_POPUP) {
		closeTreeWindow(pMsg->wParam);
		return FALSE;
	}
	return CWinApp::PreTranslateMessage(pMsg);
}

//---------------------------------------------------
//�֐���	OnAbout()
//�@�\		About�_�C�A���O
//---------------------------------------------------
void CCharu3App::OnAbout() 
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_WEB, OnWeb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAboutDlg::OnWeb()
{
	ShellExecute(NULL, NULL, _T("https://github.com/itagagaki/charu3-SE"), NULL, NULL, SW_SHOWNORMAL);
	EndDialog(IDOK);
}

//---------------------------------------------------
//�֐���	OnHelp()
//�@�\		�w���v
//---------------------------------------------------
void CCharu3App::OnHelp() 
{
	CString strFileName;
	strFileName = m_ini.m_strAppPath + HELP_FILE;
	ShellExecute(NULL,NULL,strFileName,NULL,NULL,SW_SHOWNORMAL);
}

//---------------------------------------------------
//�֐���	OnExit()
//�@�\		�I������
//---------------------------------------------------
void CCharu3App::OnExit() 
{
	stopAppendHotKey();//�ǉ��z�b�g�L�[���~
	stopHotkey();
	//�^�C�}�[��~
	KillTimer(m_pMainWnd->m_hWnd,TIMER_SELF_DIAGNOSIS);
	KillTimer(m_pMainWnd->m_hWnd,TIMER_ACTIVE);
	KillTimer(m_pMainWnd->m_hWnd,TIMER_MOUSE);

	if(m_hLangDll)	FreeLibrary(m_hLangDll);
	m_ini.unHookKey();
	m_pTree->saveDataFile(m_ini.m_strDataFile,m_ini.m_strPluginName,NULL);
	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CGeneral::writeLog(m_ini.m_strDebugLog,_T("OnExit\n"),_ME_NAME_,__LINE__);
	}
}

//---------------------------------------------------
//�֐���	OnOption()
//�@�\		�ݒ�
//---------------------------------------------------
void CCharu3App::OnOption() 
{
	if(m_nPhase != PHASE_IDOL && m_nPhase != PHASE_POPUP) return;
	int nPhase = m_nPhase;
	m_nPhase = PHASE_OPTION;

	//�^�C�}�[��~
	KillTimer(m_pMainWnd->m_hWnd,TIMER_ACTIVE);
	KillTimer(m_pMainWnd->m_hWnd,TIMER_SELF_DIAGNOSIS);
	KillTimer(m_pMainWnd->m_hWnd,TIMER_MOUSE);

	COptMainDialog dlgOption(NULL,m_ini.m_nOptionPage);
	CInit iniBkup;

	iniBkup = m_ini;

	if(nPhase == PHASE_IDOL)	m_ini.unHookKey();
	stopAppendHotKey();//�ǉ��z�b�g�L�[���~
	stopHotkey();

	//�f�o�b�O���O����
	if(m_ini.m_nDebug) {
		CGeneral::writeLog(m_ini.m_strDebugLog,_T("OnOption\n"),_ME_NAME_,__LINE__);
	}
	int ret = dlgOption.DoModal();
	if(ret == IDOK) {
		m_ini.writeAllInitData();
		//�f�o�b�O���O����
		if(m_ini.m_nDebug) {
			CGeneral::writeLog(m_ini.m_strDebugLog,_T("writeAllInitData\n"),_ME_NAME_,__LINE__);
		}
	}
	else m_ini = iniBkup;

	if(m_ini.m_visual.m_nResetTree && nPhase != PHASE_POPUP && iniBkup.m_visual.m_strResourceName != m_ini.m_visual.m_strResourceName)
		resetTreeDialog();//�c���[�č\�z

	registerHotkey();
	setAppendHotKey();//�ǉ��z�b�g�L�[��ݒ�
	if(nPhase == PHASE_IDOL)	m_ini.setHookKey(m_hSelfWnd);

	//���Ȑf�f�^�C�}�[�Z�b�g
	if(m_ini.m_etc.m_nSelfDiagnosisTime)
		SetTimer(m_pMainWnd->m_hWnd,TIMER_SELF_DIAGNOSIS,m_ini.m_etc.m_nSelfDiagnosisTime,NULL);
	//�Ď��^�C�}�[�Z�b�g �A�C�h����ԂŃX�g�b�N���[�h���Ȃ�
	if(m_nPhase == PHASE_IDOL && m_isStockMode && m_ini.m_etc.m_nActiveTime)
		SetTimer(m_pMainWnd->m_hWnd,TIMER_ACTIVE,m_ini.m_etc.m_nActiveTime,NULL);

	//�}�E�X�Ď��^�C�}�[
	if(m_ini.m_pop.m_nCornerPopup)
		SetTimer(m_pMainWnd->m_hWnd,TIMER_MOUSE,m_ini.m_pop.m_nCornerPopupTime,NULL);

	m_hActiveKeyWnd = NULL;
	m_nPhase = nPhase;
}

//---------------------------------------------------
//�֐���	OnDataSave()
//�@�\		�ʖ��ۑ�
//---------------------------------------------------
void CCharu3App::OnDataSave() 
{
	CFileDialog *pFileDialog;
	CString strFilter;
	strFilter.LoadString(APP_INF_FILE_FILTER);
	pFileDialog = new CFileDialog(FALSE,_T("*.c3d"),NULL,NULL,strFilter + _T("||"));

	if(pFileDialog) {
		if(IDOK == pFileDialog->DoModal()) {
			//�f�o�b�O���O����
			if(m_ini.m_nDebug) {
				CString strText;
				strText.Format(_T("OnDataSave \"%s\"\n"),pFileDialog->GetPathName().GetString());
				CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
			}
			if(!m_pTree->saveDataFile(pFileDialog->GetPathName(),DAT_FORMAT,NULL)) {
				CString strRes;
				strRes.LoadString(APP_MES_FAILURE_DATA_SAVE);
				AfxMessageBox(strRes,MB_ICONEXCLAMATION,0);
			}
		}
		delete pFileDialog;
	}
}

//---------------------------------------------------
//�֐���	OnOnetimeClear() 
//�@�\		�ꎞ���ڂ��N���A
//---------------------------------------------------
void CCharu3App::OnOnetimeClear() 
{
	m_pTree->clearOneTime(m_pTree->GetRootItem());	
}

//---------------------------------------------------
//�֐���	OnAllLock()
//�@�\		�ꎞ���ڂ�S�ă��b�N
//---------------------------------------------------
void CCharu3App::OnAllLock() 
{
	m_pTree->clearOneTime(m_pTree->GetRootItem(),KIND_LOCK);	
}

//---------------------------------------------------
//�֐���	OnChangData()
//�@�\		�f�[�^�t�@�C���؂�ւ�
//---------------------------------------------------
void CCharu3App::OnChangData() 
{
	CString strFilter,strBuff;

	strFilter = "";
	RW_PLUGIN plugin;
	vector<RW_PLUGIN>::iterator it;
	CString strRes;
	for(it = m_pTree->m_rwPlugin.begin(); it != m_pTree->m_rwPlugin.end(); it++) {
		strRes.LoadString(APP_INF_FILE_FILTER2);
		strBuff.Format(strRes,it->m_strSoftName,it->m_strExtension,it->m_strExtension);
		strFilter = strFilter + strBuff;
	}

	CMyFileDialog *pFileDialog;

	CString strFilterC3;
	strFilterC3.LoadString(APP_INF_FILE_FILTER);
	pFileDialog = new CMyFileDialog(TRUE,_T("c3d"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilterC3 + _T("|") + strFilter,NULL,true);

	if(pFileDialog) {
		pFileDialog->m_ofn.lpstrInitialDir = m_ini.m_strAppPath;
		strRes.LoadString(APP_INF_CHECKBOX_MACRO);
		if(IDOK == pFileDialog->DoModal(TRUE,strRes)) {
			//�f�o�b�O���O����
			if(m_ini.m_nDebug) {
				CString strText;
				strText.Format(_T("OnChangData \"%s\"\n"),pFileDialog->GetPathName().GetString());
				CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
			}
			
			CString strNameBkup,strPlugBkup;
			strNameBkup = m_ini.m_strDataFile;
			m_ini.m_strDataFile = pFileDialog->GetPathName();

			//�K���v���O�C��������
			bool isFound = false;
			OPENFILENAME fileName;
			fileName = pFileDialog->getFileName();

			//�\�t�g����ݒ�
			strPlugBkup = m_ini.m_strPluginName;
			if(fileName.nFilterIndex == 1)
				m_ini.m_strPluginName = DAT_FORMAT;
			else
				m_ini.m_strPluginName = m_pTree->m_rwPlugin[fileName.nFilterIndex - 2].m_strSoftName;

			if(m_pTree->loadDataFileDef(m_ini.m_strDataFile,m_ini.m_strPluginName)) {	
				//�}�N���v���O�C������
				if(pFileDialog->m_isAutoMacro) 	m_ini.m_strMacroPluginName = m_ini.m_strPluginName;
			}
			else {
				strRes.LoadString(APP_MES_UNKNOWN_FORMAT);
				AfxMessageBox(strRes,MB_ICONEXCLAMATION,0);
				m_ini.m_strDataFile = strNameBkup;
				m_ini.m_strPluginName = strPlugBkup;
				m_pTree->loadDataFileDef(m_ini.m_strDataFile,m_ini.m_strPluginName);
			}
			//��Ԃ�ۑ�
			m_ini.writeEnvInitData();
		}
		delete pFileDialog;
	}	
}

//---------------------------------------------------
//�֐���	OnAddData()
//�@�\		�f�[�^�ǉ�
//---------------------------------------------------
void CCharu3App::OnAddData() 
{
	CAddDialog addDialog(NULL,NULL);
	addDialog.setMacroTempate(&m_ini.m_vctMacro,&m_ini.m_vctDataMacro);
	//�ǉ��_�C�A���O���J��
	int nRet = addDialog.DoModal();
}

//---------------------------------------------------
//�֐���	OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
//�@�\		���łɊ��蓖�Ă��Ă��郁�b�Z�[�W���Ȃ��ꍇ
//---------------------------------------------------
BOOL CCharu3App::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if(pHandlerInfo == NULL) {	//�@���łɊ��蓖�Ă��Ă��郁�b�Z�[�W���Ȃ��ꍇ�c�c
		if(nCode == CN_COMMAND) {
			int nMax = m_pTree->m_rwPlugin.size();//�f�[�^�����擾
			for(int i = 0; i <= nMax; i++) {
				if(nID == (UINT)(IDM_MACRO_CHARU + i)) {//�K���v���O�C����ݒ�
					if(i == 0) m_ini.m_strMacroPluginName = DAT_FORMAT;
					else {
						m_ini.m_strMacroPluginName = m_pTree->m_rwPlugin[i-1].m_strSoftName;
					}
					m_ini.writeEnvInitData();
					break;
				}
			}
		}
	}
	
	return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//---------------------------------------------------
//�֐���	OnIconDecide()  
//�@�\		�A�C�R������������
//---------------------------------------------------
void CCharu3App::OnIconDecide() 
{
	CString strRes;
	strRes.LoadString(APP_MES_DECIDE_ICONS);
	int nRet = AfxMessageBox(strRes,MB_YESNO|MB_ICONEXCLAMATION|MB_APPLMODAL);

	if(nRet == IDYES) {
		m_pTree->allIconCheck();
	}		
}

//---------------------------------------------------
//�֐���	OnBbsOpen()
//�@�\		�T�|�[�g�f�����J��
//---------------------------------------------------
void CCharu3App::OnBbsOpen() 
{
	ShellExecute(NULL,NULL,m_ini.m_strBBS,NULL,NULL,SW_SHOWNORMAL);
	
}

//---------------------------------------------------
//�֐���	OnStockStop()
//�@�\		�X�g�b�N���[�h�ؑ�
//---------------------------------------------------
void CCharu3App::OnStockStop() 
{
	toggleStockMode();
}

//---------------------------------------------------
//�֐���	OnVisualFile() 
//�@�\		�r�W���A���ݒ��ǂݍ���
//---------------------------------------------------
void CCharu3App::OnVisualFile() 
{
	CString strFileName;
	CFileDialog *pFileDialog;
	CString strRes;
	strRes.LoadString(APP_INF_FILE_FILTER_VISUAL_PREF);
	pFileDialog = new CFileDialog(TRUE,_T("json"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strRes,NULL);

	CInit iniBkup;
	iniBkup = m_ini;

	if(pFileDialog) {
		pFileDialog->m_ofn.lpstrInitialDir = m_ini.m_strAppPath;
		if(IDOK == pFileDialog->DoModal()) {
			if(m_ini.m_nDebug) {
				CString strText;
				strText.Format(_T("OnVisualFile \"%s\"\n"),pFileDialog->GetPathName().GetString());
				CGeneral::writeLog(m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
			}
			strFileName = pFileDialog->GetPathName();
			nlohmann::json j = nlohmann::json::parse(std::ifstream(strFileName));
			double n;
			std::string s;
			CString cs;
			cs = CGeneral::getPrefCString(j, "IconFile");
			if (!cs.IsEmpty()) m_ini.m_visual.m_strResourceName = cs;
			cs = CGeneral::getPrefCString(j, "FontName");
			if (!cs.IsEmpty()) m_ini.m_visual.m_strFontName = cs;
			if (CGeneral::getPrefNumber(j, "FontSize", n) && n >= 0) m_ini.m_visual.m_nFontSize = static_cast<int>(n);
			s = CGeneral::getPrefString(j, "BackColor");
			if (!s.empty()) m_ini.m_visual.m_nBackColor = Color::parse(s);
			s = CGeneral::getPrefString(j, "BorderColor");
			if (!s.empty()) m_ini.m_visual.m_nBorderColor = Color::parse(s);
			s = CGeneral::getPrefString(j, "TextColor");
			if (!s.empty()) m_ini.m_visual.m_nTextColor = Color::parse(s);
		}
		delete pFileDialog;

		if(m_ini.m_visual.m_nResetTree && iniBkup.m_visual.m_strResourceName != m_ini.m_visual.m_strResourceName)
			resetTreeDialog();
	}
}

//---------------------------------------------------
//�֐���	resetTreeDialog()
//�@�\		�c���[�č\�z
//---------------------------------------------------
void CCharu3App::OnResetTree() 
{
	resetTreeDialog();//�c���[�č\�z
}


