/*----------------------------------------------------------
	SJIS��EUC�ɕϊ�
									2001/08/04 (c)C+ Factory
----------------------------------------------------------*/

#include "stdafx.h"

#define SJIS1(c)	((c) >= 0x81 && (c) <= 0x9F || (c) >= 0xE0 && (c) <= 0xF0)
#define SJIS2(c2)	((c2) >= 0x40 && (c2) <= 0xFC && (c2) != 0x7F)
void URLEncode(char *szSource,char *szResult,int nSize);

//---------------------------------------------------
// �f�[�^�\����
//---------------------------------------------------
struct STRING_DATA
{
	TCHAR	m_cKind;		//�f�[�^���
	TCHAR	m_cIcon;		//�A�C�R�����
	int		m_nMyID;		//�f�[�^��ID
	int		m_nParentID;	//�e�f�[�^��ID
	time_t	m_timeCreate;	//�쐬����
	time_t	m_timeEdit;		//�ύX����
	CString  m_strTitle;	//�ݒ�f�[�^�^�C�g��
	CString  m_strData;		//�ݒ�f�[�^������
	CString  m_strMacro;	//�g���p������f�[�^
};

//--------------------------------------------------
//�֐���	CharuPlugIn
//�@�\		�v���O�C���֐��{��
//--------------------------------------------------
extern "C" __declspec (dllexport) bool CharuPlugIn
	(TCHAR *strSource,TCHAR *strResult,int nSize,STRING_DATA *data,void *pVoid)
{
	bool isRet = false;
	*strResult = NULL;
	//---------------��������R�[�h�������Ƃ����ł�---------------------
	char *strTmpSource,*strTmpResult;
	int nLength,nBuffSize;
	nBuffSize = _tcslen(strSource);
	nBuffSize = (nBuffSize + 1) * sizeof(TCHAR); 

	strTmpSource = new char[nBuffSize];
	strTmpResult = new char[nBuffSize * 3];

	//Unicode��S-JIS�ɕϊ�����
	#ifdef _UNICODE
		ZeroMemory(strTmpSource,nBuffSize);
		nLength = ::WideCharToMultiByte(CP_ACP, 0, strSource,_tcsclen(strSource),NULL,0,NULL,NULL);
		::WideCharToMultiByte(CP_ACP,0,strSource,nLength,strTmpSource,nBuffSize,"",NULL);
	#else
		strcpy(strTmpSource,strSource);
	#endif
	
	//URL�G���R�[�h����
	URLEncode(strTmpSource,strTmpResult,nBuffSize * 3);

	//���ɂ��ǂ�	
	#ifdef _UNICODE
		ZeroMemory(strResult,nSize);
		nLength = MultiByteToWideChar(CP_ACP,0,strTmpResult,-1,strResult,nSize);
	#else
		strncpy(strResult,strTmpResult,nBuffSize * 3);
	#endif
	
	delete strTmpSource;
	delete strTmpResult;
		
	//---------------�����܂ŃR�[�h�������Ƃ����ł�---------------------
	isRet = true;
	return isRet;
}

//--------------------------------------------------
//�֐���	URLEncode(char *szSource,char *szResult,int nSize)
//�@�\		URL�ϊ�������
//--------------------------------------------------
void URLEncode(char *szSource,char *szResult,int nSize)
{
	ZeroMemory(szResult,nSize);
	char strHex[4];
	while(*szSource && nSize > 0) {
		sprintf(strHex,"%%%02x",(int)(*szSource) & 0xff);
		strcat(szResult,strHex);
		nSize-=3;
		szSource++;
	}
}

