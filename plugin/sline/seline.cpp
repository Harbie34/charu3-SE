/*----------------------------------------------------------
	�s�I��
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

void getLine(TCHAR *strResult,TCHAR *strSrc);

extern "C" __declspec (dllexport) bool CharuPlugIn
	(TCHAR *strSource,TCHAR *strResult,int nSize,STRING_DATA *data,void *pVoid)
{
	bool isRet = false;
	TCHAR *strTmp,*strTmp2;
	int nLine;
	strTmp = new TCHAR[nSize];
	strTmp2 = strTmp;
	_stscanf(strSource,_T("%d,%s"),&nLine,strTmp);
	nLine--;
	for(;*strSource != NULL; strSource++) {
		//�s��؂�o��
		if(*strSource == ','){
			strSource++;
			break;
		}
	}
	_tcscpy(strTmp,strSource);	
	*strResult = NULL;
	for(;*strTmp != NULL; strTmp++) {
		//�s��؂�o��
		if(nLine <= 0){
			getLine(strResult,strTmp);
			break;
		}
		if(*strTmp == 0x0a) nLine--;
	}
	delete [] strTmp2;	
	isRet = true;
	return isRet;
}

void getLine(TCHAR *strResult,TCHAR *strSrc)
{
	for(;*strSrc != NULL; strSrc++) {
		//�s��؂�o��
		if(*strSrc == 0x0a) break;
		if(*strSrc != 0x0d){
			*strResult = *strSrc;//�R�s�[
			strResult++;
		}
	}
	*strResult = NULL;
}
