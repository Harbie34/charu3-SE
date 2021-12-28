// input.h : INPUT �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#if !defined(AFX_INPUT_H__24765F61_3A2C_11D6_B778_0004AC583E0D__INCLUDED_)
#define AFX_INPUT_H__24765F61_3A2C_11D6_B778_0004AC583E0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ���C�� �V���{��

/////////////////////////////////////////////////////////////////////////////
// CInputApp
// ���̃N���X�̓���̒�`�Ɋւ��Ă� input.cpp �t�@�C�����Q�Ƃ��Ă��������B
//
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

class CInputApp : public CWinApp
{
public:
	CInputApp();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CInputApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CInputApp)
		// ���� -  ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
		//         ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_INPUT_H__24765F61_3A2C_11D6_B778_0004AC583E0D__INCLUDED_)
