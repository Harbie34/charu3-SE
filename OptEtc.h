/*----------------------------------------------------------
	��ʐݒ�N���X�w�b�_
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_OPTETC_H__0D4DFA1E_8391_4DA0_A8C2_1C86D2C0C460__INCLUDED_)
#define AFX_OPTETC_H__0D4DFA1E_8391_4DA0_A8C2_1C86D2C0C460__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptEtc.h : �w�b�_�[ �t�@�C��
#include "resource.h"
#include "MyHotkeyCtrl.h"

#include <afxwin.h>

//---------------------------------------------------
// COptEtc �_�C�A���O
//---------------------------------------------------
class COptEtc : public CDialog
{
// �R���X�g���N�V����
public:
	COptEtc(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(COptEtc)
	enum { IDD = IDD_SETTINGS_01_GENERAL };
	CMyHotkeyCtrl	m_ctrlFifoKey;
	CMyHotkeyCtrl	m_ctrlPopupKey;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(COptEtc)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(COptEtc)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void hotkeyEnablePOP();
	afx_msg void hotkeyDisablePOP();
	afx_msg void hotkeyEnableFIFO();
	afx_msg void hotkeyDisableFIFO();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int m_nPutBackClipboard;
	int m_nShowClipboardInNotifyIconTooltip;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_OPTETC_H__0D4DFA1E_8391_4DA0_A8C2_1C86D2C0C460__INCLUDED_)
