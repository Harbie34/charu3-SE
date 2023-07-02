/*----------------------------------------------------------
	�ݒ胁�C���N���X�w�b�_
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_OPTMAINDIALOG_H__46D08C0C_FB56_4508_833C_C6377E38BFE2__INCLUDED_)
#define AFX_OPTMAINDIALOG_H__46D08C0C_FB56_4508_833C_C6377E38BFE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptMainDialog.h : �w�b�_�[ �t�@�C��
#include "OptEtc.h"
#include "OptVisual.h"
#include "OptPopup.h"
#include "OptFifo.h"
#include "OptKeySet.h"
#include "OptAdvanced.h"

#include <afxwin.h>

#define MAX_OPT_PAGE 5

//---------------------------------------------------
// COptMainDialog �_�C�A���O
//---------------------------------------------------
class COptMainDialog : public CDialog
{
// �R���X�g���N�V����
public:
	COptMainDialog(CWnd* pParent = NULL,int nPage = 0);   // �W���̃R���X�g���N�^

	COptEtc m_EtcPage;
	COptVisual m_VisualPage;
	COptPopup m_PopupPage;
	COptFifo m_FifoPage;
	COptKeySet m_KeysetPage;
	COptAdvanced m_AdvancedPage;

// �_�C�A���O �f�[�^
	//{{AFX_DATA(COptMainDialog)
	enum { IDD = IDD_SETTINGS };
	CTabCtrl	m_ctrlTab;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(COptMainDialog)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	CDialog *m_OptionPage[6];
	int m_nPage;

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(COptMainDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeOptTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeydownOptTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_OPTMAINDIALOG_H__46D08C0C_FB56_4508_833C_C6377E38BFE2__INCLUDED_)
