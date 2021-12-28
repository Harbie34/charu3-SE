
/*----------------------------------------------------------
Charu2�p�f�[�^�ǂݏ���DLL
									2002/09/02 (c)C+ Factory
----------------------------------------------------------*/

#include "stdafx.h"
#include "RW_Plugin.h"

#pragma data_seg(".shared")
//�A�v���P�[�V�������Ɗg�������w��
const TCHAR *szName = _T("Charu2");
const TCHAR *szExtn = _T("dat");

//�f�[�^�ۊǗp���X�g
list<STRING_DATA> *pData;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.shared,RWS")

#define C2KIND_NORMAL 0	//�m�[�}��������
#define C2KIND_LOCK   1	//���b�N����
#define C2KIND_FOLDER 2	//�t�H���_����
#define C2KIND_RIREKI 3	//�����t�H���_
//---------------------------------------------------
//�֐���	GetRirekiOption(CString strData,CString strKind)
//�@�\		����ݒ�����
//---------------------------------------------------
int GetFolderOption(CString strData,CString strKind)
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
			else if(strBuff == _T("")) nRet = -1;
			else nRet = _ttoi(strBuff.GetBuffer(strBuff.GetLength()));
		}
		else nRet = -1;
	}
	else nRet = -1;
	return nRet;
}

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
	if(!pData) pData = new list<STRING_DATA>;
	pData->clear();
	int nID = 0;
	time_t lTime;
	nID = time(&lTime); 

	STRING_DATA data;
	int nDataSize;
	int nBuff;
	char cBuff;
	FILE *fFile;

	data.m_cIcon = 0;
	data.m_timeCreate = 0;
	data.m_timeEdit = 0;
/*
	���ʎq
	�f�[�^���		4byte
	�t�H���_�J��	1byte
	�A�C�R�����	1byte
	���g�p			1byte
	ID				4byte
	�e��ID			4byte
	�^�C�g���T�C�Y	4byte
	�^�C�g��
	�{���T�C�Y		4byte
	�{��
*/
	//�t�@�C���̊g���q������
	char *szReadBuff;
	CString strExtention;
	strExtention = strFileName.Right(3);
	strExtention.MakeLower();
	if(strExtention != "dat" && strExtention != "cdt") return NULL;

	if(!(fFile = _tfopen(strFileName,_T("rb"))))	return NULL;//�t�@�C�����J��
	if(feof(fFile)) return NULL;

	if(!fread(&nBuff,sizeof(nBuff),1,fFile)) return NULL;//�f�[�^����ǂݍ���
	while(!feof(fFile)) {
		if(!fread(&data.m_cKind,sizeof(data.m_cKind),1,fFile)) break;//�f�[�^�̎�ނ�ǂݍ���
		if(data.m_cKind == C2KIND_NORMAL)
			data.m_cKind = KIND_ONETIME;
		else if(data.m_cKind == C2KIND_LOCK)
			data.m_cKind = KIND_LOCK;
		else if(data.m_cKind == C2KIND_FOLDER)
			data.m_cKind = KIND_FOLDER;

		if(!fread(&cBuff,sizeof(cBuff),1,fFile)) break;//�t�H���_�J��
		if(cBuff)	data.m_cKind = data.m_cKind | FOLDER_OPEN;
		if(!fread(&data.m_cIcon,sizeof(data.m_cIcon),1,fFile)) break;//�A�C�R�����
		if(data.m_cIcon == (char)200)	data.m_cKind = KIND_RIREKI;
		if(!fread(&cBuff,sizeof(cBuff),1,fFile)) break;//���g�p
		if(!fread(&data.m_nMyID,sizeof(data.m_nMyID),1,fFile)) break;//������ID��ǂݍ���
		if(!data.m_nMyID) data.m_nMyID = nID++;
		if(!fread(&data.m_nParentID,sizeof(data.m_nParentID),1,fFile)) break;//�e��ID��ǂݍ���
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

		if(data.m_cIcon < 0) data.m_cIcon = 0;
		if(data.m_cKind & KIND_FOLDER_ALL) {
			data.m_strMacro = data.m_strData;
		}
		pData->insert(pData->end(),data);
	}
	fclose(fFile);
	return pData;
}


//---------------------------------------------------
//�֐���	writeDataFile(CString strFileName,CList<STRING_DATA,STRING_DATA> *pList)
//�@�\		���X�g�\���̂��t�@�C���ɏ����o��
//---------------------------------------------------
extern "C" __declspec (dllexport) bool writeDataFile(CString strFileName,list<STRING_DATA> *pList)
{
	bool isRet = false;
	STRING_DATA Data;
	FILE *fFile;
	vector<int> arrayEndClearParent;
	int i,nDataSize,nDataCount;
	char cBuff;
#ifdef _UNICODE
	char *lpStr;
#else
	LPTSTR lpStr;
#endif
	list<STRING_DATA> listData;
	listData = *pList;
	list<STRING_DATA>::iterator it,prevIT;

	//�����t�H���_�N���A��T��
	for (it = listData.begin(); it != listData.end(); it++) {
		if((it->m_cKind & KIND_RIREKI) && GetFolderOption(it->m_strData,_T("endclear")) == 1) {
			arrayEndClearParent.insert(arrayEndClearParent.end(),it->m_nMyID);
		}
		else if(it->m_cKind & KIND_ONETIME) {
			prevIT = it;
			it++;
			listData.erase(prevIT);
			it--;
		}
		else if(it->m_cKind & KIND_LOCK) {
			for(i = 0; i < arrayEndClearParent.size(); i++) {
				if(arrayEndClearParent[i] == it->m_nParentID) {
					prevIT = it;
					it++;
					listData.erase(prevIT);
					it--;
					break;
				}
			}
		}
	}
	int nEndClearParentCount = arrayEndClearParent.size();

	//�ۑ�����f�[�^�̐��𒲂ׂ�
	nDataCount = listData.size();
	
	//�f�[�^��ۑ�����
	if(!(fFile = _tfopen(strFileName,_T("wb"))))	return isRet;//�t�@�C�����J��
	fwrite(&nDataCount,sizeof(nDataCount),1,fFile);//�f�[�^��
	
	//�f�[�^��ۑ�����
	for (it = listData.begin(); it != listData.end(); it++) {
		if(it->m_cKind != KIND_FOLDER) {
			for(i = 0; i < nEndClearParentCount; i++) {
				if(arrayEndClearParent[i] == Data.m_nParentID) {
					break;
				}
			}
		}
		Data = *it;
		if(Data.m_cKind & KIND_FOLDER)		cBuff = C2KIND_FOLDER;
		else if(Data.m_cKind & KIND_RIREKI)	cBuff = C2KIND_FOLDER;
		else if(Data.m_cKind & KIND_LOCK)	cBuff = C2KIND_LOCK;
		fwrite(&cBuff,sizeof(cBuff),1,fFile);//�f�[�^�̎��
		if(Data.m_cKind & FOLDER_OPEN)		cBuff = 1;
		else cBuff = 0;
		fwrite(&cBuff,sizeof(cBuff),1,fFile);//�J���Ă邩
		if(Data.m_cKind & KIND_RIREKI)	cBuff = (char)200;
		else cBuff = Data.m_cIcon;
		fwrite(&cBuff,sizeof(cBuff),1,fFile);//�A�C�R�����
		fwrite(&cBuff,sizeof(cBuff),1,fFile);//���g�p
		fwrite(&Data.m_nMyID,sizeof(Data.m_nMyID),1,fFile);//������ID
		fwrite(&Data.m_nParentID,sizeof(Data.m_nParentID),1,fFile);//�e��ID

#ifdef _UNICODE
		//�^�C�g����ۑ�
		nDataSize = ::WideCharToMultiByte(CP_ACP, 0, Data.m_strTitle,-1,NULL,0,NULL,NULL);
		lpStr = new char[nDataSize + 1];
		if(lpStr) {
			::WideCharToMultiByte(CP_ACP,0,Data.m_strTitle,-1,lpStr,nDataSize + 1,"",NULL);
			fwrite(&nDataSize,sizeof(nDataSize),1,fFile);//�T�C�Y
			fwrite(lpStr,nDataSize,1,fFile);
			delete lpStr;
		}
		//���͂�ۑ�
		nDataSize = ::WideCharToMultiByte(CP_ACP, 0, Data.m_strData,-1,NULL,0,NULL,NULL);
		lpStr = new char[nDataSize + 1];
		if(lpStr) {
			::WideCharToMultiByte(CP_ACP,0,Data.m_strData,-1,lpStr,nDataSize + 1,"",NULL);
			fwrite(&nDataSize,sizeof(nDataSize),1,fFile);//�T�C�Y
			fwrite(lpStr,nDataSize,1,fFile);
			delete lpStr;
		}
#else
		//�^�C�g����ۑ�
		nDataSize = Data.m_strTitle.GetLength() + 1;
		fwrite(&nDataSize,sizeof(nDataSize),1,fFile);//�T�C�Y
		lpStr = (char*)LPCTSTR(Data.m_strTitle);
		fwrite(lpStr,nDataSize,1,fFile);
		//���͂�ۑ�
		nDataSize = Data.m_strData.GetLength() + 1;
		fwrite(&nDataSize,sizeof(nDataSize),1,fFile);//�T�C�Y
		lpStr = (char*)LPCTSTR(Data.m_strData);
		fwrite(lpStr,nDataSize,1,fFile);
#endif
	}
	fclose(fFile);
	isRet = true;
	return isRet;
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
