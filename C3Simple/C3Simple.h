// C3Simple.h : C3SIMPLE �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#if !defined(AFX_C3SIMPLE_H__476A0EC9_18E7_4BD5_9AB5_F0A87E921083__INCLUDED_)
#define AFX_C3SIMPLE_H__476A0EC9_18E7_4BD5_9AB5_F0A87E921083__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ���C�� �V���{��

/////////////////////////////////////////////////////////////////////////////
// CC3SimpleApp
// ���̃N���X�̓���̒�`�Ɋւ��Ă� C3Simple.cpp �t�@�C�����Q�Ƃ��Ă��������B
//

class CC3SimpleApp : public CWinApp
{
public:
	CC3SimpleApp();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CC3SimpleApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CC3SimpleApp)
		// ���� -  ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
		//         ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_C3SIMPLE_H__476A0EC9_18E7_4BD5_9AB5_F0A87E921083__INCLUDED_)
