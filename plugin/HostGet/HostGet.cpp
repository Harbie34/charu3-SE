// HostGet.cpp : DLL �A�v���P�[�V�����p�̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

bool getHostName(TCHAR *pHostName,int nDstSize, TCHAR *pIpAddr)
{	
	struct hostent	*host;
	unsigned long	addr;
	bool isRet = true;
	char *szMBCS;

	
	#ifdef _UNICODE
	int nSize =_tcsclen(pIpAddr) + 1;
	szMBCS = new char[nSize];
	if(!szMBCS) return false;
	WideCharToMultiByte(CP_ACP,0,pIpAddr,nSize,szMBCS,nSize,"",NULL);
	addr = inet_addr(szMBCS);
	#else
	addr = inet_addr(pIpAddr);
	#endif

	host = gethostbyaddr((const char *)&addr, 4, 2);

	if (host == NULL) {
		_tcscpy(pHostName,_T("not found!"));
		isRet = false;
	}
	else {
		#ifdef _UNICODE
		MultiByteToWideChar(CP_ACP,0,host->h_name,strlen(host->h_name),pHostName,nDstSize);
		pHostName[strlen(host->h_name)] = NULL;
		#else
		_tcscpy(pHostName,host->h_name);
		#endif

	}

	#ifdef _UNICODE
	if(szMBCS) delete szMBCS;
	#endif

	return isRet;
}

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
	WSADATA		WsaData;		//�A�h���X���\����

	ZeroMemory(&WsaData, sizeof(WsaData));

	int			nRetValue;
	WORD		wRequired;

	wRequired = MAKEWORD(2,0);
	nRetValue = WSAStartup(wRequired,&WsaData);

	//�o�[�W����������
	if( WsaData.wVersion != wRequired || nRetValue != 0) {
		WSACleanup();
		return isRet;
	}	
	
	getHostName(strResult,nSize,strSource);
	
	WSACleanup();
	isRet = true;
	return isRet;
}

