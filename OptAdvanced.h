/*----------------------------------------------------------
	Setting Dialog - Advanced
----------------------------------------------------------*/

#if !defined(AFX_OPTADVANCED_H__0D4DFA1E_8391_4DA0_A8C2_1C86D2C0C460__INCLUDED_)
#define AFX_OPTADVANCED_H__0D4DFA1E_8391_4DA0_A8C2_1C86D2C0C460__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptAdvanced.h : �w�b�_�[ �t�@�C��
#include "resource.h"

#include <afxwin.h>

//---------------------------------------------------
// COptAdvanced �_�C�A���O
//---------------------------------------------------
class COptAdvanced : public CDialog
{
// �R���X�g���N�V����
public:
	COptAdvanced(CWnd* pParent = NULL) : CDialog(IDD, pParent) {}

	// �_�C�A���O �f�[�^
	//{{AFX_DATA(COptAdvanced)
	enum { IDD = IDD_SETTINGS_06_ADVANCED };
	//}}AFX_DATA

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(COptAdvanced)

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(COptAdvanced)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_OPTADVANCED_H__0D4DFA1E_8391_4DA0_A8C2_1C86D2C0C460__INCLUDED_)
