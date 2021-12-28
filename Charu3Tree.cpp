/*----------------------------------------------------------
	Charu3Tree�N���X
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "Charu3.h"
#include "Charu3Tree.h"
#include "General.h"
#include "StringWork.h"

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

//---------------------------------------------------
//�֐���	CCharu3Tree
//�@�\		�R���X�g���N�^
//---------------------------------------------------
CCharu3Tree::CCharu3Tree()
{
	m_MyStringList.clear();
	m_ImageList = NULL;
	m_nMaxID = 0;
	m_nSelectID = 0;
	m_isDrag = false;
	m_hPrevTarget = NULL;
}

//---------------------------------------------------
//�֐���	~CCharu3Tree
//�@�\		�f�X�g���N�^
//---------------------------------------------------
CCharu3Tree::~CCharu3Tree()
{
	if(m_ImageList)	delete m_ImageList;
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
bool CCharu3Tree::setPlugin(CString strPath)
{
	bool isRet = false;

	readDataFile pReadData;
	HMODULE hDLL;
	initDLL pInitDLL;
	endDLL	pEndDLL;
	writeDataFile pWriteData;
	getFormatName pGetFormatName;
	getExtension  pGetExtension;
	isUnicode pIsUnicode;
	RW_PLUGIN plugin;
	CString strFileName;

	CFileFind filefind;
	strFileName = strPath + "\\*.dll";
	BOOL isSuccess = filefind.FindFile(strFileName);
	if (!isSuccess) return isRet;

	m_rwPlugin.reserve(20);//�Ƃ肠����20���m��
	m_rwPlugin.clear();
	while(isSuccess == TRUE) {
		isSuccess = filefind.FindNextFile();
		if(filefind.IsDots() == TRUE);				//�J�����g���e
		else if(filefind.IsDirectory() == TRUE);	//�t�H���_
		else {
			hDLL = LoadLibrary(filefind.GetFilePath());//DLL�����[�h
			if(hDLL){
				//�֐��|�C���^���擾
				pIsUnicode = (isUnicode)GetProcAddress(hDLL,"isUnicode");
#ifdef _UNICODE
				if(pIsUnicode && pIsUnicode()) {
#else
				if(pIsUnicode && !pIsUnicode()) {
#endif
					pReadData = (readDataFile)GetProcAddress(hDLL,"readDataFile");
					pInitDLL = (initDLL)GetProcAddress(hDLL,"initDLL");
					pEndDLL = (endDLL)GetProcAddress(hDLL,"endDLL");
					pWriteData = (writeDataFile)GetProcAddress(hDLL,"writeDataFile");
					pGetFormatName = (getFormatName)GetProcAddress(hDLL,"getFormatName");
					pGetExtension = (getExtension)GetProcAddress(hDLL,"getExtension");

					if(pInitDLL && pEndDLL && pReadData && pWriteData && pGetFormatName && pGetExtension) {
						TCHAR strBuff[256];
						int a = sizeof(strBuff);
						plugin.m_strPluginName = filefind.GetFilePath();
						pGetExtension(strBuff,sizeof(strBuff));
						plugin.m_strExtension = strBuff;
						pGetFormatName(strBuff,sizeof(strBuff));
						plugin.m_strSoftName = strBuff;

						//�f�o�b�O���O����
						if(theApp.m_ini.m_nDebug) {
							CString strText;
							strText.Format(_T("plugin setting \"%s\"\n"),strBuff);
							CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
						}
						
						m_rwPlugin.push_back(plugin);
					}
				}
				FreeLibrary(hDLL);
			}
		}
	}
	isRet = true;
	return isRet;
}

//---------------------------------------------------
//�֐���	setImageList(POINT posSize)
//�@�\		�C���[�W���X�g�����
//---------------------------------------------------
void CCharu3Tree::setImageList(POINT posSize,CString strFileName,CString strPath)
{
	//�A�C�R���̃C���[�W���X�g�����
	if(m_ImageList == NULL) {
		::SetCurrentDirectory(strPath);

		CBitmap Bitmap;
		m_ImageList = new CImageList;

		if(strFileName == "" || !CGeneral::loadBitmapFile(strFileName,&Bitmap))
			Bitmap.LoadBitmap(IDB_ICON_BITMAP);//�r�b�g�}�b�v�����[�h

		BITMAP bitMap;
		Bitmap.GetBitmap(&bitMap);

		m_ImageList->Create(bitMap.bmHeight,bitMap.bmHeight,ILC_COLOR16 | ILC_MASK,22,2);

		m_ImageList->Add(&Bitmap,RGB(0,255,0));
		Bitmap.DeleteObject();		
		SetImageList(m_ImageList,TVSIL_NORMAL);
	}
}	

//---------------------------------------------------
//�֐���	checkRedundancyID(int nID)
//�@�\		�d��ID���`�F�b�N
//---------------------------------------------------
bool CCharu3Tree::checkRedundancyID(int nID)
{
	bool isRet = false;
	list<STRING_DATA>::iterator it;

	for (it = m_MyStringList.begin(); it != m_MyStringList.end(); it++) {
		if(it->m_nMyID == nID) {
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
bool CCharu3Tree::checkMyChild(HTREEITEM hMeItem,HTREEITEM hChildItem)
{
	bool isRet = false;

	if(!hChildItem || !hMeItem) return isRet;

	while(hChildItem) {
		hChildItem = GetParentItem(hChildItem);//�e���擾
		if(hMeItem == hChildItem) {//�����̎q���ł���
			isRet = true;
			break;
		}
	}

	return isRet;
}

//---------------------------------------------------
//�֐���	serchMyRoots(HTREEITEM hStartItem)
//�@�\		��ԏ�̐e��T��
//---------------------------------------------------
HTREEITEM CCharu3Tree::serchMyRoots(HTREEITEM hStartItem)
{
	HTREEITEM hRet = NULL,hTmp;

	hTmp = hStartItem;
	while(hTmp) {
		hRet = hTmp;
		hTmp = GetParentItem(hTmp);
		if(hTmp) Expand(hTmp,TVE_COLLAPSE);
	}
	return hRet;
}

//---------------------------------------------------
//�֐���	serchParentOption(HTREEITEM hStartItem,CString strOption)
//�@�\		�e�̃I�v�V������{���Ă���
//---------------------------------------------------
HTREEITEM CCharu3Tree::serchParentOption(HTREEITEM hStartItem,CString strOption)
{
	HTREEITEM hRet = NULL;
	STRING_DATA data;
	int nFound;
	
	strOption.MakeLower();
	hRet = hStartItem;
	while(hRet) {
		if(hRet) {
			data = getData(hRet);

			data.m_strMacro.MakeLower();
			nFound = data.m_strMacro.Find(strOption);
			if(nFound >= 0) {
				nFound = data.m_strMacro.Find(_T("="),nFound);
				if(nFound >= 0) {
					break;
				}
			}
		}
		hRet = GetParentItem(hRet);
	}
	return hRet;
}

//---------------------------------------------------
//�֐���	saveDataFile(CString strFileName,CString strPlugin)
//�@�\		�f�[�^��ۑ�����
//---------------------------------------------------
bool CCharu3Tree::saveDataFile(CString strFileName,CString strPlugin,HTREEITEM hStartItem)
{
	bool isRet = false;

	if(theApp.m_ini.m_etc.m_nDontSave) return true;

	list<STRING_DATA> tmplist;
	list<STRING_DATA>::iterator it;
	STRING_DATA Data;

	//�S�f�[�^���o��
	if(!hStartItem) {
		hStartItem = GetRootItem();
		//�ۑ��f�[�^�����X�g�ɕϊ��i�f�[�^�̏��Ԃ̓c���[�Ŏ����Ă��邽�߁j
		tree2List(hStartItem,&tmplist);
	}
	//�G�N�X�|�[�g�̏ꍇ
	else {
		Data = getData(hStartItem,&it);
		Data.m_nParentID = ROOT;
		tmplist.insert(tmplist.end(),Data);
		if(ItemHasChildren(hStartItem)) tree2List(GetChildItem(hStartItem),&tmplist);
	}


	if(strPlugin != DAT_FORMAT && strPlugin != DAT_FORMAT2) {
		isRet = saveDataFilePlugin(strFileName,strPlugin,&tmplist);
		return isRet;
	}

	int nDataSize;
	FILE *fFile;
	CString strTmp;

	//�f�[�^��ۑ�����
	if(!(fFile = _tfopen(strFileName,_T("wb"))))	return isRet;//�t�@�C�����J��

	#ifdef _UNICODE
		fwrite(DAT_FORMAT2,strlen(DAT_FORMAT2),1,fFile);//�f�[�^���ʎq
	#else
		fwrite(DAT_FORMAT,strlen(DAT_FORMAT),1,fFile);//�f�[�^���ʎq
	#endif
	for (it = tmplist.begin(); it != tmplist.end(); it++) {
		
		if(it->m_cKind & KIND_ONETIME) {//�m�[�}�����ڈȊO��ۑ�
			continue;
		}
		fwrite(&it->m_cKind,sizeof(it->m_cKind),1,fFile);//�f�[�^�̎��
		fwrite(&it->m_cIcon,sizeof(it->m_cIcon),1,fFile);//�A�C�R�����
		fwrite(&it->m_nMyID,sizeof(it->m_nMyID),1,fFile);//������ID
		fwrite(&it->m_nParentID,sizeof(it->m_nParentID),1,fFile);//�e��ID
		fwrite(&it->m_timeCreate,sizeof(it->m_timeCreate),1,fFile);//�쐬����
		fwrite(&it->m_timeEdit,sizeof(it->m_timeEdit),1,fFile);//�ύX����

		//�^�C�g��
		nDataSize = it->m_strTitle.GetLength();
#ifdef _UNICODE
		nDataSize = nDataSize * sizeof(WCHAR);
#endif
		fwrite(&nDataSize,sizeof(nDataSize),1,fFile);
		strTmp = it->m_strTitle;
		fwrite(LPCTSTR(strTmp),nDataSize,1,fFile);

		//����
		nDataSize = it->m_strData.GetLength();
#ifdef _UNICODE
		nDataSize = nDataSize * sizeof(WCHAR);
#endif
		fwrite(&nDataSize,sizeof(nDataSize),1,fFile);
		fwrite(LPCTSTR(it->m_strData),nDataSize,1,fFile);

		//�g���p������f�[�^
		nDataSize = it->m_strMacro.GetLength();
#ifdef _UNICODE
		nDataSize = nDataSize * sizeof(WCHAR);
#endif
		fwrite(&nDataSize,sizeof(nDataSize),1,fFile);
		fwrite(LPCTSTR(it->m_strMacro),nDataSize,1,fFile);
	}
	//�f�o�b�O���O����
	if(theApp.m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("wrriten charu3 data \"%s\"\n"),strFileName);
		CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}
	fclose(fFile);

	isRet = true;
	return isRet;
}

//---------------------------------------------------
//�֐���	saveDataFilePlugin(CString strFileName,CString strPlugin)
//�@�\		�v���O�C�����g���ăf�[�^��ۑ�����
//---------------------------------------------------
bool CCharu3Tree::saveDataFilePlugin(CString strFileName,CString strPlugin,list<STRING_DATA> *tmplist)
{
	bool isRet = false;
	RW_PLUGIN plugin;

	if(!getPlugin(strPlugin,&plugin)) {
		CString strRes;
		strRes.LoadString(APP_MES_NOT_FOUND_WRITEPLUGIN);
		AfxMessageBox(strRes,MB_ICONEXCLAMATION,0);
		return isRet;
	}

	HMODULE hDLL;
	writeDataFile pWriteData;

	hDLL = LoadLibrary(plugin.m_strPluginName);//DLL�����[�h
	if(hDLL){
		//�֐��|�C���^���擾
		pWriteData = (writeDataFile)GetProcAddress(hDLL,"writeDataFile");

		if(pWriteData) {
			isRet = pWriteData(strFileName,tmplist);
		}
		FreeLibrary(hDLL);
	}
	//�f�o�b�O���O����
	if(theApp.m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("wrriten plugin data \"%s\" %s %d\n"),strFileName,strPlugin,isRet);
		CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}
	return isRet;
}

//---------------------------------------------------
//�֐���	loadDataFileDef(CString strFileName,CString strPlugin)
//�@�\		�f�t�H���g���X�g�Ƀf�[�^��ǂݍ���
//---------------------------------------------------
bool CCharu3Tree::loadDataFileDef(CString strFileName,CString strPlugin) {
	DeleteAllItems();
	bool isRet = loadDataFile(strFileName,strPlugin,&m_MyStringList);
	CWnd::LockWindowUpdate();
	copyData(ROOT,TVI_ROOT,&m_MyStringList);//�c���[�Ƀf�[�^���Z�b�g
	CWnd::UnlockWindowUpdate();

	return isRet;
}	

//---------------------------------------------------
//�֐���	loadDataFile(CString strFileName,CString strPlugin)
//�@�\		�f�[�^��ǂݍ���
//---------------------------------------------------
bool CCharu3Tree::loadDataFile(CString strFileName,CString strPlugin,list<STRING_DATA> *tmplist)
{
	bool isRet = false;

	if(strPlugin != DAT_FORMAT) {
		//�v���O�C�����g���ăt�@�C������ǂݍ���
		return loadDataFilePlugin(strFileName,strPlugin,tmplist);
	}

	STRING_DATA data;
	initStringData(&data);
	int nDataSize;
	FILE *fFile;
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
	char  *szReadBuff;
	char strMBCS[256];
	wchar_t *szUniReadBuff;
	int  nVersion = 0;
	CString strExtention;
	strExtention = strFileName.Right(3);
	strExtention.MakeLower();
	list<STRING_DATA> readList;//�f�[�^���X�g

	if(strExtention != DAT_EXT) return isRet;

	if(!(fFile = _tfopen(strFileName,_T("rb"))))	return isRet;//�t�@�C�����J��
	if(!fread(strMBCS,strlen(DAT_FORMAT),1,fFile)) {;//�f�[�^���ʎq���擾
		fclose(fFile);
		return isRet;
	}

	strMBCS[strlen(DAT_FORMAT)] = NULL;
	
	if(strcmp(strMBCS,DAT_FORMAT) != 0) {
		nVersion = 1;
		if(strcmp(strMBCS,DAT_FORMAT2) != 0) {
			fclose(fFile);
			return isRet;
		}
	}

	if(feof(fFile)) {
		fclose(fFile);
		return isRet;
	}

	readList.clear();

	//�����X�g�ɓǂݍ���
	while(!feof(fFile)) {
		if(!fread(&data.m_cKind,sizeof(data.m_cKind),1,fFile)) break;//�f�[�^�̎�ނ�ǂݍ���
		if(!fread(&data.m_cIcon,sizeof(data.m_cIcon),1,fFile)) break;//�A�C�R�����
		if(!fread(&data.m_nMyID,sizeof(data.m_nMyID),1,fFile)) break;//������ID��ǂݍ���
		if(!data.m_nMyID) data.m_nMyID = makeNewID();
		if(!fread(&data.m_nParentID,sizeof(data.m_nParentID),1,fFile)) break;//�e��ID��ǂݍ���
		if(!fread(&data.m_timeCreate,sizeof(data.m_timeCreate),1,fFile)) break;//�쐬������ǂݍ���
		if(!fread(&data.m_timeEdit,sizeof(data.m_timeEdit),1,fFile)) break;//�ύX������ǂݍ���

		//������f�[�^�ǂݍ��� MBCS
		if(nVersion == 0) {
			//�^�C�g����ǂݍ���
			if(!fread(&nDataSize,sizeof(nDataSize),1,fFile)) break;
			szReadBuff = new char[nDataSize+1];
			fread(szReadBuff,nDataSize,1,fFile);
			szReadBuff[nDataSize] = NULL;
			data.m_strTitle = szReadBuff;
			delete [] szReadBuff;
			//���͂�ǂݍ���
			if(!fread(&nDataSize,sizeof(nDataSize),1,fFile)) break;
			szReadBuff = new char[nDataSize+1];
			fread(szReadBuff,nDataSize,1,fFile);
			szReadBuff[nDataSize] = NULL;
			data.m_strData = szReadBuff;
			delete [] szReadBuff;

			//�g���p������f�[�^
			if(!fread(&nDataSize,sizeof(nDataSize),1,fFile)) break;
			szReadBuff = new char[nDataSize+1];
			fread(szReadBuff,nDataSize,1,fFile);
			szReadBuff[nDataSize] = NULL;
			data.m_strMacro = szReadBuff;
			delete [] szReadBuff;
		}
		//������f�[�^�ǂݍ��݁@Unicode
		else if(nVersion == 1) {
			//�^�C�g����ǂݍ���
			if(!fread(&nDataSize,sizeof(nDataSize),1,fFile)) break;
			szUniReadBuff = new wchar_t[nDataSize+1];
			fread(szUniReadBuff,nDataSize,1,fFile);
			szUniReadBuff[nDataSize / sizeof(WCHAR)] = NULL;
#ifdef _UNICODE
			data.m_strTitle = szUniReadBuff;
#else
			data.m_strTitle = CGeneral::wideCharToCString(szUniReadBuff);
#endif
			delete [] szUniReadBuff;
			//���͂�ǂݍ���
			if(!fread(&nDataSize,sizeof(nDataSize),1,fFile)) break;
			szUniReadBuff = new wchar_t[nDataSize+1];
			fread(szUniReadBuff,nDataSize,1,fFile);
			szUniReadBuff[nDataSize / sizeof(WCHAR)] = NULL;
#ifdef _UNICODE
			data.m_strData = szUniReadBuff;
#else
			data.m_strData = CGeneral::wideCharToCString(szUniReadBuff);
#endif
			delete [] szUniReadBuff;

			//�g���p������f�[�^
			if(!fread(&nDataSize,sizeof(nDataSize),1,fFile)) break;
			szUniReadBuff = new wchar_t[nDataSize+1];
			fread(szUniReadBuff,nDataSize,1,fFile);
			szUniReadBuff[nDataSize / sizeof(WCHAR)] = NULL;
#ifdef _UNICODE
			data.m_strMacro = szUniReadBuff;
#else
			data.m_strMacro = CGeneral::wideCharToCString(szUniReadBuff);
#endif
			delete [] szUniReadBuff;
		}

		if(data.m_cIcon < KIND_DEFAULT) data.m_cIcon = KIND_DEFAULT;
		if(data.m_timeCreate < 0) data.m_timeCreate = 0;
		if(data.m_timeEdit < data.m_timeCreate) data.m_timeEdit = data.m_timeCreate;

		readList.insert(readList.end(),data);//���X�g�ɒǉ�
	}

	//�{���X�g�ɓo�^	
	tmplist->clear();
	list<STRING_DATA>::iterator stit;

	for(stit = readList.begin(); stit != readList.end(); stit++) {
		data = *stit;
		if(checkRedundancyID(data.m_nMyID)) {
			int nOldID = data.m_nMyID;
			data.m_nMyID = makeNewID();
			list<STRING_DATA>::iterator newit;
			for(newit = readList.begin(); newit != readList.end(); newit++) {
				if(newit->m_nParentID == nOldID) newit->m_nParentID = data.m_nMyID;
			}
		}
		tmplist->insert(tmplist->end(),data);//���X�g�ɒǉ�
	}
	fclose(fFile);
	//�f�o�b�O���O����
	if(theApp.m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("read charu3 data \"%s\"\n"),strFileName);
		CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}

	isRet = true;
	return isRet;
}

//---------------------------------------------------
//�֐���	getPlugin(CString strName,RW_PLUGIN* pPlugin)
//�@�\		�v���O�C��������
//---------------------------------------------------
bool CCharu3Tree::getPlugin(CString strName,RW_PLUGIN* pPlugin)
{
	bool isRet = false;
	vector<RW_PLUGIN>::iterator it;

	for(it = m_rwPlugin.begin(); it != m_rwPlugin.end(); it++) {
		if(it->m_strSoftName == strName) {
			isRet = true;
			*pPlugin = *it;
			break;
		}
	}
	return isRet;
}

//---------------------------------------------------
//�֐���	loadDataFilePlugin(CString strFileName,CString strPlugin)
//�@�\		�v���O�C���Ńf�[�^��ǂݍ���
//---------------------------------------------------
bool CCharu3Tree::loadDataFilePlugin(CString strFileName,CString strPlugin,list<STRING_DATA> *tmplist)
{
	bool isRet = false;
	RW_PLUGIN plugin;

	if(!getPlugin(strPlugin,&plugin)) {
		CString strRes;
		strRes.LoadString(APP_MES_NOT_FOUND_READPLUGIN);
		AfxMessageBox(strRes,MB_ICONEXCLAMATION,0);
		return false;
	}

	HMODULE hDLL;
	readDataFile pReadData;
	initDLL pInitDLL;
	endDLL	pEndDLL;
	time_t	timeCreate;	//�쐬����
	time(&timeCreate);

	list<STRING_DATA> *pData;//�ǂݍ��݃��X�g
	hDLL = LoadLibrary(plugin.m_strPluginName);//DLL�����[�h
	if(hDLL){
		//�֐��|�C���^���擾
		pReadData = (readDataFile)GetProcAddress(hDLL,"readDataFile");
		pInitDLL = (initDLL)GetProcAddress(hDLL,"initDLL");
		pEndDLL = (endDLL)GetProcAddress(hDLL,"endDLL");

		if(pInitDLL && pEndDLL && pReadData) {
			pInitDLL();
			pData = pReadData(strFileName,false);//�ǂݍ��݊֐����Ăяo��

			if(!pData) {
				pEndDLL();
				FreeLibrary(hDLL);
				return isRet;
			}
			tmplist->clear();
			TCHAR *szBuff;
			STRING_DATA data,data2;

			list<STRING_DATA> readList;//�f�[�^���X�g
			list<STRING_DATA>::iterator stit;
			//�����X�g�ɓo�^
			for(stit = pData->begin(); stit != pData->end(); stit++) {
				if(stit->m_strTitle != "") {
					szBuff = new TCHAR[stit->m_strTitle.GetLength()+1];
					_tcscpy(szBuff,LPCTSTR(stit->m_strTitle));
					data.m_strTitle = szBuff;
					delete [] szBuff;
				}
				else	data.m_strTitle = "";

				if(stit->m_strData != "") {
					szBuff = new TCHAR[stit->m_strData.GetLength()+1];
					_tcscpy(szBuff,LPCTSTR(stit->m_strData));
					data.m_strData = szBuff;
					delete [] szBuff;
				}
				else	data.m_strData = "";

				if(stit->m_strMacro != "") {
					szBuff = new TCHAR[stit->m_strMacro.GetLength()+1];
					_tcscpy(szBuff,LPCTSTR(stit->m_strMacro));
					data.m_strMacro = szBuff;
					delete [] szBuff;
				}
				else	data.m_strMacro = "";

				data.m_cKind = stit->m_cKind;
				data.m_nMyID = stit->m_nMyID;
				data.m_nParentID = stit->m_nParentID;

				data.m_cIcon = stit->m_cIcon;
				if(data.m_cIcon < KIND_DEFAULT) data.m_cIcon = KIND_DEFAULT;

				data.m_timeCreate = stit->m_timeCreate;
				if(data.m_timeCreate <= 0) data.m_timeCreate = timeCreate;
				data.m_timeEdit = stit->m_timeEdit;
				if(data.m_timeEdit <= 0) data.m_timeEdit = timeCreate;

				readList.insert(readList.end(),data);
			}
			pEndDLL();//���������J��

			//�{���X�g�ɓo�^	
			tmplist->clear();
			normalizationID(&readList,ROOT);//ID�𐳋K��
			for(stit = readList.begin(); stit != readList.end(); stit++) {
				tmplist->insert(tmplist->end(),*stit);//���X�g�ɒǉ�
			}
		}

		FreeLibrary(hDLL);
	}
	//�f�o�b�O���O����
	if(theApp.m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("read plugin data \"%s\" %s\n"),strFileName,strPlugin);
		CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}
	isRet = true;
	return isRet;
}

//---------------------------------------------------
//�֐���	normalizationID(list<STRING_DATA>* pList,int nParentID)
//�@�\		ID�𐳋K��(�قƂ��Charu2Pro�̃f�[�^��p)
//---------------------------------------------------
void CCharu3Tree::normalizationID(list<STRING_DATA>* pList,int nParentID)
{
	list<STRING_DATA>::iterator it;

	for(it = pList->begin(); it != pList->end(); it++) {
		//�e��ID���r
		if(it->m_nParentID == nParentID) {
			if(checkRedundancyID(it->m_nMyID)) {//ID�̏d�����`�F�b�N
				int nOldID = it->m_nMyID;
				it->m_nMyID = makeNewID();//�V����ID��U��
				list<STRING_DATA>::iterator newit;
				for(newit = pList->begin(); newit != pList->end(); newit++) {//�����̎q�̐eID��ς���
					if(newit->m_nParentID == nOldID) newit->m_nParentID = it->m_nMyID;
				}
			}
			//�����̎q���B���ċA����
			if(it->m_cKind & KIND_FOLDER_ALL)	normalizationID(pList,it->m_nMyID);
		}
	}
}

//---------------------------------------------------
//�֐���	convertMacroPlugin(CString strSourceData)
//�@�\		�}�N���������u��
//---------------------------------------------------
bool CCharu3Tree::convertMacroPlugin(STRING_DATA *SourceData,CString *strRet,CString strSelect,CString strClip,CString strSoftName)
{
	RW_PLUGIN plugin;
	TCHAR *szRet;
	bool isRet = false;

	bool isFound = getPlugin(strSoftName,&plugin);

	if(!isFound) {
		CString strRes;
		strRes.LoadString(APP_MES_NOT_FOUND_MACROPLUGIN);
		AfxMessageBox(strRes,MB_ICONEXCLAMATION,0);
		return isRet;
	}

	HMODULE hDLL;
	convertMacro pConvertMacro;

	hDLL = LoadLibrary(plugin.m_strPluginName);//DLL�����[�h
	if(hDLL){
		//�֐��|�C���^���擾
		pConvertMacro = (convertMacro)GetProcAddress(hDLL,"convertMacro");

		if(pConvertMacro) {
			int nSize = SourceData->m_strData.GetLength()*10+10240;
			szRet = NULL;
			szRet = new TCHAR[nSize];
			if(szRet) {
				isRet = pConvertMacro((TCHAR*)LPCTSTR(SourceData->m_strData),szRet,nSize,
					(TCHAR*)LPCTSTR(strSelect),(TCHAR*)LPCTSTR(strClip));
				*strRet = szRet;
				delete [] szRet;
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
void CCharu3Tree::copyData(int nParentID,HTREEITEM hParentTreeItem,list<STRING_DATA> *pList)
{
	int nRet = 0;
	HTREEITEM hTreeItem;
	list<STRING_DATA>::iterator it;

	//�c���[�f�[�^
	TV_INSERTSTRUCT TreeCtrlItem;
	
	//�f�[�^������
	for (it = pList->begin(); it != pList->end(); it++) {
		//�eID�Ɠ����̂�������
		if(it->m_nParentID == nParentID) {
			data2TreeStruct(&TreeCtrlItem,it);//�f�[�^���ʂ�
			TreeCtrlItem.hParent = hParentTreeItem;//�e��ݒ�

			hTreeItem = InsertItem(&TreeCtrlItem);//�C���T�[�g

			if(*m_nSelectID == it->m_nMyID) SelectItem(hTreeItem);

			//�t�H���_�Ȃ�ċA�Ăяo��
			if(it->m_cKind & KIND_FOLDER_ALL) {
				copyData(it->m_nMyID,hTreeItem,pList);
				if(it->m_cKind & FOLDER_OPEN)	Expand(hTreeItem,TVE_EXPAND);
				else							Expand(hTreeItem,TVE_COLLAPSE);
			}
		}
	}
}

//---------------------------------------------------
//�֐���	moveChildren(HTREEITEM hFromItem,HTREEITEM hToItem)
//�@�\		�t�H���_����t�H���_�Ɏq���������ړ�
//---------------------------------------------------
void CCharu3Tree::moveChildren(HTREEITEM hFromItem,HTREEITEM hToItem)
{
	if(! hFromItem || !hToItem) return;
	list<STRING_DATA>::iterator it;
	hFromItem = GetChildItem(hFromItem);
	do {
		//�f�[�^���擾
		STRING_DATA dataF;
		dataF = getData(hFromItem);

		//�c���[�f�[�^���擾
		TV_ITEM TreeCtrlItemFrom;
		ZeroMemory(&TreeCtrlItemFrom,sizeof(TreeCtrlItemFrom));
		TreeCtrlItemFrom.hItem = hFromItem;
		TreeCtrlItemFrom.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
		GetItem(&TreeCtrlItemFrom);

		//�ǉ��f�[�^���쐬
		TV_INSERTSTRUCT AddTreeCtrlItem;

		ZeroMemory(&AddTreeCtrlItem,sizeof(AddTreeCtrlItem));
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
		if(ItemHasChildren(hFromItem)) {
			//�^�[�Q�b�g�����߂čċA�R�s�[
			moveChildren(hFromItem,hInsertItem);
		}
		hFromItem = GetNextItem(hFromItem,TVGN_NEXT);


	}while(hFromItem);
}
//---------------------------------------------------
//�֐���	copyChildren(HTREEITEM hFromItem,HTREEITEM hToItem)
//�@�\		�t�H���_����t�H���_�Ɏq���������R�s�[
//---------------------------------------------------
void CCharu3Tree::copyChildren(HTREEITEM hFromItem,HTREEITEM hToItem)
{
	if(! hFromItem || !hToItem) return;
	list<STRING_DATA>::iterator it;
	hFromItem = GetChildItem(hFromItem);
	HTREEITEM hAddtItem = NULL;
	do {
		//�f�[�^���擾
		STRING_DATA dataF;
		dataF = getData(hFromItem);

		//�����f�[�^��}��
		if(hAddtItem)
			hAddtItem = addData(hAddtItem,dataF);
		else
			hAddtItem = addData(hToItem,dataF,true,true);
		//���Ƀt�H���_����
		if(ItemHasChildren(hFromItem)) {
			//�^�[�Q�b�g�����߂čċA�R�s�[
			copyChildren(hFromItem,hAddtItem);
		}
		hFromItem = GetNextItem(hFromItem,TVGN_NEXT);
	}while(hFromItem);
}


//---------------------------------------------------
//�֐���	addNewFolder(HTREEITEM hTreeItem)
//�@�\		�V�����t�H���_��ǉ�����
//---------------------------------------------------
HTREEITEM CCharu3Tree::addNewFolder(HTREEITEM hTreeItem,CString strName)
{
	STRING_DATA Data;
	initStringData(&Data);

	Data.m_cKind = KIND_FOLDER;
	Data.m_strData = "";
	Data.m_strTitle = strName;
	HTREEITEM hAddItem = addData(hTreeItem,Data);
	SelectItem(hAddItem);
	//�f�o�b�O���O����
	if(theApp.m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("add new folder \"%s\"\n"),strName);
		CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}

	return hAddItem;
}

//---------------------------------------------------
//�֐���	margeTreeData(HTREEITEM hTreeItem,list<STRING_DATA> *pList)
//�@�\		���X�g���}�[�W���ăc���[�ɔ��f������
//---------------------------------------------------
HTREEITEM CCharu3Tree::mergeTreeData(HTREEITEM hTreeItem,list<STRING_DATA> *pList,bool isRoot)
{
	if(pList->size() > 0) {
		STRING_DATA folder;
		int nParentID;
		CString strRes;
		if(!isRoot) {//�C���|�[�g�t�H���_�����
			strRes.LoadString(APP_INF_IMPORT_FOLDERNAME);
			hTreeItem = addNewFolder(hTreeItem,strRes);//�e�t�H���_�����
			if(!hTreeItem) return NULL;
			folder = getData(hTreeItem);
			nParentID = folder.m_nMyID;
		}
		else {//���[�g�ɓW�J����
			nParentID = ROOT;
		}
		mergeList(&m_MyStringList,pList,nParentID);
		//�e�̂��Ȃ��q�͏���
		list<STRING_DATA>::iterator it,itNext;
		int i;
		for (i = 0,it = m_MyStringList.begin(); it != m_MyStringList.end(); i++,it++) {
			STRING_DATA data;
			data = *it;
			if(it->m_nParentID != ROOT && !checkRedundancyID(it->m_nParentID)) {
				//�e�����Ȃ��̂͏������Ⴄ
				itNext = it;
				if(i > 0) itNext--;
				m_MyStringList.erase(it);
				if(i > 0) it = itNext;
				else it = m_MyStringList.begin();
			}
		}
		CWnd::LockWindowUpdate();
		if(!isRoot){
			copyData(nParentID,hTreeItem,&m_MyStringList);
		}
		else {
			DeleteAllItems();
			copyData(nParentID,TVI_ROOT,&m_MyStringList);
			hTreeItem = this->GetRootItem();
		}
		CWnd::UnlockWindowUpdate();
	}
	else hTreeItem = NULL;
	return hTreeItem;
}

//---------------------------------------------------
//�֐���	mergeList(list<STRING_DATA> *pMainList,list<STRING_DATA> *pList,int nParent)
//�@�\		���X�g���}�[�W����
//---------------------------------------------------
int CCharu3Tree::mergeList(list<STRING_DATA> *pMainList,list<STRING_DATA> *pList,int nParent)
{
	int nRet = 0,nBeginID = 0;
	list<STRING_DATA>::iterator it;
	it = pList->begin();
	nBeginID = ROOT;

	for(it = pList->begin(); it != pList->end(); it++) {
		if(it->m_nParentID == nBeginID)	it->m_nParentID = nParent;
		pMainList->insert(pMainList->end(),*it);
		nRet++;
	}

	return nRet;
}

//---------------------------------------------------
//�֐���	addData(HTREEITEM hTreeItem,STRING_DATA Data)
//�@�\		�f�[�^��ǉ�����
//---------------------------------------------------
HTREEITEM CCharu3Tree::addData(HTREEITEM hTreeItem,STRING_DATA data,bool isNewID/* = true*/,bool isChild /*fasle*/)
{
	list<STRING_DATA>::iterator it;

	if(isNewID)	data.m_nMyID = makeNewID();
	data.m_nParentID = ROOT;
	//NULL�ł���Ɛe��ROOT�ɂȂ�
	if(hTreeItem) {
		HTREEITEM hParentItem;
		if(isChild)	hParentItem = hTreeItem;
		else		hParentItem = GetParentItem(hTreeItem);
		if(hParentItem) {
			getData(hParentItem,&it);
			data.m_nParentID = it->m_nMyID;
		}
	}

	if(data.m_strTitle == "") data.m_strTitle = makeTitle(data.m_strData);

	time(&data.m_timeCreate);
	time(&data.m_timeEdit);
	it = m_MyStringList.insert(m_MyStringList.end(),data);//���X�g�ɒǉ�

	//�c���[�f�[�^�쐬
	TV_INSERTSTRUCT TreeCtrlItem;
	data2TreeStruct(&TreeCtrlItem,it);

	if(hTreeItem) {
		if(isChild) {//�q���ɂ���ꍇ�̓t�H���_�̐擪�ɓ����
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
	//�f�o�b�O���O����
	if(theApp.m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("add data %s %s %d\n"),data.m_strTitle,data.m_strData,data.m_cKind);
		CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}
	return hTreeItem;
}

//---------------------------------------------------
//�֐���	tree2List(HTREEITEM hStartItem,list<STRING_DATA> *list)
//�@�\		�c���[�\���̂��烊�X�g���쐬
//---------------------------------------------------
void CCharu3Tree::tree2List(HTREEITEM hStartItem,list<STRING_DATA> *tmplist,bool isAll/*=false*/)
{
	if(!hStartItem) return;
	list<STRING_DATA>::iterator it;
	STRING_DATA data;
	HTREEITEM hItem = hStartItem;
	do {
		data = getData(hItem,&it);

		tmplist->insert(tmplist->end(),data);
		if(ItemHasChildren(hItem) && (getDataOption(data.m_strMacro,"clearrec") != 1 || isAll)) tree2List(GetChildItem(hItem),tmplist);
		hItem = GetNextItem(hItem,TVGN_NEXT);
	}while(hItem);
}

//---------------------------------------------------
//�֐���	data2TreeStruct(TV_INSERTSTRUCT *pTreeCtrlItem,list<STRING_DATA>::iterator *pit)
//�@�\		�c���[�\���̂Ƀf�[�^���i�[
//---------------------------------------------------
void CCharu3Tree::data2TreeStruct(TV_INSERTSTRUCT *pTreeCtrlItem,list<STRING_DATA>::iterator it)
{
	//�c���[�f�[�^�쐬
	pTreeCtrlItem->hInsertAfter = TVI_LAST;
	pTreeCtrlItem->item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	STRING_DATA data;
	data.m_strTitle = it->m_strTitle;
	
	//�A�C�R����ݒ�
	int nIcon = getIconNumber(it->m_cKind,it->m_cIcon);
	pTreeCtrlItem->item.iImage = nIcon;
	pTreeCtrlItem->item.iSelectedImage = nIcon + 1;

	//���O��ݒ�
	pTreeCtrlItem->item.pszText = (TCHAR*)LPCTSTR(it->m_strTitle);
	//�f�[�^�̃A�h���X��ݒ�
	memcpy((void*)&pTreeCtrlItem->item.lParam,(void*)&it,sizeof(it));
}

//---------------------------------------------------
//�֐���	getIconNumber(char cKind,char cIcon)
//�@�\		�A�C�R���ԍ����擾
//---------------------------------------------------
int CCharu3Tree::getIconNumber(char cKind,char cIcon)
{
	int nRet = 0;
	if(cKind & KIND_FOLDER) {
		nRet = ICON_FOLDER;
	}
	else if(cKind & KIND_RIREKI) {
		nRet = ICON_RIREKI_FOLDER;
	}
	else if(cKind & KIND_LOCK){//���b�N���ڂ̏ꍇ
		if(cIcon > 0) {
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
//�֐���	deleteData(HTREEITEM hTreeItem)
//�@�\		�f�[�^���폜����
//---------------------------------------------------
void CCharu3Tree::deleteData(HTREEITEM hTreeItem)
{
	list<STRING_DATA>::iterator it;
	//�f�[�^�̃A�h���X��ݒ�
	getData(hTreeItem,&it);

	//�f�o�b�O���O����
	if(theApp.m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("delete data \"%s\" %d\n"),it->m_strTitle,it->m_cKind);
		CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}

	if(it->m_cKind & KIND_FOLDER_ALL) {
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
//�֐���	void clearFolder(HTREEITEM hStartItem)
//�@�\		�w��f�[�^�ȉ����폜����
//---------------------------------------------------
void CCharu3Tree::clearFolder(HTREEITEM hStartItem)
{
	if(!hStartItem) return;
	list<STRING_DATA>::iterator it;
	HTREEITEM hItem = hStartItem,hPrevItem;
	do {
		if(ItemHasChildren(hItem)) {
			clearFolder(GetChildItem(hItem));//�ċA�ō폜
		}
		getData(hItem,&it);
		hPrevItem = hItem;
		hItem = GetNextItem(hItem,TVGN_NEXT);
		m_MyStringList.erase(it);
		checkOut(hPrevItem);
		this->DeleteItem(hPrevItem);
	}while(hItem);
}

//---------------------------------------------------
//�֐���	void closeFolder(HTREEITEM hStartItem)
//�@�\		�w��f�[�^�ȉ������
//---------------------------------------------------
void CCharu3Tree::closeFolder(HTREEITEM hStartItem)
{
	if(!hStartItem) return;
	list<STRING_DATA>::iterator it;
	HTREEITEM hItem = hStartItem;
	do {
		if(ItemHasChildren(hItem)) {
			getData(hItem,&it);
			Expand(hItem,TVE_COLLAPSE);
			it->m_cKind = it->m_cKind & (FOLDER_OPEN ^ 0xff); 
			closeFolder(GetChildItem(hItem));//�ċA�ŕ���
		}
		hItem = GetNextItem(hItem,TVGN_NEXT);
	}while(hItem);
}

//---------------------------------------------------
//�֐���	checkFolder(HTREEITEM hStartItem,bool isCheck)
//�@�\		�w��f�[�^�ȉ��Ƀ`�F�b�N������
//---------------------------------------------------
void CCharu3Tree::checkFolder(HTREEITEM hStartItem,bool isCheck,list<HTREEITEM> *listItem)
{
	if(!hStartItem) return;
	list<STRING_DATA>::iterator it;
	SetCheck(hStartItem,isCheck);
	HTREEITEM hItem = GetChildItem(hStartItem);
	do {
		if(ItemHasChildren(hItem)) {
			checkFolder(hItem,isCheck,listItem);
		}
		SetCheck(hItem,isCheck);
		if(isCheck) {//���X�g�ɒǉ�
			listItem->insert(listItem->end(),hItem);
		}
		else {//���X�g����폜
			list<HTREEITEM>::iterator it;
			for(it = listItem->begin(); it != listItem->end(); it++) {
				if(*it == hItem) {
					listItem->erase(it);
					break;
				}
			}
		}
		hItem = GetNextItem(hItem,TVGN_NEXT);
	}while(hItem);
}

//---------------------------------------------------
//�֐���	void clearOneTime(HTREEITEM hStartItem)
//�@�\		�ꎞ���ڂ��폜����
//---------------------------------------------------
void CCharu3Tree::clearOneTime(HTREEITEM hStartItem,int nKind/*KIND_LOCK���ƍ폜�łȂ����b�N����*/)
{
	if(!hStartItem) return;
	list<STRING_DATA>::iterator it;
	HTREEITEM hItem = hStartItem,hPrevItem;
	do {
		if(ItemHasChildren(hItem)) {
			clearOneTime(GetChildItem(hItem),nKind);//�ċA�ŏ���
		}
		getData(hItem,&it);
		hPrevItem = hItem;
		hItem = GetNextItem(hItem,TVGN_NEXT);
		if(it->m_cKind & KIND_ONETIME) {
			if(nKind == KIND_LOCK) {//���b�N�����̏ꍇ
				STRING_DATA data = getData(hPrevItem);
				data.m_cKind = KIND_LOCK;
				editData(hPrevItem,data);
			}
			else {//�N���A�����ɏꍇ
				m_MyStringList.erase(it);
				checkOut(hPrevItem);
				DeleteItem(hPrevItem);
			}
		}
	}while(hItem);
}


//---------------------------------------------------
//�֐���	editData(HTREEITEM hTreeItem,STRING_DATA Data)
//�@�\		�f�[�^��ύX����
//---------------------------------------------------
void CCharu3Tree::editData(HTREEITEM hTreeItem,STRING_DATA Data)
{
	list<STRING_DATA>::iterator it;

	//�f�[�^�̃A�h���X��ݒ�
	getData(hTreeItem,&it);
	time(&Data.m_timeEdit);

	*it = Data;
	int nIcon = getIconNumber(Data.m_cKind,Data.m_cIcon);

	SetItemImage(hTreeItem,nIcon,nIcon + 1);
	SetItemText(hTreeItem,Data.m_strTitle);
	//�f�o�b�O���O����
	if(theApp.m_ini.m_nDebug) {
		CString strText;
		strText.Format(_T("edit data \"%s\" %d\n"),it->m_strTitle,it->m_cKind);
		CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}
}
void CCharu3Tree::changeIcon(HTREEITEM hTreeItem,int nID) 
{
	STRING_DATA data = getData(hTreeItem);
	data.m_cIcon = nID;
	editData(hTreeItem,data);
}

//---------------------------------------------------
//�֐���	getData(HTREEITEM hTreeItem)
//�@�\		�c���[�n���h������f�[�^���擾
//---------------------------------------------------
STRING_DATA CCharu3Tree::getData(HTREEITEM hTreeItem)
{
	STRING_DATA retData;
	list<STRING_DATA>::iterator it;

	//�f�[�^�̃A�h���X��ݒ�
	getData(hTreeItem,&it);

	retData = *it;
	
	return retData;
}
STRING_DATA CCharu3Tree::getData(HTREEITEM hTreeItem,list<STRING_DATA>::iterator *it)
{
	STRING_DATA retData;

	//�f�[�^�̃A�h���X��ݒ�
	long lParam = GetItemData(hTreeItem);
	memcpy((void*)it,(void*)&lParam,sizeof(lParam));

	retData = **it;
	
	
	return retData;
}

//---------------------------------------------------
//�֐���	serchItem()
//�@�\		�w��̃A�C�e�����������A�I������
//---------------------------------------------------
HTREEITEM CCharu3Tree::serchItem(int nKind,int nLogic,CString strKey,HTREEITEM hStartItem)
{
	vector<CString> m_keyVector;//�����L�[
	vector<CString>::iterator keyIT;
	int nEnd;
	bool isTitleFound = false,isDataFound = false,isFound = false;
	STRING_DATA data,parentData;
	HTREEITEM hTreeItem;

	//�]����؂���
	strKey.TrimLeft();
	strKey.TrimRight();

	//�L�[���[�h���X�y�[�X�Ő؂�o��
	CString strRes;
	strRes.LoadString(APP_INF_2BYTESPACE);
	strKey.Replace(strRes,_T(" "));
	while(1) {
		nEnd = strKey.Find(_T(" "));//�󔒂ŋ�؂�
		if(nEnd == -1) break;
		m_keyVector.insert(m_keyVector.end(),strKey.Left(nEnd));//�z��ɒǉ�
		strKey = strKey.Right(strKey.GetLength() - nEnd);//�ǉ���������؂���
		strKey.TrimLeft();
	}
	m_keyVector.insert(m_keyVector.end(),strKey);//�z��ɒǉ�

	//����
	if(!hStartItem) {
		hStartItem = GetRootItem();
	}
	hTreeItem = getTrueNextItem(hStartItem);
	while(hTreeItem != hStartItem) {
		m_hSerchItem = hTreeItem;
		data = getData(hTreeItem);
		data.m_strData = data.m_strData + data.m_strMacro;//�}�N���������Ώۂɂ���
		if(!hTreeItem)	break;
		//�^�C�g������
		if(nKind == SERCH_KIND_TITLE || nKind == SERCH_KIND_ALL) {
			if(nLogic == SERCH_LOGIC_AND)	isTitleFound = true;
			else							isTitleFound = false;
			for(keyIT = m_keyVector.begin(); keyIT != m_keyVector.end(); keyIT++) {
				//AND����
				if(data.m_strTitle.Find(*keyIT) == -1 && nLogic == SERCH_LOGIC_AND) {
					isTitleFound = false;
					break;
				}
				//OR����
				else if(data.m_strTitle.Find(*keyIT) != -1 && nLogic == SERCH_LOGIC_OR) {
					isTitleFound = true;
					break;
				}
			}
			isFound = isTitleFound;
			if(isFound) break;
		}
		//�f�[�^����
		if((nKind == SERCH_KIND_TEXT || nKind == SERCH_KIND_ALL) && !isFound) {
			if(nLogic == SERCH_LOGIC_AND)	isDataFound = true;
			else							isDataFound = false;
			for(keyIT = m_keyVector.begin(); keyIT != m_keyVector.end(); keyIT++) {
				//AND����
				if(data.m_strData.Find(*keyIT) == -1 && nLogic == SERCH_LOGIC_AND) {
					isDataFound = false;
					break;
				}
				//OR����
				else if(data.m_strData.Find(*keyIT) != -1 && nLogic == SERCH_LOGIC_OR) {
					isDataFound = true;
					break;
				}
			}
			isFound = isDataFound;
			if(isFound) break;
		}
		hTreeItem = getTrueNextItem(hTreeItem);
	}
	if(isFound && hTreeItem) {
		SelectItem(hTreeItem);//����������I��
	}
	else hTreeItem = NULL;
	return hTreeItem;
}

//---------------------------------------------------
//�֐���	HTREEITEM serchTitle(HTREEITEM hStartItem,CString strKey)
//�@�\		�^�C�g������(�O����v)
//---------------------------------------------------
HTREEITEM CCharu3Tree::serchTitle(HTREEITEM hStartItem,CString strKey,int isLower)
{
	HTREEITEM hRetItem = NULL;

	if(!hStartItem) return NULL;
	list<STRING_DATA>::iterator it;
	STRING_DATA data;
	HTREEITEM hItem = hStartItem;
	hItem = getTrueNextItem(hItem);
	while(hItem && hStartItem != hItem) {
		data = getData(hItem);
		if(isLower) data.m_strTitle.MakeLower();
		if(data.m_strTitle.Find(strKey) == 0) {//����
			hRetItem = hItem;
			break;
		}
		hItem = getTrueNextItem(hItem);
	}
	if(hStartItem == hItem)	hRetItem = hItem;

	return hRetItem;
}
//---------------------------------------------------
//�֐���	getTrueNextItem(HTREEITEM hTreeItem)
//�@�\		���̃A�C�e�����擾(�F�X�������Ă܂�)
//---------------------------------------------------
HTREEITEM CCharu3Tree::getTrueNextItem(HTREEITEM hTreeItem)
{
	HTREEITEM hRetTreeItem,hParentItem;

	if(!hTreeItem) {		//���[�g��ݒ�
		hRetTreeItem = GetRootItem();
	}
	else if(!GetChildItem(hTreeItem)) {//�q�������邩���ׂ�
		hRetTreeItem = GetNextSiblingItem(hTreeItem);//�Z��n���h�����擾
		if(!hRetTreeItem) {//�Z�킪�������Ȃ����͐e�̌Z��
			hParentItem = hTreeItem;
			do {
				hParentItem = GetParentItem(hParentItem );
				if(!hParentItem) {
					hRetTreeItem = NULL;
					break;
				}
				hRetTreeItem = GetNextSiblingItem(hParentItem);//�Z��n���h�����擾
			}while(hParentItem != GetRootItem() && !hRetTreeItem);
		}
	}
	else {
		hRetTreeItem = GetChildItem(hTreeItem);//�q���̃n���h�����擾
	}
	if(!hRetTreeItem){
		hRetTreeItem = GetRootItem();//�ă��[�v�ŕK�v�Ȃ̂Ŗ߂�
	}
	return hRetTreeItem;
}

//---------------------------------------------------
//�֐���	getLastVisibleItem()
//�@�\		�Ō�̉��A�C�e�����擾
//---------------------------------------------------
HTREEITEM CCharu3Tree::getLastVisibleItem()
{
	HTREEITEM hRetTreeItem;
	HTREEITEM hTreeItemTmp;
	hRetTreeItem = GetRootItem();
	while(hRetTreeItem) {
		hTreeItemTmp = hRetTreeItem;
		hRetTreeItem = GetNextVisibleItem(hRetTreeItem);//�Ō�̉��A�C�e�����擾
	}
	if(hTreeItemTmp)	hRetTreeItem = hTreeItemTmp;
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
	for(int i = 0; i < nSize; hTreeItem = getTrueNextItem(hTreeItem),i++) {
		if(hTreeItem) {
			data = getData(hTreeItem);
			cOrg = data.m_cIcon;
			data.m_cIcon = decideIcon(data.m_strData);
			if(cOrg != data.m_cIcon) editData(hTreeItem,data);
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
	if(strData.Find(_T("<charu2MACRO_KEY>"),0) > -1 || strData.Find(_T("<charuMACRO_KEY>"),0) > -1){
			cIcon = KIND_KEY;
	}
	//�}�N�����܂܂�Ă���
	else if(strData.Find(_T("<charu2MACRO>"),0) > -1 || strData.Find(_T("<charuMACRO>"),0) > -1) {
		//�v���O�C��
		if(strData.Find(_T("$PLUG-IN<"),0) > -1){
			cIcon = KIND_PLUG;
		}
		//�V�F��
		else if(strData.Find(_T("$SHELL<"),0) > -1){
			cIcon = KIND_EXE;
		}
		//�֘A�t��
		else if(strData.Find(_T("$RELATE<"),0) > -1){
			cIcon = KIND_RELATE;
		}
		//�I���e�L�X�g
		else if(strData.Find(_T("$SEL"),0) > -1){
			cIcon = KIND_SELECT;
		}
		//�N���b�v�{�[�h
		else if(strData.Find(_T("$CLIP"),0) > -1){
			cIcon = KIND_CLIP;
		}
		//���t
		else if(strData.Find(_T("$Y"),0) > -1 || strData.Find(_T("$y"),0) > -1 || strData.Find(_T("$WH"),0) > -1
			|| strData.Find(_T("$MM"),0) > -1 || strData.Find(_T("$M"),0) > -1 || strData.Find(_T("$D"),0) > -1
			|| strData.Find(_T("$ddd"),0) > -1 || strData.Find(_T("$dd"),0) > -1 || strData.Find(_T("$d"),0) > -1
			|| strData.Find(_T("$H"),0) > -1 || strData.Find(_T("$h"),0) > -1 || strData.Find(_T("$m"),0) > -1
			|| strData.Find(_T("$s"),0) > -1 || strData.Find(_T("$ampm"),0) > -1 || strData.Find(_T("$AMPM"),0) > -1){
			cIcon = KIND_DATE;
		}
	}
	return cIcon;
}


//---------------------------------------------------
//�֐���	makeTitle(CString strData)
//�@�\		�ۑ������񂩂�^�C�g�������
//---------------------------------------------------
CString CCharu3Tree::makeTitle(CString strData,int nTitleLength)
{
	CString strBuff;
	strData = strData.Left(256);
	strData.Replace(_T("�@"),_T(""));
	strData.Replace(_T("\r\n"),_T(""));
	strData.Replace(_T("\r"),_T(""));
	strData.Replace(_T("\n"),_T(""));
	strData.Replace(_T("\t"),_T(" "));
	UStringWork::compressionSpace((TCHAR*)(LPCTSTR)strData);
	strData.TrimLeft();
	strData.TrimRight();
	if(nTitleLength > strData.GetLength())	strBuff = strData;
	else	strBuff = strData.Left(nTitleLength);

	return strBuff;
}


//---------------------------------------------------
//�֐���	getOneTimeText(bool isFirst)
//�@�\		�ŏ����Ō�̈ꎞ���ڂ��擾
//---------------------------------------------------
HTREEITEM CCharu3Tree::getOneTimeText(int nType)
{
	CString strRet;
	STRING_DATA data;
	HTREEITEM hRet = NULL;

	//������o���̏ꍇ FIFO ��ԉ��̈ꎞ���ڂ�T��
	int nSize = m_MyStringList.size();
	HTREEITEM hTreeItem = GetRootItem();
	for(int i = 0; i < nSize; hTreeItem = GetNextItem(hTreeItem,TVGN_NEXT),i++) {
		if(hTreeItem) {
			data = getData(hTreeItem);
			if(data.m_cKind & KIND_ONETIME) {
				//�f�o�b�O���O����
				if(theApp.m_ini.m_nDebug) {
					CString strText;
					strText.Format(_T("getOneTimeText \"%s\" %s %d\n"),data.m_strTitle,data.m_strData,data.m_cKind);
					CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
				}

				strRet = data.m_strData;
				hRet = hTreeItem;
				if(nType == 2) break;
			}
		}
	}
	return hRet;
}

//---------------------------------------------------
//�֐���	getDataOption(CString strData,CString strKind)
//�@�\		�f�[�^�⏕�ݒ�����
//---------------------------------------------------
int CCharu3Tree::getDataOption(CString strData,CString strKind)
{
	int nRet = 0,nFound,nFoundEnd;
	CString strBuff;

	strData.MakeLower();
	nFound = strData.Find(strKind);
	if(nFound >= 0) {
		nFound = strData.Find(_T("="),nFound);
		if(nFound >= 0) {
			nFoundEnd = strData.Find(_T("\n"),nFound);
			if(nFoundEnd <= 0) nFoundEnd = strData.GetLength();
			strBuff = strData.Mid(nFound+1,nFoundEnd - (nFound+1));
			strBuff.TrimRight();
			strBuff.TrimLeft();
			if(strBuff == _T("yes")) nRet = 1;
			else if(strBuff == _T("no")) nRet = 0;
			else if(strBuff == _T( "")) nRet = -1;
			else nRet = _ttoi(strBuff.GetBuffer(strBuff.GetLength()));
		}
		else nRet = -1;
		//�f�o�b�O���O����
		if(theApp.m_ini.m_nDebug) {
			CString strText;
			strText.Format(_T("getDataOption \"%s\" %d\n"),strKind,nRet);
			CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
		}
	}
	else nRet = -1;

	return nRet;
}

//---------------------------------------------------
//�֐���	getDataOptionHex(CString strData,CString strKind)
//�@�\		�f�[�^�⏕�ݒ�����
//---------------------------------------------------
DWORD CCharu3Tree::getDataOptionHex(CString strData,CString strKind)
{
	DWORD dwRet = -1;
	int nFound,nFoundEnd;
	CString strBuff;

	strData.MakeLower();
	nFound = strData.Find(strKind);
	if(nFound >= 0) {
		nFound = strData.Find(_T("="),nFound);
		if(nFound >= 0) {
			nFoundEnd = strData.Find(_T("\n"),nFound);
			if(nFoundEnd <= 0) nFoundEnd = strData.GetLength();
			strBuff = strData.Mid(nFound+1,nFoundEnd - (nFound+1));
			strBuff.TrimRight();
			strBuff.TrimLeft();
			_stscanf(strBuff,_T("%x"),&dwRet);
		}
		//�f�o�b�O���O����
		if(theApp.m_ini.m_nDebug) {
			CString strText;
			strText.Format(_T("getDataOptionHex \"%s\" 0x%x\n"),strKind,dwRet);
			CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
		}
	}

	return dwRet;
}

//---------------------------------------------------
//�֐���	getDataOptionStr(CString strData,CString strKind)
//�@�\		�f�[�^�⏕�ݒ�����
//---------------------------------------------------
CString CCharu3Tree::getDataOptionStr(CString strData,CString strKind)
{
	int nFound,nFoundEnd;
	CString strBuff,strRet,strDataCopy;
	strRet = _T("");

	strDataCopy = strData;
	strData.MakeLower();
	nFound = strData.Find(strKind);
	if(nFound >= 0) {
		nFound = strData.Find(_T("="),nFound);
		if(nFound >= 0) {
			nFoundEnd = strData.Find(_T("\n"),nFound);
			if(nFoundEnd <= 0) nFoundEnd = strData.GetLength();
			strBuff = strDataCopy.Mid(nFound+1,nFoundEnd - (nFound+1));
			strBuff.TrimRight();
			strBuff.TrimLeft();
			strRet = strBuff;
		}
		//�f�o�b�O���O����
		if(theApp.m_ini.m_nDebug) {
			CString strText;
			strText.Format(_T("getDataOptionHex \"%s\" %s\n"),strKind,strRet);
			CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
		}
	}
	return strRet;
}

//---------------------------------------------------
//�֐���	addDataToRecordFolder(STRING_DATA data)
//�@�\		��������������
//---------------------------------------------------
void CCharu3Tree::addDataToRecordFolder(STRING_DATA data,CString strClipBkup)
{
	int nSize = m_MyStringList.size(),nRirekiCount,nIsLock,i,nDoDuplication;
	int nNumber = *m_nRecNumber;
	list<STRING_DATA>::iterator it;

	STRING_DATA parentData;
	HTREEITEM hTreeItem,hStart = NULL;
	//�f�o�b�O���O����
	if(theApp.m_ini.m_nDebug) {
		CString strText;
		CGeneral::writeLog(theApp.m_ini.m_strDebugLog,_T("addDataToRecordFolder\n"),_ME_NAME_,__LINE__);
		strText.Format(_T("hRootItem:%d TreeSize:%d\n"),GetRootItem(),nSize);
		CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
	}
//	hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_ROOT, 0);
	for(hTreeItem = GetRootItem(),i = 0; i < nSize && hTreeItem && hTreeItem != hStart;i++,hTreeItem = getTrueNextItem(hTreeItem)) {
		parentData = getData(hTreeItem,&it);
		hStart = GetRootItem();
		//�����t�H���_������
		if(parentData.m_cKind & KIND_RIREKI) {
			nDoDuplication = getDataOption(parentData.m_strMacro,_T("duplicationcheck"));
			if((nDoDuplication && strClipBkup != data.m_strData) || !nDoDuplication) {
				if(*m_nRecNumber == nNumber)	nNumber++;
				//�f�o�b�O���O����
				if(theApp.m_ini.m_nDebug) {
					CString strText;
					strText.Format(_T("number:%d handle:%d title:%s ID:%d\n"),i,hTreeItem,parentData.m_strTitle,parentData.m_nMyID);
					CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
				}
				nIsLock = getDataOption(parentData.m_strMacro,_T("lock"));//������ʂ��擾
				if(!nIsLock)	data.m_cKind = KIND_ONETIME;
				else			data.m_cKind = KIND_LOCK;

				nRirekiCount = getDataOption(parentData.m_strMacro,_T("count"));//���𐔂��擾
				if(nRirekiCount <= 0) nRirekiCount = 2;
				int nDeleteCount = nRirekiCount;
				deleteExcessChildren(GetChildItem(hTreeItem),&nDeleteCount);//�]�v�Ȏq������

				int nTitleLength = getDataOption(parentData.m_strMacro,_T("titlelen"));//�^�C�g���̕�����
				CString strTitle;
				if(nTitleLength > 0 && nTitleLength < 256) {
					strTitle = makeTitle(data.m_strData,nTitleLength);
				}
				else
					strTitle = makeTitle(data.m_strData);
				data.m_strTitle = strTitle;

				if(getDataOption(parentData.m_strMacro,_T("number"))) {//�ԍ������邩�ǂ���
					data.m_strTitle.Format(_T("%02d : %s"),*m_nRecNumber,strTitle);
					if(nNumber > nRirekiCount) nNumber = 0;
				}

				//�K�w��������
				nRirekiCount = getDataOption(parentData.m_strMacro,_T("classhistory"));//�K�w���𐔂��擾
				if(nRirekiCount > 0) {
					classHistoryFolder(hTreeItem,nRirekiCount);
				}
				//�f�o�b�O���O����
				if(theApp.m_ini.m_nDebug) {
					CString strText;
					strText.Format(_T("addDataToRecordFolder \"%s\" %s %d\n"),data.m_strTitle,data.m_strData,nNumber);
					CGeneral::writeLog(theApp.m_ini.m_strDebugLog,strText,_ME_NAME_,__LINE__);
				}

				addData(hTreeItem,data,true,true);//�ǉ�
				nSize = m_MyStringList.size();
			}
		}
	}
	*m_nRecNumber = nNumber;
}

//---------------------------------------------------
//�֐���	classHistoryFolder(HTREEITEM hTreeItem)
//�@�\		�K�w��������
//---------------------------------------------------
void CCharu3Tree::classHistoryFolder(HTREEITEM hTreeItem,int nRirekiCount)
{
	int nChildCount = getChildCount(hTreeItem,true);//�����t�H���_�����̃A�C�e�������擾
	if(nChildCount >= nRirekiCount) {//�K�w���𐔂𒴂��Ă���
		HTREEITEM hFirstFolder = getFirstFolder(hTreeItem);
		if(!hFirstFolder || (getChildCount(hFirstFolder) >= nRirekiCount)) {//�K�w���𐔂𒴂��Ă邩�A�t�H���_�������ꍇ�͐V�����t�H���_�����
			if(hFirstFolder) {//�}���t�H���_�ʒu�����߂�
				hFirstFolder = GetNextItem(hFirstFolder,TVGN_PREVIOUS);
				if(!hFirstFolder) hFirstFolder = hTreeItem;
			}
			else {//�ŏ��̊K�w�t�H���_�̏ꍇ �Ō�̎q�A�C�e�����擾
				hFirstFolder = getLastChild(hTreeItem);
			}
			STRING_DATA data;
			initStringData(&data);

			data.m_cKind = KIND_FOLDER;
			data.m_strData = "";
			data.m_strTitle.LoadString(APP_INF_CLASS_HISTORY);
			addData(hFirstFolder,data);//�K�w�����t�H���_���쐬
			hFirstFolder = getFirstFolder(hTreeItem);//���߂ăt�H���_�ʒu���擾
		}
		if(hFirstFolder) {//���Ԃꕪ���ړ�
			HTREEITEM hLastChild = getLastChild(hTreeItem);

			//�f�[�^���擾
			STRING_DATA data,dataTarget;
			data       = getData(hLastChild);
			dataTarget = getData(hFirstFolder);

			//���f�[�^���擾
			TV_ITEM TreeCtrlItem;
			ZeroMemory(&TreeCtrlItem,sizeof(TreeCtrlItem));
			TreeCtrlItem.hItem = hLastChild;
			TreeCtrlItem.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
			this->GetItem(&TreeCtrlItem);

			//�ǉ��f�[�^���쐬
			TV_INSERTSTRUCT AddTreeCtrlItem;

			ZeroMemory(&AddTreeCtrlItem,sizeof(AddTreeCtrlItem));
			AddTreeCtrlItem.item.mask = TreeCtrlItem.mask;
			AddTreeCtrlItem.item.iImage = TreeCtrlItem.iImage;
			AddTreeCtrlItem.item.iSelectedImage = TreeCtrlItem.iSelectedImage;
			AddTreeCtrlItem.item.pszText = data.m_strTitle.GetBuffer(data.m_strTitle.GetLength());
			AddTreeCtrlItem.item.lParam = TreeCtrlItem.lParam;
			AddTreeCtrlItem.hInsertAfter = TVI_FIRST;
			AddTreeCtrlItem.hParent = hFirstFolder;
	
			data.m_nParentID = dataTarget.m_nMyID;
			editData(hLastChild,data);
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
	if(!hStartItem) return NULL;
	list<STRING_DATA>::iterator it;
	HTREEITEM hItem = hStartItem;
	hItem = GetChildItem(hItem);
	do {
		getData(hItem,&it);
		STRING_DATA data = *it;
		if(it->m_cKind & KIND_FOLDER_ALL) 	break;
		hItem = GetNextItem(hItem,TVGN_NEXT);
	}while(hItem);
	return hItem;
}

//---------------------------------------------------
//�֐���	HTREEITEM getLastChild(HTREEITEM hStartItem)
//�@�\		�q�A�C�e���̍Ō��T��
//---------------------------------------------------
HTREEITEM CCharu3Tree::getLastChild(HTREEITEM hStartItem)
{
	if(!hStartItem) return NULL;
	list<STRING_DATA>::iterator it;
	HTREEITEM hItem = hStartItem,hPrevItem;
	hItem = GetChildItem(hItem);
	do {
		getData(hItem,&it);
		if(it->m_cKind & KIND_DATA_ALL) hPrevItem = hItem;
		hItem = GetNextItem(hItem,TVGN_NEXT);
	}while(hItem);
	return hPrevItem;
}

//---------------------------------------------------
//�֐���	deleteExcessChildren(HTREEITEM hTreeItem,int nCount)
//�@�\		�]�v�Ȏq�̐�������
//---------------------------------------------------
void CCharu3Tree::deleteExcessChildren(HTREEITEM hTreeItem,int *nCount)
{
	if(!hTreeItem) return;
	list<STRING_DATA>::iterator it;
	HTREEITEM hPrevItem;
	do {
		getData(hTreeItem,&it);
		if(ItemHasChildren(hTreeItem)) {
			deleteExcessChildren(GetChildItem(hTreeItem),nCount);
		}
		hPrevItem = hTreeItem;
		hTreeItem = GetNextItem(hTreeItem,TVGN_NEXT);
		if(*nCount <= 1 && !ItemHasChildren(hPrevItem)) {//�q�������Ă����܂�
			deleteData(hPrevItem);
		}
		else if(it->m_cKind & KIND_DATA_ALL)	(*nCount)--;
	}while(hTreeItem);
	return;
}

//---------------------------------------------------
//�֐���	getChildCount(HTREEITEM hTreeItem)
//�@�\		�q�̐��𐔂���
//---------------------------------------------------
int CCharu3Tree::getChildCount(HTREEITEM hTreeItem,bool isBrotherOnly)
{
	int nChildren = 0;
	if(hTreeItem) hTreeItem = GetChildItem(hTreeItem);
	if(!hTreeItem) return nChildren;
	list<STRING_DATA>::iterator it;
	do {
		getData(hTreeItem,&it);
		if(!isBrotherOnly && ItemHasChildren(hTreeItem)) {
			nChildren += getChildCount(hTreeItem);
		}
		if(it->m_cKind & KIND_DATA_ALL)	nChildren++;
		hTreeItem = GetNextItem(hTreeItem,TVGN_NEXT);
	}while(hTreeItem);



	return nChildren;
}

//---------------------------------------------------
//�֐���	moveFolderTop(HTREEITEM hTreeItem)
//�@�\		�f�[�^���t�H���_�̐擪�Ɉړ�
//---------------------------------------------------
HTREEITEM CCharu3Tree::moveFolderTop(HTREEITEM hTreeItem)
{
	HTREEITEM hRet = NULL;
	if(!hTreeItem) return hRet;

	STRING_DATA dataF;
	HTREEITEM hParent = GetParentItem(hTreeItem);

	if(!hParent) hParent = GetRootItem();
	dataF = getData(hTreeItem);

	//�c���[�f�[�^���擾
	TV_ITEM TreeCtrlItemFrom;
	ZeroMemory(&TreeCtrlItemFrom,sizeof(TreeCtrlItemFrom));
	TreeCtrlItemFrom.hItem = hTreeItem;
	TreeCtrlItemFrom.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	GetItem(&TreeCtrlItemFrom);

	//�ǉ��f�[�^���쐬
	TV_INSERTSTRUCT AddTreeCtrlItem;

	ZeroMemory(&AddTreeCtrlItem,sizeof(AddTreeCtrlItem));
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
//�֐���	removeCheck()
//�@�\		�`�F�b�N���O��
//---------------------------------------------------
void CCharu3Tree::removeCheck()
{
	int nSize = m_MyStringList.size(),i;
	HTREEITEM hTreeItem;
	for(hTreeItem = GetRootItem(),i = 0; i < nSize && hTreeItem;i++,hTreeItem = getTrueNextItem(hTreeItem)) {
		SetCheck(hTreeItem,false);
	}
}

//---------------------------------------------------
//�֐���	checkItem(HTREEITEM hItem)
//�@�\		�f�[�^���`�F�b�N����
//---------------------------------------------------
void CCharu3Tree::checkItem(HTREEITEM hItem)
{
	ModifyStyle(NULL,TVS_CHECKBOXES,NULL);
	if(ItemHasChildren(hItem)) {
		if(GetCheck(hItem)) {
			checkFolder(hItem,false,&m_ltCheckItems);
		}
		else {
			checkFolder(hItem,false,&m_ltCheckItems);
			checkFolder(hItem,true,&m_ltCheckItems);
		}
	}
	else if(hItem) {
		SetCheck(hItem,!GetCheck(hItem));
		if(GetCheck(hItem)) {//���X�g�ɒǉ�
			m_ltCheckItems.insert(m_ltCheckItems.end(),hItem);
		}
		else {//���X�g����폜
			list<HTREEITEM>::iterator it;
			for(it = m_ltCheckItems.begin(); it != m_ltCheckItems.end(); it++) {
				if(*it == hItem) {
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
	if(ItemHasChildren(hItem)) {
		checkFolder(hItem,false,&m_ltCheckItems);
	}
	else if(hItem) {
		SetCheck(hItem,false);
		list<HTREEITEM>::iterator it;
		for(it = m_ltCheckItems.begin(); it != m_ltCheckItems.end(); it++) {
			if(*it == hItem) {
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
	if(m_isDrag) {
		CPoint ptTree(point);

		MapWindowPoints(this,&ptTree,1);
		m_pDragImage->DragMove(point);
		m_pDragImage->DragLeave(this);
		UINT uHitTest = TVHT_ONITEM;
		m_hDragTarget = this->HitTest(point,&uHitTest);
		if(m_hDragTarget) {
			STRING_DATA data;
			data = getData(m_hDragTarget);
			//�t�H���_�Ŏq�������Ȃ���ΑI���A����ȊO�͑}���}�[�N
			if(data.m_cKind & KIND_FOLDER_ALL && !GetChildItem(m_hDragTarget)) {
				SetInsertMark(NULL);
				SelectItem(m_hDragTarget);
			}
			else {
				SetInsertMark(m_hDragTarget,TRUE);
				SelectItem(m_hDragTarget);
			}
		}
		else {
			if(point.y <= 0) {
				m_hDragTarget = GetFirstVisibleItem();
				m_hDragTarget = GetNextItem(m_hDragTarget,TVGN_PREVIOUSVISIBLE);
			}
			m_pDragImage->DragLeave(this);
			SelectItem(m_hDragTarget);
			return;
		}
		this->UpdateWindow();
		m_pDragImage->DragEnter(this, point);
	}
	else {
		CPoint ptTree(point);

		MapWindowPoints(this,&ptTree,1);
		m_pDragImage->DragMove(point);
		m_pDragImage->DragLeave(this);
		UINT uHitTest = TVHT_ONITEMLABEL ;
		HTREEITEM hTarget = this->HitTest(point,&uHitTest);
		if(hTarget && m_hPrevTarget != hTarget && uHitTest & 0x06) {
			::PostMessage(this->GetParent()->m_hWnd,WM_TIPS_CHANGE,(unsigned int)hTarget,0);
			m_hPrevTarget = hTarget;
		}
		else if(m_hPrevTarget != hTarget){
			::PostMessage(this->GetParent()->m_hWnd,WM_TIPS_CHANGE,NULL,0);
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

	CPoint ptImage,ptHotSpot;

	RECT rSelItem;
	this->GetItemRect(m_hDragItem, &rSelItem,NULL);
	m_pDragImage = this->CreateDragImage(m_hDragItem);

	m_pDragImage->DragShowNolock(TRUE);
	ptHotSpot.x = pNMTreeView->ptDrag.x + rSelItem.right;
	ptHotSpot.y = pNMTreeView->ptDrag.y + rSelItem.bottom;
	m_pDragImage->SetDragCursorImage(0,ptHotSpot);
	CPoint ptCursorPos;
	GetCursorPos(&ptCursorPos);
	m_pDragImage->BeginDrag(0,CPoint(0,0));
	m_pDragImage->DragEnter(this,pNMTreeView->ptDrag);
	SetCapture();

//	m_nDragPosY = pNMTreeView->ptDrag.y;
	m_isDrag = true;

	*pResult = 0;
}

//---------------------------------------------------
//�֐���	OnLButtonUp(UINT nFlags, CPoint point) 
//�@�\		�{�^���𗣂���
//---------------------------------------------------
void CCharu3Tree::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(!m_isDrag) return;

	m_isDrag = false;
	m_pDragImage->DragLeave(this);
	m_pDragImage->EndDrag();
	delete m_pDragImage;
	ReleaseCapture();
	SetInsertMark(NULL);

	//�A�C�e���̈ړ�����������
	if(m_hDragTarget && m_hDragTarget != m_hDragItem) {
		//�����̎q���ɂ͈ړ��ł��Ȃ�
		if(!checkMyChild(m_hDragItem,m_hDragTarget)) {
			AfxGetApp()->DoWaitCursor(1);

			//�f�[�^���擾
			STRING_DATA data,dataTarget;
			data       = getData(m_hDragItem);
			dataTarget = getData(m_hDragTarget);

			//�c���[�f�[�^���擾
			TV_ITEM TreeCtrlItem;
			ZeroMemory(&TreeCtrlItem,sizeof(TreeCtrlItem));
			TreeCtrlItem.hItem = m_hDragItem;
			TreeCtrlItem.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
			this->GetItem(&TreeCtrlItem);

			//�ǉ��f�[�^���쐬
			TV_INSERTSTRUCT AddTreeCtrlItem;

			ZeroMemory(&AddTreeCtrlItem,sizeof(AddTreeCtrlItem));
			AddTreeCtrlItem.hInsertAfter = m_hDragTarget;
			AddTreeCtrlItem.item.mask = TreeCtrlItem.mask;
			AddTreeCtrlItem.item.iImage = TreeCtrlItem.iImage;
			AddTreeCtrlItem.item.iSelectedImage = TreeCtrlItem.iSelectedImage;
			AddTreeCtrlItem.item.pszText = data.m_strTitle.GetBuffer(data.m_strTitle.GetLength());
			AddTreeCtrlItem.item.lParam = TreeCtrlItem.lParam;

			//�e��ݒ�
			if(dataTarget.m_cKind & KIND_FOLDER_ALL && !ItemHasChildren(m_hDragTarget)) {
				AddTreeCtrlItem.hInsertAfter = TVI_FIRST;
				AddTreeCtrlItem.hParent = m_hDragTarget;//�t�H���_
				data.m_nParentID = dataTarget.m_nMyID;
				editData(m_hDragItem,data);
			}
			else {
				AddTreeCtrlItem.hParent = GetParentItem(m_hDragTarget);//����ȊO
				data.m_nParentID = dataTarget.m_nParentID;
				editData(m_hDragItem,data);
			}
			//�f�[�^���ړ�
			if(data.m_cKind & KIND_FOLDER_ALL && ItemHasChildren(m_hDragItem)) {
				copyChildren(m_hDragItem,InsertItem(&AddTreeCtrlItem));
				checkOut(m_hDragItem);
				clearFolder(GetChildItem(m_hDragItem));
				DeleteItem(m_hDragItem);
			}
			else {
				SelectItem(InsertItem(&AddTreeCtrlItem));
				checkOut(m_hDragItem);
				DeleteItem(m_hDragItem);
			}

			AfxGetApp()->DoWaitCursor(0);
		}
	}
	CTreeCtrl::OnLButtonUp(nFlags, point);
}

//---------------------------------------------------
//�֐���	OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
//�@�\		�t�H���_�J����
//---------------------------------------------------
void CCharu3Tree::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	list<STRING_DATA>::iterator it;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if(hItem) {
		getData(hItem,&it);
		//����
		if(pNMTreeView->action == 1)
			it->m_cKind = it->m_cKind & (FOLDER_OPEN ^ 0xff); 
		//�J��
		else if(pNMTreeView->action == 2)
			it->m_cKind = it->m_cKind | FOLDER_OPEN;
	}
	*pResult = 0;
}

void CCharu3Tree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	list<STRING_DATA>::iterator it;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if(hItem) {
		getData(hItem,&it);
		setSelectID(it->m_nMyID);
	}
	*pResult = 0;
}

void CCharu3Tree::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	if(lpwndpos) {
		CTreeCtrl::OnWindowPosChanging(lpwndpos);
	}
}

//---------------------------------------------------
//�֐���	setScrollBar
//�@�\		�X�N���[���o�[�ݒ�
//---------------------------------------------------
void CCharu3Tree::setScrollBar()
{
	//�X�N���[���o�[�̏�Ԃ�ς��鎞���������悤�ɂ��Ƃ��悤��

	
	int nPageH,nPageV;
	SCROLLINFO infoHorz,infoVert;
	static SCROLLINFO stInfoHorz,stInfoVert;
	infoHorz.cbSize = sizeof(SCROLLINFO);
	infoVert.cbSize = sizeof(SCROLLINFO);
	if(theApp.m_ini.m_visual.m_nScrollbar == SB_BOTH) {//�����t����
		this->GetScrollInfo(SB_HORZ,&infoHorz,SIF_ALL);
		nPageH = infoHorz.nPage;
		this->GetScrollInfo(SB_VERT,&infoVert,SIF_ALL);
		nPageV = infoVert.nPage;

//		if(memcmp(&infoHorz,&stInfoHorz,sizeof(SCROLLINFO)) != 0 || memcmp(&infoVert,&stInfoVert,sizeof(SCROLLINFO)) != 0) {
			if(nPageH > 1 && nPageV > 1) this->ShowScrollBar(SB_BOTH,TRUE);
			if(nPageH > 1)	this->ShowScrollBar(SB_HORZ,TRUE);
			if(nPageV > 1)	this->ShowScrollBar(SB_VERT,TRUE);

			stInfoHorz = infoHorz;
			stInfoVert = infoVert;
//		}
	}
	//��������
	else if(theApp.m_ini.m_visual.m_nScrollbar == SB_HORZ) {
		this->GetScrollInfo(SB_HORZ,&infoHorz,SIF_ALL);
		nPageH = infoHorz.nPage;
//		if(memcmp(&infoHorz,&stInfoHorz,sizeof(SCROLLINFO)) != 0) {
			if(nPageH > 1)	this->ShowScrollBar(SB_HORZ,TRUE);
			this->ShowScrollBar(SB_VERT,FALSE);

			stInfoHorz = infoHorz;
//		}
	}
	//��������
	else if(theApp.m_ini.m_visual.m_nScrollbar == SB_VERT) {
		this->GetScrollInfo(SB_VERT,&infoVert,SIF_ALL);
		nPageV = infoVert.nPage;
//		if(memcmp(&infoVert,&stInfoVert,sizeof(SCROLLINFO)) != 0) {
			if(nPageV > 1)	this->ShowScrollBar(SB_VERT,TRUE);
			this->ShowScrollBar(SB_HORZ,FALSE);

			stInfoVert = infoVert;
//		}
	}
	else{
		this->ShowScrollBar(SB_BOTH,FALSE);
	}

}

//---------------------------------------------------
//�֐���	OnMouseWheel
//�@�\		�z�C�[������
//---------------------------------------------------
BOOL CCharu3Tree::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(theApp.m_ini.m_visual.m_nScrollbar != SB_BOTH && theApp.m_ini.m_visual.m_nScrollbar != SB_VERT) {
		if(zDelta < 0)
			SendMessage(WM_KEYDOWN,VK_DOWN,0);
		else
			SendMessage(WM_KEYDOWN,VK_UP,0);
	}
	return CTreeCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CCharu3Tree::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	setScrollBar();
	
	*pResult = 0;
}
