/*----------------------------------------------------------
	�u��
									2003/03/24 (c)C+ Factory
----------------------------------------------------------*/

#include "stdafx.h"
#include "StringWork.h"

#include <list>
using namespace std;

//---------------------------------------------------
// �f�[�^�\����
//---------------------------------------------------
struct STRING_DATA
{
	char	m_cKind;		//�f�[�^���
	char	m_cIcon;		//�A�C�R�����
	int		m_nMyID;		//�f�[�^��ID
	int		m_nParentID;	//�e�f�[�^��ID
	time_t	m_timeCreate;	//�쐬����
	time_t	m_timeEdit;		//�ύX����
	CString  m_strTitle;	//�ݒ�f�[�^�^�C�g��
	CString  m_strData;		//�ݒ�f�[�^������
	CString  m_strMacro;	//�g���p������f�[�^
};

struct DIC_DATA
{
	CString strSrc;
	CString strDst;
};

extern "C" __declspec (dllexport) bool CharuPlugIn
	(TCHAR *strSource,TCHAR *strResult,int nSize,STRING_DATA *data,void *pVoid)
{

	bool isRet = false;
	*strResult = NULL;
	CString strTmp;
	strTmp = strSource;
	//---------------��������R�[�h�������Ƃ����ł�---------------------

	list<DIC_DATA> lstDic;
	FILE *InFile;
	TCHAR strBuff[65535];//,*szSplit1,*szSplit2;
	CString strFileName,strReadBuff,strSplit1,strSplit2;

	int nFound = strTmp.Find(_T(","));
	if(nFound >= 0 && nFound < 256 && nFound < strTmp.GetLength()) {
		strFileName = strTmp.Left(nFound);
		strTmp = strTmp.Right(strTmp.GetLength() - nFound - 1);

		//�����ǂݍ���
		DIC_DATA dicData;
		if((InFile = _tfopen(strFileName,_T("r"))) == NULL)	return isRet;
		while(!feof(InFile)) {
			if(!_fgetts(strBuff,sizeof(strBuff),InFile)) break;//�ǂ�
AfxMessageBox(strBuff);
			strReadBuff = strBuff;
			nFound = strReadBuff.Find(_T("\t"));
			strSplit1 = strReadBuff.Left(nFound); 
			strSplit2 = strReadBuff.Right(strReadBuff.GetLength() - nFound - 1);
			if(strSplit1 != "" && strSplit2 != "") {
//			if(UStringWork::splitString(strBuff,'\t',&szSplit1,&szSplit2,NULL) == 2) {
				strReadBuff = strSplit1;
				strReadBuff.Replace(_T("\\n"),_T("\n"));
				strReadBuff.Replace(_T("\\t"),_T("\t"));
				strReadBuff.Replace(_T("\\\\"),_T("\\"));
				dicData.strSrc = strReadBuff;
				strReadBuff = strSplit2;
				strReadBuff.Replace(_T("\n"),_T(""));
				strReadBuff.Replace(_T("\\n"),_T("\n"));
				strReadBuff.Replace(_T("\\t"),_T("\t"));
				strReadBuff.Replace(_T("\\\\"),_T("\\"));
				dicData.strDst = strReadBuff;
				lstDic.push_back(dicData);
			}
		}
		//�u������
		list<DIC_DATA>::iterator it;
		for(it = lstDic.begin(); lstDic.end() != it; it++) {
			strTmp.Replace(it->strSrc,it->strDst);
		}

		_tcscpy(strResult,strTmp.GetBuffer(strTmp.GetLength()));	
		isRet = true;
	}
	//---------------�����܂ŃR�[�h�������Ƃ����ł�---------------------
	return isRet;
}


