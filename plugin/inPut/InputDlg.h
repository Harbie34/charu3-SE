#if !defined(AFX_INPUTDLG_H__FB55BF38_6D3D_455A_94DB_A1708708618D__INCLUDED_)
#define AFX_INPUTDLG_H__FB55BF38_6D3D_455A_94DB_A1708708618D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputDlg.h : �w�b�_�[ �t�@�C��
//
#include "resource.h"       // ���C�� �V���{��
#include "stdafx.h"
/////////////////////////////////////////////////////////////////////////////
// CInputDlg �_�C�A���O

class CInputDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CInputDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CInputDlg)
	enum { IDD = IDD_DIALOG };
	CString	m_strEdit;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CInputDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_INPUTDLG_H__FB55BF38_6D3D_455A_94DB_A1708708618D__INCLUDED_)
