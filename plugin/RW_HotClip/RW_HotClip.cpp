
/*----------------------------------------------------------
HtoClip�p�f�[�^�ǂݏ���DLL
									2002/09/02 (c)C+ Factory
----------------------------------------------------------*/

#include "stdafx.h"
#include "RW_Plugin.h"

#pragma data_seg(".shared")
//�A�v���P�[�V�������Ɗg�������w��
const TCHAR *szName = _T("hotclip");
const TCHAR *szExtn = _T("txt");

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
	Data.m_cIcon = 0;
	Data.m_timeCreate = 0;
	Data.m_timeEdit = 0;

	if(!pData) pData = new list<STRING_DATA>;
	pData->clear();

	int nID = 0;
	time_t lTime;
	nID = time(&lTime); 

	//�f�[�^�ǂݍ���
	FILE *InFile;
	char strBuff[65535],*szReadBuff;
	CString strText,strRirekiFileName;
	list<STRING_DATA> lstFolder;


	//�܂��͗����t�@�C����ǂݍ���
	int nFound = strFileName.ReverseFind('\\');
	if(nFound >= 0) {
		strRirekiFileName = strFileName.Left(nFound);
		strRirekiFileName = strRirekiFileName + "\\HotClip.hch";
	}
	else strRirekiFileName = ".\\HotClip.hch";

	int nDataSize;
	CFile MyFile;

	//�t�@�C�����J��
	if(MyFile.Open(strRirekiFileName,CFile::modeRead,NULL))	{
		CFileStatus fileStatus;
		int nFolderID;
		MyFile.GetStatus(fileStatus);
		lTime = fileStatus.m_ctime.GetTime();
		nID = (int)lTime;

		if(!MyFile.Read(strBuff,35)) goto error;//�w�b�_��ǂݍ���
		strBuff[35] = NULL;
		if(strcmp(strBuff,"HotClip Clipboard History Data File") != 0) goto error;
		if(!MyFile.Read(strBuff,2)) goto error;

		Data.m_nParentID = -1;
		Data.m_cKIND = KIND_RIREKI;//���
		Data.m_nMyID = nID++;
		nFolderID = Data.m_nMyID;

		Data.m_strTitle = _T("@r�����t�H���_");
		Data.m_strMacro = _T("count=100\r\n");
		pData->insert(pData->end(),Data);

		while(1) {
			if(!MyFile.Read(strBuff,2)) break;//�S�~��ǂݍ���
			if(!MyFile.Read(&nDataSize,sizeof(nDataSize))) break;//�f�[�^�T�C�Y��ǂݍ���
			Data.m_nParentID = nFolderID;
			Data.m_nMyID = nID++;
			
			Data.m_cKIND = KIND_LOCK;//���
			
			//���͂�ǂݍ���
			szReadBuff = new char[nDataSize + 1];
			MyFile.Read(szReadBuff,nDataSize);
			szReadBuff[nDataSize] = NULL;
			Data.m_strData = szReadBuff;
			delete [] szReadBuff;
			Data.m_strTitle = Data.m_strData.Left(32);
			Data.m_strTitle.Replace(_T("\n"),_T("\\n"));
			Data.m_strTitle.Replace(_T("\t"),_T(""));

			pData->insert(pData->end(),Data);
			if(!MyFile.Read(strBuff,4)) break;//�S�~��ǂݍ���
		}
		MyFile.Close();
	}
error:
	
	//�e�L�X�g�f�[�^�t�@�C�����J��
	if((InFile = _tfopen(strFileName,_T("r"))) == NULL)	return NULL;

	lstFolder.clear();
	while(!feof(InFile)) {
		fgets(strBuff,sizeof(strBuff),InFile);//�ǂ�
		strText = strBuff;
		strText.Replace(_T("\t"),_T(""));
		strText.Replace(_T("\n"),_T(""));
		strText.Replace(_T("\r"),_T(""));
		if(strText.Left(2) == _T("[[")) {//�t�H���_
			Data.m_cKIND = KIND_FOLDER;//���
			if(lstFolder.size() == 0)	Data.m_nParentID = ROOT;
			else						Data.m_nParentID = (--lstFolder.end())->m_nMyID;
			Data.m_nMyID = nID;nID++;

			strText.Delete(0,2);
			Data.m_strTitle = strText;
			Data.m_strData = strText;
			pData->insert(pData->end(),Data);
			lstFolder.insert(lstFolder.end(),Data);
		}
		else if(strText.Left(2) == _T("]]")) {//�t�H���_�I���
			if(lstFolder.size() > 0) lstFolder.erase(--lstFolder.end());
		}
		else if(strText.Left(2) == _T("[<")) {//�^�C�g���t��
			Data.m_cKIND = KIND_LOCK;//���
			if(lstFolder.size() == 0)	Data.m_nParentID = ROOT;
			else						Data.m_nParentID = (--lstFolder.end())->m_nMyID;
			Data.m_nMyID = nID++;

			int nfind = strText.Find(_T(">]"));
			Data.m_strTitle = strText.Right(strText.GetLength() - nfind - 2);
			strText.Replace(_T("\\n"),_T("\n"));
			strText.Replace(_T("\\t"),_T("	"));
			strText.Replace(_T("\\\\"),_T("\\"));
			strText = strText.Mid(2,nfind - 2);
			Data.m_strData = strText;
			pData->insert(pData->end(),Data);
		}
//		else if(strText.Left(2) == "[""") {//�V�F�����s
//		}
		else if(strText == "[=Separator=]") {//�Z�p���[�^�[
			Data.m_cKIND = KIND_LOCK;//���
			if(lstFolder.size() == 0)	Data.m_nParentID = ROOT;
			else						Data.m_nParentID = (--lstFolder.end())->m_nMyID;
			Data.m_nMyID = nID++;

			Data.m_strTitle = _T("������������������");
			Data.m_strData = _T("");
			pData->insert(pData->end(),Data);
		}
		else if(strText == _T("[=Break=]") || strText == _T("[=BarBreak=]") || strText.Left(2) == _T("[#")) {//���̍ۖ����̕�����
			continue;
		}
		else if(strText != _T("")){
			Data.m_cKIND = KIND_LOCK;//���
			if(lstFolder.size() == 0)	Data.m_nParentID = ROOT;
			else						Data.m_nParentID = (--lstFolder.end())->m_nMyID;
			Data.m_nMyID = nID++;

			Data.m_strTitle = strText;
			strText.Replace(_T("\\n"),_T("\n"));
			strText.Replace(_T("\\t"),_T("	"));
			strText.Replace(_T("\\\\"),_T("\\"));
			Data.m_strData = strText;
			Data.m_cIcon = 0;
			pData->insert(pData->end(),Data);
		}
	}
	fclose(InFile);
	return pData;
}


//---------------------------------------------------
//�֐���	writeDataFile(CString strFileName,CList<STRING_DATA,STRING_DATA> *pList)
//�@�\		���X�g�\���̂��t�@�C���ɏ����o��
//---------------------------------------------------
extern "C" __declspec (dllexport) bool writeDataFile(CString strFileName,list<STRING_DATA> *pList)
{
	bool isRet = false;
	CString strRirekiFileName;

	//�܂��͗����t�@�C�������쐬
	int nFound = strFileName.ReverseFind(_T('\\'));
	if(nFound >= 0) {
		strRirekiFileName = strFileName.Left(nFound);
		strRirekiFileName = strRirekiFileName + _T("\\HotClip.hch");
	}
	else strRirekiFileName = _T(".\\HotClip.hch");

	list<STRING_DATA>::iterator it;
	STRING_DATA Data;
	int nDataSize,nRirekiID = -1;;
	CFile MyFile(strRirekiFileName,CFile::modeWrite | CFile::modeCreate);
	char szGomi[] = {(char)0x13,(char)0x14},szGomi2[] = {(char)0x05,(char)0x04,(char)0x0d,(char)0x0a};
	char *szHead = "HotClip Clipboard History Data File\r\n";

	MyFile.Write(szHead,strlen(szHead));

	for(it = pList->begin(); it != pList->end(); it++) {
		if(it->m_cKIND & KIND_RIREKI) {//�����t�H���_��ID��ۑ�
			nRirekiID = it->m_nMyID;
			break;
		}
	}
#ifndef _UNICODE
	LPTSTR lpStr;
#else
	int nLength;
	char *szWriteBuff;
#endif
	if(nRirekiID < 0) return isRet;
	//�f�[�^��ۑ�����
	for(it = pList->begin(); it != pList->end(); it++) {
		if(it->m_nParentID == nRirekiID) {//�����f�[�^�𔻒�
			MyFile.Write(szGomi,2);
			//���͂�ۑ�
#ifdef _UNICODE
			nLength = ::WideCharToMultiByte(CP_ACP, 0, it->m_strData,-1,NULL,0,NULL,NULL);
			szWriteBuff = new char[nLength];
			::WideCharToMultiByte(CP_ACP,0,it->m_strData,-1,szWriteBuff,nLength,"",NULL);
			nDataSize = nLength;//it->m_strData.GetLength() + 1;
//			szWriteBuff[nLength] = NULL;
			MyFile.Write(&nDataSize,sizeof(nDataSize));
			MyFile.Write(szWriteBuff,nDataSize);
			MyFile.Write(szGomi2,4);
			delete szWriteBuff;
#else
			nDataSize = it->m_strData.GetLength() + 1;
			MyFile.Write(&nDataSize,sizeof(nDataSize));
			lpStr = (char*)LPCTSTR(it->m_strData);
			MyFile.Write(lpStr,nDataSize);
			MyFile.Write(szGomi2,4);
#endif
		}
	}
	MyFile.Close();
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
	CString strTmp,strTime,strAMPM;
	CTime cTime = CTime::GetCurrentTime();
	_tcscpy(szRet,_T(""));

	strTmp = szSource;

	//2000 ����4�� \Y ����
	strTime.Format(_T("%d"),cTime.GetYear());
	strTmp.Replace(_T("\\Y"),strTime);
	//2000 ����4�� \0Y ����
	strTime.Format(_T("%04d"),cTime.GetYear());
	strTmp.Replace(_T("\\0Y"),strTime);

	//00 ����2�� \y ����2��
	strTime.Format(_T("%d"),cTime.GetYear() - 2000);
	strTmp.Replace(_T("\\y"),strTime);
	//00 ����2�� \y ����2��
	strTime.Format(_T("%02d"),cTime.GetYear() - 2000);
	strTmp.Replace(_T("\\0y"),strTime);

	//May ���p��\�L \b ���p��ȗ�
	if(strTmp.Find(_T("\\e")) != -1) {
		CString strMonth[12];
		strMonth[0] = _T("Jan"); 
		strMonth[1] = _T("Feb"); 
		strMonth[2] = _T("Mar"); 
		strMonth[3] = _T("Apr"); 
		strMonth[4] = _T("May"); 
		strMonth[5] = _T("Jun"); 
		strMonth[6] = _T("Jul"); 
		strMonth[7] = _T("Aug"); 
		strMonth[8] = _T("Sep"); 
		strMonth[9] = _T("Oct"); 
		strMonth[10] = _T("Nov");
		strMonth[11] = _T("Dec");
		strTmp.Replace(_T("\\e"),strMonth[cTime.GetMonth() - 1]);
	}
	//May ���p��\�L \B ���p��
	if(strTmp.Find(_T("\\E")) != -1) {
		CString strMonth[12];
		strMonth[0] = _T("January"); 
		strMonth[1] = _T("February"); 
		strMonth[2] = _T("March"); 
		strMonth[3] = _T("April"); 
		strMonth[4] = _T("May"); 
		strMonth[5] = _T("June"); 
		strMonth[6] = _T("July"); 
		strMonth[7] = _T("August"); 
		strMonth[8] = _T("September"); 
		strMonth[9] = _T("October"); 
		strMonth[10] = _T("November");
		strMonth[11] = _T("December");
		strTmp.Replace(_T("\\E"),strMonth[cTime.GetMonth() - 1]);
	}

	//��2�� \0m ��01
	strTime.Format(_T("%02d"),cTime.GetMonth());
	strTmp.Replace(_T("\\0M"),strTime);
	//��2�� \m �� 1
	strTime.Format(_T("%d"),cTime.GetMonth());
	strTmp.Replace(_T("\\M"),strTime);

	//06 ���t����2�� \0d ���t 01
	strTime.Format(_T("%02d"),cTime.GetDay());
	strTmp.Replace(_T("\\0d"),strTime);
	//6 ���t����2�� \d ���t 6
	strTime.Format(_T("%d"),cTime.GetDay());
	strTmp.Replace(_T("\\d"),strTime);

	//Saturday �j���p�� \A �j���p��
	if(strTmp.Find(_T("\\A")) != -1) {
		CString strYoubi[7];
		strYoubi[0] = _T("SunDay"); 
		strYoubi[1] = _T("MonDay"); 
		strYoubi[2] = _T("TuesDay"); 
		strYoubi[3] = _T("WednesDay"); 
		strYoubi[4] = _T("ThursDay"); 
		strYoubi[5] = _T("FriDay"); 
		strYoubi[6] = _T("SaturDay"); 
		strTmp.Replace(_T("\\A"),strYoubi[cTime.GetDayOfWeek() - 1]);
	}

	//Sat �j���p��ȈՕ\�L \a �j���p��ȗ�
	if(strTmp.Find(_T("\\a")) != -1) {
		CString strYoubi[7];
		strYoubi[0] = _T("Sun"); 
		strYoubi[1] = _T("Mon"); 
		strYoubi[2] = _T("Tue"); 
		strYoubi[3] = _T("Wed"); 
		strYoubi[4] = _T("Thu"); 
		strYoubi[5] = _T("Fri"); 
		strYoubi[6] = _T("Sat"); 
		strTmp.Replace(_T("\\a"),strYoubi[cTime.GetDayOfWeek() - 1]);
	}

	//�y�j �j�� \w �j��
	if(strTmp.Find(_T("\\w")) != -1) {
		CString strYoubi[7];
		strYoubi[0] = _T("��"); 
		strYoubi[1] = _T("��"); 
		strYoubi[2] = _T("��"); 
		strYoubi[3] = _T("��"); 
		strYoubi[4] = _T("��"); 
		strYoubi[5] = _T("��"); 
		strYoubi[6] = _T("�y"); 
		strTmp.Replace(_T("\\w"),strYoubi[cTime.GetDayOfWeek() - 1]);
	}

	//21 ����24���ԕ\�L \H ����24
	strTime.Format(_T("%02d"),cTime.GetHour());
	strTmp.Replace(_T("\\0H"),strTime);
	strTime.Format(_T("%d"),cTime.GetHour());
	strTmp.Replace(_T("\\H"),strTime);

	//09 ���� \I ����12
	if(strTmp.Find(_T("\\h")) != -1) {
		int nHours = cTime.GetHour();
		if(nHours > 11) nHours -= 12;
		strTime.Format(_T("%d"),nHours);
		strTmp.Replace(_T("\\h"),strTime);
	}
	if(strTmp.Find(_T("\\0h")) != -1) {
		int nHours = cTime.GetHour();
		if(nHours > 11) nHours -= 12;
		strTime.Format(_T("%02d"),nHours);
		strTmp.Replace(_T("\\0h"),strTime);
	}

	//46 �� \M ��00
	strTime.Format(_T("%d"),cTime.GetMinute());
	strTmp.Replace(_T("\\m"),strTime);
	strTime.Format(_T("%02d"),cTime.GetMinute());
	strTmp.Replace(_T("\\0m"),strTime);

	//02 �b \S �b
	strTime.Format(_T("%02d"),cTime.GetSecond());
	strTmp.Replace(_T("\\0s"),strTime);
	strTime.Format(_T("%d"),cTime.GetSecond());
	strTmp.Replace(_T("\\s"),strTime);

	//PM \p AM
	int nHours = cTime.GetHour();
	if(nHours > 11) strAMPM = _T("PM");
	else			strAMPM = _T("AM");
	strTmp.Replace(_T("\\P"),strAMPM);

	nHours = cTime.GetHour();
	if(nHours > 11) strAMPM = _T("�ߌ�");
	else			strAMPM = _T("�ߑO");
	strTmp.Replace(_T("\\G"),strAMPM);

	if(strTmp.GetLength() <= nSize) {
		_tcscpy(szRet,LPCTSTR(strTmp));
	}
	isRet = true;
	return isRet;
}
