/*----------------------------------------------------------
    Charu3
    �A�v���P�[�V�����p�N���X�̋@�\��`	'2002.11.15 (c)C+ Factory
----------------------------------------------------------*/
#include "stdafx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if false
// TODO: Did not handle this well with Visual Studio 2019.
#include <MULTIMON.H>
#endif
#include <dwmapi.h>
#include <ctype.h>
#include <list>
#include <vector>

#include "Charu3.h"
#include "InitialDialog.h"
#include "EditDialog.h"
#include "OptMainDialog.h"
#include "StringWork.h"

#ifdef _ME_NAME_
#undef _ME_NAME_
#endif
#define _ME_NAME_ _T("Charu3.cpp")

//---------------------------------------------------
//�֐���	MonitorEnumFunc
//�@�\		���j�^�[�񋓃R�[���o�b�N
//---------------------------------------------------
BOOL CALLBACK MonitorEnumFunc(HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM lParam)
{
    MONITORINFOEX MonitorInfoEx = {};
    static int count = 0;

    CArray<RECT, RECT>* pArray;
    pArray = (CArray<RECT, RECT>*)lParam;
    count++;
    MonitorInfoEx.cbSize = sizeof(MonitorInfoEx);
    if (!GetMonitorInfo(hMonitor, &MonitorInfoEx)) {
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
    //ON_COMMAND(IDM_EXIT, OnExit)
    ON_COMMAND(IDM_ABOUT, OnAbout)
    ON_COMMAND(IDM_ISSUES, OnIssues)
    ON_COMMAND(IDM_DISCUSSIONS, OnDiscussions)
    ON_COMMAND(IDM_WIKI, OnWiki)
    ON_COMMAND(IDM_OPTION, OnOption)
    ON_COMMAND(IDM_STOCK_STOP, OnStockStop)
    ON_COMMAND(IDM_ADD_DATA, OnAddData)
    ON_COMMAND(IDM_CHANG_DATA, OnChangData)
    ON_COMMAND(IDM_RESET_TREE, OnResetTree)
    ON_COMMAND(IDM_DATA_SAVE, OnExport)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------
//�֐���	CCharu3App()
//�@�\		CCharu3App�N���X�̃R���X�g���N�^
//---------------------------------------------------
CCharu3App::CCharu3App()// : m_treeDlg(&m_tree)
    : m_isCloseKey(false)
    , m_isStockMode(false)
    , m_nPhase(PHASE_START)
    , m_dwDoubleKeyPopTime(0)
    , m_dwDoubleKeyFifoTime(0)
    , m_hSelectItemBkup(NULL)
{
    m_focusInfo.m_hActiveWnd = NULL;
    m_focusInfo.m_hFocusWnd = NULL;

    int i, nCount = 0, nSize;

    int stKeycode[] = { 0x08,0x09,0x0c,0x0d,0x10,0x11,0x12,0x13,
                        0x14,0x1B,0x20,0x21,0x22,0x23,0x24,0x25,
                        0x26,0x27,0x28,0x29,0x2b,0x2c,0x2d,0x2e,
                        0x2f,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x90,
                        0x91,0x92,0xBB,0xBC, };

    const char* stKeyName[] = { "BS","TAB","CLEAR","ENTER","SHIFT","CTRL","ALT","PAUSE",
                        "CAPSLOCK","ESC","SPACE","PAGEUP","PAGEDOWN","END","HOME","LEFT",
                        "UP","RIGHT","DOWN","SELECT","EXECUTE","PRINTSCREEN","INSERT","DEL",
                        "HELP","NUM*","NUM+","NUMSEP","NUM-","NUM.","NUM/","NUMLOCK",
                        "SCROLLLOCK","NUM=","SEMICOLON","COMMA", };

    //�e���L�[�̐���
    for (i = 0x60; i <= 0x69; i++, nCount++) {
        m_keyStruct[nCount].nKeyCode = i;
        m_keyStruct[nCount].strName.Format(_T("NUM%d"), i - 0x60);
    }
    //�t�@���N�V�����L�[
    for (i = 0x70; i <= 0x87; i++, nCount++) {
        m_keyStruct[nCount].nKeyCode = i;
        m_keyStruct[nCount].strName.Format(_T("F%d"), i - 0x6f);
    }

    nSize = _countof(stKeycode);
    //�g���L�[
    for (i = 0; i < nSize; i++, nCount++) {
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
    if (m_pTreeDlg)	delete m_pTreeDlg;
    if (m_pTree)		delete m_pTree;
    if (m_hMutex)	CloseHandle(m_hMutex);
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
    m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, pszMutexObjectName);
    //	HWND hActiveWnd = ::GetForegroundWindow();
    CGeneral::getFocusInfo(&m_focusInfo);

    if (m_hMutex) {
        CloseHandle(m_hMutex);
        m_hMutex = NULL;
        return FALSE;
    }
    m_hMutex = CreateMutex(FALSE, 0, pszMutexObjectName);

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
    if (m_hLangDll) {
        AfxSetResourceHandle(m_hLangDll);
    }

    //���C���t�H�[���̍쐬
    CMainFrame* pFrame = new CMainFrame;
    m_pMainWnd = pFrame;
    // �t���[�������\�[�X���烍�[�h���č쐬���܂�
    pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);
    pFrame->ShowWindow(SW_HIDE);
    pFrame->UpdateWindow();
    m_pMainFrame = pFrame;

    HWND hWndTop;
    m_hSelfWnd = CWnd::GetSafeOwner_(NULL, &hWndTop);

    m_pTreeDlg->setTree(m_pTree);
    m_pTreeDlg->Create(IDD_DATA_TREE_VIEW, this->m_pMainWnd);
    //	::SetForegroundWindow(hActiveWnd);

    if (init()) {
        CGeneral::setFocusInfo(&m_focusInfo);
        //	setAppendKeyInit(m_focusInfo.m_hActiveWnd,&m_keySet);//�L�[�ݒ��ύX

        m_nPhase = PHASE_IDOL;
        return TRUE;
    }
    else {
        if (m_hLangDll) {
            FreeLibrary(m_hLangDll);
        }
        return FALSE;
    }
}

int CCharu3App::ExitInstance()
{
    if (m_hLangDll) {
        FreeLibrary(m_hLangDll);
    }
    return CWinApp::ExitInstance();
}
//---------------------------------------------------
// �A�v���P�[�V�����̃o�[�W�������Ŏg���� CAboutDlg �_�C�A���O
//---------------------------------------------------
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD), m_strVersion(_T(""))
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    TCHAR fileName[MAX_PATH + 1];
    ::GetModuleFileName(NULL, fileName, MAX_PATH + 1); // TODO: Check return value
    DWORD bufSize = ::GetFileVersionInfoSize(fileName, NULL);
    BYTE* pVersion = new BYTE[bufSize];
    if (::GetFileVersionInfo(fileName, NULL, bufSize, pVersion)) {
        UINT queryLen;
        VS_FIXEDFILEINFO* pFileInfo = nullptr;
        ::VerQueryValue(pVersion, _T("\\"), (void**)&pFileInfo, &queryLen);
        WORD* pLangCode = nullptr;
        ::VerQueryValue(pVersion, _T("\\VarFileInfo\\Translation"), (void**)&pLangCode, &queryLen);
        CString subBlock;
        subBlock.Format(_T("\\StringFileInfo\\%04X%04X\\ProductVersion"), pLangCode[0], pLangCode[1]);
        TCHAR* pStrInfo = nullptr;
        ::VerQueryValue(pVersion, (LPTSTR)(LPCTSTR)subBlock, (void**)&pStrInfo, &queryLen);
        m_strVersion.Format(_T("%s Version %s"), ABOUT_NAME, pStrInfo);
    }
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
bool CCharu3App::init()
{
    m_ini.initialize();

    m_pTree->setImageList(theApp.m_ini.m_IconSize, theApp.m_ini.m_visual.m_strResourceName, m_ini.m_strAppPath);
    m_pTree->setInitInfo(&m_ini.m_nTreeID, &m_ini.m_nSelectID, &m_ini.m_nRecNumber);//ID�����l��ݒ�

    if (m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("start \"%s\"\n"), ABOUT_NAME);
        CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }

    m_pTree->setPlugin(m_ini.m_strRwPluginFolder);

    {
        bool ok = m_pTree->loadDataFileDef(m_ini.m_strDataPath, m_ini.m_strDataFormat);
        while (!ok) {
            CInitialDialog dlg;
            int ret = dlg.DoModal();
            switch (ret) {
            case IDC_NEW:
            {
                CString file = NewFile();
                if (file != _T("")) {
                    m_ini.m_strDataPath = file;
                    m_ini.m_strDataFormat = DAT_FORMAT;
                    m_ini.writeEnvInitData();
                    CString text, caption;
                    (void)text.LoadString(APP_MES_NEWFILE_READY);
                    (void)caption.LoadStringW(AFX_IDS_APP_TITLE);
                    MessageBox(m_hSelfWnd, text, caption, MB_OK | MB_ICONINFORMATION);
                    ok = true;
                }
                break;
            }
            case IDC_OPEN:
                if (SelectFile()) {
                    CString text, caption;
                    (void)text.LoadString(APP_MES_DATA_READY);
                    (void)caption.LoadStringW(AFX_IDS_APP_TITLE);
                    MessageBox(m_hSelfWnd, text, caption, MB_OK | MB_ICONINFORMATION);
                    ok = true;
                }
                break;
            case IDC_QUIT:
            case IDCANCEL:
            default:
                return false;
            }
        }
    }

    if (m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("read data file\"%s\" %s\n"), m_ini.m_strDataPath.GetString(), m_ini.m_strDataFormat.GetString());
        CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }

    //�N���b�v�{�[�h�N���X�̏����� �ύX���m��ݒ�(���C���t���[���Ń��b�Z�[�W���������Ă܂�)
    m_clipboard.getClipboardText(m_strSavedClipboard, m_ini.m_nClipboardRetryTimes, m_ini.m_nClipboardRetryInterval);
    m_clipboard.setParent(this->m_pMainWnd->m_hWnd);
    m_keySet = m_ini.m_key.m_defKeySet;

    TCHAR strKeyLayoutName[256];
    GetKeyboardLayoutName(strKeyLayoutName);
    m_ini.m_keyLayout = LoadKeyboardLayout(strKeyLayoutName, KLF_REPLACELANG);

    if (m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("KeyLayoutName \"%s\"\n"), strKeyLayoutName);
        CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }

    m_ini.setHookKey(m_hSelfWnd);

    //�z�b�g�L�[��ݒ�
    registerHotkey();
    setAppendHotKey();

    return true;
}

bool CCharu3App::SelectFile()
{
    CString strDisplay, strPattern;
    (void)strDisplay.LoadString(APP_INF_FILE_FILTER_C3D_DISPLAY);
    (void)strPattern.LoadString(APP_INF_FILE_FILTER_C3D_PATTERN);
    CString strFilter = strDisplay + _T('\0') + strPattern + _T('\0');
    for (std::vector<RW_PLUGIN>::iterator it = m_pTree->m_rwPlugin.begin(); it != m_pTree->m_rwPlugin.end(); it++) {
        CString strFormat, strDisplay, strPattern;
        (void)strFormat.LoadString(APP_INF_FILE_FILTER_FMT_DISPLAY);
        strDisplay.Format(strFormat, it->m_strFormatName, it->m_strExtension);
        (void)strFormat.LoadString(APP_INF_FILE_FILTER_FMT_PATTERN);
        strPattern.Format(strFormat, it->m_strExtension);
        strFilter = strFilter + strDisplay + _T('\0') + strPattern + _T('\0'); // NOTE: Don't use operator +=
    }
    strFilter = strFilter + _T('\0') + _T('\0'); // NOTE: Don't use operator +=

    OPENFILENAME param;
    ZeroMemory(&param, sizeof param);
    TCHAR tcPath[MAX_PATH] = _T("");
    param.lStructSize = sizeof param;
    param.hwndOwner = m_hSelfWnd;
    param.lpstrFilter = strFilter.GetBuffer();
    param.lpstrCustomFilter = NULL;
    param.nFilterIndex = 1;
    param.lpstrFile = tcPath;
    param.nMaxFile = MAX_PATH;
    param.lpstrFileTitle = NULL;
    param.lpstrInitialDir = NULL;
    param.lpstrTitle = NULL;
    param.Flags = OFN_FILEMUSTEXIST;
    param.nFileOffset = 0;
    param.nFileExtension = 0;
    param.lpstrDefExt = NULL;
    if (GetOpenFileName(&param)) {
        if (m_ini.m_bDebug) {
            CString strText;
            strText.Format(_T("SelectFile \"%s\"\n"), tcPath);
            CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
        }

        CString path = CString(tcPath);
        CString format = param.nFilterIndex < 2 ? DAT_FORMAT : m_pTree->m_rwPlugin[param.nFilterIndex - 2].m_strFormatName;

        if (m_pTree->loadDataFileDef(path, format)) {
            m_ini.m_strDataPath = path;
            m_ini.m_strDataFormat = format;
            m_ini.m_bReadOnly = ((param.Flags & OFN_READONLY) != 0);
            m_ini.writeEnvInitData();
            return true;
        }
        else {
            CString strRes;
            (void)strRes.LoadString(APP_MES_UNKNOWN_FORMAT);
            AfxMessageBox(strRes, MB_ICONEXCLAMATION, 0);
            return false;
        }
    }
    else {
        if (m_ini.m_bDebug) {
            CGeneral::writeLog(m_ini.m_strDebugLog, _T("SelectFile failed\n"), _ME_NAME_, __LINE__);
        }
        return false;
    }
}

CString CCharu3App::NewFile()
{
    CString strTitle, strDisplay, strPattern;
    (void)strTitle.LoadString(APP_INF_EXPORT_CAPTION);
    (void)strDisplay.LoadString(APP_INF_FILE_FILTER_C3D_DISPLAY);
    (void)strPattern.LoadString(APP_INF_FILE_FILTER_C3D_PATTERN);
    CString strFilter = strDisplay + _T('\0') + strPattern + _T('\0') + _T('\0') + _T('\0');

    OPENFILENAME param;
    ZeroMemory(&param, sizeof param);
    TCHAR tcPath[MAX_PATH] = _T("");
    param.lStructSize = sizeof param;
    param.hwndOwner = m_hSelfWnd;
    param.lpstrTitle = strTitle.GetBuffer();
    param.lpstrFilter = strFilter.GetBuffer();
    param.lpstrDefExt = _T(DAT_EXT);
    param.lpstrCustomFilter = NULL;
    param.nFilterIndex = 1;
    param.lpstrFile = tcPath;
    param.nMaxFile = MAX_PATH;
    param.lpstrFileTitle = NULL;
    param.lpstrInitialDir = NULL;
    param.Flags = OFN_OVERWRITEPROMPT;
    param.nFileOffset = 0;
    param.nFileExtension = 0;
    if (GetSaveFileName(&param)) {
        //if (param.Flags & OFN_EXTENSIONDIFFERENT) {
        //	_tcscat_s(tcPath, MAX_PATH, _T("."));
        //	_tcscat_s(tcPath, MAX_PATH, _T(DAT_EXT));
        //}
        if (m_ini.m_bDebug) {
            CString strText;
            strText.Format(_T("NewFile \"%s\"\n"), tcPath);
            CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
        }
        return CString(tcPath);
    }
    else {
        if (m_ini.m_bDebug) {
            CGeneral::writeLog(m_ini.m_strDebugLog, _T("NewFile failed\n"), _ME_NAME_, __LINE__);
        }
        return CString(_T(""));
    }
}

//---------------------------------------------------
//�֐���	registerHotkey()
//�@�\		�z�b�g�L�[��ݒ肷��
//---------------------------------------------------
void CCharu3App::registerHotkey()
{
    UINT uMod = 0, uVK = 0;
    switch (m_ini.m_pop.m_nDoubleKeyPOP) {
    case 1:	uMod = MOD_SHIFT; break;
    case 2:	uMod = MOD_CONTROL; break;
    case 3:	uMod = MOD_ALT; break;
    default:
    case 0:
        uMod = m_ini.m_pop.m_uMod_Pouup;
        uVK = m_ini.m_pop.m_uVK_Pouup;
        break;
    }
    if (!RegisterHotKey(NULL, HOTKEY_POPUP, uMod, uVK)) {//�|�b�v�A�b�v�L�[
        CString strRes;
        (void)strRes.LoadString(APP_MES_FAILURE_HOTKEY);

        if (m_ini.m_bDebug) {
            CGeneral::writeLog(m_ini.m_strDebugLog, _T("False hotkey setting popup\n"), _ME_NAME_, __LINE__);
        }
        //		AfxMessageBox(strRes + _T("Popup Key"),MB_ICONEXCLAMATION,0);
    }

    uMod = 0, uVK = 0;
    switch (m_ini.m_pop.m_nDoubleKeyFIFO) {
    case 1:	uMod = MOD_SHIFT; break;
    case 2:	uMod = MOD_CONTROL; break;
    case 3:	uMod = MOD_ALT; break;
    default:
    case 0:
        uMod = m_ini.m_pop.m_uMod_Fifo;
        uVK = m_ini.m_pop.m_uVK_Fifo;
        break;
    }
    if (!RegisterHotKey(NULL, HOTKEY_FIFO, uMod, uVK)) {//����FIFO�L�[
        CString strRes;
        (void)strRes.LoadString(APP_MES_FAILURE_HOTKEY);

        if (m_ini.m_bDebug) {
            CGeneral::writeLog(m_ini.m_strDebugLog, _T("False hotkey setting stuckmode\n"), _ME_NAME_, __LINE__);
        }
        //		AfxMessageBox(strRes + _T("Stock mode Key"),MB_ICONEXCLAMATION,0);
    }

    if (m_ini.m_bDebug) {
        CGeneral::writeLog(m_ini.m_strDebugLog, _T("registerHotkey\n"), _ME_NAME_, __LINE__);
    }
}

//---------------------------------------------------
//�֐���	stopHotkey()
//�@�\		�z�b�g�L�[���~�߂�
//---------------------------------------------------
void CCharu3App::stopHotkey()
{
    UnregisterHotKey(NULL, HOTKEY_POPUP);
    UnregisterHotKey(NULL, HOTKEY_FIFO);

    if (m_ini.m_bDebug) {
        CGeneral::writeLog(m_ini.m_strDebugLog, _T("stopHotkey\n"), _ME_NAME_, __LINE__);
    }
}

//---------------------------------------------------
//�֐���	setAppendHotKey()
//�@�\		�ǉ��z�b�g�L�[��ݒ�
//---------------------------------------------------
void CCharu3App::setAppendHotKey()
{
    HOT_KEY_CODE keyData;
    std::list<STRING_DATA>::iterator it;

    m_hotkeyVector.clear();

    CString strKey;
    STRING_DATA data;
    HTREEITEM hRet = NULL;

    if (m_isStockMode)
        RegisterHotKey(NULL, HOTKEY_PASTE, m_keySet.m_uMod_Paste, m_keySet.m_uVK_Paste);//�y�[�X�g�L�[

    int nSize = m_pTree->m_MyStringList.size();
    HTREEITEM hTreeItem = m_pTree->GetRootItem();
    for (int i = 0; i < nSize && hTreeItem; hTreeItem = m_pTree->getTrueNextItem(hTreeItem), i++) {
        if (hTreeItem) {
            data = m_pTree->getData(hTreeItem);
            strKey = m_pTree->getDataOptionStr(data.m_strMacro, EXMACRO_HOT_KEY);
            if (strKey != "") {
                //�z�b�g�L�[�ݒ��ϊ�
                convHotKeyConf(strKey, &keyData.m_uModKey, &keyData.m_uVkCode, &keyData.m_isDoubleClick);
                keyData.m_strMacroName = EXMACRO_HOT_KEY;
                keyData.m_nDataID = data.m_nMyID;
                keyData.m_hItem = hTreeItem;
                keyData.m_dwDoubleKeyTime = 0;
                m_hotkeyVector.insert(m_hotkeyVector.end(), keyData);//�ݒ�A���C�ɒǉ�
                int nret = RegisterHotKey(NULL, HOT_ITEM_KEY + m_hotkeyVector.size() - 1, keyData.m_uModKey, keyData.m_uVkCode);//�z�b�g�L�[�����W�X�g

                if (m_ini.m_bDebug) {
                    CString strText;
                    strText.Format(_T("setAppendHotKey hotkey \"%s\" %d\n"), strKey.GetString(), nret);
                    CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
                }
            }
            strKey = m_pTree->getDataOptionStr(data.m_strMacro, EXMACRO_DIRECT_COPY);
            if (strKey != "") {
                //�z�b�g�L�[�ݒ��ϊ�
                convHotKeyConf(strKey, &keyData.m_uModKey, &keyData.m_uVkCode, &keyData.m_isDoubleClick);
                keyData.m_strMacroName = EXMACRO_DIRECT_COPY;
                keyData.m_nDataID = data.m_nMyID;
                keyData.m_hItem = hTreeItem;
                keyData.m_dwDoubleKeyTime = 0;
                m_hotkeyVector.insert(m_hotkeyVector.end(), keyData);//�ݒ�A���C�ɒǉ�
                int nret = RegisterHotKey(NULL, HOT_ITEM_KEY + m_hotkeyVector.size() - 1, keyData.m_uModKey, keyData.m_uVkCode);//�z�b�g�L�[�����W�X�g

                if (m_ini.m_bDebug) {
                    CString strText;
                    strText.Format(_T("setAppendHotKey directcopy \"%s\" %d\n"), strKey.GetString(), nret);
                    CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
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
    UnregisterHotKey(NULL, HOTKEY_PASTE);
    int nSize = m_hotkeyVector.size();
    for (int i = 0; i < nSize; i++) {
        UnregisterHotKey(NULL, HOT_ITEM_KEY + i);
    }

    if (m_ini.m_bDebug) {
        CGeneral::writeLog(m_ini.m_strDebugLog, _T("stopAppendHotKey\n"), _ME_NAME_, __LINE__);
    }
}

//---------------------------------------------------
//�֐���	convHotKeyConf()
//�@�\		�����񂩂�z�b�g�L�[�̐ݒ�����
//---------------------------------------------------
void CCharu3App::convHotKeyConf(CString strKey, UINT* pMod, UINT* pVK, bool* isDoubleClick)
{
    int nFound;
    CString strVK, strMod;
    strKey.MakeUpper();
    nFound = strKey.ReverseFind('+');
    *isDoubleClick = false;

    if (nFound < 0) {//+�ŋ�؂��ĂȂ��ꍇ
        *pMod = NULL;
        strVK = strKey;
        strVK.TrimLeft();
        strVK.TrimRight();
        if (strVK == "SHIFT*2") {
            *pMod = MOD_SHIFT;
        }
        else if (strVK == "CTRL*2") {
            *pMod = MOD_CONTROL;
        }
        else if (strVK == "ALT*2") {
            *pMod = MOD_ALT;
        }
        if (*pMod) {
            *isDoubleClick = true;
            *pVK = NULL;
            return;
        }
    }
    else {//+�ŋ�؂��Ă�ꍇ
        strVK = strKey.Right(strKey.GetLength() - nFound - 1);
        strVK.TrimLeft();
        strVK.TrimRight();

        *pMod = 0;
        if (strKey.Find(_T("SHIFT")) >= 0) {
            *pMod = *pMod | MOD_SHIFT;
        }
        if (strKey.Find(_T("CTRL")) >= 0) {
            *pMod = *pMod | MOD_CONTROL;
        }
        if (strKey.Find(_T("ALT")) >= 0) {
            *pMod = *pMod | MOD_ALT;
        }
    }
    *pVK = getKeycode(strVK.GetBuffer(strVK.GetLength()), false);
}
//---------------------------------------------------
//�֐���	getKeycode(char *szKeyName)
//�@�\		�L�[������R�[�h�𓾂�
//---------------------------------------------------
int CCharu3App::getKeycode(TCHAR* szKeyName, bool scanLayout)
{
    CString strTmp;
    strTmp = szKeyName;
    strTmp.MakeUpper();

    int i, nRet = 0;

    if (strTmp.Find(_T("0X")) >= 0)
        _stscanf_s(szKeyName, _T("0x%2x"), &nRet);

    if (!nRet) {
        if (_tcsclen(szKeyName) == 1) {
            if (scanLayout || *szKeyName > 0x7f) {
                SHORT code = VkKeyScanEx(*szKeyName, m_ini.m_keyLayout);
                nRet = code & 0xff;
            }
            else {
                nRet = toupper(*szKeyName);
            }
        }
        else {
            for (i = 0; m_keyStruct[i].nKeyCode > 0 && i < 256; i++) {
                if (m_keyStruct[i].strName == strTmp) {
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
bool CCharu3App::setAppendKeyInit(HWND hTopWindow, COPYPASTE_KEY* keySet)
{
    TCHAR strWindowName[1024] = {};
    *strWindowName = (TCHAR)NULL;
    CString strWinName;
    bool isRet = false;

    if (!hTopWindow || m_hActiveKeyWnd == hTopWindow) return isRet;//�O�Ɠ����n���h���Ȃ�A��
    if (!GetWindowText(hTopWindow, strWindowName, _countof(strWindowName))) return isRet;//�L���v�V�������擾

    strWinName = strWindowName;
    *keySet = m_ini.getAppendKeyInit(strWinName);
    if (keySet->m_nMessage <= -1)	*keySet = m_ini.m_key.m_defKeySet;
    m_hActiveKeyWnd = hTopWindow;

    if (m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("setAppendKeyInit %s %d %d %d %d %d\n"), strWinName.GetString(), keySet->m_uMod_Copy, keySet->m_uVK_Copy, keySet->m_uMod_Paste, keySet->m_uVK_Paste, keySet->m_nMessage);
        CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }

    isRet = true;
    return isRet;
}

//---------------------------------------------------
//�֐���	popupTreeWindow
//�@�\		�|�b�v�A�b�v��\��
//---------------------------------------------------
void CCharu3App::popupTreeWindow(POINT pos, bool keepSelection, HTREEITEM hOpenItem)
{
    if (m_nPhase != PHASE_IDOL) return;
    m_nPhase = PHASE_POPUP;
    m_ini.unHookKey();
    stopAppendHotKey();//�ǉ��z�b�g�L�[���~

//	CGeneral::getFocusInfo(&m_focusInfo);

    if (m_focusInfo.m_hActiveWnd == this->m_pMainFrame->m_hWnd) {
        m_nPhase = PHASE_IDOL;
        return;
    }

    if (m_isStockMode)	KillTimer(m_pMainWnd->m_hWnd, TIMER_ACTIVE);//�Ď��^�C�}�[��~

    if (m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("popupTreeWindow x=%d y=%d keep=%s\n"), pos.x, pos.y, keepSelection ? _T("true") : _T("false"));
        CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }

    CGeneral::setAbsoluteForegroundWindow(m_pMainWnd->m_hWnd);//�������A�N�e�B�u�ɐݒ�
    m_pTreeDlg->showWindowPos(pos, m_ini.m_DialogSize, SW_SHOW, keepSelection, hOpenItem);
}

//---------------------------------------------------
//�֐���	popupTreeWinMC(HWND hForeground)
//�@�\		�|�b�v�A�b�v���}�E�X�J�[�\���ʒu�ɕ\��
//---------------------------------------------------
void CCharu3App::popupTreeWinMC(HWND hForeground)
{
    POINT pos;
    GetCursorPos(&pos);
    pos.x -= m_ini.m_DialogSize.x;
    pos.y -= m_ini.m_DialogSize.y;
    adjustLocation(&pos);
    //	CGeneral::getFocusInfo(&theApp.m_focusInfo,hForeground);
    popupTreeWindow(pos, m_ini.m_pop.m_bKeepSelection);
}
//---------------------------------------------------
//�֐���	adjustLocation(POINT pos)
//�@�\		�|�b�v�A�b�v�ʒu��K���f�X�N�g�b�v���ɂ���
//---------------------------------------------------
void CCharu3App::adjustLocation(POINT* pos)
{
    RECT DeskTopSize;
    int nMonCount = GetSystemMetrics(SM_CMONITORS);

    if (nMonCount <= 1) {
        //�f�X�N�g�b�v�̃T�C�Y���擾
        int nDektopWidth = GetSystemMetrics(SM_CXSCREEN);
        int nDesktopHeight = GetSystemMetrics(SM_CYFULLSCREEN);
        //�𑜓x�̎擾
        int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
        int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

        if (m_ini.m_bDebug) {
            CString strText;
            strText.Format(_T("reviseWindowPos %d %d %d %d\n"), nDektopWidth, nDesktopHeight, nScreenWidth, nScreenHeight);
            CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
        }

        HWND hDeskTop = GetDesktopWindow();
        if (hDeskTop) ::GetWindowRect(hDeskTop, &DeskTopSize);
        else return;
    }
    else {
        CArray<RECT, RECT> arrayRect;
        arrayRect.RemoveAll();
        EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)MonitorEnumFunc, (LPARAM)&arrayRect);

        int nSize = arrayRect.GetSize(), nCurrentMon = 0, nWidth, nHeight;
        CString strBuff;
        double dDistance, dMinDistance = -1.0f;
        for (int i = 0; i < nSize; i++) {
            strBuff.Format(_T("left:%d top:%d right:%d bottom:%d"), arrayRect[i].left, arrayRect[i].top, arrayRect[i].right, arrayRect[i].bottom);
            nWidth = abs((arrayRect[i].left + arrayRect[i].right) / 2 - pos->x);
            nHeight = abs((arrayRect[i].top + arrayRect[i].bottom) / 2 - pos->y);

            dDistance = sqrt((nWidth * nWidth) + (nHeight * nHeight));
            if (dDistance < dMinDistance || i == 0) {
                dMinDistance = dDistance;
                nCurrentMon = i;
            }
        }
        DeskTopSize = arrayRect[nCurrentMon];
    }
    //�E�B���h�E�ʒu��␳
    if (pos->y + m_ini.m_DialogSize.y > DeskTopSize.bottom)
        pos->y -= (pos->y + m_ini.m_DialogSize.y) - DeskTopSize.bottom;
    if (pos->x + m_ini.m_DialogSize.x > DeskTopSize.right)
        pos->x -= (pos->x + m_ini.m_DialogSize.x) - DeskTopSize.right;

    if (pos->y < DeskTopSize.top)
        pos->y = DeskTopSize.top;
    if (pos->x < DeskTopSize.left)
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
    if (::GetAsyncKeyState(VK_SHIFT) < 0) isPaste = false;

    if (m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("closeTreeWindow %d\n"), nRet);
        CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }

    //�A�N�e�B�u�E�B���h�E�𕜋A
    if (nRet == IDOK) {
        CString strClip, strSelect;
        m_clipboard.getClipboardText(strClip, m_ini.m_nClipboardRetryTimes, m_ini.m_nClipboardRetryInterval);//�N���b�v�{�[�h��ۑ�

        setAppendKeyInit(m_focusInfo.m_hActiveWnd, &m_keySet);//�L�[�ݒ��ύX
        //�L�[���������̂�҂�
        while (::GetAsyncKeyState(VK_MENU) < 0 || ::GetAsyncKeyState(VK_CONTROL) < 0 ||
            ::GetAsyncKeyState(VK_SHIFT) < 0 || ::GetAsyncKeyState(VK_RETURN) < 0) Sleep(50);
        CGeneral::setFocusInfo(&m_focusInfo);//�^�[�Q�b�g���t�H�[�J�X

        if (m_ini.m_bDebug) {
            CString strText;
            strText.Format(_T("setFocusInfo active:%p focus:%p\n"), m_focusInfo.m_hActiveWnd, m_focusInfo.m_hFocusWnd);
            CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
        }

        //�\��t������
        if (m_pTree->m_ltCheckItems.size() > 0) {//�����I���f�[�^������
            strSelect = getSelectString(m_keySet, m_focusInfo.m_hFocusWnd); // TODO: Doing unconditional copy action. Does not consider the necessity.

            if (m_ini.m_bDebug) {
                CString strText;
                strText.Format(_T("closeTreeWindow sel:%s clip:%s\n"), strSelect.GetString(), strClip.GetString());
                CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
            }

            std::list<HTREEITEM>::iterator it;
            for (it = m_pTree->m_ltCheckItems.begin(); it != m_pTree->m_ltCheckItems.end(); it++) {
                if (m_pTree->GetItemState(*it, TVIF_HANDLE)) {
                    data = m_pTree->getData(*it);

                    if (m_ini.m_bDebug) {
                        CString strText;
                        strText.Format(_T("closeTreeWindow check paste %s\n"), data.m_strTitle.GetString());
                        CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
                    }
                    playData(data, strClip, strSelect, isPaste, false);
                }
            }
            //�ꎞ���ڂ͏���
            for (it = m_pTree->m_ltCheckItems.begin(); it != m_pTree->m_ltCheckItems.end(); it++) {
                if (m_pTree->GetItemState(*it, TVIF_HANDLE)) {
                    HTREEITEM hItem = *it;
                    *it = NULL;
                    data = m_pTree->getData(hItem);
                    if (data.m_cKind & KIND_ONETIME) {
                        m_hSelectItemBkup = NULL;
                    }
                }
            }
        }
        else if (m_pTreeDlg->m_selectDataPtr != nullptr) {//�ʏ�I���f�[�^
            bool requiresSelectionText = (m_pTreeDlg->m_selectDataPtr->m_strData.Find(_T("$SEL")) != -1); // TODO: This test is true even if $SEL is outside <charu3MACRO>
            strSelect = requiresSelectionText ? getSelectString(m_keySet, m_focusInfo.m_hFocusWnd) : "";
            data = *(m_pTreeDlg->m_selectDataPtr);

            if (m_ini.m_bDebug) {
                CString strText;
                strText.Format(_T("closeTreeWindow sel:%s clip:%s title:%s\n"), strSelect.GetString(), strClip.GetString(), data.m_strTitle.GetString());
                CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
            }
            playData(data, strClip, strSelect, isPaste);
        }
    }
    else if (::GetForegroundWindow() == m_focusInfo.m_hActiveWnd) {
        CGeneral::setFocusInfo(&m_focusInfo);
    }
    if (m_hSelectItemBkup) {
        m_pTree->SelectItem(m_hSelectItemBkup);
        m_hSelectItemBkup = NULL;
    }

    //�f�[�^��ۑ�
    SaveData();
    m_ini.writeEnvInitData();//���ݒ��ۑ�

    //�Ď��^�C�}�[�Z�b�g
    if (m_isStockMode && m_ini.m_nWindowCheckInterval > 0) {
        SetTimer(m_pMainWnd->m_hWnd, TIMER_ACTIVE, m_ini.m_nWindowCheckInterval, NULL);
    }

    setAppendHotKey();//�ǉ��z�b�g�L�[��ݒ�
    m_ini.setHookKey(m_hSelfWnd);

    if (m_pTree->GetStyle() & TVS_CHECKBOXES) {
        resetTreeDialog();
        CGeneral::setFocusInfo(&m_focusInfo);
    }
    else {
        m_pTree->ClearChecks();
    }
    ASSERT(m_nPhase == PHASE_POPUP);
    m_nPhase = PHASE_IDOL;
}

void CCharu3App::SaveData()
{
    if (m_ini.m_bReadOnly) {
        return;
    }
    if (m_pTree->saveDataToFile(m_ini.m_strDataPath, m_ini.m_strDataFormat, NULL)) {
        return;
    }

    CString strRes;
    (void)strRes.LoadString(APP_MES_FAILURE_DATA_SAVE);
    AfxMessageBox(strRes, MB_ICONEXCLAMATION, 0);
}

//---------------------------------------------------
//�֐���	playData(STRING_DATA data,CString strClip)
//�@�\		�f�[�^�̓W�J����������
//---------------------------------------------------
void CCharu3App::playData(STRING_DATA data, CString strClip, CString strSelect, bool isPaste, bool isChange)
{
    HTREEITEM hSelectItem = m_pTree->GetSelectedItem();
    CString strPaste;

    //�}�N������
    {
        HTREEITEM hMacroItem = NULL;
        if (hSelectItem) {
            hMacroItem = m_pTree->searchParentOption(hSelectItem, _T("macro"));//��ԋ߂��e�������̃}�N���𒲂ׂ�
        }
        CString strMacro = m_ini.m_strDataFormat;
        if (hMacroItem) {
            STRING_DATA* macroDataPtr = m_pTree->getDataPtr(hMacroItem);
            strMacro = m_pTree->getDataOptionStr(macroDataPtr->m_strMacro, _T("macro"));
        }
        strPaste = convertMacro(&data, strSelect, strClip, strMacro);
    }

    //�e�L�X�g��\��t������
    if (isPaste) {
        if (m_ini.m_bDebug) {
            CString strText;
            strText.Format(_T("playData active:%p focus:%p\n"), m_focusInfo.m_hActiveWnd, m_focusInfo.m_hFocusWnd);
            CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
        }

        execData(strPaste, m_keySet, hSelectItem, m_focusInfo.m_hFocusWnd);

        //�N���b�v�{�[�h���A
        if (m_ini.m_etc.m_bPutBackClipboard && strClip != "") {
            m_strSavedClipboard = strClip;
            m_clipboard.setClipboardText(strClip.GetString(), m_ini.m_nClipboardRetryTimes, m_ini.m_nClipboardRetryInterval);
        }
        if (isChange) {
            //�\��t���f�[�^���t�H���_�̐擪�Ɉړ�
            HTREEITEM hMoveItem = NULL;
            if (hSelectItem) {
                hMoveItem = m_pTree->searchParentOption(hSelectItem, _T("move"));
            }
            if (hMoveItem) {
                STRING_DATA* macroDataPtr = m_pTree->getDataPtr(hMoveItem);
                int nIsMove = m_pTree->getDataOption(macroDataPtr->m_strMacro, _T("move"));
                if (nIsMove) {
                    hSelectItem = m_pTree->moveFolderTop(hSelectItem);
                    if (hSelectItem) m_pTree->SelectItem(hSelectItem);
                }
            }
            //�ꎞ���ڂ͏���
            if (m_pTree->getDatakind(hSelectItem) & KIND_ONETIME) {
                m_pTree->deleteData(hSelectItem);
                m_hSelectItemBkup = NULL;
            }
        }
    }
    else {
        m_clipboard.setClipboardText(strPaste, m_ini.m_nClipboardRetryTimes, m_ini.m_nClipboardRetryInterval);
    }
}

//---------------------------------------------------
//�֐���	playHotItem(nTarget)
//�@�\		�z�b�g�A�C�e������
//---------------------------------------------------
void CCharu3App::playHotItem(int nTarget)
{
    if (nTarget >= 0 && static_cast<size_t>(nTarget) < m_hotkeyVector.size()) {
        HOT_KEY_CODE keyData;
        keyData = m_hotkeyVector.at(nTarget);
        if (!keyData.m_uVkCode) {
            DWORD dwTime = timeGetTime();
            if (static_cast<int>(dwTime - keyData.m_dwDoubleKeyTime) > m_ini.m_pop.m_nDCKeyPopTime) {
                m_hotkeyVector[nTarget].m_dwDoubleKeyTime = dwTime;
                return;
            }
        }
        STRING_DATA data;//�e�L�X�g�f�[�^

        if (keyData.m_hItem) {
            data = m_pTree->getData(keyData.m_hItem);
            CString strMacroData;
            CString strSelect, strMacro;
            //�t�H���_�̏ꍇ
            if (data.m_cKind & KIND_FOLDER_ALL) {
                POINT pos;
                //�_�C���N�g�R�s�[
                if (keyData.m_strMacroName == EXMACRO_DIRECT_COPY) {
                    m_nPhase = PHASE_LOCK;
                    stopAppendHotKey();//�ǉ��z�b�g�L�[���~
                    STRING_DATA dataChild;
                    CGeneral::getCaretPos(&pos, &m_focusInfo);//�L�����b�g�ʒu���擾(�n���h������邽��)

                    //�L�[���������̂�҂�
                    while (::GetAsyncKeyState(VK_MENU) < 0 || ::GetAsyncKeyState(VK_CONTROL) < 0 ||
                        ::GetAsyncKeyState(VK_SHIFT) < 0 || ::GetAsyncKeyState(keyData.m_uVkCode) < 0) Sleep(50);
                    keyUpDown(keyData.m_uModKey, keyData.m_uVkCode, KEY_UP);//�L�[�𗣂������i���ꂪ�����ƑI���e�L�X�g�擾�Ŏ��s�j

                    setAppendKeyInit(m_focusInfo.m_hActiveWnd, &m_keySet);//�L�[�ݒ��ύX
                    strSelect = getSelectString(m_keySet, m_focusInfo.m_hFocusWnd);//�I�𕶎��擾
                    if (strSelect != "") {
                        dataChild.m_cKind = KIND_LOCK;
                        dataChild.m_nParentID = data.m_nMyID;
                        dataChild.m_strData = strSelect;
                        int nTitleLength = m_pTree->getDataOption(data.m_strMacro, _T("titlelen"));//�^�C�g���̕�����
                        if (nTitleLength < 1 || nTitleLength > 256) nTitleLength = 32;
                        dataChild.m_strTitle = m_pTree->makeTitle(strSelect, nTitleLength);

                        if (m_ini.m_bDebug) {
                            CString strText;
                            strText.Format(_T("Direct copy folder %s\n"), dataChild.m_strTitle.GetString());
                            CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
                        }

                        m_pTree->addData(keyData.m_hItem, dataChild, true, true);
                    }
                    setAppendHotKey();//�ǉ��z�b�g�L�[��ݒ�
                    m_nPhase = PHASE_IDOL;
                }
                //�z�b�g�L�[
                else if (keyData.m_strMacroName == EXMACRO_HOT_KEY) {
                    m_hSelectItemBkup = m_pTree->GetSelectedItem();

                    m_pTree->SelectItem(keyData.m_hItem);
                    getPopupPos(&pos, m_ini.m_pop.m_nPopupPos);//�|�b�v�A�b�v�ʒu���擾
                    adjustLocation(&pos);
                    popupTreeWindow(pos, true, keyData.m_hItem);
                }
            }
            else {//��`���͈ꔭ�\��t��
                m_nPhase = PHASE_LOCK;
                stopAppendHotKey();//�ǉ��z�b�g�L�[���~

                CString strClip, strPaste;
                m_clipboard.getClipboardText(strClip, m_ini.m_nClipboardRetryTimes, m_ini.m_nClipboardRetryInterval);//�N���b�v�{�[�h��ۑ�
                strPaste = data.m_strData;

                POINT pos;

                //�L�[���������̂�҂�
                while (::GetAsyncKeyState(VK_MENU) < 0 || ::GetAsyncKeyState(VK_CONTROL) < 0 ||
                    ::GetAsyncKeyState(VK_SHIFT) < 0 || ::GetAsyncKeyState(keyData.m_uVkCode) < 0) Sleep(50);
                keyUpDown(keyData.m_uModKey, keyData.m_uVkCode, KEY_UP);//�L�[�𗣂������i���ꂪ�����ƑI���e�L�X�g�擾�Ŏ��s�j

                int nIsMove;

                //�_�C���N�g�R�s�[
                if (keyData.m_strMacroName == EXMACRO_DIRECT_COPY) {
                    CGeneral::getCaretPos(&pos, &m_focusInfo);//�L�����b�g�ʒu���擾
                    setAppendKeyInit(m_focusInfo.m_hActiveWnd, &m_keySet);//�L�[�ݒ��ύX
                    strSelect = getSelectString(m_keySet, m_focusInfo.m_hFocusWnd);//�I�𕶎��擾
                    data.m_strData = strSelect;
                    m_pTree->editData(keyData.m_hItem, data);

                    if (m_ini.m_bDebug) {
                        CString strText;
                        strText.Format(_T("Direct copy data %s\n"), data.m_strTitle.GetString());
                        CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
                    }
                }
                //�z�b�g�L�[
                else if (keyData.m_strMacroName == EXMACRO_HOT_KEY) {
                    //�}�N������
                    {
                        HTREEITEM hMacroItem = m_pTree->searchParentOption(keyData.m_hItem, _T("macro"));//��ԋ߂��e�������̃}�N���𒲂ׂ�
                        strMacro = m_ini.m_strDataFormat;
                        if (hMacroItem) {
                            STRING_DATA* macroDataPtr = m_pTree->getDataPtr(hMacroItem);
                            strMacro = m_pTree->getDataOptionStr(macroDataPtr->m_strMacro, ("macro"));
                        }
                    }

                    CGeneral::getCaretPos(&pos, &m_focusInfo);//�L�����b�g�ʒu���擾
                    setAppendKeyInit(m_focusInfo.m_hActiveWnd, &m_keySet);//�L�[�ݒ��ύX

                    strSelect = getSelectString(m_keySet, m_focusInfo.m_hFocusWnd);//�I�𕶎��擾
                    strPaste = convertMacro(&data, strSelect, strClip, strMacro);//�}�N���ϊ�

                    if (m_ini.m_bDebug) {
                        CString strText;
                        strText.Format(_T("Direct paste data %s active:%p focus:%p\n"), strPaste.GetString(), m_focusInfo.m_hActiveWnd, m_focusInfo.m_hFocusWnd);
                        CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
                    }

                    //�e�L�X�g��\��t������
                    execData(strPaste, m_keySet, keyData.m_hItem, m_focusInfo.m_hFocusWnd);

                    //�\��t���f�[�^���t�H���_�̐擪�Ɉړ�
                    {
                        HTREEITEM hMoveItem = m_pTree->searchParentOption(keyData.m_hItem, _T("move"));
                        if (hMoveItem) {
                            STRING_DATA* macroDataPtr = m_pTree->getDataPtr(hMoveItem);
                            nIsMove = m_pTree->getDataOption(macroDataPtr->m_strMacro, _T("move"));
                            if (nIsMove) {
                                keyData.m_hItem = m_pTree->moveFolderTop(keyData.m_hItem);
                            }
                        }
                    }
                    //�N���b�v�{�[�h���A
                    if (m_ini.m_etc.m_bPutBackClipboard && strClip != "") {
                        m_clipboard.setClipboardText(strClip.GetString(), m_ini.m_nClipboardRetryTimes, m_ini.m_nClipboardRetryInterval);
                    }
                    //�ꎞ���ڂ͏���
                    if (m_pTree->getDatakind(keyData.m_hItem) & KIND_ONETIME) {
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
CString CCharu3App::getSelectString(COPYPASTE_KEY key, HWND hWnd)
{
    m_clipboard.setClipboardText(CString(), m_ini.m_nClipboardRetryTimes, m_ini.m_nClipboardRetryInterval);
    if (!hWnd)	hWnd = m_focusInfo.m_hFocusWnd;
    CString strSelect;

    TCHAR strWindowName[1024] = {};
    *strWindowName = (TCHAR)NULL;
    CString strWinName;
    int i;
    COPYPASTE_KEY keySet;

    GetWindowText(m_focusInfo.m_hActiveWnd, strWindowName, _countof(strWindowName));
    strWinName = strWindowName;

    keySet.m_nMessage = 0;

    for (i = 0; keySet.m_nMessage > -1; i++) {
        keySet = m_ini.getAppendKeyInit(strWinName, i);
        if (keySet.m_nMessage <= -1 && i == 0)	keySet = key;
        if (keySet.m_uVK_Copy) {
            if (keySet.m_nMessage > -1) {
                if (keySet.m_nMessage == 0) {//�C�x���g����
                    keyUpDown(keySet.m_uMod_Copy, keySet.m_uVK_Copy, KEY_DOWN);
                    keyUpDown(keySet.m_uMod_Copy, keySet.m_uVK_Copy, KEY_UP);
                    Sleep(keySet.m_nCopyWait);//�E�F�C�g
                    keyUpDownMessage(keySet.m_uMod_Copy, keySet.m_uVK_Copy, KEY_UP, hWnd);
                }
                else if (keySet.m_nMessage == 1) {//Charu2Pro����
                    keyUpDownC2(keySet.m_uMod_Copy, keySet.m_uVK_Copy, KEY_DOWN);
                    keyUpDownC2(keySet.m_uMod_Copy, keySet.m_uVK_Copy, KEY_UP);
                    Sleep(keySet.m_nCopyWait);//�E�F�C�g
                    keyUpDownMessage(keySet.m_uMod_Copy, keySet.m_uVK_Copy, KEY_DOWN, hWnd);
                    Sleep(keySet.m_nCopyWait / 4);//�E�F�C�g
                    keyUpDownMessage(keySet.m_uMod_Copy, keySet.m_uVK_Copy, KEY_DOWN, hWnd);
                    keyUpDownMessage(keySet.m_uMod_Copy, keySet.m_uVK_Copy, KEY_UP, hWnd);
                    Sleep(keySet.m_nCopyWait);//�E�F�C�g
                }
                else if (keySet.m_nMessage == 2) {
                    ::PostMessage(hWnd, key.m_copyMessage.Msg, key.m_copyMessage.wParam, key.m_copyMessage.lParam);//���b�Z�[�W����
    //				::SendMessage(hWnd,WM_COPY,NULL,NULL);//���b�Z�[�W����
                }

                if (m_ini.m_bDebug) {
                    CString strText;
                    strText.Format(_T("getSelectString %d %s\n"), keySet.m_nMessage, strSelect.GetString());
                    CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
                }
            }
        }
    }

    if (m_clipboard.getClipboardText(strSelect, m_ini.m_nClipboardRetryTimes, m_ini.m_nClipboardRetryInterval)) {
        return strSelect;
    }

    return CString();
}

//---------------------------------------------------
//�֐���	execKeyMacro(CString strKeyMacro)
//�@�\		�L�[�{�[�h�}�N�������s
//---------------------------------------------------
void CCharu3App::execKeyMacro(CString strKeyMacro)
{
    TCHAR* szKeyMacro, strKeyCode[256], * szSplit;
    int nCount, nSleep, nKey[16] = {}, i, nKeyCount;
    szKeyMacro = strKeyMacro.GetBuffer(strKeyMacro.GetLength());

    //�\��t����̃X���[�v���擾
    szSplit = UStringWork::awk(szKeyMacro, strKeyCode, _countof(strKeyCode), 3, _T(';'));
    if (szSplit) {
        nSleep = _ttoi(strKeyCode);
        if (nSleep > 10000) nSleep = 10000;
    }
    else         nSleep = 0;

    //�\��t���񐔂��擾
    szSplit = UStringWork::awk(szKeyMacro, strKeyCode, _countof(strKeyCode), 2, _T(';'));
    if (szSplit) {
        nCount = _ttoi(strKeyCode);
        if (nCount > 256) nCount = 256;
        *szSplit = NULL;
    }
    else         nCount = 1;

    //�L�[�z����擾�@�ő��16�L�[
    for (i = 0; i <= 15; i++) {
        UStringWork::awk(szKeyMacro, strKeyCode, _countof(strKeyCode), i + 1, _T(','));
        if (_tcsclen(strKeyCode) > 0) nKey[i] = getKeycode(strKeyCode, true);
        else break;
    }
    nKeyCount = i - 1;//�L�[�̌�

    //�L�[������
    for (; nCount > 0; nCount--) {
        for (i = 0; i <= nKeyCount; i++) {
            keybd_event(nKey[i], 0, KEYEVENTF_EXTENDEDKEY, 0);//����
        }
    }
    for (i = 0; i <= nKeyCount; i++) {
        keybd_event(nKey[i], 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);//�L�[�𗣂�
    }
    Sleep(nSleep);
}

//---------------------------------------------------
//�֐���	execData()
//�@�\		�L�[�{�[�h�}�N�������s���Ȃ���f�[�^��\��t����
//---------------------------------------------------
void CCharu3App::execData(CString strPaste, COPYPASTE_KEY key, HTREEITEM hTargetItem, HWND hWnd)
{
    HTREEITEM hMacroItem;
    CString strCut, strKeyMacro;
    int nStart = 0;
    int nFoundStart = -1;
    int nFoundEnd;
    int nMacroLenS;
    int nMacroLenE;

    //�r�t�H�A�L�[����
    if (hTargetItem)	hMacroItem = m_pTree->searchParentOption(hTargetItem, _T("beforkey"));
    else			hMacroItem = NULL;
    if (hMacroItem) {
        STRING_DATA* macroDataPtr = m_pTree->getDataPtr(hMacroItem);
        strKeyMacro = m_pTree->getDataOptionStr(macroDataPtr->m_strMacro, _T("beforkey"));
        if (strKeyMacro != "") execKeyMacro(strKeyMacro);
    }

    //�}�N����u��
    strPaste.Replace(_T("<charu2MACRO_KEY>"), MACRO_START_KEY);
    strPaste.Replace(_T("</charu2MACRO_KEY>"), MACRO_END_KEY);

    nMacroLenS = _tcsclen(MACRO_START_KEY);
    nMacroLenE = _tcsclen(MACRO_END_KEY);

    do {
        nFoundStart = strPaste.Find(MACRO_START_KEY, nStart);
        strKeyMacro = "";
        //�L�[�}�N�����܂�
        if (nFoundStart >= 0) {
            nFoundEnd = strPaste.Find(MACRO_END_KEY, nFoundStart);
            strCut = strPaste.Mid(nStart, nFoundStart - nStart);
            if (nFoundEnd > nFoundStart) {
                nStart = nFoundEnd + nMacroLenE;
                strKeyMacro = strPaste.Mid(nFoundStart + nMacroLenS, nFoundEnd - nFoundStart - nMacroLenS);
            }
            else	nStart = nFoundStart + nMacroLenS;
        }
        else 	strCut = strPaste.Mid(nStart, strPaste.GetLength() - nStart);
        //�ʂɓ\��t������
        if (strCut != _T("")) {
            //			pasteData(strCut,key,hWnd);//�\��t��
            //			m_strlClipBackup = strCut;

            TCHAR strWindowName[1024] = {};
            *strWindowName = (char)NULL;
            CString strWinName;
            int i;
            COPYPASTE_KEY keySet;

            GetWindowText(m_focusInfo.m_hActiveWnd, strWindowName, _countof(strWindowName));
            strWinName = strWindowName;

            keySet.m_nMessage = 0;
            for (i = 0; keySet.m_nMessage > -1; i++) {
                keySet = m_ini.getAppendKeyInit(strWinName, i);
                if (keySet.m_nMessage <= -1 && i == 0)	keySet = key;
                if (keySet.m_nMessage > -1) {
                    pasteData(strCut, keySet, hWnd);//�\��t��
                }
            }
            m_strSavedClipboard = strCut;
        }
        //�L�[�}�N�������s
        if (strKeyMacro != _T("")) {
            execKeyMacro(strKeyMacro);
        }
    } while (nFoundStart >= 0);

    //�A�t�^�[�L�[����
    if (hTargetItem)	hMacroItem = m_pTree->searchParentOption(hTargetItem, _T("afterkey"));
    else			hMacroItem = NULL;
    if (hMacroItem) {
        STRING_DATA* macroDataPtr = m_pTree->getDataPtr(hMacroItem);
        strKeyMacro = m_pTree->getDataOptionStr(macroDataPtr->m_strMacro, _T("afterkey"));
        if (strKeyMacro != _T("")) execKeyMacro(strKeyMacro);
    }
}

//---------------------------------------------------
//�֐���	pasteData()
//�@�\		�f�[�^��\��t����
//---------------------------------------------------
void CCharu3App::pasteData(CString strPaste, COPYPASTE_KEY key, HWND hWnd)
{
    if (m_isStockMode) {
        m_strSavedClipboard = strPaste;
    }
    m_clipboard.setClipboardText(strPaste, m_ini.m_nClipboardRetryTimes, m_ini.m_nClipboardRetryInterval);

    if (key.m_nMessage == 0) {//�C�x���g����
        keyUpDown(key.m_uMod_Paste, key.m_uVK_Paste, KEY_DOWN);
        keyUpDown(key.m_uMod_Paste, key.m_uVK_Paste, KEY_UP);
        Sleep(key.m_nPasteWait);
    }
    else if (key.m_nMessage == 1) {//Charu2Pro����
        keyUpDownC2(key.m_uMod_Paste, key.m_uVK_Paste, KEY_DOWN);
        keyUpDownC2(key.m_uMod_Paste, key.m_uVK_Paste, KEY_UP);
        Sleep(key.m_nPasteWait);
    }
    else if (key.m_nMessage == 2) {
        ::PostMessage(hWnd, key.m_pasteMessage.Msg, key.m_pasteMessage.wParam, key.m_pasteMessage.lParam);//���b�Z�[�W����
//		::PostMessage(hWnd,WM_PASTE,NULL,NULL);//���b�Z�[�W����
    }

    if (m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("pasteData %d %s %x %x active:%p focus:%p\n"),
            key.m_nMessage,
            strPaste.GetString(),
            key.m_uMod_Paste,
            key.m_uVK_Paste,
            m_focusInfo.m_hActiveWnd,
            m_focusInfo.m_hFocusWnd);
        CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }
}

//---------------------------------------------------
//�֐���	keyUpDown()
//�@�\		���z�L�[������
//---------------------------------------------------
void CCharu3App::keyUpDown(UINT uMod, UINT uVKCode, int nFlag)
{
    if (nFlag & KEY_DOWN) {
        if (uMod & MOD_ALT)
            keybd_event(VK_MENU, (BYTE)MapVirtualKey(VK_MENU, 0), KEYEVENTF_EXTENDEDKEY, 0);
        if (uMod & MOD_CONTROL)
            keybd_event(VK_CONTROL, (BYTE)MapVirtualKey(VK_CONTROL, 0), KEYEVENTF_EXTENDEDKEY, 0);
        if (uMod & MOD_SHIFT)
            keybd_event(VK_SHIFT, (BYTE)MapVirtualKey(VK_SHIFT, 0), KEYEVENTF_EXTENDEDKEY, 0);

        keybd_event(uVKCode, 0, KEYEVENTF_EXTENDEDKEY, 0);
    }
    if (nFlag & KEY_UP) {
        keybd_event(uVKCode, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);//�L�[�𗣂�

        if (uMod & MOD_ALT)
            keybd_event(VK_MENU, (BYTE)MapVirtualKey(VK_MENU, 0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        if (uMod & MOD_CONTROL)
            keybd_event(VK_CONTROL, (BYTE)MapVirtualKey(VK_CONTROL, 0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        if (uMod & MOD_SHIFT)
            keybd_event(VK_SHIFT, (BYTE)MapVirtualKey(VK_SHIFT, 0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
}

//---------------------------------------------------
//�֐���	keyUpDownC2()
//�@�\		���z�L�[������
//---------------------------------------------------
void CCharu3App::keyUpDownC2(UINT uMod, UINT uVKCode, int nFlag)
{
    if (nFlag & KEY_DOWN) {
        if (uMod & MOD_ALT)		keybd_event(VK_MENU, 0, NULL, 0);
        if (uMod & MOD_CONTROL)	keybd_event(VK_CONTROL, 0, NULL, 0);
        if (uMod & MOD_SHIFT)	keybd_event(VK_SHIFT, 0, NULL, 0);

        keybd_event(uVKCode, 0, NULL, 0);
    }
    else if (nFlag & KEY_UP) {
        keybd_event(uVKCode, 0, KEYEVENTF_KEYUP, 0);//�L�[�𗣂�

        if (uMod & MOD_ALT)		keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
        if (uMod & MOD_CONTROL)	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
        if (uMod & MOD_SHIFT)	keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    }
}

//---------------------------------------------------
//�֐���	keyUpDownMessage()
//�@�\		���z�L�[������
//---------------------------------------------------
void CCharu3App::keyUpDownMessage(UINT uMod, UINT uVKCode, int nFlag, HWND hWnd)
{
    if (nFlag & KEY_DOWN) {
        if (uMod & MOD_ALT)		::SendMessage(hWnd, WM_KEYDOWN, VK_MENU, 1);
        if (uMod & MOD_CONTROL)	::SendMessage(hWnd, WM_KEYDOWN, VK_CONTROL, 1);
        if (uMod & MOD_SHIFT)	::SendMessage(hWnd, WM_KEYDOWN, VK_SHIFT, 1);

        ::SendMessage(hWnd, WM_KEYDOWN, uVKCode, 1);
    }
    if (nFlag & KEY_UP) {
        ::SendMessage(hWnd, WM_KEYUP, uVKCode, 1);

        if (uMod & MOD_ALT)		::SendMessage(hWnd, WM_KEYUP, VK_MENU, 1);
        if (uMod & MOD_CONTROL)	::SendMessage(hWnd, WM_KEYUP, VK_CONTROL, 1);
        if (uMod & MOD_SHIFT)	::SendMessage(hWnd, WM_KEYUP, VK_SHIFT, 1);
    }
}
//---------------------------------------------------
//�֐���	convertMacro(CString strSourceData)
//�@�\		�}�N���������u��
//---------------------------------------------------
CString CCharu3App::convertMacro(STRING_DATA* SourceData, CString strSelect, CString strClip, CString strSoftName)
{
    SetCurrentDirectory(m_ini.m_strAppPath);

    if (strSoftName == _T("no")) return SourceData->m_strData;

    CString strSourceData;
    if (strSoftName != DAT_FORMAT) {
        if (m_pTree->convertMacroPlugin(SourceData, &strSourceData, strSelect, strClip, strSoftName))
            return strSourceData;
    }

    int nStart, nEnd, nMove, nTagStart, nTagEnd;

    CString strTag, strTagEnd, strBuff, strBuff2, strTime;
    CString strShellStart = _T("$SHELL<"), strShellEnd = _T(">$SHELL");
    CString strRelateStart = _T("$RELATE<"), strRelateEnd = _T(">$RELATE");
    CString strPlugStart = _T("$PLUG-IN<"), strPlugEnd = _T(">$PLUG-IN");

    //�}�N���돈���΍�
    TCHAR strMark[2] = {};
    strMark[0] = 0x7f;
    strMark[1] = NULL;
    bool isSelect = false, isClip = false;

    CString strRepBuff1, strRepBuff2, strRepBuff3, strRepBuff4, strRepBuff5;
    strRepBuff1 = _T("<charuMACR@c3mO@>");
    strRepBuff2 = _T("</charuMACR@c3mO@>");
    strRepBuff3 = _T("@c3m$@PLUG-IN<");
    strRepBuff4 = _T(">@c3m$@PLUG-IN");
    strRepBuff5 = _T("@c3m$@CLIP");

    if (SourceData->m_strData.Find(_T("$SEL")) != -1) {
        strSelect.Replace(MACRO_START, strRepBuff1);
        strSelect.Replace(MACRO_END, strRepBuff2);
        strSelect.Replace(strPlugStart, strRepBuff3);
        strSelect.Replace(strPlugEnd, strRepBuff4);
        strSelect.Replace(_T("$CLIP"), strRepBuff5);
        isSelect = true;
    }
    if (SourceData->m_strData.Find(_T("$CLIP")) != -1) {
        strClip.Replace(MACRO_START, strRepBuff1);
        strClip.Replace(MACRO_END, strRepBuff2);
        strClip.Replace(strPlugStart, strRepBuff3);
        strClip.Replace(strPlugEnd, strRepBuff4);
        isClip = true;
    }

    CTime cTime = CTime::GetCurrentTime();
    strSourceData = SourceData->m_strData;

    strSourceData.Replace(_T("<charu2MACRO>"), MACRO_START);
    strSourceData.Replace(_T("</charu2MACRO>"), MACRO_END);
    strTag = MACRO_START;
    strTagEnd = MACRO_END;
    nEnd = 0;
    while (1) {
        nStart = strSourceData.Find(strTag, nEnd);//�^�O�̐擪���擾
        if (nStart == -1) break;
        nEnd = strSourceData.Find(strTagEnd, nStart);//�^�O�̏I�����擾
        if (nEnd == -1) nEnd = strSourceData.GetLength();
        if (nStart > nEnd) break;
        nEnd += strTagEnd.GetLength();

        strBuff = strSourceData.Mid(nStart, nEnd - nStart);//�^�O�����؂�o��
        strBuff2 = strBuff;
        strBuff2.Replace(strTag, _T(""));
        strBuff2.Replace(strTagEnd, _T(""));

        if (strBuff2.Find(_T("$Y")) != -1) {//2000 ����4��
            strTime.Format(_T("%04d"), cTime.GetYear());
            strBuff2.Replace(_T("$Y"), strTime);
        }
        if (strBuff2.Find(_T("$y")) != -1) {//00 ����2��
            strTime.Format(_T("%02d"), cTime.GetYear() - 2000);
            strBuff2.Replace(_T("$y"), strTime);
        }
        if (strBuff2.Find(_T("$WH")) != -1) {//00 �a��2��
            strTime.Format(_T("%02d"), cTime.GetYear() - 1988);
            strBuff2.Replace(_T("$WH"), strTime);
        }

        if (strBuff2.Find(_T("$MM")) != -1) {//May ���p��\�L
            CString strMonth[] = { _T("Jan"),_T("Feb"),_T("Mar"),_T("Apr"),_T("May"),_T("Jun"),_T("Jul"),_T("Aug"),_T("Sep"),_T("Oct"),_T("Nov"),_T("Dec") };
            strBuff2.Replace(_T("$MM"), strMonth[cTime.GetMonth() - 1]);
        }
        if (strBuff2.Find(_T("$M")) != -1) {//05 ��2��
            strTime.Format(_T("%02d"), cTime.GetMonth());
            strBuff2.Replace(_T("$M"), strTime);
        }

        if (strBuff2.Find(_T("$D")) != -1) {//06 ���t����2��
            strTime.Format(_T("%02d"), cTime.GetDay());
            strBuff2.Replace(_T("$D"), strTime);
        }
        if (strBuff2.Find(_T("$ddd")) != -1) {//Saturday �j���p��
            CString strYoubi[] = { _T("Sunday"),_T("Monday"),_T("Tuesday"),_T("Wednesday"),_T("Thursday"),_T("Friday"),_T("Saturday") };
            strBuff2.Replace(_T("$ddd"), strYoubi[cTime.GetDayOfWeek() - 1]);
        }
        if (strBuff2.Find(_T("$dd")) != -1) {//Sat �j���p��ȈՕ\�L
            CString strYoubi[] = { _T("Sun"),_T("Mon"),_T("Tue"),_T("Wed"),_T("Thu"),_T("Fri"),_T("Sat") };
            strBuff2.Replace(_T("$dd"), strYoubi[cTime.GetDayOfWeek() - 1]);
        }
        if (strBuff2.Find(_T("$d")) != -1) {//�y�j �j��
            CString strYoubi[] = { _T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("�y") };
            strBuff2.Replace(_T("$d"), strYoubi[cTime.GetDayOfWeek() - 1]);
        }

        if (strBuff2.Find(_T("$H")) != -1) {//21 ����24���ԕ\�L
            strTime.Format(_T("%02d"), cTime.GetHour());
            strBuff2.Replace(_T("$H"), strTime);
        }
        if (strBuff2.Find(_T("$h")) != -1) {//09 ����
            int nHours = cTime.GetHour();
            if (nHours > 11) nHours -= 12;
            strTime.Format(_T("%02d"), nHours);
            strBuff2.Replace(_T("$h"), strTime);
        }
        if (strBuff2.Find(_T("$m")) != -1) {//46 ��
            strTime.Format(_T("%02d"), cTime.GetMinute());
            strBuff2.Replace(_T("$m"), strTime);
        }
        if (strBuff2.Find(_T("$s")) != -1) {//02 �b
            strTime.Format(_T("%02d"), cTime.GetSecond());
            strBuff2.Replace(_T("$s"), strTime);
        }

        if (strBuff2.Find(_T("$ampm")) != -1) {//�ߌ�
            int nHours = cTime.GetHour();
            if (nHours > 11) strTime = _T("�ߌ�");
            else			strTime = _T("�ߑO");
            strBuff2.Replace(_T("$ampm"), strTime);
        }
        if (strBuff2.Find(_T("$AMPM")) != -1) {//PM
            int nHours = cTime.GetHour();
            if (nHours > 11) strTime = _T("PM");
            else			strTime = _T("AM");
            strBuff2.Replace(_T("$AMPM"), strTime);
        }
        if (strBuff2.Find(_T("$SEL")) != -1) {//�I�𕔕�
            strBuff2.Replace(_T("$SEL"), strSelect);
        }
        if (strBuff2.Find(_T("$CLIP")) != -1) {//�I�𕔕�
            strBuff2.Replace(_T("$CLIP"), strClip);
        }
        if (strBuff2.Find(strPlugStart) != -1) {//�v���O�C��
            int nPlugLength = strPlugStart.GetLength();
            int nPlugEndLength = strPlugEnd.GetLength();

            nTagStart = strBuff2.Find(strPlugStart);
            nTagEnd = strBuff2.Find(strPlugEnd);

            if (nTagEnd < nTagStart) {
                strBuff2.Delete(nTagStart, nPlugLength);
            }
            else {

                HMODULE hDLL;
                CharuPlugIn pPlugIn;

                while (strBuff2.Find(strPlugStart) != -1) {
                    int nPlugStart = strBuff2.Find(strPlugStart) + nPlugLength;//�J�n�ʒu�擾
                    int nPlugEnd = strBuff2.Find(strPlugEnd, nPlugStart);//�I���ʒu�擾
                    if (nPlugEnd == -1) nPlugEnd = strBuff2.GetLength();

                    CString strPlug, strDllName, strTmp;
                    TCHAR* strRet;
                    strPlug = strBuff2.Mid(nPlugStart, nPlugEnd - nPlugStart);
                    int nComma = strPlug.Find(_T(","), 0);
                    if (nComma >= 0) {
                        strDllName = strPlug.Left(nComma);
                        strTmp = strPlug.Right(strPlug.GetLength() - nComma - 1);
                    }
                    else {
                        strDllName = _T("");
                        strTmp = strPlug;
                    }

                    //DLL�����[�h
                    hDLL = LoadLibrary(strDllName);
                    if (hDLL) {
                        pPlugIn = (CharuPlugIn)GetProcAddress(hDLL, "CharuPlugIn");
                        if (pPlugIn) {
                            int nSize = strTmp.GetLength() * 5;
                            if (nSize < 10240) nSize = 10240;
                            strRet = new TCHAR[nSize];
                            if (strRet) {
                                CString strPluginRet;
                                int nTmpLen = strTmp.GetLength() + 1;
                                if (strTmp.Find(_T("@c3m")) >= 0) {
                                    strTmp.Replace(_T("@c3mO@"), _T("O"));
                                    strTmp.Replace(_T("@c3m$@"), _T("$"));
                                }
                                //								int nBuffSize = nSize * sizeof(TCHAR);

                                pPlugIn(strTmp.GetBuffer(strTmp.GetLength() + 1), strRet, nSize, SourceData, (void*)&m_focusInfo);
                                strPluginRet = strRet;
                                strPluginRet.Replace(MACRO_START, strRepBuff1);
                                strPluginRet.Replace(MACRO_END, strRepBuff2);
                                strPluginRet.Replace(strPlugStart, strRepBuff3);
                                strPluginRet.Replace(strPlugEnd, strRepBuff4);

                                strBuff2.Delete(nPlugStart, strDllName.GetLength() + nTmpLen);
                                strBuff2.Insert(nPlugStart, strPluginRet);//�u��
                                delete[]strRet;
                            }
                        }
                        else {
                            CString strRes;
                            (void)strRes.LoadString(APP_MES_NO_ENTRY_FUNC);
                            AfxMessageBox(strRes, MB_ICONEXCLAMATION | MB_SYSTEMMODAL, 0);
                        }
                        FreeLibrary(hDLL);
                    }
                    else {
                        CString strMessage, strRes;
                        (void)strRes.LoadString(APP_MES_NOT_FOUND);
                        strMessage.Format(strRes, strDllName);
                        AfxMessageBox(strMessage, MB_ICONEXCLAMATION | MB_SYSTEMMODAL, 0);
                    }
                    strBuff2.Delete(strBuff2.Find(strPlugStart), nPlugLength);
                    strBuff2.Delete(strBuff2.Find(strPlugEnd), nPlugEndLength);
                }
            }
        }
        if (strBuff2.Find(strRelateStart) != -1) {//�֘A�t��
            int nRelateLength = strRelateStart.GetLength();
            nTagStart = strBuff2.Find(strRelateStart);
            nTagEnd = strBuff2.Find(strRelateEnd);

            if (nTagEnd < nTagStart) {
                strBuff2.Delete(nTagStart, nRelateLength);
            }
            else {
                while (strBuff2.Find(strRelateStart) != -1) {
                    int nMacroStart = strBuff2.Find(strRelateStart);
                    int nRelateStart = nMacroStart + nRelateLength;//�J�n�ʒu�擾
                    int nRelateEnd = strBuff2.Find(strRelateEnd, nRelateStart);//�I���ʒu�擾
                    if (nRelateEnd == -1) nRelateEnd = strBuff2.GetLength();

                    CString strRelate;
                    strRelate = strBuff2.Mid(nRelateStart, nRelateEnd - nRelateStart);//�R�}���h���C���؂�o��

                    ShellExecute(NULL, NULL, strRelate, NULL, NULL, SW_SHOWNORMAL);
                    strBuff2.Delete(nMacroStart, nRelateLength + nRelateEnd - nMacroStart);//�^�O���폜
                }
            }
        }
        if (strBuff2.Find(strShellStart) != -1) {//�V�F��
            int nShellLength = strShellStart.GetLength();
            int nRelateLength = strRelateStart.GetLength();

            nTagStart = strBuff2.Find(strShellStart);
            nTagEnd = strBuff2.Find(strShellEnd);

            if (nTagEnd < nTagStart) {
                strBuff2.Delete(nTagStart, nShellLength);
            }
            else {
                while (strBuff2.Find(strShellStart) >= 0) {
                    int macroStartIndex = strBuff2.Find(strShellStart);
                    int commandStartIndex = macroStartIndex + nShellLength;
                    int commandEndIndex = strBuff2.Find(strShellEnd, commandStartIndex);
                    if (commandEndIndex < 0) commandEndIndex = strBuff2.GetLength();
                    CString strCommand = strBuff2.Mid(commandStartIndex, commandEndIndex - commandStartIndex);

                    STARTUPINFO si;
                    PROCESS_INFORMATION pi;
                    ZeroMemory(&si, sizeof(si));
                    si.cb = sizeof(si);
                    ZeroMemory(&pi, sizeof(pi));
                    CreateProcess(NULL, strCommand.GetBuffer(strCommand.GetLength()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
                    WaitForInputIdle(pi.hProcess, INFINITE);
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);

                    strBuff2.Delete(macroStartIndex, nShellLength + commandEndIndex - macroStartIndex);//�^�O���폜
                }
            }
        }

        nMove = strBuff2.GetLength();// - strBuff.GetLength();
        strSourceData.Delete(nStart, strBuff.GetLength());
        strSourceData.Insert(nStart, strBuff2);
        nEnd = nStart + nMove;
    }
    if (strSourceData.Find(_T("@c3m")) >= 0) {
        strSourceData.Replace(_T("@c3mO@"), _T("O"));
        strSourceData.Replace(_T("@c3m$@"), _T("$"));
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
void CCharu3App::onClipboardChanged(CString strClipboard)
{
    if (m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("onClipboardChanged \"%s\"\n"), strClipboard.GetString());
        CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }

    //�A���ŋ�̃N���b�v�{�[�h�X�V�C�x���g���N����̂ő΍�
    //2007/10/27-20:20:19-------------------
    static int nEmptyCnt = 0;
    if (strClipboard == "") {
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

    // Check size
    int nLimit = m_ini.m_key.m_nHistoryLimit;
    const HWND hActiveWindow = ::GetForegroundWindow();
    CString strTitle = CGeneral::getWindowTitle(hActiveWindow);
    if (strTitle != "") {
        CHANGE_KEY key;
        key = m_ini.getAppendKeyInit2(strTitle);
        if (key.m_nMatch >= 0) nLimit = key.m_nHistoryLimit;
    }
    nLimit *= 1024;
    if (nLimit >= 0 && strClipboard.GetLength() > nLimit) {
        return;
    }

    if (m_nPhase == PHASE_IDOL && /*strClipBord != m_strlClipBackup &&*/ strClipboard != "") {
        STRING_DATA data;
        data.m_cKind = KIND_ONETIME;
        data.m_strData = strClipboard;
        data.m_cIcon = KIND_DEFAULT;

        m_nPhase = PHASE_LOCK;
        KillTimer(m_pMainWnd->m_hWnd, TIMER_ACTIVE);

        if (m_ini.m_bDebug) {
            CString strText;
            strText.Format(_T("clipboard record \"%s\"\n"), strClipboard.GetString());
            CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
        }

        if (m_isStockMode) {
            if (!(m_ini.m_fifo.m_bDontSaveSameDataAsLast && strClipboard == m_strPreviousStocked)) {
                if (m_ini.m_fifo.m_strCopySound != _T("")) {
                    PlaySound(m_ini.m_fifo.m_strCopySound, NULL, SND_ASYNC | SND_FILENAME);
                }
                m_pTree->addData(NULL, data);
                m_strPreviousStocked = strClipboard;
            }
        }

        m_pTree->addDataToRecordFolder(data, m_strPreviousRecordedToHistory);
        m_strPreviousRecordedToHistory = strClipboard;

        if (m_isStockMode && m_ini.m_nWindowCheckInterval > 0) {
            SetTimer(m_pMainWnd->m_hWnd, TIMER_ACTIVE, m_ini.m_nWindowCheckInterval, NULL);
        }

        m_nPhase = PHASE_IDOL;
    }

    m_strSavedClipboard = strClipboard;
}

//---------------------------------------------------
//�֐���	fifoClipbord()
//�@�\		�X�g�b�N���[�h����
//---------------------------------------------------
void CCharu3App::fifoClipbord()
{
    if (!m_isStockMode) {
        return;
    }

    if (m_ini.m_fifo.m_nFifo) {
        CString text;
        HTREEITEM hItem = m_pTree->getOneTimeItem(m_ini.m_fifo.m_nFifo);
        if (hItem) {
            text = m_pTree->getDataPtr(hItem)->m_strData;
            m_pTree->deleteData(hItem);
        }
        else {
            text = _T("");
        }
        if (text != _T("")) {
            if (m_ini.m_bDebug) {
                CString strText;
                strText.Format(_T("fifoClipbord text:%s\n"), text.GetString());
                CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
            }
            if (m_clipboard.setClipboardText(text, m_ini.m_nClipboardRetryTimes, m_ini.m_nClipboardRetryInterval)) {
                if (m_ini.m_fifo.m_strPasteSound != _T("")) {
                    PlaySound(m_ini.m_fifo.m_strPasteSound, NULL, SND_ASYNC | SND_FILENAME);
                }
                m_strSavedClipboard = text;
            }
        }
    }

    // It comes in here by pressing the paste key, which is registered as a
    // hotkey. So this key combination should be currently held down.
    // Thus, if we leave the modifier key as it isand send events that
    // release the main keyand press it again, the target window will
    // recognize that the paste key is pressed.
    UnregisterHotKey(NULL, HOTKEY_PASTE);
    keybd_event(m_keySet.m_uVK_Paste, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(m_keySet.m_uVK_Paste, 0, 0, 0);
    RegisterHotKey(NULL, HOTKEY_PASTE, m_keySet.m_uMod_Paste, m_keySet.m_uVK_Paste);

    if (m_ini.m_fifo.m_bAutoOff && !m_pTree->getOneTimeItem(m_ini.m_fifo.m_nFifo)) {
        toggleStockMode(); // Turn off stock mode due to one-time item is gone
    }
}

//---------------------------------------------------
//�֐���	toggleStockMode()
//�@�\		�X�g�b�N���[�h�ؑ�
//---------------------------------------------------
void CCharu3App::toggleStockMode()
{
    if (m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("toggleStockMode %d\n"), !m_isStockMode);
        CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }

    m_isStockMode = !m_isStockMode;
    m_pMainFrame->changeTrayIcon(m_isStockMode);
    if (m_isStockMode) {
        m_strSavedClipboard = _T("");
        setAppendKeyInit(::GetForegroundWindow(), &m_keySet);
        RegisterHotKey(NULL, HOTKEY_PASTE, theApp.m_keySet.m_uMod_Paste, theApp.m_keySet.m_uVK_Paste);//�y�[�X�g�L�[
        if (m_ini.m_nWindowCheckInterval > 0) {
            SetTimer(m_pMainWnd->m_hWnd, TIMER_ACTIVE, m_ini.m_nWindowCheckInterval, NULL);
        }
    }
    else {
        KillTimer(m_pMainWnd->m_hWnd, TIMER_ACTIVE);
        UnregisterHotKey(NULL, HOTKEY_PASTE);
        if (m_ini.m_fifo.m_bCleanupAtTurnOff) {
            m_pTree->cleanupOneTimeItems(m_pTree->GetRootItem());
        }
    }
}

//---------------------------------------------------
//�֐���	getPopupPos(POINT *pPos,int nPodType,HWND hTarget)
//�@�\		�ݒ�ɉ������|�b�v�A�b�v�ʒu���擾
//---------------------------------------------------
void CCharu3App::getPopupPos(POINT* pPos, int nPosType)
{
    RECT rectDeskTop;

    //�^�[�Q�b�g�̃n���h������邽�߂Ɏ��s
    CGeneral::getCaretPos(pPos, &m_focusInfo);//�L�����b�g�ʒu���擾

    //�f�X�N�g�b�v�̃T�C�Y���擾
    if (nPosType > POPUP_POS_MOUSE) {
        HWND hDeskTop = GetDesktopWindow();
        ::GetWindowRect(hDeskTop, &rectDeskTop);
    }
    switch (nPosType) {
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
//�֐���	resetTreeDialog()
//�@�\		�c���[�_�C�A���O���Đ���
//---------------------------------------------------
void  CCharu3App::resetTreeDialog()
{
    std::list<STRING_DATA> stringList;//�f�[�^���X�g

    //�f�[�^�̏��Ԃ𐳋K�����ăR�s�[
    HTREEITEM hStartItem = m_pTree->GetRootItem();
    m_pTree->tree2List(hStartItem, &stringList, true);

    if (m_pTree) {
        delete m_pTree;
    }
    if (m_pTreeDlg) {
        delete m_pTreeDlg;
    }
    m_pTreeDlg = new CMyTreeDialog;
    m_pTree = new CCharu3Tree;
    if (m_pTree) {
        m_pTreeDlg->setTree(m_pTree);
        m_pTreeDlg->Create(IDD_DATA_TREE_VIEW, this->m_pMainWnd);
        m_pTree->setImageList(m_ini.m_IconSize, theApp.m_ini.m_visual.m_strResourceName, m_ini.m_strAppPath);
        m_pTree->setInitInfo(&m_ini.m_nTreeID, &m_ini.m_nSelectID, &m_ini.m_nRecNumber);//ID�����l��ݒ�
        m_pTree->setPlugin(m_ini.m_strRwPluginFolder);
        m_pTree->m_MyStringList = stringList;
        m_pTree->CWnd::LockWindowUpdate();
        m_pTree->copyData(ROOT, TVI_ROOT, &m_pTree->m_MyStringList);//�c���[�Ƀf�[�^���Z�b�g
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
    if (pMsg->message == WM_KEY_HOOK) {
        if (m_ini.m_bDebug) {
            CString strText;
            strText.Format(_T("WM_KEY_HOOK %d %d\n"), pMsg->wParam, pMsg->lParam);
            CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
        }
        UINT uMod = 0;
        if (m_ini.m_pop.m_nDoubleKeyPOP) {
            switch (m_ini.m_pop.m_nDoubleKeyPOP) {
            case 1:	uMod = MOD_SHIFT; break;
            case 2:	uMod = MOD_CONTROL; break;
            case 3:	uMod = MOD_ALT; break;
            }
            if (CGeneral::mod2VK(uMod) == pMsg->wParam) {
                pMsg->message = WM_HOTKEY;
                pMsg->wParam = HOTKEY_POPUP;
            }
        }
        if (m_ini.m_pop.m_nDoubleKeyFIFO) {
            switch (m_ini.m_pop.m_nDoubleKeyFIFO) {
            case 1:	uMod = MOD_SHIFT; break;
            case 2:	uMod = MOD_CONTROL; break;
            case 3:	uMod = MOD_ALT; break;
            }
            if (CGeneral::mod2VK(uMod) == pMsg->wParam) {
                pMsg->message = WM_HOTKEY;
                pMsg->wParam = HOTKEY_FIFO;
            }
        }
        //�t�H���_�z�b�g�L�[����
        int nSize = m_hotkeyVector.size();
        for (int i = 0; i < nSize; i++) {
            if (m_hotkeyVector[i].m_isDoubleClick) {
                if (CGeneral::mod2VK(m_hotkeyVector[i].m_uModKey) == pMsg->wParam) {
                    pMsg->message = WM_HOTKEY;
                    pMsg->wParam = HOT_ITEM_KEY + i;
                    break;
                }
            }
        }
    }

    if (m_nPhase == PHASE_IDOL && pMsg->message == WM_TIMER) {
        // �Ď��^�C�}�[�����i�E�B���h�E���؂�ւ���Ă�����X�g�b�N���[�h�̃y�[�X�g�L�[��؂�ւ���j
        if (pMsg->wParam == TIMER_ACTIVE && m_isStockMode) {
            if (setAppendKeyInit(::GetForegroundWindow(), &m_keySet)) {
                UnregisterHotKey(NULL, HOTKEY_PASTE);
                RegisterHotKey(NULL, HOTKEY_PASTE, theApp.m_keySet.m_uMod_Paste, theApp.m_keySet.m_uVK_Paste);
            }
        }
    }
    //�z�b�g�L�[����
    if (pMsg->message == WM_HOTKEY) {
        switch (pMsg->wParam) {
        case HOTKEY_POPUP://�|�b�v�A�b�v
            if (m_ini.m_bDebug) {
                CGeneral::writeLog(m_ini.m_strDebugLog, _T("HOTKEY_POPUP\n"), _ME_NAME_, __LINE__);
            }
            if (m_ini.m_pop.m_nDoubleKeyPOP) {//�_�u���L�[�N���b�N
                DWORD dwTime = timeGetTime();
                if (static_cast<int>(dwTime - m_dwDoubleKeyPopTime) > m_ini.m_pop.m_nDCKeyPopTime) {
                    m_dwDoubleKeyPopTime = dwTime;
                    return FALSE;
                }
            }
            if (m_nPhase == PHASE_IDOL) {
                POINT pos;
                getPopupPos(&pos, m_ini.m_pop.m_nPopupPos);//�|�b�v�A�b�v�ʒu���擾

                if (m_ini.m_bDebug) {
                    CString strText;
                    strText.Format(_T("getPopupPos %d %d\n"), pos.x, pos.y);
                    CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
                }

                adjustLocation(&pos);

                if (m_ini.m_bDebug) {
                    CString strText;
                    strText.Format(_T("reviseWindowPos %d %d\n"), pos.x, pos.y);
                    CGeneral::writeLog(m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
                }

                popupTreeWindow(pos, m_ini.m_pop.m_bKeepSelection);
            }
            else	m_isCloseKey = true;//����X�C�b�`�����Ă���
            return FALSE;
            break;

        case HOTKEY_FIFO://�X�g�b�N���[�h�؂�ւ�
            if (m_ini.m_bDebug) {
                CGeneral::writeLog(m_ini.m_strDebugLog, _T("HOTKEY_FIFO\n"), _ME_NAME_, __LINE__);
            }

            if (m_ini.m_pop.m_nDoubleKeyFIFO) {//�_�u���L�[�N���b�N
                DWORD dwTime = timeGetTime();
                if (static_cast<int>(dwTime - m_dwDoubleKeyFifoTime) > m_ini.m_pop.m_nDCKeyFifoTime) {
                    m_dwDoubleKeyFifoTime = dwTime;
                    return FALSE;
                }
            }
            toggleStockMode(); // Toggle stock mode by hotkey
            return FALSE;
            break;

        case HOTKEY_PASTE://����FIFO����
            if (m_ini.m_bDebug) {
                CGeneral::writeLog(m_ini.m_strDebugLog, _T("HOTKEY_PASTE\n"), _ME_NAME_, __LINE__);
            }
            fifoClipbord();
            return FALSE;
            break;

            //�z�b�g�A�C�e��
        default:
            if (m_nPhase == PHASE_IDOL) {
                int nTarget = pMsg->wParam - HOT_ITEM_KEY;
                playHotItem(nTarget);
                return FALSE;
            }
            break;
        }
    }
    //����X�C�b�`�������ĂāA�L�[�A�b�v�̎��Ƀ|�b�v�A�b�v�����
    //(ALT�̎��ɑΏۃA�v���̃��j���[���J�����Ⴄ�̂�h��)
    else if ((pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP) && m_isCloseKey) {
        m_pTreeDlg->PostMessage(WM_TREE_CLOSE, IDCANCEL, NULL);
        m_isCloseKey = false;
    }
    //�|�b�v�A�b�v������閽�߂������̂ŕ���
    if (pMsg->message == WM_TREE_CLOSE && m_nPhase == PHASE_POPUP) {
        closeTreeWindow(pMsg->wParam);
        return FALSE;
    }
    return CWinApp::PreTranslateMessage(pMsg);
}

//---------------------------------------------------
//�֐���	OnExit()
//�@�\		�I������
//---------------------------------------------------
void CCharu3App::OnExit()
{
    stopAppendHotKey();
    stopHotkey();
    KillTimer(m_pMainWnd->m_hWnd, TIMER_ACTIVE);
    KillTimer(m_pMainWnd->m_hWnd, TIMER_MOUSE);
    if (m_hLangDll) {
        FreeLibrary(m_hLangDll);
    }
    m_ini.unHookKey();
    SaveData();
    if (m_ini.m_bDebug) {
        CGeneral::writeLog(m_ini.m_strDebugLog, _T("OnExit\n"), _ME_NAME_, __LINE__);
    }
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
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------
void CCharu3App::OnIssues()
{
    ShellExecute(NULL, NULL, _T("https://github.com/itagagaki/charu3-SE/issues"), NULL, NULL, SW_SHOWNORMAL);
}

void CCharu3App::OnDiscussions()
{
    ShellExecute(NULL, NULL, _T("https://github.com/itagagaki/charu3-SE/discussions"), NULL, NULL, SW_SHOWNORMAL);
}

void CCharu3App::OnWiki()
{
    ShellExecute(NULL, NULL, _T("https://github.com/itagagaki/charu3-SE/wiki"), NULL, NULL, SW_SHOWNORMAL);
}

//---------------------------------------------------
//�֐���	OnOption()
//�@�\		�ݒ�
//---------------------------------------------------
void CCharu3App::OnOption()
{
    if (m_nPhase != PHASE_IDOL && m_nPhase != PHASE_POPUP) return;
    int nPhase = m_nPhase;
    m_nPhase = PHASE_OPTION;

    //�^�C�}�[��~
    KillTimer(m_pMainWnd->m_hWnd, TIMER_ACTIVE);
    KillTimer(m_pMainWnd->m_hWnd, TIMER_MOUSE);

    COptMainDialog dlgOption(NULL, m_ini.m_nOptionPage);
    CInit iniBkup;

    iniBkup = m_ini;

    if (nPhase == PHASE_IDOL)	m_ini.unHookKey();
    stopAppendHotKey();//�ǉ��z�b�g�L�[���~
    stopHotkey();

    if (m_ini.m_bDebug) {
        CGeneral::writeLog(m_ini.m_strDebugLog, _T("OnOption\n"), _ME_NAME_, __LINE__);
    }
    int ret = dlgOption.DoModal();
    if (ret == IDOK) {
        m_ini.writeAllInitData();

        if (m_ini.m_bDebug) {
            CGeneral::writeLog(m_ini.m_strDebugLog, _T("writeAllInitData\n"), _ME_NAME_, __LINE__);
        }
    }
    else m_ini = iniBkup;

    CRect rect;
    m_pTreeDlg->GetWindowRect(&rect);

    if (iniBkup.m_visual.m_strResourceName != m_ini.m_visual.m_strResourceName) {
        resetTreeDialog();
    }

    if (ret == IDOK && nPhase == PHASE_POPUP) {
        m_pTreeDlg->ShowWindow(false);
        m_pTreeDlg->showWindowPos(rect.TopLeft(), m_ini.m_DialogSize, SW_SHOW, true);
    }

    registerHotkey();
    setAppendHotKey();//�ǉ��z�b�g�L�[��ݒ�
    if (nPhase == PHASE_IDOL)	m_ini.setHookKey(m_hSelfWnd);

    //�Ď��^�C�}�[�Z�b�g �A�C�h����ԂŃX�g�b�N���[�h���Ȃ�
    if (m_nPhase == PHASE_IDOL && m_isStockMode && m_ini.m_nWindowCheckInterval > 0) {
        SetTimer(m_pMainWnd->m_hWnd, TIMER_ACTIVE, m_ini.m_nWindowCheckInterval, NULL);
    }

    m_hActiveKeyWnd = NULL;
    m_nPhase = nPhase;
}

//---------------------------------------------------
//�֐���	OnChangData()
//�@�\		�f�[�^�t�@�C���؂�ւ�
//---------------------------------------------------
void CCharu3App::OnChangData()
{
    SelectFile();
}

//---------------------------------------------------
//�֐���	OnExport()
//�@�\		�G�N�X�|�[�g
//---------------------------------------------------
void CCharu3App::OnExport()
{
    CString file = NewFile();
    if (file != _T("")) {
        if (!m_pTree->saveDataToFile(file, DAT_FORMAT, NULL)) {
            CString strRes;
            (void)strRes.LoadString(APP_MES_FAILURE_DATA_SAVE);
            AfxMessageBox(strRes, MB_ICONEXCLAMATION, 0);
        }
    }
}

//---------------------------------------------------
//�֐���	OnAddData()
//�@�\		�f�[�^�ǉ�
//---------------------------------------------------
void CCharu3App::OnAddData()
{
    STRING_DATA data;
    CEditDialog editDialog(NULL, &data, true);
    int nRet = editDialog.DoModal();
    if (IDOK == nRet) {
        theApp.m_pTree->addData(NULL, data);
    }
}

//---------------------------------------------------
//�֐���	OnStockStop()
//�@�\		�X�g�b�N���[�h�ؑ�
//---------------------------------------------------
void CCharu3App::OnStockStop()
{
    toggleStockMode(); // Toggle stock mode by main menu
}

//---------------------------------------------------
//�֐���	resetTreeDialog()
//�@�\		�c���[�č\�z
//---------------------------------------------------
void CCharu3App::OnResetTree()
{
    resetTreeDialog();//�c���[�č\�z
}
