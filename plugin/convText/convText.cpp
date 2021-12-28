/*----------------------------------------------------------
	�e�L�X�g�F�X�ϊ�
									2001/08/04 (c)C+ Factory
----------------------------------------------------------*/

#include "stdafx.h"
#include "..\PsTxCvtL115\PsTxCvtL.h"

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

extern "C" __declspec (dllexport) bool CharuPlugIn
	(TCHAR *strSource,TCHAR *strResult,int nSize,STRING_DATA *data,void *pVoid)
{
	bool isRet = false;
	*strResult = NULL;
	
	//---------------��������R�[�h�������Ƃ����ł�---------------------
	int outFile;
	char *strTmp,*strTmp2,*szCommma;;
	int nType;
	strTmp = new char[nSize];
	int nLength;

	#ifdef _UNICODE
		ZeroMemory(strTmp,sizeof(strTmp));
		nLength = ::WideCharToMultiByte(CP_ACP, 0, strSource,_tcsclen(strSource),NULL,0,NULL,NULL);
		::WideCharToMultiByte(CP_ACP,0,strSource,nLength,strTmp,nSize,"",NULL);
	#else
		strcpy(strTmp,strSource);
	#endif

	if(sscanf(strTmp,"%d,%s",&nType) != 2) {
		delete [] strTmp;
		return isRet;
	}
	bool isCamma = false;
	szCommma = strTmp;
	strTmp2 = strTmp;
	for(;*szCommma != NULL; szCommma++) {
		if(isCamma) {
			*strTmp = *szCommma;
			strTmp++;
		}
		if(!isCamma && *szCommma == ','){
			isCamma = true;
		}
	}
	*strTmp = NULL;
	strTmp = strTmp2;
	
	if(nType <111 || nType > 416) {
		delete [] strTmp;
		AfxMessageBox(_T("�ϊ��^�C�v���s���ł��B"));
		return isRet;
	}

	if((outFile = _topen(_T("convTempIn.$$$"), _O_CREAT | _O_BINARY | _O_WRONLY)) == -1) {
		delete [] strTmp;
		AfxMessageBox(_T("�t�@�C��I/O�G���[ create"));
		return isRet;
	}
	nLength = strlen(strTmp);
	_write(outFile,strTmp,nLength);
	close(outFile);
	
	Pfc_TxCvt("convTempIn.$$$","convTempOut.$$$",nType);

	CFile fileIn;
	try {
		fileIn.Open(_T("convTempOut.$$$"),CFile::modeRead);
	}
	catch (CFileException *e){
		delete [] strTmp;
		AfxMessageBox(_T("�t�@�C��I/O�G���[ read"));
		e->Delete();
		return isRet;
	}
	#ifdef _UNICODE
		fileIn.Read(strTmp,fileIn.GetLength());
		strTmp[fileIn.GetLength()] = NULL;
		ZeroMemory(strResult,sizeof(strResult));
		nLength = MultiByteToWideChar(CP_ACP,0,strTmp,-1,strResult,nSize);
	#else
		fileIn.Read(strResult,fileIn.GetLength());
		strResult[fileIn.GetLength()] = NULL;
	#endif

	fileIn.Close();

	SetFileAttributes(_T("convTempIn.$$$"),0x20);
	SetFileAttributes(_T("convTempOut.$$$"),0x20);
	DeleteFile(_T("convTempIn.$$$"));
	DeleteFile(_T("convTempOut.$$$"));
	delete [] strTmp;
	//---------------�����܂ŃR�[�h�������Ƃ����ł�---------------------
	isRet = true;
	return isRet;
}

