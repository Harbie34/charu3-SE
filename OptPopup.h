/*----------------------------------------------------------
	�|�b�v�A�b�v�ݒ�N���X�w�b�_
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_OPTPOPUP_H__3B3D553D_3B12_401F_A02E_A6ED8AF583C8__INCLUDED_)
#define AFX_OPTPOPUP_H__3B3D553D_3B12_401F_A02E_A6ED8AF583C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptPopup.h : �w�b�_�[ �t�@�C��
#include "resource.h"

#include <afxwin.h>

//---------------------------------------------------
// COptPopup �_�C�A���O
//---------------------------------------------------
class COptPopup : public CDialog
{
// �R���X�g���N�V����
public:
	COptPopup(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(COptPopup)
	enum { IDD = IDD_OPT_POPUP };
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(COptPopup)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(COptPopup)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCbnSelchangeOptPopupPos();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int m_nSelectByTypingCaseInsensitive;
	int m_nSelectByTypingAutoPaste;
	int m_nSelectByTypingAutoExpand;
	int m_nScrollVertical;
	int m_nScrollHorizontal;
	int m_nSingleExpand;
	int m_nKeepSelection;
	int m_nKeepFolders;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_OPTPOPUP_H__3B3D553D_3B12_401F_A02E_A6ED8AF583C8__INCLUDED_)
