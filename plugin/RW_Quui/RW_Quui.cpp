
/*----------------------------------------------------------
Quui�p�f�[�^�ǂݏ���DLL
									2002/09/25 (c)C+ Factory
----------------------------------------------------------*/

#include "stdafx.h"
#include "RW_Plugin.h"

#pragma data_seg(".shared")
//�A�v���P�[�V�������Ɗg�������w��
const TCHAR *szName = _T("Quui");
const TCHAR *szExtn = _T("qui");

//�f�[�^�ۊǗp���X�g
list<STRING_DATA> *pData;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.shared,RWS")

//---------------------------------------------------
//�֐���	void initDLL()
//�@�\		����������
//---------------------------------------------------
extern "C" __declspec (dllexport) void initDLL()
{
	pData = NULL;
}

//---------------------------------------------------
//�֐���	void endDLL()
//�@�\		�������
//---------------------------------------------------
extern "C" __declspec (dllexport) void endDLL()
{
	if(pData) {
		pData->clear();
		delete pData;
	}
}

//---------------------------------------------------
//�֐���	bool isUnicode()
//�@�\		unicode����
//---------------------------------------------------
extern "C" __declspec (dllexport) bool isUnicode()
{
#ifdef _UNICODE
	return true;
#else
	return false;
#endif
}

//---------------------------------------------------
//�֐���	getFormatName(TCHAR *szBuff,int nSize)
//�@�\		�`�����̂��擾
//---------------------------------------------------
extern "C" __declspec (dllexport) void getFormatName(TCHAR *szBuff,int nSize)
{
	if(sizeof(szName) > nSize) return;
	_tcscpy(szBuff,szName);
}

//---------------------------------------------------
//�֐���	getExtension(TCHAR *szBuff,int nSize)
//�@�\		�g���q���擾
//---------------------------------------------------
extern "C" __declspec (dllexport) void getExtension(TCHAR *szBuff,int nSize)
{
	if(sizeof(szExtn) > nSize) return;
	_tcscpy(szBuff,szExtn);
}


//---------------------------------------------------
//�֐���	readDataFile(CString strFileName,bool isImport)
//�@�\		���X�g�\���̂Ƀt�@�C����ǂݍ���
//---------------------------------------------------
extern "C" __declspec (dllexport) list<STRING_DATA>* readDataFile(CString strFileName,bool isImport)
{
	STRING_DATA Data;

	if(!pData) pData = new list<STRING_DATA>;
	pData->clear();
	int nID ;
	time_t lTime;

	CFile MyFile;
	//�t�@�C�����J��
	if(MyFile.Open(strFileName,CFile::modeRead,NULL))	{
		CFileStatus fileStatus;
		MyFile.GetStatus(fileStatus);
		lTime = fileStatus.m_ctime.GetTime();
		nID = (int)lTime;
		MyFile.Close();
	}
	else return NULL;

	Data.m_cIcon = 0;		//�A�C�R�����
	Data.m_timeCreate = lTime;	//�쐬����
	Data.m_timeEdit = lTime;		//�ύX����

	//--------------------------------��������ύX------------------------------------
	vector<CString> vecTitle;
	vector<CString> vecText;

	//�����t�H���_�쐬
	int nParent2;
	Data.m_nParentID = ROOT;
	Data.m_cKIND = KIND_RIREKI;//���
	Data.m_nMyID = nID++;

	Data.m_strTitle = _T("�����t�H���_");
	Data.m_strMacro = _T("count=100\r\n");
	pData->insert(pData->end(),Data);

	//�f�[�^�ǂݍ���
	CFile QuuiFile;
	char *strTmp,strTmpSt[64000],cBuff[3];
	ZeroMemory(&strTmpSt,sizeof(strTmpSt));
	CString strBuff;
	strBuff = _T("");
	int nSeq = 1;

	//�t�@�C�����J��
	if(!QuuiFile.Open(strFileName,CFile::modeRead,NULL)) {
		return NULL;
	}
	strTmp = strTmpSt;

	//�^�C�g����0x0d0a��؂�ŁA���̂��Ƃ�NULL��؂�Ŗ{��
	if(QuuiFile.Read(&cBuff,3) < 3) {
		return NULL;
	}
	while(QuuiFile.Read(&cBuff,1) >= 1) {
		if(nSeq == 1) {//�^�C�g��
			if(*cBuff == 0x0d) {//���s
				*strTmp = (char)NULL;
				strBuff = strTmpSt;
				vecTitle.insert(vecTitle.end(),strBuff);
				strTmp = strTmpSt;//�|�C���^���A
				QuuiFile.Read(&cBuff,1);
			}
			else if(*cBuff == NULL) {//�^�C�g���I���
				nSeq = 2;
				strTmp = strTmpSt;//�|�C���^���A
			}
			else {
				*strTmp = *cBuff;
				strTmp++;
			}
		}
		else if(nSeq == 2) {//�{��
			if(*cBuff == NULL) {//�{���I���
				*strTmp = NULL;
				strBuff = strTmpSt;
				vecText.insert(vecText.end(),strBuff);
				strTmp = strTmpSt;//�|�C���^���A
			}
			else {
				*strTmp = *cBuff;
				strTmp++;
			}
		}
	}
	QuuiFile.Close();
	//�f�[�^��ǉ�

	int i;
	for(i = 0; i < vecTitle.size() && i < vecText.size(); i++) {
		if((i % 20) == 0) {
			Data.m_strTitle.Format(_T("%d_Quui�f�[�^"),i/20+1);
			Data.m_strData.Format(_T("%d_Quui�f�[�^"),i/20+1);
			Data.m_cKIND = KIND_FOLDER;//���
			Data.m_nMyID = nID++;
			nParent2 = Data.m_nMyID;
			Data.m_nParentID = ROOT;//nParent;
			pData->insert(pData->end(),Data);
		}
		Data.m_cKIND = KIND_LOCK;
		Data.m_nMyID = nID++;
		Data.m_nParentID = nParent2;
		Data.m_strTitle = vecTitle[i];
		Data.m_strData = vecText[i];
		pData->insert(pData->end(),Data);
	}
	
	return pData;
}


//---------------------------------------------------
//�֐���	writeDataFile(CString strFileName,CList<STRING_DATA,STRING_DATA> *pList)
//�@�\		���X�g�\���̂��t�@�C���ɏ����o��
//---------------------------------------------------
extern "C" __declspec (dllexport) bool writeDataFile(CString strFileName,list<STRING_DATA> *pList)
{
	list<STRING_DATA>::iterator it;

	STRING_DATA Data;
	CString strBuff;

	CFile MyFile(strFileName,CFile::modeWrite | CFile::modeCreate);

	char strTmp[256];
	strTmp[0] = (char)0xe7;
	strTmp[1] = (char)0x23;
	strTmp[2] = (char)0x00;
	MyFile.Write(&strTmp,3);
#ifdef _UNICODE
	char *szWriteBuff;
	int nLength;
#else
	LPTSTR lpStr;
#endif

	for(it = pList->begin(); it != pList->end(); it++) {
		Data = *it;
		if(!(Data.m_cKIND & KIND_FOLDER_ALL)) {
			strBuff = Data.m_strTitle + "\r\n";
#ifdef _UNICODE
			nLength = ::WideCharToMultiByte(CP_ACP, 0, strBuff,-1,NULL,0,NULL,NULL);
			szWriteBuff = new char[nLength];
			::WideCharToMultiByte(CP_ACP,0,strBuff,-1,szWriteBuff,nLength,"",NULL);
			MyFile.Write(szWriteBuff,nLength-1);
			delete szWriteBuff;
#else
			lpStr = (LPTSTR)LPCTSTR(strBuff);
			MyFile.Write(lpStr,strBuff.GetLength());
#endif
		}
	}
	strTmp[0] = (char)0x00;
	strTmp[1] = (char)0x00;
	MyFile.Write(&strTmp,1);
	for(it = pList->begin(); it != pList->end(); it++) {
		Data = *it;
		if(!(Data.m_cKIND & KIND_FOLDER_ALL)) {
#ifdef _UNICODE
			nLength = ::WideCharToMultiByte(CP_ACP, 0, Data.m_strData,-1,NULL,0,NULL,NULL);
			szWriteBuff = new char[nLength];
			::WideCharToMultiByte(CP_ACP,0,Data.m_strData,-1,szWriteBuff,nLength,"",NULL);
			MyFile.Write(szWriteBuff,nLength);
			delete szWriteBuff;
#else
			lpStr = (LPTSTR)LPCTSTR(Data.m_strData);
			MyFile.Write(lpStr,Data.m_strData.GetLength());
			MyFile.Write(&strTmp,1);
#endif
		}
	}
	strTmp[0] = (char)0x00;
	strTmp[1] = (char)0xe7;
	strTmp[2] = (char)0x23;
	strTmp[3] = (char)0x01;
	MyFile.Write(&strTmp,4);
	
	MyFile.Close();

	return true;
}


//---------------------------------------------------
//�֐���	convertMacro(TCHAR *szSource,TCHAR *szRet,int nSize,TCHAR *szSelect,TCHAR *szClip)
//�@�\		�}�N�������v���O�C��
//---------------------------------------------------
extern "C" __declspec (dllexport) bool convertMacro(TCHAR *szSource,TCHAR *szRet,int nSize,TCHAR *szSelect,TCHAR *szClip)
{
	bool isRet = false;
	return isRet;
}
