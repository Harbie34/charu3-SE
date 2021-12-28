/*----------------------------------------------------------
	���p���t���v���O�C��
									2001/08/15 (c)C+ Factory
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
	CString strTmp,strQuotation;
	strTmp = strSource;

	int nFound = strTmp.Find(_T(","));
	if(nFound >= 0 && nFound < 256 && nFound < strTmp.GetLength()) {
		strQuotation = strTmp.Left(nFound);
		strTmp = strTmp.Right(strTmp.GetLength() - nFound - 1);
	
		strTmp = strQuotation + strTmp;
		strTmp.Replace(_T("\n"),_T("\n") + strQuotation);
		_tcscpy(strResult,strTmp.GetBuffer(strTmp.GetLength()+1));
	}
	//---------------�����܂ŃR�[�h�������Ƃ����ł�---------------------
	isRet = true;
	return isRet;
}

