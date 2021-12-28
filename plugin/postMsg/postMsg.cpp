/*----------------------------------------------------------
	�e�L�X�g�F�X�ϊ�
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

struct FOCUS_INFO {
	HWND m_hActiveWnd;
	HWND m_hFocusWnd;
};

//---------------------------------------------------
//�֐���	CharuPlugIn
//�@�\		�v���O�C���֐�
//---------------------------------------------------
extern "C" __declspec (dllexport) bool CharuPlugIn
	(TCHAR *strSource,TCHAR *strResult,int nSize,STRING_DATA *data,void *pVoid)
{
	bool isRet = false;
	*strResult = NULL;
	DWORD wMsg,dwWParam,dwLParam;
	TCHAR cPost,cTarget;
	
	//---------------��������R�[�h�������Ƃ����ł�---------------------
	if(_stscanf(strSource,_T("%c,%c,%x,%x,%x"),&cPost,&cTarget,&wMsg,&dwWParam,&dwLParam) == 5) {
		FOCUS_INFO *focusInfo;
		focusInfo = (FOCUS_INFO*)pVoid;
		HWND hWnd;
		if(cTarget == _T('c'))	hWnd = focusInfo->m_hFocusWnd;
			else				hWnd = focusInfo->m_hActiveWnd;
		if(cPost == _T('s')) {
			::SendMessage(hWnd,wMsg,dwWParam,dwLParam);
		}
		else if(cPost == _T('p')) {
			::PostMessage(hWnd,wMsg,dwWParam,dwLParam);
		}
	}
	//---------------�����܂ŃR�[�h�������Ƃ����ł�---------------------
	isRet = true;
	return isRet;
}

