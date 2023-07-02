#if !defined(AFX_OPTKEYSETEDITDLG_H__A0D37959_215D_4FCE_856F_A470CFAB6F9A__INCLUDED_)
#define AFX_OPTKEYSETEDITDLG_H__A0D37959_215D_4FCE_856F_A470CFAB6F9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptKeySetEditDlg.h : �w�b�_�[ �t�@�C��
//
#include "resource.h"
#include "Init.h"
#include "MyHotkeyCtrl.h"

#include <afxwin.h>

/////////////////////////////////////////////////////////////////////////////
// COptKeySetEditDlg �_�C�A���O

class COptKeySetEditDlg : public CDialog
{
// �R���X�g���N�V����
public:
	COptKeySetEditDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(COptKeySetEditDlg)
	enum { IDD = IDD_OPT_ADD_KEYSET };
	CMyHotkeyCtrl	m_pasteKey;
	CMyHotkeyCtrl	m_copyKey;
	CString	m_pasteMessage;
	CString	m_copyMessage;
	int		m_keyAction;
	int		m_matchCombo;
	CString	m_caption;
	int m_copyWait;
	int m_pasteWait;
	int		m_nHistoryLimit;
	//}}AFX_DATA
	friend BOOL CALLBACK EnumWindowTitle(HWND hwnd, LPARAM lParam);

	void setKeyInfo(CHANGE_KEY keyInfo) {
		m_key = keyInfo;
	}
	CHANGE_KEY getKeyInfo(){
		return m_key;
	};
// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(COptKeySetEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	CHANGE_KEY m_key;

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(COptKeySetEditDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_OPTKEYSETEDITDLG_H__A0D37959_215D_4FCE_856F_A470CFAB6F9A__INCLUDED_)
