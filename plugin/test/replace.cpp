/*----------------------------------------------------------
	�u��
									2001/08/04 (c)C+ Factory
----------------------------------------------------------*/

#include "stdafx.h"

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
	_tcscpy(strResult,_T("<charuMACRO>$PLUG-IN<.\\plugin\\LineMod.dll,,$CLIP,,>,,,,,1,,0,$SEL>$PLUG-IN</charuMACRO>"));	
	//---------------�����܂ŃR�[�h�������Ƃ����ł�---------------------
	isRet = true;
	return isRet;
}


