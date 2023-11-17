/*----------------------------------------------------------
    Charu3Tree�N���X
                                    2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "Charu3Tree.h"
#include "StringWork.h"
#include "Charu3.h"

#include <list>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _ME_NAME_
#undef _ME_NAME_
#endif
#define _ME_NAME_ _T("Charu3Tree.cpp")

extern CCharu3App theApp;

namespace {

    bool findKeywords(CString text, std::vector<CString> words)
    {
        if (theApp.m_ini.m_bSearchCaseInsensitive) {
            text.MakeLower();
        }
        bool found = (theApp.m_ini.m_nSearchLogic == SEARCH_LOGIC_AND);
        for (std::vector<CString>::iterator it = words.begin(); it != words.end(); it++) {
            bool matched = (text.Find(*it) != -1);
            if (!matched && theApp.m_ini.m_nSearchLogic == SEARCH_LOGIC_AND) {
                found = false;
                break;
            }
            if (matched && theApp.m_ini.m_nSearchLogic == SEARCH_LOGIC_OR) {
                found = true;
                break;
            }
        }
        return found;
    }

} // anonymous namespace

//---------------------------------------------------
//�֐���	CCharu3Tree
//�@�\		�R���X�g���N�^
//---------------------------------------------------
CCharu3Tree::CCharu3Tree()
    : m_dragState(DragState::NOT_DRAGGING)
    , m_nHoverTick(0)
    , m_hDragItem(nullptr)
    , m_hDragTarget(nullptr)
    , m_hPrevTarget(nullptr)
    , m_pDragImage(nullptr)
    , m_ImageList(nullptr)
    , m_pMaxID(nullptr)
    , m_pSelectID(nullptr)
    , m_pRecNumber(nullptr)
{
    m_MyStringList.clear();
}

//---------------------------------------------------
//�֐���	~CCharu3Tree
//�@�\		�f�X�g���N�^
//---------------------------------------------------
CCharu3Tree::~CCharu3Tree()
{
    if (m_ImageList)	delete m_ImageList;
}

BEGIN_MESSAGE_MAP(CCharu3Tree, CTreeCtrl)
    //{{AFX_MSG_MAP(CCharu3Tree)
    ON_WM_MOUSEMOVE()
    ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
    ON_WM_LBUTTONUP()
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
    ON_WM_MOUSEWHEEL()
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
    ON_WM_WINDOWPOSCHANGING()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------
//�֐���	setPlugin(CString strPath)
//�@�\		�v���O�C������ݒ肷��
//---------------------------------------------------
void CCharu3Tree::setPlugin(CString strPath)
{
    m_rwPlugin.reserve(20);//�Ƃ肠����20���m��
    m_rwPlugin.clear();

    CFileFind filefind;
    BOOL more = filefind.FindFile(strPath + _T("\\*.dll"));
    while (more) {
        more = filefind.FindNextFile();
        if (!filefind.IsDirectory()) {
            HMODULE hDLL = LoadLibrary(filefind.GetFilePath());//DLL�����[�h
            if (hDLL) {
                //�֐��|�C���^���擾
                isUnicode pIsUnicode = (isUnicode)GetProcAddress(hDLL, "isUnicode");
#ifdef _UNICODE
                if (pIsUnicode && pIsUnicode())
#else
                if (pIsUnicode && !pIsUnicode())
#endif
                {
                    readDataFile pReadData = (readDataFile)GetProcAddress(hDLL, "readDataFile");
                    initDLL pInitDLL = (initDLL)GetProcAddress(hDLL, "initDLL");
                    endDLL pEndDLL = (endDLL)GetProcAddress(hDLL, "endDLL");
                    writeDataFile pWriteData = (writeDataFile)GetProcAddress(hDLL, "writeDataFile");
                    getFormatName pGetFormatName = (getFormatName)GetProcAddress(hDLL, "getFormatName");
                    getExtension pGetExtension = (getExtension)GetProcAddress(hDLL, "getExtension");

                    if (pInitDLL && pEndDLL && pReadData && pWriteData && pGetFormatName && pGetExtension) {
                        TCHAR tcFormat[256], tcExt[256];
                        pGetExtension(tcExt, sizeof tcExt);
                        pGetFormatName(tcFormat, sizeof tcFormat);
                        RW_PLUGIN plugin(filefind.GetFilePath(), tcFormat, tcExt);
                        m_rwPlugin.push_back(plugin);

                        if (theApp.m_ini.m_bDebug) {
                            CString strText;
                            strText.Format(_T("plugin setting \"%s\"\n"), plugin.m_strPluginFilePath.GetString());
                            CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
                        }
                    }
                }
                FreeLibrary(hDLL);
            }
        }
    }
}

//---------------------------------------------------
//�֐���	setImageList(POINT posSize)
//�@�\		�C���[�W���X�g�����
//---------------------------------------------------
void CCharu3Tree::setImageList(POINT posSize, CString strFileName, CString strPath)
{
    //�A�C�R���̃C���[�W���X�g�����
    if (m_ImageList == nullptr) {
        ::SetCurrentDirectory(strPath);

        CBitmap Bitmap;
        m_ImageList = new CImageList;

        if (strFileName == "" || !CGeneral::loadBitmapFile(strFileName, &Bitmap))
            Bitmap.LoadBitmap(IDB_ICON_BITMAP);//�r�b�g�}�b�v�����[�h

        BITMAP bitMap;
        Bitmap.GetBitmap(&bitMap);

        m_ImageList->Create(bitMap.bmHeight, bitMap.bmHeight, ILC_COLOR16 | ILC_MASK, 22, 2);

        m_ImageList->Add(&Bitmap, RGB(0, 255, 0));
        Bitmap.DeleteObject();
        SetImageList(m_ImageList, TVSIL_NORMAL);
    }
}

//---------------------------------------------------
//�֐���	checkRedundancyID(int nID)
//�@�\		�d��ID���`�F�b�N
//---------------------------------------------------
bool CCharu3Tree::checkRedundancyID(int nID)
{
    bool isRet = false;
    std::list<STRING_DATA>::iterator it;

    for (it = m_MyStringList.begin(); it != m_MyStringList.end(); it++) {
        if (it->m_nMyID == nID) {
            isRet = true;
            break;
        }
    }
    return isRet;
}

//---------------------------------------------------
//�֐���	checkMyChild(HTREEITEM hChildItem,int nMyID)
//�@�\		�����̎q���t�H���_�����ׂ�
//---------------------------------------------------
bool CCharu3Tree::checkMyChild(HTREEITEM hMeItem, HTREEITEM hChildItem)
{
    bool isRet = false;

    if (!hChildItem || !hMeItem) return isRet;

    while (hChildItem) {
        hChildItem = GetParentItem(hChildItem);//�e���擾
        if (hMeItem == hChildItem) {//�����̎q���ł���
            isRet = true;
            break;
        }
    }

    return isRet;
}

//---------------------------------------------------
//�֐���	searchMyRoots(HTREEITEM hStartItem)
//�@�\		��ԏ�̐e��T��
//---------------------------------------------------
HTREEITEM CCharu3Tree::searchMyRoots(HTREEITEM hStartItem)
{
    HTREEITEM hRet = nullptr, hTmp;

    hTmp = hStartItem;
    while (hTmp) {
        hRet = hTmp;
        hTmp = GetParentItem(hTmp);
        if (hTmp) Expand(hTmp, TVE_COLLAPSE);
    }
    return hRet;
}

//---------------------------------------------------
//�֐���	searchParentOption(HTREEITEM hStartItem,CString strOption)
//�@�\		�e�̃I�v�V������{���Ă���
//---------------------------------------------------
HTREEITEM CCharu3Tree::searchParentOption(HTREEITEM hStartItem, CString strOption)
{
    HTREEITEM hRet = nullptr;
    STRING_DATA data;
    int nFound;

    strOption.MakeLower();
    hRet = hStartItem;
    while (hRet) {
        if (hRet) {
            data = getData(hRet);

            data.m_strMacro.MakeLower();
            nFound = data.m_strMacro.Find(strOption);
            if (nFound >= 0) {
                nFound = data.m_strMacro.Find(_T("="), nFound);
                if (nFound >= 0) {
                    break;
                }
            }
        }
        hRet = GetParentItem(hRet);
    }
    return hRet;
}

//---------------------------------------------------
//�֐���	saveDataToFile(CString strFileName,CString strPlugin)
//�@�\		�f�[�^��ۑ�����
//---------------------------------------------------
bool CCharu3Tree::saveDataToFile(CString strFileName, CString strPlugin, HTREEITEM hStartItem)
{
    std::list<STRING_DATA> tmplist;
    STRING_DATA Data;

    //�S�f�[�^���o��
    if (!hStartItem) {
        hStartItem = GetRootItem();
        //�ۑ��f�[�^�����X�g�ɕϊ��i�f�[�^�̏��Ԃ̓c���[�Ŏ����Ă��邽�߁j
        tree2List(hStartItem, &tmplist);
    }
    //�G�N�X�|�[�g�̏ꍇ
    else {
        Data = getData(hStartItem);
        Data.m_nParentID = ROOT;
        tmplist.insert(tmplist.end(), Data);
        if (ItemHasChildren(hStartItem)) tree2List(GetChildItem(hStartItem), &tmplist);
    }

    if (strPlugin != DAT_FORMAT && strPlugin != DAT_FORMAT2) {
        return SaveDataWithPlugin(strFileName, strPlugin, &tmplist);
    }

    FILE* fFile = nullptr;
    if (_tfopen_s(&fFile, strFileName, _T("wb")) != 0) {
        return false;
    }
    if (nullptr == fFile) {
        return false;
    }

#ifdef _UNICODE
    fwrite(DAT_FORMAT2, strlen(DAT_FORMAT2), 1, fFile);//�f�[�^���ʎq
#else
    fwrite(DAT_FORMAT, strlen(DAT_FORMAT), 1, fFile);//�f�[�^���ʎq
#endif
    for (std::list<STRING_DATA>::iterator it = tmplist.begin(); it != tmplist.end(); it++) {

        if (it->m_cKind & KIND_ONETIME) {//�m�[�}�����ڈȊO��ۑ�
            continue;
        }
        fwrite(&it->m_cKind, sizeof(it->m_cKind), 1, fFile);//�f�[�^�̎��
        fwrite(&it->m_cIcon, sizeof(it->m_cIcon), 1, fFile);//�A�C�R�����
        fwrite(&it->m_nMyID, sizeof(it->m_nMyID), 1, fFile);//������ID
        fwrite(&it->m_nParentID, sizeof(it->m_nParentID), 1, fFile);//�e��ID
        fwrite(&it->m_timeCreate, sizeof(it->m_timeCreate), 1, fFile);//�쐬����
        fwrite(&it->m_timeEdit, sizeof(it->m_timeEdit), 1, fFile);//�ύX����

        //�^�C�g��
        int nDataSize = it->m_strTitle.GetLength();
#ifdef _UNICODE
        nDataSize = nDataSize * sizeof(WCHAR);
#endif
        fwrite(&nDataSize, sizeof(nDataSize), 1, fFile);
        CString strTmp = it->m_strTitle;
        fwrite(LPCTSTR(strTmp), nDataSize, 1, fFile);

        //����
        nDataSize = it->m_strData.GetLength();
#ifdef _UNICODE
        nDataSize = nDataSize * sizeof(WCHAR);
#endif
        fwrite(&nDataSize, sizeof(nDataSize), 1, fFile);
        fwrite(LPCTSTR(it->m_strData), nDataSize, 1, fFile);

        //�g���p������f�[�^
        nDataSize = it->m_strMacro.GetLength();
#ifdef _UNICODE
        nDataSize = nDataSize * sizeof(WCHAR);
#endif
        fwrite(&nDataSize, sizeof(nDataSize), 1, fFile);
        fwrite(LPCTSTR(it->m_strMacro), nDataSize, 1, fFile);
    }

    if (theApp.m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("wrriten charu3 data \"%s\"\n"), strFileName.GetString());
        CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }
    fclose(fFile);

    return true;
}

//---------------------------------------------------
//�֐���	saveDataWithPlugin(CString strFileName,CString strPlugin)
//�@�\		�v���O�C�����g���ăf�[�^��ۑ�����
//---------------------------------------------------
bool CCharu3Tree::SaveDataWithPlugin(CString strFileName, CString strPlugin, std::list<STRING_DATA>* tmplist)
{
    bool isRet = false;
    RW_PLUGIN plugin;

    if (!getPlugin(strPlugin, &plugin)) {
        CString strRes;
        (void)strRes.LoadString(APP_MES_NOT_FOUND_WRITEPLUGIN);
        AfxMessageBox(strRes, MB_ICONEXCLAMATION, 0);
        return isRet;
    }

    HMODULE hDLL;
    writeDataFile pWriteData;

    hDLL = LoadLibrary(plugin.m_strPluginFilePath);//DLL�����[�h
    if (hDLL) {
        //�֐��|�C���^���擾
        pWriteData = (writeDataFile)GetProcAddress(hDLL, "writeDataFile");

        if (pWriteData) {
            isRet = pWriteData(strFileName, tmplist);
        }
        FreeLibrary(hDLL);
    }

    if (theApp.m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("wrriten plugin data \"%s\" %s %d\n"), strFileName.GetString(), strPlugin.GetString(), isRet);
        CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }
    return isRet;
}

//---------------------------------------------------
//�֐���	loadDataFileDef(CString strFileName,CString strPlugin)
//�@�\		�f�t�H���g���X�g�Ƀf�[�^��ǂݍ���
//---------------------------------------------------
bool CCharu3Tree::loadDataFileDef(CString strFileName, CString strPlugin) {
    DeleteAllItems();
    bool isRet = loadDataFile(strFileName, strPlugin, &m_MyStringList);
    CWnd::LockWindowUpdate();
    copyData(ROOT, TVI_ROOT, &m_MyStringList);//�c���[�Ƀf�[�^���Z�b�g
    CWnd::UnlockWindowUpdate();

    return isRet;
}

//---------------------------------------------------
//�֐���	loadDataFile(CString strFileName,CString strPlugin)
//�@�\		�f�[�^��ǂݍ���
//---------------------------------------------------
bool CCharu3Tree::loadDataFile(CString strFileName, CString strPlugin, std::list<STRING_DATA>* tmplist)
{
    bool isRet = false;

    if (strPlugin != DAT_FORMAT) {
        //�v���O�C�����g���ăt�@�C������ǂݍ���
        return LoadDataWithPlugin(strFileName, strPlugin, tmplist);
    }

    STRING_DATA data;
    int nDataSize;
    FILE* fFile;
    /*
        ���ʎq
        �f�[�^���		1byte
        �A�C�R�����	1byte
        ID				4byte
        �e��ID			4byte
        �쐬����		4byte
        �^�C�g���T�C�Y	4byte
        �^�C�g��
        �{���T�C�Y		4byte
        �{��
        �}�N���T�C�Y	4byte
        �}�N��
    */
    //�t�@�C���̊g���q������
    char* szReadBuff;
    char strMBCS[256];
    wchar_t* szUniReadBuff;
    int  nVersion = 0;
    CString strExtention;
    strExtention = strFileName.Right(3);
    strExtention.MakeLower();
    std::list<STRING_DATA> readList;//�f�[�^���X�g

    if (strExtention != DAT_EXT) return isRet;

    if (_tfopen_s(&fFile, strFileName, _T("rb")) != 0)	return isRet;//�t�@�C�����J��
    if (!fread(strMBCS, strlen(DAT_FORMAT), 1, fFile)) {
        ;//�f�[�^���ʎq���擾
        fclose(fFile);
        return isRet;
    }

    strMBCS[strlen(DAT_FORMAT)] = '\0';

    if (strcmp(strMBCS, DAT_FORMAT) != 0) {
        nVersion = 1;
        if (strcmp(strMBCS, DAT_FORMAT2) != 0) {
            fclose(fFile);
            return isRet;
        }
    }

    if (feof(fFile)) {
        fclose(fFile);
        return isRet;
    }

    readList.clear();

    //�����X�g�ɓǂݍ���
    while (!feof(fFile)) {
        if (!fread(&data.m_cKind, sizeof(data.m_cKind), 1, fFile)) break;//�f�[�^�̎�ނ�ǂݍ���
        if (!fread(&data.m_cIcon, sizeof(data.m_cIcon), 1, fFile)) break;//�A�C�R�����
        if (!fread(&data.m_nMyID, sizeof(data.m_nMyID), 1, fFile)) break;//������ID��ǂݍ���
        if (!data.m_nMyID) data.m_nMyID = makeNewID();
        if (!fread(&data.m_nParentID, sizeof(data.m_nParentID), 1, fFile)) break;//�e��ID��ǂݍ���
        if (!fread(&data.m_timeCreate, sizeof(data.m_timeCreate), 1, fFile)) break;//�쐬������ǂݍ���
        if (!fread(&data.m_timeEdit, sizeof(data.m_timeEdit), 1, fFile)) break;//�ύX������ǂݍ���

        //������f�[�^�ǂݍ��� MBCS
        if (nVersion == 0) {
            //�^�C�g����ǂݍ���
            if (!fread(&nDataSize, sizeof(nDataSize), 1, fFile)) break;
            szReadBuff = new char[nDataSize + 1];
            fread(szReadBuff, nDataSize, 1, fFile);
            szReadBuff[nDataSize] = '\0';
            data.m_strTitle = szReadBuff;
            delete[] szReadBuff;
            //���͂�ǂݍ���
            if (!fread(&nDataSize, sizeof(nDataSize), 1, fFile)) break;
            szReadBuff = new char[nDataSize + 1];
            fread(szReadBuff, nDataSize, 1, fFile);
            szReadBuff[nDataSize] = '\0';
            data.m_strData = szReadBuff;
            delete[] szReadBuff;

            //�g���p������f�[�^
            if (!fread(&nDataSize, sizeof(nDataSize), 1, fFile)) break;
            szReadBuff = new char[nDataSize + 1];
            fread(szReadBuff, nDataSize, 1, fFile);
            szReadBuff[nDataSize] = '\0';
            data.m_strMacro = szReadBuff;
            delete[] szReadBuff;
        }
        //������f�[�^�ǂݍ��݁@Unicode
        else if (nVersion == 1) {
            //�^�C�g����ǂݍ���
            if (!fread(&nDataSize, sizeof(nDataSize), 1, fFile)) break;
            szUniReadBuff = new wchar_t[nDataSize + 1];
            fread(szUniReadBuff, nDataSize, 1, fFile);
            szUniReadBuff[nDataSize / sizeof(WCHAR)] = '\0';
#ifdef _UNICODE
            data.m_strTitle = szUniReadBuff;
#else
            data.m_strTitle = CGeneral::wideCharToCString(szUniReadBuff);
#endif
            delete[] szUniReadBuff;
            //���͂�ǂݍ���
            if (!fread(&nDataSize, sizeof(nDataSize), 1, fFile)) break;
            szUniReadBuff = new wchar_t[nDataSize + 1];
            fread(szUniReadBuff, nDataSize, 1, fFile);
            szUniReadBuff[nDataSize / sizeof(WCHAR)] = '\0';
#ifdef _UNICODE
            data.m_strData = szUniReadBuff;
#else
            data.m_strData = CGeneral::wideCharToCString(szUniReadBuff);
#endif
            delete[] szUniReadBuff;

            //�g���p������f�[�^
            if (!fread(&nDataSize, sizeof(nDataSize), 1, fFile)) break;
            szUniReadBuff = new wchar_t[nDataSize + 1];
            fread(szUniReadBuff, nDataSize, 1, fFile);
            szUniReadBuff[nDataSize / sizeof(WCHAR)] = '\0';
#ifdef _UNICODE
            data.m_strMacro = szUniReadBuff;
#else
            data.m_strMacro = CGeneral::wideCharToCString(szUniReadBuff);
#endif
            delete[] szUniReadBuff;
        }

        if (data.m_cIcon < KIND_DEFAULT) data.m_cIcon = KIND_DEFAULT;
        if (data.m_timeCreate < 0) data.m_timeCreate = 0;
        if (data.m_timeEdit < data.m_timeCreate) data.m_timeEdit = data.m_timeCreate;

        readList.insert(readList.end(), data);//���X�g�ɒǉ�
    }

    //�{���X�g�ɓo�^
    tmplist->clear();
    std::list<STRING_DATA>::iterator stit;

    for (stit = readList.begin(); stit != readList.end(); stit++) {
        data = *stit;
        if (checkRedundancyID(data.m_nMyID)) {
            int nOldID = data.m_nMyID;
            data.m_nMyID = makeNewID();
            std::list<STRING_DATA>::iterator newit;
            for (newit = readList.begin(); newit != readList.end(); newit++) {
                if (newit->m_nParentID == nOldID) newit->m_nParentID = data.m_nMyID;
            }
        }
        tmplist->insert(tmplist->end(), data);//���X�g�ɒǉ�
    }
    fclose(fFile);

    if (theApp.m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("read charu3 data \"%s\" %d\n"), strFileName.GetString(), tmplist->size());
        CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }

    isRet = true;
    return isRet;
}

//---------------------------------------------------
//�֐���	getPlugin(CString strName,RW_PLUGIN* pPlugin)
//�@�\		�v���O�C��������
//---------------------------------------------------
bool CCharu3Tree::getPlugin(CString strName, RW_PLUGIN* pPlugin)
{
    for (std::vector<RW_PLUGIN>::iterator it = m_rwPlugin.begin(); it != m_rwPlugin.end(); it++) {
        if (it->m_strFormatName == strName) {
            *pPlugin = *it;
            return true;
        }
    }
    return false;
}

//---------------------------------------------------
//�֐���	loadDataFilePlugin(CString strFileName,CString strPlugin)
//�@�\		�v���O�C���Ńf�[�^��ǂݍ���
//---------------------------------------------------
bool CCharu3Tree::LoadDataWithPlugin(CString strFileName, CString strPlugin, std::list<STRING_DATA>* tmplist)
{
    bool isRet = false;
    RW_PLUGIN plugin;

    if (!getPlugin(strPlugin, &plugin)) {
        CString strRes;
        (void)strRes.LoadString(APP_MES_NOT_FOUND_READPLUGIN);
        AfxMessageBox(strRes, MB_ICONEXCLAMATION, 0);
        return false;
    }

    HMODULE hDLL;
    readDataFile pReadData;
    initDLL pInitDLL;
    endDLL	pEndDLL;
    time_t	timeCreate;	//�쐬����
    time(&timeCreate);

    std::list<STRING_DATA>* pData;//�ǂݍ��݃��X�g
    hDLL = LoadLibrary(plugin.m_strPluginFilePath);//DLL�����[�h
    if (hDLL) {
        //�֐��|�C���^���擾
        pReadData = (readDataFile)GetProcAddress(hDLL, "readDataFile");
        pInitDLL = (initDLL)GetProcAddress(hDLL, "initDLL");
        pEndDLL = (endDLL)GetProcAddress(hDLL, "endDLL");

        if (pInitDLL && pEndDLL && pReadData) {
            pInitDLL();
            pData = pReadData(strFileName, false);//�ǂݍ��݊֐����Ăяo��

            if (!pData) {
                pEndDLL();
                FreeLibrary(hDLL);
                return isRet;
            }
            tmplist->clear();
            TCHAR* szBuff;
            STRING_DATA data, data2;

            std::list<STRING_DATA> readList;//�f�[�^���X�g
            std::list<STRING_DATA>::iterator stit;
            //�����X�g�ɓo�^
            for (stit = pData->begin(); stit != pData->end(); stit++) {
                if (stit->m_strTitle != "") {
                    size_t size = stit->m_strTitle.GetLength() + 1;
                    szBuff = new TCHAR[size];
                    _tcscpy_s(szBuff, size, LPCTSTR(stit->m_strTitle));
                    data.m_strTitle = szBuff;
                    delete[] szBuff;
                }
                else	data.m_strTitle = "";

                if (stit->m_strData != "") {
                    size_t size = stit->m_strData.GetLength() + 1;
                    szBuff = new TCHAR[size];
                    _tcscpy_s(szBuff, size, LPCTSTR(stit->m_strData));
                    data.m_strData = szBuff;
                    delete[] szBuff;
                }
                else	data.m_strData = "";

                if (stit->m_strMacro != "") {
                    size_t size = stit->m_strMacro.GetLength() + 1;
                    szBuff = new TCHAR[size];
                    _tcscpy_s(szBuff, size, LPCTSTR(stit->m_strMacro));
                    data.m_strMacro = szBuff;
                    delete[] szBuff;
                }
                else	data.m_strMacro = "";

                data.m_cKind = stit->m_cKind;
                data.m_nMyID = stit->m_nMyID;
                data.m_nParentID = stit->m_nParentID;

                data.m_cIcon = stit->m_cIcon;
                if (data.m_cIcon < KIND_DEFAULT) data.m_cIcon = KIND_DEFAULT;

                data.m_timeCreate = stit->m_timeCreate;
                if (data.m_timeCreate <= 0) data.m_timeCreate = timeCreate;
                data.m_timeEdit = stit->m_timeEdit;
                if (data.m_timeEdit <= 0) data.m_timeEdit = timeCreate;

                readList.insert(readList.end(), data);
            }
            pEndDLL();//���������J��

            //�{���X�g�ɓo�^
            tmplist->clear();
            normalizationID(&readList, ROOT);//ID�𐳋K��
            for (stit = readList.begin(); stit != readList.end(); stit++) {
                tmplist->insert(tmplist->end(), *stit);//���X�g�ɒǉ�
            }
        }

        FreeLibrary(hDLL);
    }

    if (theApp.m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("read plugin data \"%s\" %s\n"), strFileName.GetString(), strPlugin.GetString());
        CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }

    isRet = true;
    return isRet;
}

//---------------------------------------------------
//�֐���	normalizationID(list<STRING_DATA>* pList,int nParentID)
//�@�\		ID�𐳋K��(�قƂ��Charu2Pro�̃f�[�^��p)
//---------------------------------------------------
void CCharu3Tree::normalizationID(std::list<STRING_DATA>* pList, int nParentID)
{
    std::list<STRING_DATA>::iterator it;

    for (it = pList->begin(); it != pList->end(); it++) {
        //�e��ID���r
        if (it->m_nParentID == nParentID) {
            if (checkRedundancyID(it->m_nMyID)) {//ID�̏d�����`�F�b�N
                int nOldID = it->m_nMyID;
                it->m_nMyID = makeNewID();//�V����ID��U��
                std::list<STRING_DATA>::iterator newit;
                for (newit = pList->begin(); newit != pList->end(); newit++) {//�����̎q�̐eID��ς���
                    if (newit->m_nParentID == nOldID) newit->m_nParentID = it->m_nMyID;
                }
            }
            //�����̎q���B���ċA����
            if (it->m_cKind & KIND_FOLDER_ALL)	normalizationID(pList, it->m_nMyID);
        }
    }
}

//---------------------------------------------------
//�֐���	convertMacroPlugin(CString strSourceData)
//�@�\		�}�N���������u��
//---------------------------------------------------
bool CCharu3Tree::convertMacroPlugin(STRING_DATA* SourceData, CString* strRet, CString strSelect, CString strClip, CString strSoftName)
{
    RW_PLUGIN plugin;
    TCHAR* szRet;
    bool isRet = false;

    bool isFound = getPlugin(strSoftName, &plugin);

    if (!isFound) {
        CString strRes;
        (void)strRes.LoadString(APP_MES_NOT_FOUND_MACROPLUGIN);
        AfxMessageBox(strRes, MB_ICONEXCLAMATION, 0);
        return isRet;
    }

    HMODULE hDLL;
    convertMacro pConvertMacro;

    hDLL = LoadLibrary(plugin.m_strPluginFilePath);//DLL�����[�h
    if (hDLL) {
        //�֐��|�C���^���擾
        pConvertMacro = (convertMacro)GetProcAddress(hDLL, "convertMacro");

        if (pConvertMacro) {
            int nSize = SourceData->m_strData.GetLength() * 10 + 10240;
            szRet = new TCHAR[nSize];
            if (szRet) {
                isRet = pConvertMacro((TCHAR*)LPCTSTR(SourceData->m_strData), szRet, nSize,
                    (TCHAR*)LPCTSTR(strSelect), (TCHAR*)LPCTSTR(strClip));
                *strRet = CString(szRet);
                delete[] szRet;
            }
        }
        FreeLibrary(hDLL);
    }
    return isRet;
}

//---------------------------------------------------
//�֐���	copyData
//����		int nParentID �e��ID
//          HTREEITEM hParentTreeItem �e�̃|�C���^
//�@�\		������f�[�^���X�g���t�H���_�\������
//---------------------------------------------------
void CCharu3Tree::copyData(int nParentID, HTREEITEM hParentTreeItem, std::list<STRING_DATA>* pList)
{
    int nRet = 0;
    HTREEITEM hTreeItem;
    std::list<STRING_DATA>::iterator it;

    //�c���[�f�[�^
    TV_INSERTSTRUCT TreeCtrlItem;

    //�f�[�^������
    for (it = pList->begin(); it != pList->end(); it++) {
        //�eID�Ɠ����̂�������
        if (it->m_nParentID == nParentID) {
            data2TreeStruct(&TreeCtrlItem, it);//�f�[�^���ʂ�
            TreeCtrlItem.hParent = hParentTreeItem;//�e��ݒ�

            hTreeItem = InsertItem(&TreeCtrlItem);//�C���T�[�g

            if (*m_pSelectID == it->m_nMyID) SelectItem(hTreeItem);

            //�t�H���_�Ȃ�ċA�Ăяo��
            if (it->m_cKind & KIND_FOLDER_ALL) {
                copyData(it->m_nMyID, hTreeItem, pList);
                if (it->m_cKind & FOLDER_OPEN)	Expand(hTreeItem, TVE_EXPAND);
                else							Expand(hTreeItem, TVE_COLLAPSE);
            }
        }
    }
}

//---------------------------------------------------
//�֐���	moveChildren(HTREEITEM hFromItem,HTREEITEM hToItem)
//�@�\		�t�H���_����t�H���_�Ɏq���������ړ�
//---------------------------------------------------
void CCharu3Tree::moveChildren(HTREEITEM hFromItem, HTREEITEM hToItem)
{
    if (!hFromItem || !hToItem) return;
    std::list<STRING_DATA>::iterator it;
    hFromItem = GetChildItem(hFromItem);
    do {
        //�f�[�^���擾
        STRING_DATA dataF;
        dataF = getData(hFromItem);

        //�c���[�f�[�^���擾
        TV_ITEM TreeCtrlItemFrom;
        ZeroMemory(&TreeCtrlItemFrom, sizeof(TreeCtrlItemFrom));
        TreeCtrlItemFrom.hItem = hFromItem;
        TreeCtrlItemFrom.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
        GetItem(&TreeCtrlItemFrom);

        //�ǉ��f�[�^���쐬
        TV_INSERTSTRUCT AddTreeCtrlItem;

        ZeroMemory(&AddTreeCtrlItem, sizeof(AddTreeCtrlItem));
        AddTreeCtrlItem.hInsertAfter = hToItem;
        AddTreeCtrlItem.item.mask = TreeCtrlItemFrom.mask;
        AddTreeCtrlItem.item.iImage = TreeCtrlItemFrom.iImage;
        AddTreeCtrlItem.item.iSelectedImage = TreeCtrlItemFrom.iSelectedImage;
        AddTreeCtrlItem.item.pszText = dataF.m_strTitle.GetBuffer(dataF.m_strTitle.GetLength());
        AddTreeCtrlItem.item.lParam = TreeCtrlItemFrom.lParam;
        AddTreeCtrlItem.hParent = hToItem;//�t�H���_

        //�����f�[�^��}��
        HTREEITEM hInsertItem = InsertItem(&AddTreeCtrlItem);
        //���Ƀt�H���_����
        if (ItemHasChildren(hFromItem)) {
            //�^�[�Q�b�g�����߂čċA�R�s�[
            moveChildren(hFromItem, hInsertItem);
        }
        hFromItem = GetNextItem(hFromItem, TVGN_NEXT);
    } while (hFromItem);
}

//---------------------------------------------------
//�֐���	copyChildren(HTREEITEM hFromItem,HTREEITEM hToItem)
//�@�\		�t�H���_����t�H���_�Ɏq���������R�s�[
//---------------------------------------------------
void CCharu3Tree::copyChildren(HTREEITEM hFromItem, HTREEITEM hToItem)
{
    if (!hFromItem || !hToItem) return;
    std::list<STRING_DATA>::iterator it;
    hFromItem = GetChildItem(hFromItem);
    HTREEITEM hAddtItem = nullptr;
    while (hFromItem) {
        //�f�[�^���擾
        STRING_DATA dataF;
        dataF = getData(hFromItem);

        //�����f�[�^��}��
        if (hAddtItem)
            hAddtItem = addData(hAddtItem, dataF);
        else
            hAddtItem = addData(hToItem, dataF, true, true);
        // �q���R�s�[�i��������΁j
        copyChildren(hFromItem, hAddtItem);
        hFromItem = GetNextItem(hFromItem, TVGN_NEXT);
    }
}

//---------------------------------------------------
//�֐���	margeTreeData(HTREEITEM hTreeItem,list<STRING_DATA> *pList)
//�@�\		���X�g���}�[�W���ăc���[�ɔ��f������
//---------------------------------------------------
// TODO: Behavior when isRoot == true has not been verified. It seems strange.
HTREEITEM CCharu3Tree::mergeTreeData(HTREEITEM hTreeItem, std::list<STRING_DATA>* pList, bool isRoot)
{
    if (pList->size() > 0) {
        STRING_DATA folder;
        int nParentID;
        CString strRes;
        if (!isRoot) {//�C���|�[�g�t�H���_�����
            (void)strRes.LoadString(APP_INF_IMPORT_FOLDERNAME);
            hTreeItem = addNewFolder(hTreeItem, strRes);//�e�t�H���_�����
            if (!hTreeItem) return nullptr;
            folder = getData(hTreeItem);
            nParentID = folder.m_nMyID;
        }
        else {//���[�g�ɓW�J����
            nParentID = ROOT;
        }
        mergeList(&m_MyStringList, pList, nParentID);
        //�e�̂��Ȃ��q�͏���
        std::list<STRING_DATA>::iterator it, itNext;
        int i;
        for (i = 0, it = m_MyStringList.begin(); it != m_MyStringList.end(); i++, it++) {
            STRING_DATA data;
            data = *it;
            if (it->m_nParentID != ROOT && !checkRedundancyID(it->m_nParentID)) {
                //�e�����Ȃ��̂͏������Ⴄ
                itNext = it;
                if (i > 0) itNext--;
                m_MyStringList.erase(it);
                if (i > 0) it = itNext;
                else it = m_MyStringList.begin();
            }
        }
        CWnd::LockWindowUpdate();
        if (!isRoot) {
            copyData(nParentID, hTreeItem, &m_MyStringList);
        }
        else {
            DeleteAllItems();
            copyData(nParentID, TVI_ROOT, &m_MyStringList);
            hTreeItem = this->GetRootItem();
        }
        CWnd::UnlockWindowUpdate();
    }
    else hTreeItem = nullptr;
    return hTreeItem;
}

//---------------------------------------------------
//�֐���	mergeList(list<STRING_DATA> *pMainList,list<STRING_DATA> *pList,int nParent)
//�@�\		���X�g���}�[�W����
//---------------------------------------------------
int CCharu3Tree::mergeList(std::list<STRING_DATA>* pMainList, std::list<STRING_DATA>* pList, int nParent)
{
    int nRet = 0, nBeginID = 0;
    std::list<STRING_DATA>::iterator it;
    it = pList->begin();
    nBeginID = ROOT;

    for (it = pList->begin(); it != pList->end(); it++) {
        if (it->m_nParentID == nBeginID)	it->m_nParentID = nParent;
        pMainList->insert(pMainList->end(), *it);
        nRet++;
    }

    return nRet;
}

//---------------------------------------------------
//�֐���	addNewFolder(HTREEITEM hTreeItem)
//�@�\		�V�����t�H���_��ǉ�����
//---------------------------------------------------
HTREEITEM CCharu3Tree::addNewFolder(HTREEITEM hTreeItem, CString strName)
{
    STRING_DATA Data;
    Data.m_cKind = KIND_FOLDER;
    Data.m_strTitle = strName;
    HTREEITEM hAddItem = addData(hTreeItem, Data);
    SelectItem(hAddItem);

    if (theApp.m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("add new folder \"%s\"\n"), strName.GetString());
        CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }

    return hAddItem;
}

//---------------------------------------------------
//�֐���	addData(HTREEITEM hTreeItem,STRING_DATA Data)
//�@�\		�f�[�^��ǉ�����
//---------------------------------------------------
HTREEITEM CCharu3Tree::addData(HTREEITEM hTreeItem, STRING_DATA data, bool isNewID/* = true*/, bool isChild /*fasle*/)
{
    if (isNewID)	data.m_nMyID = makeNewID();
    data.m_nParentID = ROOT;
    //NULL�ł���Ɛe��ROOT�ɂȂ�
    if (hTreeItem) {
        HTREEITEM hParentItem;
        if (isChild)	hParentItem = hTreeItem;
        else		hParentItem = GetParentItem(hTreeItem);
        if (hParentItem) {
            STRING_DATA* parentDataPtr = getDataPtr(hParentItem);
            data.m_nParentID = parentDataPtr->m_nMyID;
        }
    }

    if (data.m_strTitle == "") data.m_strTitle = makeTitle(data.m_strData);

    time(&data.m_timeCreate);
    time(&data.m_timeEdit);
    std::list<STRING_DATA>::iterator it = m_MyStringList.insert(m_MyStringList.end(), data);//���X�g�ɒǉ�

    //�c���[�f�[�^�쐬
    TV_INSERTSTRUCT TreeCtrlItem;
    data2TreeStruct(&TreeCtrlItem, it);

    if (hTreeItem) {
        if (isChild) {//�q���ɂ���ꍇ�̓t�H���_�̐擪�ɓ����
            TreeCtrlItem.hParent = hTreeItem;
            TreeCtrlItem.hInsertAfter = TVI_FIRST;//�ǂ��ɓ���邩�w��
        }
        else {//�Z��̏ꍇ�͎����̉��ɓ����
            TreeCtrlItem.hInsertAfter = hTreeItem;
            TreeCtrlItem.hParent = GetParentItem(hTreeItem);
        }
    }
    else {//���[�g�ɓ����ꍇ�͐擪�ɓ����
        TreeCtrlItem.hInsertAfter = TVI_FIRST;//�ǂ��ɓ���邩�w��
        TreeCtrlItem.hParent = TVI_ROOT;//�e��ݒ�
    }

    //�c���[�ɒǉ�
    hTreeItem = InsertItem(&TreeCtrlItem);

    if (theApp.m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("add data %s %s %d\n"), data.m_strTitle.GetString(), data.m_strData.GetString(), data.m_cKind);
        CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }
    return hTreeItem;
}

//---------------------------------------------------
//�֐���	editData(HTREEITEM hTreeItem,STRING_DATA Data)
//�@�\		�f�[�^��ύX����
//---------------------------------------------------
void CCharu3Tree::editData(HTREEITEM hTreeItem, STRING_DATA Data)
{
    time(&Data.m_timeEdit);
    STRING_DATA* dataPtr = getDataPtr(hTreeItem);
    *dataPtr = Data;
    int nIcon = getIconNumber(Data.m_cKind, Data.m_cIcon);
    SetItemImage(hTreeItem, nIcon, nIcon + 1);
    SetItemText(hTreeItem, Data.m_strTitle);

    if (theApp.m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("edit data \"%s\" %d\n"), dataPtr->m_strTitle.GetString(), dataPtr->m_cKind);
        CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }
}

//---------------------------------------------------
//�֐���	editData(HTREEITEM hTreeItem,STRING_DATA Data)
//�@�\		�f�[�^��ύX����
//---------------------------------------------------
void CCharu3Tree::editData2(HTREEITEM hTreeItem)
{
    STRING_DATA* pData = getDataPtr(hTreeItem);
    time(&pData->m_timeEdit);
    int nIcon = getIconNumber(pData->m_cKind, pData->m_cIcon);
    SetItemImage(hTreeItem, nIcon, nIcon + 1);
    SetItemText(hTreeItem, pData->m_strTitle);

    if (theApp.m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("edit data \"%s\" %d\n"), pData->m_strTitle.GetString(), pData->m_cKind);
        CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }
}

//---------------------------------------------------
//�֐���	deleteData(HTREEITEM hTreeItem)
//�@�\		�f�[�^���폜����
//---------------------------------------------------
void CCharu3Tree::deleteData(HTREEITEM hTreeItem)
{
    //�f�[�^�̃A�h���X��ݒ�
    STRING_DATA* dataPtr = getDataPtr(hTreeItem);

    if (theApp.m_ini.m_bDebug) {
        CString strText;
        strText.Format(_T("delete data \"%s\" %d\n"), dataPtr->m_strTitle.GetString(), dataPtr->m_cKind);
        CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }

    std::list<STRING_DATA>::iterator it = findData(dataPtr);
    if (it->m_cKind & KIND_FOLDER_ALL) {
        //�t�H���_���ċA�ō폜
        clearFolder(GetChildItem(hTreeItem));//�t�H���_�̎q���폜
        m_MyStringList.erase(it);
        checkOut(hTreeItem);
        DeleteItem(hTreeItem);
    }
    else {
        m_MyStringList.erase(it);
        checkOut(hTreeItem);
        this->DeleteItem(hTreeItem);
    }
}

//---------------------------------------------------
//�֐���	tree2List(HTREEITEM hStartItem,list<STRING_DATA> *list)
//�@�\		�c���[�\���̂��烊�X�g���쐬
//---------------------------------------------------
void CCharu3Tree::tree2List(HTREEITEM hStartItem, std::list<STRING_DATA>* tmplist, bool isAll /* = false */)
{
    if (!hStartItem) return;
    STRING_DATA data;
    HTREEITEM hItem = hStartItem;
    do {
        data = getData(hItem);

        tmplist->insert(tmplist->end(), data);
        if (ItemHasChildren(hItem) && (getDataOption(data.m_strMacro, "clearrec") != 1 || isAll)) tree2List(GetChildItem(hItem), tmplist);
        hItem = GetNextItem(hItem, TVGN_NEXT);
    } while (hItem);
}

//---------------------------------------------------
//�֐���	data2TreeStruct(TV_INSERTSTRUCT *pTreeCtrlItem,list<STRING_DATA>::iterator *pit)
//�@�\		�c���[�\���̂Ƀf�[�^���i�[
//---------------------------------------------------
void CCharu3Tree::data2TreeStruct(TV_INSERTSTRUCT* pTreeCtrlItem, std::list<STRING_DATA>::iterator it)
{
    //�c���[�f�[�^�쐬
    pTreeCtrlItem->hInsertAfter = TVI_LAST;
    pTreeCtrlItem->item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    STRING_DATA data;
    data.m_strTitle = it->m_strTitle;

    //�A�C�R����ݒ�
    int nIcon = getIconNumber(it->m_cKind, it->m_cIcon);
    pTreeCtrlItem->item.iImage = nIcon;
    pTreeCtrlItem->item.iSelectedImage = nIcon + 1;

    //���O��ݒ�
    pTreeCtrlItem->item.pszText = (TCHAR*)LPCTSTR(it->m_strTitle);
    //�f�[�^�̃A�h���X��ݒ�
    pTreeCtrlItem->item.lParam = (LPARAM) & *it;
}

//---------------------------------------------------
//�֐���	findData(STRING_DATA *ptr)
//�@�\		�f�[�^���X�g����ptr���w���Ă���v�f��T��
//---------------------------------------------------
std::list<STRING_DATA>::iterator CCharu3Tree::findData(STRING_DATA* dataPtr)
{
    std::list<STRING_DATA>::iterator it;
    for (it = m_MyStringList.begin(); it != m_MyStringList.end(); it++) {
        if (&*it == dataPtr) {
            break;
        }
    }
    return it;
}

//---------------------------------------------------
//�֐���	getIconNumber(char cKind,char cIcon)
//�@�\		�A�C�R���ԍ����擾
//---------------------------------------------------
int CCharu3Tree::getIconNumber(char cKind, char cIcon)
{
    int nRet = 0;
    if (cKind & KIND_FOLDER) {
        nRet = ICON_FOLDER;
    }
    else if (cKind & KIND_RIREKI) {
        nRet = ICON_RIREKI_FOLDER;
    }
    else if (cKind & KIND_LOCK) {//���b�N���ڂ̏ꍇ
        if (cIcon > 0) {
            int nIcon = cIcon * 2;
            nRet = ICON_KEY + nIcon;
        }
        else {
            nRet = ICON_KEY;
        }
    }
    else {//�ꎞ���ڂ̏ꍇ
        nRet = ICON_ONETIME;
    }
    return nRet;
}

//---------------------------------------------------
//�֐���	void clearFolder(HTREEITEM hItem)
//�@�\		�w��f�[�^�ȉ����폜����
//---------------------------------------------------
void CCharu3Tree::clearFolder(HTREEITEM hItem)
{
    while (hItem) {
        if (ItemHasChildren(hItem)) {
            clearFolder(GetChildItem(hItem));//�ċA�ō폜
        }
        STRING_DATA* dataPtr = getDataPtr(hItem);
        std::list<STRING_DATA>::iterator it = findData(dataPtr);
        m_MyStringList.erase(it);
        HTREEITEM hNextItem = GetNextItem(hItem, TVGN_NEXT);
        checkOut(hItem);
        this->DeleteItem(hItem);
        hItem = hNextItem;
    }
}

//---------------------------------------------------
//�֐���	void closeFolder(HTREEITEM hStartItem)
//�@�\		�w��f�[�^�ȉ������
//---------------------------------------------------
void CCharu3Tree::closeFolder(HTREEITEM hStartItem)
{
    if (!hStartItem) return;
    HTREEITEM hItem = hStartItem;
    do {
        if (ItemHasChildren(hItem)) {
            STRING_DATA* dataPtr = getDataPtr(hItem);
            Expand(hItem, TVE_COLLAPSE);
            dataPtr->m_cKind &= FOLDER_OPEN ^ 0xff;
            closeFolder(GetChildItem(hItem));//�ċA�ŕ���
        }
        hItem = GetNextItem(hItem, TVGN_NEXT);
    } while (hItem);
}

//---------------------------------------------------
//�֐���	checkFolder(HTREEITEM hStartItem,bool isCheck)
//�@�\		�w��f�[�^�ȉ��Ƀ`�F�b�N������
//---------------------------------------------------
void CCharu3Tree::checkFolder(HTREEITEM hStartItem, bool isCheck, std::list<HTREEITEM>* listItem)
{
    if (!hStartItem) return;
    std::list<STRING_DATA>::iterator it;
    SetCheck(hStartItem, isCheck);
    HTREEITEM hItem = GetChildItem(hStartItem);
    do {
        if (ItemHasChildren(hItem)) {
            checkFolder(hItem, isCheck, listItem);
        }
        SetCheck(hItem, isCheck);
        if (isCheck) {//���X�g�ɒǉ�
            listItem->insert(listItem->end(), hItem);
        }
        else {//���X�g����폜
            std::list<HTREEITEM>::iterator it;
            for (it = listItem->begin(); it != listItem->end(); it++) {
                if (*it == hItem) {
                    listItem->erase(it);
                    break;
                }
            }
        }
        hItem = GetNextItem(hItem, TVGN_NEXT);
    } while (hItem);
}

//---------------------------------------------------
//�֐���	void cleanupOneTimeItems(HTREEITEM hStartItem)
//�@�\		�ꎞ���ڂ�����
//---------------------------------------------------
void CCharu3Tree::cleanupOneTimeItems(HTREEITEM hStartItem, int nKind /*KIND_LOCK���ƍ폜�łȂ����b�N����*/)
{
    if (!hStartItem) return;
    HTREEITEM hItem = hStartItem, hPrevItem;
    do {
        if (ItemHasChildren(hItem)) {
            cleanupOneTimeItems(GetChildItem(hItem), nKind);
        }
        STRING_DATA* dataPtr = getDataPtr(hItem);
        std::list<STRING_DATA>::iterator it = findData(dataPtr);
        hPrevItem = hItem;
        hItem = GetNextItem(hItem, TVGN_NEXT);
        if (it->m_cKind & KIND_ONETIME) {
            if (nKind == KIND_LOCK) {//���b�N�����̏ꍇ
                STRING_DATA data = getData(hPrevItem);
                data.m_cKind = KIND_LOCK;
                editData(hPrevItem, data);
            }
            else {//�N���A�����ɏꍇ
                m_MyStringList.erase(it);
                checkOut(hPrevItem);
                DeleteItem(hPrevItem);
            }
        }
    } while (hItem);
}

void CCharu3Tree::changeIcon(HTREEITEM hTreeItem, int nID)
{
    STRING_DATA data = getData(hTreeItem);
    data.m_cIcon = nID;
    editData(hTreeItem, data);
}

//---------------------------------------------------
//�֐���	getData(HTREEITEM hTreeItem)
//�@�\		�c���[�n���h������f�[�^���擾
//---------------------------------------------------
STRING_DATA CCharu3Tree::getData(HTREEITEM hTreeItem)
{
    return *getDataPtr(hTreeItem);
}
STRING_DATA* CCharu3Tree::getDataPtr(HTREEITEM hTreeItem)
{
    DWORD_PTR lParam = GetItemData(hTreeItem);
    return (STRING_DATA*)lParam;
}

//---------------------------------------------------
//�֐���	searchItem()
//�@�\		�w��̃A�C�e�����������A�I������
//---------------------------------------------------
HTREEITEM CCharu3Tree::searchItem(HTREEITEM hStartItem, bool backward)
{
    std::vector<CString> words;

    // Split strKeywords into words
    {
        CString& strKeywords = theApp.m_ini.m_strSearchKeywords;
        CString tokens, resToken;
        (void)tokens.LoadString(APP_INF_2BYTESPACE);
        tokens += _T(" \t\n\v\f\r");
        int curPos = 0;
        resToken = strKeywords.Tokenize(tokens, curPos);
        while (resToken != _T("")) {
            if (theApp.m_ini.m_bSearchCaseInsensitive) {
                resToken.MakeLower();
            }
            words.push_back(resToken);
            resToken = strKeywords.Tokenize(tokens, curPos);
        }
    }

    // Find
    HTREEITEM hTreeItem = hStartItem ? (backward ? getTruePrevItem(hStartItem) : getTrueNextItem(hStartItem)) : GetRootItem();
    HTREEITEM hEndItem = hTreeItem;
    bool found = false;
    while (hTreeItem) {
        STRING_DATA* data = getDataPtr(hTreeItem);
        if (theApp.m_ini.m_nSearchTarget & SEARCH_TARGET_NAME) {
            found = findKeywords(data->m_strTitle, words);
            if (found) {
                break;
            }
        }
        if (theApp.m_ini.m_nSearchTarget & SEARCH_TARGET_DATA) {
            found = findKeywords(data->m_strData + data->m_strMacro, words);
            if (found) {
                break;
            }
        }
        hTreeItem = backward ? getTruePrevItem(hTreeItem) : getTrueNextItem(hTreeItem);
        if (hTreeItem == hEndItem) {
            break;
        }
    }

    if (found && hTreeItem) {
        SelectItem(hTreeItem);
    }
    else {
        hTreeItem = nullptr;
    }
    return hTreeItem;
}

//---------------------------------------------------
//�֐���	HTREEITEM serchTitle(HTREEITEM hStartItem,CString strKey)
//�@�\		�^�C�g������(�O����v)
//---------------------------------------------------
HTREEITEM CCharu3Tree::searchTitle(HTREEITEM hStartItem, CString strKey, bool makeLower)
{
    if (!hStartItem) {
        return nullptr;
    }

    HTREEITEM hItem = hStartItem;
    do {
        CString itemName = getDataPtr(hItem)->m_strTitle;
        if (makeLower) {
            itemName.MakeLower();
        }
        if (itemName.Find(strKey) == 0) {
            return hItem; // matched
        }
        // try next
        hItem = getTrueNextItem(hItem);
    } while (hItem && hItem != hStartItem);

    // not found
    return nullptr;
}

//---------------------------------------------------
//�֐���	getTrueNextItem(HTREEITEM hTreeItem)
//�@�\		���̃A�C�e�����擾(�F�X�������Ă܂�)
//---------------------------------------------------
HTREEITEM CCharu3Tree::getTrueNextItem(HTREEITEM hTreeItem)
{
    HTREEITEM hRetTreeItem, hParentItem;

    if (!hTreeItem) {		//���[�g��ݒ�
        hRetTreeItem = GetRootItem();
    }
    else if (!GetChildItem(hTreeItem)) {//�q�������邩���ׂ�
        hRetTreeItem = GetNextSiblingItem(hTreeItem);//�Z��n���h�����擾
        if (!hRetTreeItem) {//�Z�킪�������Ȃ����͐e�̌Z��
            hParentItem = hTreeItem;
            do {
                hParentItem = GetParentItem(hParentItem);
                if (!hParentItem) {
                    hRetTreeItem = nullptr;
                    break;
                }
                hRetTreeItem = GetNextSiblingItem(hParentItem);//�Z��n���h�����擾
            } while (hParentItem != GetRootItem() && !hRetTreeItem);
        }
    }
    else {
        hRetTreeItem = GetChildItem(hTreeItem);//�q���̃n���h�����擾
    }
    if (!hRetTreeItem) {
        hRetTreeItem = GetRootItem();//�ă��[�v�ŕK�v�Ȃ̂Ŗ߂�
    }
    return hRetTreeItem;
}

HTREEITEM CCharu3Tree::getTruePrevItem(HTREEITEM hTreeItem)
{
    HTREEITEM hRetTreeItem = nullptr;
    if (hTreeItem) {
        hRetTreeItem = GetPrevSiblingItem(hTreeItem);
        if (!hRetTreeItem) {
            HTREEITEM hParentItem = hTreeItem;
            do {
                hParentItem = GetParentItem(hParentItem);
                if (!hParentItem) {
                    break;
                }
                hRetTreeItem = GetPrevSiblingItem(hParentItem);
            } while (hParentItem != GetRootItem() && !hRetTreeItem);
        }
    }
    if (!hRetTreeItem) {
        hRetTreeItem = getLastSiblingItem(GetRootItem());
    }
    if (hRetTreeItem) {
        if (GetChildItem(hRetTreeItem)) {
            hRetTreeItem = getLastDescendantItem(hRetTreeItem);
        }
    }
    return hRetTreeItem;
}

HTREEITEM CCharu3Tree::getLastSiblingItem(HTREEITEM hTreeItem)
{
    HTREEITEM next;

    if (!hTreeItem) {
        return nullptr;
    }
    while (next = GetNextSiblingItem(hTreeItem)) {
        hTreeItem = next;
    }
    return hTreeItem;
}

HTREEITEM CCharu3Tree::getLastDescendantItem(HTREEITEM hTreeItem)
{
    HTREEITEM next;

    if (!hTreeItem) {
        return nullptr;
    }
    while (next = GetChildItem(hTreeItem)) {
        hTreeItem = getLastSiblingItem(next);
    }
    return hTreeItem;
}

//---------------------------------------------------
//�֐���	getLastVisibleItem()
//�@�\		�Ō�̉��A�C�e�����擾
//---------------------------------------------------
HTREEITEM CCharu3Tree::getLastVisibleItem()
{
    HTREEITEM hRetTreeItem;
    HTREEITEM hTreeItemTmp = nullptr;
    hRetTreeItem = GetRootItem();
    while (hRetTreeItem) {
        hTreeItemTmp = hRetTreeItem;
        hRetTreeItem = GetNextVisibleItem(hRetTreeItem);//�Ō�̉��A�C�e�����擾
    }
    if (hTreeItemTmp)	hRetTreeItem = hTreeItemTmp;
    return hRetTreeItem;
}

//---------------------------------------------------
//�֐���	allIconCheck()
//�@�\		�S�ẴA�C�R�����������ĕύX����
//---------------------------------------------------
void CCharu3Tree::allIconCheck()
{
    HTREEITEM hTreeItem;
    char cOrg;
    STRING_DATA data;
    hTreeItem = GetRootItem();
    int nSize = m_MyStringList.size();
    for (int i = 0; i < nSize; hTreeItem = getTrueNextItem(hTreeItem), i++) {
        if (hTreeItem) {
            data = getData(hTreeItem);
            cOrg = data.m_cIcon;
            data.m_cIcon = decideIcon(data.m_strData);
            if (cOrg != data.m_cIcon) editData(hTreeItem, data);
        }
    }
}

//---------------------------------------------------
//�֐���	decideIcon(CString strData)
//�@�\		�A�C�R��������
//---------------------------------------------------
char CCharu3Tree::decideIcon(CString strData)
{
    char cIcon = -1;

    //�L�[�}�N��
    if (strData.Find(_T("<charu2MACRO_KEY>"), 0) > -1 || strData.Find(_T("<charuMACRO_KEY>"), 0) > -1) {
        cIcon = KIND_KEY;
    }
    //�}�N�����܂܂�Ă���
    else if (strData.Find(_T("<charu2MACRO>"), 0) > -1 || strData.Find(_T("<charuMACRO>"), 0) > -1) {
        //�v���O�C��
        if (strData.Find(_T("$PLUG-IN<"), 0) > -1) {
            cIcon = KIND_PLUG;
        }
        //�V�F��
        else if (strData.Find(_T("$SHELL<"), 0) > -1) {
            cIcon = KIND_EXE;
        }
        //�֘A�t��
        else if (strData.Find(_T("$RELATE<"), 0) > -1) {
            cIcon = KIND_RELATE;
        }
        //�I���e�L�X�g
        else if (strData.Find(_T("$SEL"), 0) > -1) {
            cIcon = KIND_SELECT;
        }
        //�N���b�v�{�[�h
        else if (strData.Find(_T("$CLIP"), 0) > -1) {
            cIcon = KIND_CLIP;
        }
        //���t
        else if (strData.Find(_T("$Y"), 0) > -1 || strData.Find(_T("$y"), 0) > -1 || strData.Find(_T("$WH"), 0) > -1
            || strData.Find(_T("$MM"), 0) > -1 || strData.Find(_T("$M"), 0) > -1 || strData.Find(_T("$D"), 0) > -1
            || strData.Find(_T("$ddd"), 0) > -1 || strData.Find(_T("$dd"), 0) > -1 || strData.Find(_T("$d"), 0) > -1
            || strData.Find(_T("$H"), 0) > -1 || strData.Find(_T("$h"), 0) > -1 || strData.Find(_T("$m"), 0) > -1
            || strData.Find(_T("$s"), 0) > -1 || strData.Find(_T("$ampm"), 0) > -1 || strData.Find(_T("$AMPM"), 0) > -1) {
            cIcon = KIND_DATE;
        }
    }
    return cIcon;
}

//---------------------------------------------------
//�֐���	makeTitle(CString strData)
//�@�\		�ۑ������񂩂�^�C�g�������
//---------------------------------------------------
CString CCharu3Tree::makeTitle(CString strData, int nTitleLength)
{
    CString strBuff;
    strData = strData.Left(256);
    strData.Replace(_T("�@"), _T(""));
    strData.Replace(_T("\r\n"), _T(""));
    strData.Replace(_T("\r"), _T(""));
    strData.Replace(_T("\n"), _T(""));
    strData.Replace(_T("\t"), _T(" "));
    UStringWork::compressionSpace((TCHAR*)(LPCTSTR)strData);
    strData.TrimLeft();
    strData.TrimRight();
    if (nTitleLength > strData.GetLength())	strBuff = strData;
    else	strBuff = strData.Left(nTitleLength);

    return strBuff;
}

//---------------------------------------------------
//�֐���	getOneTimeItem(bool isFirst)
//�@�\		�ŏ����Ō�̈ꎞ���ڂ��擾
//---------------------------------------------------
HTREEITEM CCharu3Tree::getOneTimeItem(int nType)
{
    HTREEITEM hRet = nullptr;

    //������o���̏ꍇ FIFO ��ԉ��̈ꎞ���ڂ�T��
    int nSize = m_MyStringList.size();
    HTREEITEM hTreeItem = GetRootItem();
    for (int i = 0; i < nSize && hTreeItem; i++, hTreeItem = GetNextItem(hTreeItem, TVGN_NEXT)) {
        STRING_DATA* data = getDataPtr(hTreeItem);
        if (data->m_cKind & KIND_ONETIME) {
            hRet = hTreeItem;
            if (theApp.m_ini.m_bDebug) {
                CString strText;
                strText.Format(_T("getOneTimeItem \"%s\" %s %d\n"), data->m_strTitle.GetString(), data->m_strData.GetString(), data->m_cKind);
                CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
            }
            if (nType == 2) break;
        }
    }
    return hRet;
}

//---------------------------------------------------
//�֐���	hasDataOption(CString strData, CString strKind)
//�@�\		�f�[�^�ɃI�v�V�����v���p�e�B�����邩�ǂ�����Ԃ�
//---------------------------------------------------
bool CCharu3Tree::hasDataOption(CString strData, CString strKind)
{
    return strData.MakeLower().Find(strKind) >= 0;
}

//---------------------------------------------------
//�֐���	getDataOption(CString strData, CString strKind)
//�@�\		�f�[�^�̃I�v�V�����v���p�e�B���擾
//---------------------------------------------------
int CCharu3Tree::getDataOption(CString strData, CString strKind)
{
    int nRet = 0, nFound, nFoundEnd;
    CString strBuff;

    nFound = strData.MakeLower().Find(strKind);
    if (nFound >= 0) {
        nFound = strData.Find(_T("="), nFound);
        if (nFound >= 0) {
            nFoundEnd = strData.Find(_T("\n"), nFound);
            if (nFoundEnd <= 0) nFoundEnd = strData.GetLength();
            strBuff = strData.Mid(nFound + 1, nFoundEnd - (nFound + 1));
            strBuff.TrimRight();
            strBuff.TrimLeft();
            if (strBuff == _T("yes")) nRet = 1;
            else if (strBuff == _T("no")) nRet = 0;
            else if (strBuff == _T("")) nRet = -1;
            else nRet = _ttoi(strBuff.GetBuffer(strBuff.GetLength()));
        }
        else nRet = -1;

        if (theApp.m_ini.m_bDebug) {
            CString strText;
            strText.Format(_T("getDataOption \"%s\" %d\n"), strKind.GetString(), nRet);
            CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
        }
    }
    else nRet = -1;

    return nRet;
}

//---------------------------------------------------
//�֐���	getDataOptionHex(CString strData,CString strKind)
//�@�\		�f�[�^�⏕�ݒ�����
//---------------------------------------------------
DWORD CCharu3Tree::getDataOptionHex(CString strData, CString strKind)
{
    DWORD dwRet = -1;
    int nFound, nFoundEnd;
    CString strBuff;

    strData.MakeLower();
    nFound = strData.Find(strKind);
    if (nFound >= 0) {
        nFound = strData.Find(_T("="), nFound);
        if (nFound >= 0) {
            nFoundEnd = strData.Find(_T("\n"), nFound);
            if (nFoundEnd <= 0) nFoundEnd = strData.GetLength();
            strBuff = strData.Mid(nFound + 1, nFoundEnd - (nFound + 1));
            strBuff.TrimRight();
            strBuff.TrimLeft();
            _stscanf_s(strBuff, _T("%x"), &dwRet);
        }

        if (theApp.m_ini.m_bDebug) {
            CString strText;
            strText.Format(_T("getDataOptionHex \"%s\" 0x%x\n"), strKind.GetString(), dwRet);
            CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
        }
    }

    return dwRet;
}

//---------------------------------------------------
//�֐���	getDataOptionStr(CString strData,CString strKind)
//�@�\		�f�[�^�⏕�ݒ�����
//---------------------------------------------------
CString CCharu3Tree::getDataOptionStr(CString strData, CString strKind)
{
    int nFound, nFoundEnd;
    CString strBuff, strRet, strDataCopy;
    strRet = _T("");

    strDataCopy = strData;
    strData.MakeLower();
    nFound = strData.Find(strKind);
    if (nFound >= 0) {
        nFound = strData.Find(_T("="), nFound);
        if (nFound >= 0) {
            nFoundEnd = strData.Find(_T("\n"), nFound);
            if (nFoundEnd <= 0) nFoundEnd = strData.GetLength();
            strBuff = strDataCopy.Mid(nFound + 1, nFoundEnd - (nFound + 1));
            strBuff.TrimRight();
            strBuff.TrimLeft();
            strRet = strBuff;
        }

        if (theApp.m_ini.m_bDebug) {
            CString strText;
            strText.Format(_T("getDataOptionHex \"%s\" %s\n"), strKind.GetString(), strRet.GetString());
            CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
        }
    }
    return strRet;
}

//---------------------------------------------------
//�֐���	addDataToRecordFolder(STRING_DATA data)
//�@�\		��������������
//---------------------------------------------------
void CCharu3Tree::addDataToRecordFolder(STRING_DATA data, CString strClipBkup)
{
    int nSize = m_MyStringList.size(), nRirekiCount, nIsLock, i, nDoDuplication;
    int nNumber = *m_pRecNumber;

    STRING_DATA parentData;
    HTREEITEM hTreeItem, hStart = nullptr;

    if (theApp.m_ini.m_bDebug) {
        CString strText;
        CGeneral::writeLog(theApp.m_ini.m_strDebugLog, _T("addDataToRecordFolder\n"), _ME_NAME_, __LINE__);
        strText.Format(_T("hRootItem:%p TreeSize:%d\n"), GetRootItem(), nSize);
        CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
    }
    //	hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    for (hTreeItem = GetRootItem(), i = 0; i < nSize && hTreeItem && hTreeItem != hStart; i++, hTreeItem = getTrueNextItem(hTreeItem)) {
        parentData = getData(hTreeItem);
        hStart = GetRootItem();
        //�����t�H���_������
        if (parentData.m_cKind & KIND_RIREKI) {
            nDoDuplication = getDataOption(parentData.m_strMacro, _T("duplicationcheck"));
            if ((nDoDuplication && strClipBkup != data.m_strData) || !nDoDuplication) {
                if (*m_pRecNumber == nNumber)	nNumber++;

                if (theApp.m_ini.m_bDebug) {
                    CString strText;
                    strText.Format(_T("number:%d handle:%p title:%s ID:%d\n"), i, hTreeItem, parentData.m_strTitle.GetString(), parentData.m_nMyID);
                    CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
                }

                nIsLock = getDataOption(parentData.m_strMacro, _T("lock"));//������ʂ��擾
                if (!nIsLock)	data.m_cKind = KIND_ONETIME;
                else			data.m_cKind = KIND_LOCK;

                nRirekiCount = getDataOption(parentData.m_strMacro, _T("count"));//���𐔂��擾
                if (nRirekiCount <= 0) nRirekiCount = 2;
                int nDeleteCount = nRirekiCount;
                deleteExcessChildren(GetChildItem(hTreeItem), &nDeleteCount);//�]�v�Ȏq������

                int nTitleLength = getDataOption(parentData.m_strMacro, _T("titlelen"));//�^�C�g���̕�����
                CString strTitle;
                if (nTitleLength > 0 && nTitleLength < 256) {
                    strTitle = makeTitle(data.m_strData, nTitleLength);
                }
                else
                    strTitle = makeTitle(data.m_strData);
                data.m_strTitle = strTitle;

                if (getDataOption(parentData.m_strMacro, _T("number"))) {//�ԍ������邩�ǂ���
                    data.m_strTitle.Format(_T("%02d : %s"), *m_pRecNumber, strTitle.GetString());
                    if (nNumber > nRirekiCount) nNumber = 0;
                }

                // archiving
                {
                    int archiveBy;
                    if (hasDataOption(parentData.m_strMacro, _T("archiveunit"))) {
                        archiveBy = getDataOption(parentData.m_strMacro, _T("archiveunit"));
                    }
                    else {
                        archiveBy = getDataOption(parentData.m_strMacro, _T("classhistory")); // for historical compatibility
                    }
                    if (archiveBy > 0) {
                        archiveHistory(hTreeItem, archiveBy);
                    }
                }

                if (theApp.m_ini.m_bDebug) {
                    CString strText;
                    strText.Format(_T("addDataToRecordFolder \"%s\" %s %d\n"), data.m_strTitle.GetString(), data.m_strData.GetString(), nNumber);
                    CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
                }

                addData(hTreeItem, data, true, true);//�ǉ�
                nSize = m_MyStringList.size();
            }
        }
    }
    *m_pRecNumber = nNumber;
}

//---------------------------------------------------
//�֐���	archiveHistory(HTREEITEM hTreeItem)
//�@�\		�����A�[�J�C�u����
//---------------------------------------------------
void CCharu3Tree::archiveHistory(HTREEITEM hTreeItem, int nRirekiCount)
{
    int nChildCount = getChildCount(hTreeItem, true);//�����t�H���_�����̃A�C�e�������擾
    if (nChildCount >= nRirekiCount) {//�K�w���𐔂𒴂��Ă���
        HTREEITEM hFirstFolder = getFirstFolder(hTreeItem);
        if (!hFirstFolder || (getChildCount(hFirstFolder) >= nRirekiCount)) {//�K�w���𐔂𒴂��Ă邩�A�t�H���_�������ꍇ�͐V�����t�H���_�����
            if (hFirstFolder) {//�}���t�H���_�ʒu�����߂�
                hFirstFolder = GetNextItem(hFirstFolder, TVGN_PREVIOUS);
                if (!hFirstFolder) hFirstFolder = hTreeItem;
            }
            else {//�ŏ��̊K�w�t�H���_�̏ꍇ �Ō�̎q�A�C�e�����擾
                hFirstFolder = getLastChild(hTreeItem);
            }
            STRING_DATA data;
            data.m_cKind = KIND_FOLDER;
            (void)data.m_strTitle.LoadString(APP_INF_CLASS_HISTORY);
            addData(hFirstFolder, data);//�K�w�����t�H���_���쐬
            hFirstFolder = getFirstFolder(hTreeItem);//���߂ăt�H���_�ʒu���擾
        }
        if (hFirstFolder) {//���Ԃꕪ���ړ�
            HTREEITEM hLastChild = getLastChild(hTreeItem);

            //�f�[�^���擾
            STRING_DATA data, dataTarget;
            data = getData(hLastChild);
            dataTarget = getData(hFirstFolder);

            //���f�[�^���擾
            TV_ITEM TreeCtrlItem;
            ZeroMemory(&TreeCtrlItem, sizeof(TreeCtrlItem));
            TreeCtrlItem.hItem = hLastChild;
            TreeCtrlItem.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
            this->GetItem(&TreeCtrlItem);

            //�ǉ��f�[�^���쐬
            TV_INSERTSTRUCT AddTreeCtrlItem;

            ZeroMemory(&AddTreeCtrlItem, sizeof(AddTreeCtrlItem));
            AddTreeCtrlItem.item.mask = TreeCtrlItem.mask;
            AddTreeCtrlItem.item.iImage = TreeCtrlItem.iImage;
            AddTreeCtrlItem.item.iSelectedImage = TreeCtrlItem.iSelectedImage;
            AddTreeCtrlItem.item.pszText = data.m_strTitle.GetBuffer(data.m_strTitle.GetLength());
            AddTreeCtrlItem.item.lParam = TreeCtrlItem.lParam;
            AddTreeCtrlItem.hInsertAfter = TVI_FIRST;
            AddTreeCtrlItem.hParent = hFirstFolder;

            data.m_nParentID = dataTarget.m_nMyID;
            editData(hLastChild, data);
            InsertItem(&AddTreeCtrlItem);
            checkOut(hLastChild);
            DeleteItem(hLastChild);
        }
    }
}

//---------------------------------------------------
//�֐���	HTREEITEM getFirstFolder(HTREEITEM hStartItem)
//�@�\		�ŏ��̃t�H���_��T��
//---------------------------------------------------
HTREEITEM CCharu3Tree::getFirstFolder(HTREEITEM hStartItem)
{
    if (!hStartItem) return nullptr;
    HTREEITEM hItem = hStartItem;
    hItem = GetChildItem(hItem);
    do {
        STRING_DATA* dataPtr = getDataPtr(hItem);
        if (dataPtr->m_cKind & KIND_FOLDER_ALL) 	break;
        hItem = GetNextItem(hItem, TVGN_NEXT);
    } while (hItem);
    return hItem;
}

//---------------------------------------------------
//�֐���	HTREEITEM getLastChild(HTREEITEM hStartItem)
//�@�\		�q�A�C�e���̍Ō��T��
//---------------------------------------------------
HTREEITEM CCharu3Tree::getLastChild(HTREEITEM hStartItem)
{
    if (!hStartItem) return nullptr;
    HTREEITEM hItem = hStartItem, hPrevItem = nullptr;
    hItem = GetChildItem(hItem);
    do {
        STRING_DATA* dataPtr = getDataPtr(hItem);
        if (dataPtr->m_cKind & KIND_DATA_ALL) hPrevItem = hItem;
        hItem = GetNextItem(hItem, TVGN_NEXT);
    } while (hItem);
    return hPrevItem;
}

//---------------------------------------------------
//�֐���	deleteExcessChildren(HTREEITEM hTreeItem,int nCount)
//�@�\		�]�v�Ȏq�̐�������
//---------------------------------------------------
void CCharu3Tree::deleteExcessChildren(HTREEITEM hTreeItem, int* nCount)
{
    if (!hTreeItem) return;
    HTREEITEM hPrevItem;
    do {
        STRING_DATA* dataPtr = getDataPtr(hTreeItem);
        if (ItemHasChildren(hTreeItem)) {
            deleteExcessChildren(GetChildItem(hTreeItem), nCount);
        }
        hPrevItem = hTreeItem;
        hTreeItem = GetNextItem(hTreeItem, TVGN_NEXT);
        if (*nCount <= 1 && !ItemHasChildren(hPrevItem)) {//�q�������Ă����܂�
            deleteData(hPrevItem);
        }
        else if (dataPtr->m_cKind & KIND_DATA_ALL)	(*nCount)--;
    } while (hTreeItem);
    return;
}

//---------------------------------------------------
//�֐���	getChildCount(HTREEITEM hTreeItem)
//�@�\		�q�̐��𐔂���
//---------------------------------------------------
int CCharu3Tree::getChildCount(HTREEITEM hTreeItem, bool isBrotherOnly)
{
    int nChildren = 0;
    if (hTreeItem) hTreeItem = GetChildItem(hTreeItem);
    if (!hTreeItem) return nChildren;
    std::list<STRING_DATA>::iterator it;
    do {
        STRING_DATA* dataPtr = getDataPtr(hTreeItem);
        if (!isBrotherOnly && ItemHasChildren(hTreeItem)) {
            nChildren += getChildCount(hTreeItem);
        }
        if (dataPtr->m_cKind & KIND_DATA_ALL)	nChildren++;
        hTreeItem = GetNextItem(hTreeItem, TVGN_NEXT);
    } while (hTreeItem);

    return nChildren;
}

//---------------------------------------------------
//�֐���	moveFolderTop(HTREEITEM hTreeItem)
//�@�\		�f�[�^���t�H���_�̐擪�Ɉړ�
//---------------------------------------------------
HTREEITEM CCharu3Tree::moveFolderTop(HTREEITEM hTreeItem)
{
    HTREEITEM hRet = nullptr;
    if (!hTreeItem) return hRet;

    STRING_DATA dataF;
    HTREEITEM hParent = GetParentItem(hTreeItem);

    if (!hParent) hParent = GetRootItem();
    dataF = getData(hTreeItem);

    //�c���[�f�[�^���擾
    TV_ITEM TreeCtrlItemFrom;
    ZeroMemory(&TreeCtrlItemFrom, sizeof(TreeCtrlItemFrom));
    TreeCtrlItemFrom.hItem = hTreeItem;
    TreeCtrlItemFrom.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
    GetItem(&TreeCtrlItemFrom);

    //�ǉ��f�[�^���쐬
    TV_INSERTSTRUCT AddTreeCtrlItem;

    ZeroMemory(&AddTreeCtrlItem, sizeof(AddTreeCtrlItem));
    AddTreeCtrlItem.hInsertAfter = TVI_FIRST;
    AddTreeCtrlItem.item.mask = TreeCtrlItemFrom.mask;
    AddTreeCtrlItem.item.iImage = TreeCtrlItemFrom.iImage;
    AddTreeCtrlItem.item.iSelectedImage = TreeCtrlItemFrom.iSelectedImage;
    AddTreeCtrlItem.item.pszText = dataF.m_strTitle.GetBuffer(dataF.m_strTitle.GetLength());
    AddTreeCtrlItem.item.lParam = TreeCtrlItemFrom.lParam;
    AddTreeCtrlItem.hParent = hParent;//�t�H���_

    //�����f�[�^��}��
    hRet = InsertItem(&AddTreeCtrlItem);
    checkOut(hTreeItem);
    DeleteItem(hTreeItem);
    return hRet;
}

//---------------------------------------------------
//�֐���	ClearChecks()
//�@�\		�`�F�b�N���O��
//---------------------------------------------------
void CCharu3Tree::ClearChecks()
{
    int nSize = m_MyStringList.size(), i;
    HTREEITEM hTreeItem;
    for (hTreeItem = GetRootItem(), i = 0; i < nSize && hTreeItem; i++, hTreeItem = getTrueNextItem(hTreeItem)) {
        SetCheck(hTreeItem, false);
    }
}

//---------------------------------------------------
//�֐���	checkItem(HTREEITEM hItem)
//�@�\		�f�[�^���`�F�b�N����
//---------------------------------------------------
void CCharu3Tree::checkItem(HTREEITEM hItem)
{
    ModifyStyle(NULL, TVS_CHECKBOXES, NULL);
    if (ItemHasChildren(hItem)) {
        if (GetCheck(hItem)) {
            checkFolder(hItem, false, &m_ltCheckItems);
        }
        else {
            checkFolder(hItem, false, &m_ltCheckItems);
            checkFolder(hItem, true, &m_ltCheckItems);
        }
    }
    else if (hItem) {
        SetCheck(hItem, !GetCheck(hItem));
        if (GetCheck(hItem)) {//���X�g�ɒǉ�
            m_ltCheckItems.insert(m_ltCheckItems.end(), hItem);
        }
        else {//���X�g����폜
            std::list<HTREEITEM>::iterator it;
            for (it = m_ltCheckItems.begin(); it != m_ltCheckItems.end(); it++) {
                if (*it == hItem) {
                    m_ltCheckItems.erase(it);
                    break;
                }
            }
        }
    }
}

//---------------------------------------------------
//�֐���	checkOut(HTREEITEM hItem)
//�@�\		�`�F�b�N���O��
//---------------------------------------------------
void CCharu3Tree::checkOut(HTREEITEM hItem)
{
    if (ItemHasChildren(hItem)) {
        checkFolder(hItem, false, &m_ltCheckItems);
    }
    else if (hItem) {
        SetCheck(hItem, false);
        std::list<HTREEITEM>::iterator it;
        for (it = m_ltCheckItems.begin(); it != m_ltCheckItems.end(); it++) {
            if (*it == hItem) {
                m_ltCheckItems.erase(it);
                break;
            }
        }
    }
}

//---------------------------------------------------
// CCharu3Tree ���b�Z�[�W �n���h��
//---------------------------------------------------

//---------------------------------------------------
//�֐���	Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
//�@�\		�N���G�C�g����
//---------------------------------------------------
BOOL CCharu3Tree::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
    BOOL isRet = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
    return isRet;
}

//---------------------------------------------------
//�֐���	OnMouseMove(UINT nFlags, CPoint point)
//�@�\		�}�E�X�J�[�\���ړ�
//---------------------------------------------------
void CCharu3Tree::OnMouseMove(UINT nFlags, CPoint point)
{
    if (IsDragging()) {
        CPoint ptTree(point);

        MapWindowPoints(this, &ptTree, 1);
        m_pDragImage->DragMove(point);
        m_pDragImage->DragLeave(this);
        UINT uHitTest = TVHT_ONITEM;
        HTREEITEM hTarget = HitTest(point, &uHitTest);
        if (hTarget) {
            if (hTarget != m_hDragTarget) {
                m_nHoverTick = GetTickCount64();
            }
            STRING_DATA* pTargetData = getDataPtr(hTarget);
            RECT rect;
            GetItemRect(hTarget, &rect, FALSE);
            LONG height = rect.bottom - rect.top;
            if (pTargetData->m_cKind & KIND_FOLDER_ALL) {
                if (!(pTargetData->m_cKind & FOLDER_OPEN) && ItemHasChildren(hTarget) && GetTickCount64() - m_nHoverTick >= 1000) {
                    Expand(hTarget, TVE_EXPAND);
                    pTargetData->m_cKind |= FOLDER_OPEN;
                }
                if (pTargetData->m_cKind & FOLDER_OPEN && ItemHasChildren(hTarget)) {
                    if (ptTree.y < rect.top + height / 3) {
                        m_dragState = DragState::INSERT_BEFORE;
                    }
                    else {
                        m_dragState = DragState::PLACE_INSIDE;
                    }
                }
                else {
                    if (ptTree.y < rect.top + height / 3) {
                        m_dragState = DragState::INSERT_BEFORE;
                    }
                    else if (ptTree.y > rect.bottom - height / 3) {
                        m_dragState = DragState::INSERT_AFTER;
                    }
                    else {
                        m_dragState = DragState::PLACE_INSIDE;
                    }
                }
            }
            else {
                if (ptTree.y < rect.top + height / 2) {
                    m_dragState = DragState::INSERT_BEFORE;
                }
                else {
                    m_dragState = DragState::INSERT_AFTER;
                }
            }
            if (DragState::PLACE_INSIDE == m_dragState) {
                SelectItem(m_hDragTarget);
                SetInsertMark(NULL);
            }
            else {
                SelectItem(NULL);
                SetInsertMark(hTarget, DragState::INSERT_AFTER == m_dragState);
            }
        }
        else {
            SelectItem(NULL);
#pragma warning(push)
#pragma warning(disable:6387)
            // SetInsertMark is declared as:
            //     BOOL SetInsertMark(_In_ HTREEITEM hItem, _In_ BOOL fAfter = TRUE);
            // The specification is to remove the insert mark if hItem is NULL.
            // But SetInsertMark(NULL) warns C6387 because hItem has an _In_ annotation. Annoying!
            SetInsertMark(NULL);
#pragma warning(pop)
        }
        m_hDragTarget = hTarget;
        this->UpdateWindow();
        m_pDragImage->DragEnter(this, point);
    }
    else {
        CPoint ptTree(point);

        MapWindowPoints(this, &ptTree, 1);
        m_pDragImage->DragMove(point);
        m_pDragImage->DragLeave(this);
        UINT uHitTest = TVHT_ONITEMLABEL;
        HTREEITEM hTarget = this->HitTest(point, &uHitTest);
        if (hTarget && m_hPrevTarget != hTarget && uHitTest & 0x06) {
            ::PostMessage(this->GetParent()->m_hWnd, WM_TIPS_CHANGE, (unsigned int)hTarget, 0);
            m_hPrevTarget = hTarget;
        }
        else if (m_hPrevTarget != hTarget) {
            ::PostMessage(this->GetParent()->m_hWnd, WM_TIPS_CHANGE, NULL, 0);
        }
    }
    CTreeCtrl::OnMouseMove(nFlags, point);
}

//---------------------------------------------------
//�֐���	OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
//�@�\		�h���b�O�J�n
//---------------------------------------------------
void CCharu3Tree::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    m_hDragItem = pNMTreeView->itemNew.hItem;

    CPoint ptImage, ptHotSpot;

    RECT rSelItem;
    this->GetItemRect(m_hDragItem, &rSelItem, NULL);
    m_pDragImage = this->CreateDragImage(m_hDragItem);

    m_pDragImage->DragShowNolock(TRUE);
    ptHotSpot.x = pNMTreeView->ptDrag.x + rSelItem.right;
    ptHotSpot.y = pNMTreeView->ptDrag.y + rSelItem.bottom;
    m_pDragImage->SetDragCursorImage(0, ptHotSpot);
    CPoint ptCursorPos;
    GetCursorPos(&ptCursorPos);
    m_pDragImage->BeginDrag(0, CPoint(0, 0));
    m_pDragImage->DragEnter(this, pNMTreeView->ptDrag);
    SetCapture();

    m_hDragTarget = nullptr;
    m_dragState = DragState::INSERT_AFTER;
    SelectItem(NULL);

    *pResult = 0;
}

//---------------------------------------------------
//�֐���	OnLButtonUp(UINT nFlags, CPoint point)
//�@�\		�{�^���𗣂���
//---------------------------------------------------
void CCharu3Tree::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (!IsDragging()) return;

    m_pDragImage->DragLeave(this);
    m_pDragImage->EndDrag();
    delete m_pDragImage;
    ReleaseCapture();
#pragma warning(push)
#pragma warning(disable:6387)
    // SetInsertMark is declared as:
    //     BOOL SetInsertMark(_In_ HTREEITEM hItem, _In_ BOOL fAfter = TRUE);
    // The specification is to remove the insert mark if hItem is NULL.
    // But SetInsertMark(NULL) warns C6387 because hItem has an _In_ annotation. Annoying!
    SetInsertMark(NULL);
#pragma warning(pop)

    //�A�C�e���̈ړ�����������
    if (m_hDragTarget && m_hDragTarget != m_hDragItem) {
        //�����̎q���ɂ͈ړ��ł��Ȃ�
        if (!checkMyChild(m_hDragItem, m_hDragTarget)) {
            AfxGetApp()->DoWaitCursor(1);

            //�f�[�^���擾
            STRING_DATA* pData = getDataPtr(m_hDragItem);
            STRING_DATA* pDataTarget = getDataPtr(m_hDragTarget);

            // �ړ�����c���[�A�C�e���̑������擾
            TV_ITEM TreeCtrlItem;
            ZeroMemory(&TreeCtrlItem, sizeof(TreeCtrlItem));
            TreeCtrlItem.hItem = m_hDragItem;
            TreeCtrlItem.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
            GetItem(&TreeCtrlItem);

            //�ǉ��f�[�^���쐬
            TV_INSERTSTRUCT AddTreeCtrlItem;
            ZeroMemory(&AddTreeCtrlItem, sizeof(AddTreeCtrlItem));
            AddTreeCtrlItem.item.mask = TreeCtrlItem.mask;
            AddTreeCtrlItem.item.iImage = TreeCtrlItem.iImage;
            AddTreeCtrlItem.item.iSelectedImage = TreeCtrlItem.iSelectedImage;
            AddTreeCtrlItem.item.pszText = pData->m_strTitle.GetBuffer(pData->m_strTitle.GetLength());
            AddTreeCtrlItem.item.lParam = TreeCtrlItem.lParam;

            // �ړ���̐e�Ƒ}���ʒu������
            if (pDataTarget->m_cKind & KIND_FOLDER_ALL && DragState::PLACE_INSIDE == m_dragState) {
                AddTreeCtrlItem.hParent = m_hDragTarget;
                AddTreeCtrlItem.hInsertAfter = TVI_FIRST;
                pData->m_nParentID = pDataTarget->m_nMyID;
            }
            else {
                AddTreeCtrlItem.hParent = GetParentItem(m_hDragTarget);
                if (DragState::INSERT_BEFORE == m_dragState) {
                    AddTreeCtrlItem.hInsertAfter = GetPrevSiblingItem(m_hDragTarget);
                    if (NULL == AddTreeCtrlItem.hInsertAfter) {
                        AddTreeCtrlItem.hInsertAfter = TVI_FIRST;
                    }
                }
                else {
                    AddTreeCtrlItem.hInsertAfter = m_hDragTarget;
                }
                pData->m_nParentID = pDataTarget->m_nParentID;
            }

            // �V�c���[�A�C�e����}��
            HTREEITEM hNewItem = InsertItem(&AddTreeCtrlItem);
            // ���c���[�A�C�e���̎q����V�c���[�A�C�e���ɃR�s�[
            copyChildren(m_hDragItem, hNewItem);
            // ���c���[�A�C�e���Ƃ��̎q���̃`�F�b�N���O��
            checkOut(m_hDragItem);
            // ���c���[�A�C�e���̎q�����폜
            clearFolder(GetChildItem(m_hDragItem));
            // ���c���[�A�C�e�����폜
            DeleteItem(m_hDragItem);
            // �V�c���[�A�C�e����I��
            SelectItem(hNewItem);

            AfxGetApp()->DoWaitCursor(0);
        }
    }
    m_dragState = DragState::NOT_DRAGGING;
    CTreeCtrl::OnLButtonUp(nFlags, point);
}

//---------------------------------------------------
//�֐���	OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult)
//�@�\		�t�H���_�J����
//---------------------------------------------------
void CCharu3Tree::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    HTREEITEM hItem = pNMTreeView->itemNew.hItem;
    if (hItem) {
        STRING_DATA* dataPtr = getDataPtr(hItem);
        //����
        if (pNMTreeView->action == 1)
            dataPtr->m_cKind &= FOLDER_OPEN ^ 0xff;
        //�J��
        else if (pNMTreeView->action == 2)
            dataPtr->m_cKind |= FOLDER_OPEN;
    }
    *pResult = 0;
}

void CCharu3Tree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    HTREEITEM hItem = pNMTreeView->itemNew.hItem;
    if (hItem) {
        STRING_DATA* dataPtr = getDataPtr(hItem);
        setSelectID(dataPtr->m_nMyID);
    }
    *pResult = 0;
}

void CCharu3Tree::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
{
    if (lpwndpos) {
        CTreeCtrl::OnWindowPosChanging(lpwndpos);
    }
}

//---------------------------------------------------
//�֐���	setScrollBar
//�@�\		�X�N���[���o�[�ݒ�
//---------------------------------------------------
void CCharu3Tree::setScrollBar()
{
    SCROLLINFO si;

    bool showV = theApp.m_ini.m_visual.m_bScrollbarVertical;
    if (showV) {
        ZeroMemory(&si, sizeof(si));
        si.cbSize = sizeof si;
        si.fMask = SIF_PAGE;
        showV = this->GetScrollInfo(SB_VERT, &si) && si.nPage > 1;
    }
    ShowScrollBar(SB_VERT, showV);

    bool showH = theApp.m_ini.m_visual.m_bScrollbarHorizontal;
    if (showH) {
        ZeroMemory(&si, sizeof(si));
        si.cbSize = sizeof si;
        si.fMask = SIF_PAGE;
        showH = this->GetScrollInfo(SB_HORZ, &si) && si.nPage > 1;
    }
    ShowScrollBar(SB_HORZ, showH);
}

//---------------------------------------------------
//�֐���	OnMouseWheel
//�@�\		�z�C�[������
//---------------------------------------------------
BOOL CCharu3Tree::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if (zDelta < 0)
        SendMessage(WM_KEYDOWN, VK_DOWN, 0);
    else
        SendMessage(WM_KEYDOWN, VK_UP, 0);
    return CTreeCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CCharu3Tree::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    setScrollBar();

    *pResult = 0;
}
