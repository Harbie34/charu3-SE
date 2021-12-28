
/*----------------------------------------------------------
CLCL�p�f�[�^�ǂݏ���DLL
									2003/04/30 (c)C+ Factory
----------------------------------------------------------*/

#include "stdafx.h"
#include "RW_Plugin.h"

#pragma data_seg(".shared")
//�A�v���P�[�V�������Ɗg�������w��
const TCHAR *szName = _T("CLCL");
const TCHAR *szExtn = _T("dat");

//�f�[�^�ۊǗp���X�g
list<STRING_DATA> *pData;
#pragma data_seg()
//#pragma comment(linker, "/SECTION:.shared,RWS")


bool readCLCLData(CFile *pMyFile,STRING_DATA *pData);
bool readUntilZero(CFile *pMyFile,char *szReadBuff,int nSize);

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
//�֐���	getFormatName(char *szBuff,int nSize)
//�@�\		�`�����̂��擾
//---------------------------------------------------
extern "C" __declspec (dllexport) void getFormatName(TCHAR *szBuff,int nSize)
{
	if(sizeof(szName) > nSize) return;
	_tcscpy(szBuff,szName);
}

//---------------------------------------------------
//�֐���	getExtension(char *szBuff,int nSize)
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
	Data.m_cIcon = 0;
	Data.m_timeCreate = 0;
	Data.m_timeEdit = 0;

	if(!pData) pData = new list<STRING_DATA>;
	pData->clear();

	int nID = 0;
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

	//--------------------------------��������ύX------------------------------------
	//�f�[�^�ǂݍ���
	CString strText,strFolder,strRirekiFileName;
	list<STRING_DATA> lstFolder;
	list<int> lstParent;

	//�܂��͗����t�@�C����ǂݍ���
	int nFound = strFileName.ReverseFind('\\');
	if(nFound >= 0) strFolder = strFileName.Left(nFound) + _T("\\");
	else 			strFolder = _T(".\\");
	strRirekiFileName = strFolder + _T("history.dat");
	int nFolderID;

	//�t�@�C�����J��
	if(MyFile.Open(strRirekiFileName,CFile::modeRead,NULL))	{

		Data.m_nParentID = -1;
		Data.m_cKIND = KIND_RIREKI;//���
		Data.m_nMyID = nID++;
		nFolderID = Data.m_nMyID;

		Data.m_strTitle = _T("@h����");
		Data.m_strMacro = _T("count=200\r\nClassHistory=20\r\n");
		pData->insert(pData->end(),Data);
		Data.m_strMacro = _T("");

		Data.m_cIcon = 0;
		Data.m_strMacro = "";
		Data.m_nParentID = nFolderID;
		while(1) {
			Data.m_nMyID = nID++;
			//�f�[�^�ǂݍ���
			if(readCLCLData(&MyFile,&Data)) {
				if(Data.m_cKIND == -2) continue;
				else if(Data.m_cKIND == -1) {
					//1�K�w�オ��
					list<int>::iterator it;
					it = lstParent.end();
					it--;
					Data.m_nParentID = *it;
					lstParent.pop_back(); 
				}
				else {
					if(Data.m_strTitle == "") {
						Data.m_strTitle = Data.m_strData.Left(32);
						Data.m_strTitle.Replace(_T("\n"),_T(""));
						Data.m_strTitle.Replace(_T("\t"),_T(""));
					}
					pData->insert(pData->end(),Data);
					if(Data.m_cKIND & KIND_FOLDER) {
						lstParent.push_back(Data.m_nParentID);
						Data.m_nParentID = Data.m_nMyID;
					}
				}
			}
			else break;
		}
		MyFile.Close();
	}
//regist.dat
	strRirekiFileName = strFolder + _T("regist.dat");
	lstParent.clear();

	//�t�@�C�����J��
	if(MyFile.Open(strRirekiFileName,CFile::modeRead,NULL))	{

		Data.m_nParentID = -1;
		Data.m_cKIND = KIND_FOLDER;//���
		Data.m_nMyID = nID++;
		nFolderID = Data.m_nMyID;

		Data.m_strTitle = _T("@t�o�^�A�C�e��");
		Data.m_strMacro = _T("");
		pData->insert(pData->end(),Data);
		Data.m_strMacro = _T("");

		Data.m_cIcon = 0;
		Data.m_strMacro = "";
		Data.m_nParentID = nFolderID;
		while(1) {
			Data.m_nMyID = nID++;
			//�f�[�^�ǂݍ���
			if(readCLCLData(&MyFile,&Data)) {
				if(Data.m_cKIND == -2) continue;
				else if(Data.m_cKIND == -1) {
					//1�K�w�オ��
					list<int>::iterator it;
					it = lstParent.end();
					it--;
					Data.m_nParentID = *it;
					lstParent.pop_back(); 
				}
				else {
					if(Data.m_strTitle == "") {
						Data.m_strTitle = Data.m_strData.Left(32);
						Data.m_strTitle.Replace(_T("\n"),_T(""));
						Data.m_strTitle.Replace(_T("\t"),_T(""));
					}
					pData->insert(pData->end(),Data);
					if(Data.m_cKIND & KIND_FOLDER) {
						lstParent.push_back(Data.m_nParentID);
						Data.m_nParentID = Data.m_nMyID;
					}
				}
			}
			else break;
		}
		MyFile.Close();
	}

	return pData;
}

//---------------------------------------------------
//�֐���	readCLCLData(CFile *pMyFile,STRING_DATA *pData)
//�@�\		CLCL�f�[�^1����ǂݍ���
//---------------------------------------------------
bool readCLCLData(CFile *pMyFile,STRING_DATA *pData)
{
	bool isRet = false;
	char cRead,strReadBuff[65535];
	CString strTmp;
	//�f�[�^���
	if(!pMyFile->Read(&cRead,1)) return isRet;
	if(cRead == 01) {//�f�[�^
		pData->m_cKIND = KIND_LOCK;
	}
	else if(cRead == 04) {//�t�H���_
		pData->m_cKIND = KIND_FOLDER;
	}
	else if(cRead == 05) {//1�K�w�オ��
		pData->m_cKIND = -1;
		return true;
	}
	else if(cRead <= 10) return isRet;
	else {
		pMyFile->Seek(-1,CFile::current);
		goto data_section;
	}
	//�^�C�g��
	if(!readUntilZero(pMyFile,strReadBuff,sizeof(strReadBuff))) return isRet;
	pData->m_strTitle = strReadBuff;
	if(pData->m_cKIND & KIND_FOLDER) {//�t�H���_
		if(pData->m_strTitle == "") pData->m_strTitle = _T("�t�H���_");
		return true;
	}
	//�X�V����
	DWORD dwTime;
	if(!readUntilZero(pMyFile,strReadBuff,sizeof(strReadBuff))) return isRet;
	sscanf(strReadBuff,"%x",&dwTime);
	pData->m_timeCreate = dwTime;
	pData->m_timeEdit = dwTime;
	//���� �E�B���h�E�� 0�܂�
	if(!readUntilZero(pMyFile,strReadBuff,sizeof(strReadBuff))) return isRet;
/*	//���� �c�[���p������ 0�܂�
	if(!readUntilZero(pMyFile,strReadBuff,sizeof(strReadBuff))) return isRet;
	//���� �c�[���plong 0�܂� atoi����
	if(!readUntilZero(pMyFile,strReadBuff,sizeof(strReadBuff))) return isRet;
	//���� �I�v�V���� 0�܂�
	if(!readUntilZero(pMyFile,strReadBuff,sizeof(strReadBuff))) return isRet;
*/	//���� �w�b�_�J�n�ʒu�܂ŃX�L�b�v 02�܂�
	do {
		if(!pMyFile->Read(&cRead,1)) return isRet;
	} while(cRead != 0x02);
data_section:
	//�T�C�Y0�܂� atoi���� 10�i��
	if(!readUntilZero(pMyFile,strReadBuff,sizeof(strReadBuff))) return isRet;
	int nSize = atoi(strReadBuff);
	//�`�� 0�܂�
	if(!readUntilZero(pMyFile,strReadBuff,sizeof(strReadBuff))) return isRet;
	if(strcmp(strReadBuff,"TEXT") != 0) pData->m_cKIND = -2;
/*	//���� �c�[���p������ 0�܂�
	if(!readUntilZero(pMyFile,strReadBuff,sizeof(strReadBuff))) return isRet;
	//���� �c�[���plong 0�܂� atoi����
	if(!readUntilZero(pMyFile,strReadBuff,sizeof(strReadBuff))) return isRet;
	//���� �I�v�V���� 0�܂�
	if(!readUntilZero(pMyFile,strReadBuff,sizeof(strReadBuff))) return isRet;
*/	//03 �f�[�^�J�n�ʒu�܂ŃX�L�b�v 03�܂�
	do {
		if(!pMyFile->Read(&cRead,1)) return isRet;
	} while(cRead != 0x03);
	//�T�C�Y���f�[�^ 0�܂�
	char *pDataBuff;
	pDataBuff = new char[nSize];
	if(!pMyFile->Read(pDataBuff,nSize)) return isRet;
	pData->m_strData = pDataBuff;
	delete pDataBuff;

	isRet = true;
	return isRet;

/*
�f�[�^�̎��  1byte 01�ӂ� 04�t�H���_

�t�H���_ 0�܂Ń^�C�g��

�f�[�^
�^�C�g��      0�܂�
�X�V����      0�܂� 16�i��
���� �E�B���h�E�� 0�܂�
���� �c�[���p������ 0�܂�
���� �c�[���plong 0�܂� atoi����
���� �I�v�V���� 0�܂�
���� �w�b�_�J�n�ʒu�܂ŃX�L�b�v 02�܂�
�T�C�Y0�܂� atoi���� 10�i��
TEXT �`�� 0�܂�
���� �c�[���p������ 0�܂�
���� �c�[���plong 0�܂� atoi����
���� �I�v�V���� 0�܂�
03 �f�[�^�J�n�ʒu�܂ŃX�L�b�v 03�܂�
�T�C�Y���f�[�^ 0�܂�
*/

}

//---------------------------------------------------
//�֐���	readUntilZero(CFile *pMyFile,char *szReadBuff,int nSize)
//�@�\		0�܂ŕ������ǂݍ���
//---------------------------------------------------
bool readUntilZero(CFile *pMyFile,char *szReadBuff,int nSize)
{
	int i = 0;
	bool isRet = false;
	char cRead;
	do {
		if(!pMyFile->Read(&cRead,1)) return isRet;
		*szReadBuff = cRead;
		szReadBuff++;i++;
	} while(cRead && i < nSize);
	*szReadBuff = NULL;

	isRet = true;
	return isRet;
}


//---------------------------------------------------
//�֐���	writeDataFile(CString strFileName,CList<STRING_DATA,STRING_DATA> *pList)
//�@�\		���X�g�\���̂��t�@�C���ɏ����o��
//---------------------------------------------------
extern "C" __declspec (dllexport) bool writeDataFile(CString strFileName,list<STRING_DATA> *pList)
{
	bool isRet = false;
	isRet = true;
	return isRet;

}



//---------------------------------------------------
//�֐���	convertMacro(char *szSource,char *szRet,int nSize,char *szSelect,char *szClip)
//�@�\		�}�N�������v���O�C��
//---------------------------------------------------
extern "C" __declspec (dllexport) bool convertMacro(TCHAR *szSource,TCHAR *szRet,int nSize,TCHAR *szSelect,TCHAR *szClip)
{
	bool isRet = false;
	return isRet;
}
