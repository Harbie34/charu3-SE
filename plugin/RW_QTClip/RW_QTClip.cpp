
/*----------------------------------------------------------
QTClip�p�f�[�^�ǂݏ���DLL
									2002/09/08 (c)C+ Factory
----------------------------------------------------------*/

#include "stdafx.h"
#include "RW_Plugin.h"

#pragma data_seg(".shared")
//�A�v���P�[�V�������Ɗg�������w��
const TCHAR *szName = _T("QTClip");
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

bool isSJIS(char *szSource)
{
	bool isRet = false;
	
	if(((unsigned char)*szSource >= 0x80 && (unsigned char)*szSource <= 0x9f) ||
		((unsigned char)*szSource >= 0xe0 && (unsigned char)*szSource <= 0xff)) 
//		if(((unsigned char)*(szSource + 1) >= 0x40 && (unsigned char)*(szSource + 1) <= 0x7e) ||
//			((unsigned char)*szSource >= 0x80 && (unsigned char)*szSource <= 0xfc)) 
			isRet = true;

	return isRet;
}

void convQTString(CString strSource,CString *strName,CString *strData);

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

	//�f�[�^�ǂݍ���
	list<STRING_DATA> lstFolder;

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
	FILE *InFile;
	char strBuff[65535];
	CString strText,strRirekiFileName;
	
	//�e�L�X�g�f�[�^�t�@�C�����J��
	if((InFile = _tfopen(strFileName,_T("r"))) == NULL)	return NULL;

	lstFolder.clear();
	while(!feof(InFile)) {
		if(!fgets(strBuff,sizeof(strBuff),InFile)) break;;//�ǂ�
		strText = strBuff;
		strText.TrimRight();
		if(strText.Left(1) == _T("	")) continue;
		if(strText.Left(1) == _T(">")) {//�t�H���_
			Data.m_cKIND = KIND_FOLDER;//���
			if(lstFolder.size() == 0)	Data.m_nParentID = ROOT;
			else						Data.m_nParentID = (--lstFolder.end())->m_nMyID;
			Data.m_nMyID = nID++;

			strText.Delete(0,1);
			Data.m_strTitle = strText;
			Data.m_strData = _T("");
			pData->insert(pData->end(),Data);
			lstFolder.insert(lstFolder.end(),Data);
		}
		else if(strText.Left(1) == _T("<")) {//�t�H���_�I���
			if(lstFolder.size() > 0) lstFolder.erase(--lstFolder.end());
		}
		else if(strText.Left(2) == _T("*C")) {//�����t�H���_
			Data.m_nParentID = -1;
			Data.m_cKIND = KIND_RIREKI;//���
			Data.m_nMyID = nID++;

			strText.Replace(_T("	"),_T(""));
			Data.m_strTitle = strText;
			Data.m_strMacro = _T("count=100\r\n");
			pData->insert(pData->end(),Data);

		}
		else if(strText == _T("-")) {//�Z�p���[�^�[
			Data.m_cKIND = KIND_LOCK;//���
			if(lstFolder.size() == 0)	Data.m_nParentID = ROOT;
			else							Data.m_nParentID = (--lstFolder.end())->m_nMyID;
			Data.m_nMyID = nID++;

			Data.m_strTitle = _T("������������������");
			Data.m_strData = _T("");
			pData->insert(pData->end(),Data);
		}
		else if(strText.Left(2) == _T("*E") || strText.Left(3) == _T("*EX")){
			Data.m_cKIND = KIND_LOCK;//���
			if(lstFolder.size() == 0)	Data.m_nParentID = ROOT;
			else						Data.m_nParentID = (--lstFolder.end())->m_nMyID;
			Data.m_nMyID = nID++;

			Data.m_strTitle = strText;
			strText.Replace(_T("\\n"),_T("\n"));
			Data.m_strData = strText;
			pData->insert(pData->end(),Data);
		}
		else if(strText.Left(2) == _T("*T") || strText.Left(2) == _T("*P")) {//���̍ۖ����̕�����
			continue;
		}
		else if(strText.Left(1) == _T("*")){//�܂Ƃ߂č��ڂ̏ꍇ
			strText.Delete(0,2);
			char *szBuff;
			char str2Byte[3];

			szBuff = strBuff;
			if(strlen(szBuff) < 3) continue;
			szBuff += 2;
			szBuff[strlen(szBuff)-1] = NULL;
			Data.m_cKIND = KIND_LOCK;//���
			while(*szBuff) {
				if(isSJIS(szBuff)) {//2�o�C�g�����̏ꍇ
					strncpy(str2Byte,szBuff,2);
					str2Byte[2] = NULL;
					szBuff += 2;
				}
				else {//1�o�C�g�̏ꍇ
					strncpy(str2Byte,szBuff,1);
					str2Byte[1] = NULL;
					szBuff++;
				}
				if(lstFolder.size() == 0)	Data.m_nParentID = ROOT;
				else						Data.m_nParentID = (--lstFolder.end())->m_nMyID;
				Data.m_nMyID = nID++;
				Data.m_strTitle = str2Byte;
				Data.m_strData = str2Byte;
				pData->insert(pData->end(),Data);
			}
		}
		else if(strText != ""){
			Data.m_cKIND = KIND_LOCK;//���
			if(lstFolder.size() == 0)	Data.m_nParentID = ROOT;
			else						Data.m_nParentID = (--lstFolder.end())->m_nMyID;
			Data.m_nMyID = nID++;
			convQTString(strText,&Data.m_strTitle,&Data.m_strData);
			strText.Replace(_T("\\n"),_T("\n"));
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
	return true;
}


void convQTString(CString strSource,CString *strName,CString *strData)
{

	int nTab = strSource.Find(_T("	"),0);

	if(nTab > 0) {
		*strName = strSource.Left(nTab);
		*strData = strSource.Mid(nTab+1,strSource.GetLength() - nTab);
		strData->Replace(_T("	"),_T(""));
		strData->Replace(_T("\\t"),_T("	"));
		strData->Replace(_T("\\n"),_T("\n"));
		strData->Replace(_T("\\\\"),_T("\\"));
	}
	else {
		*strName = strSource;
		*strData = strSource;
	}
}

//---------------------------------------------------
//�֐���	convertMacro(char *szSource,char *szRet,int nSize,char *szSelect,char *szClip)
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
	strTime.Format(_T("%04d"),cTime.GetYear());
	strTmp.Replace(_T("\\Y"),strTime);

	//00 ����2�� \y ����2��
	strTime.Format(_T("%02d"),cTime.GetYear() - 2000);
	strTmp.Replace(_T("\\y"),strTime);

	//00 �a��2�� \g ����
	strTime.Format(_T("���� %02d"),cTime.GetYear() - 1988);
	strTmp.Replace(_T("\\g"),strTime);

	//May ���p��\�L \b ���p��ȗ�
	if(strTmp.Find(_T("\\b")) != -1) {
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
		strTmp.Replace(_T("\\b"),strMonth[cTime.GetMonth() - 1]);
	}
	//May ���p��\�L \B ���p��
	if(strTmp.Find(_T("\\B")) != -1) {
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
		strTmp.Replace(_T("\\B"),strMonth[cTime.GetMonth() - 1]);
	}

	//��2�� \0m ��01
	strTime.Format(_T("%02d"),cTime.GetMonth());
	strTmp.Replace(_T("\\0m"),strTime);
	//��2�� \m �� 1
	strTime.Format(_T("%d"),cTime.GetMonth());
	strTmp.Replace(_T("\\m"),strTime);

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
	strTmp.Replace(_T("\\H"),strTime);

	//09 ���� \I ����12
	if(strTmp.Find(_T("\\I")) != -1) {
		int nHours = cTime.GetHour();
		if(nHours > 11) nHours -= 12;
		strTime.Format(_T("%02d"),nHours);
		strTmp.Replace(_T("\\I"),strTime);
	}

	//46 �� \M ��00
	strTime.Format(_T("%02d"),cTime.GetMinute());
	strTmp.Replace(_T("\\M"),strTime);

	//02 �b \S �b
	strTime.Format(_T("%02d"),cTime.GetSecond());
	strTmp.Replace(_T("\\S"),strTime);

	//PM \p AM
	int nHours = cTime.GetHour();
	if(nHours > 11) strAMPM = _T("PM");
	else			strAMPM = _T("AM");
	strTmp.Replace(_T("\\p"),strAMPM);

	//\r ����12 10:10:10 AM
	if(strTmp.Find(_T("\\r")) != -1) {
		int nHours = cTime.GetHour();
		if(nHours > 11) nHours -= 12;
		strTime.Format(_T("%02d:%02d:%02d %s"),nHours,cTime.GetMinute(),cTime.GetSecond(),strAMPM);
		strTmp.Replace(_T("\\r"),strTime);
	}
	//\X 23:59:00
	if(strTmp.Find(_T("\\X")) != -1) {
		strTime.Format(_T("%02d:%02d:%02d"),cTime.GetHour(),cTime.GetMinute(),cTime.GetSecond());
		strTmp.Replace(_T("\\X"),strTime);
	}

	//\0x ���t2001/01/10
	if(strTmp.Find(_T("\\0x")) != -1) {
		strTime.Format(_T("%04d/%02d/%02d"),cTime.GetYear(),cTime.GetMonth(),cTime.GetDay());
		strTmp.Replace(_T("\\0x"),strTime);
	}
	//\x ���t2001/ 1/10
	if(strTmp.Find(_T("\\x")) != -1) {
		strTime.Format(_T("%4d/%2d/%2d"),cTime.GetYear(),cTime.GetMonth(),cTime.GetDay());
		strTmp.Replace(_T("\\x"),strTime);
	}

	if(strTmp.GetLength() <= nSize) {
		_tcscpy(szRet,LPCTSTR(strTmp));
	}
	isRet = true;
	return isRet;
}
