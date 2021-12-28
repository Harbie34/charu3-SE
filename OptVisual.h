/*----------------------------------------------------------
	�r�W���A���ݒ�N���X�w�b�_
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_OPTVISUAL_H__CE6482E8_6118_46DF_B05A_C898A00E9122__INCLUDED_)
#define AFX_OPTVISUAL_H__CE6482E8_6118_46DF_B05A_C898A00E9122__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptVisual.h : �w�b�_�[ �t�@�C��
//
#include "PaletStatic.h"

#define	MAX_FONT 256
//---------------------------------------------------
// COptVisual �_�C�A���O
//---------------------------------------------------
class COptVisual : public CDialog
{
// �R���X�g���N�V����
public:
	COptVisual(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^
	friend int CALLBACK EnumFontProc(ENUMLOGFONT *lpelf,NEWTEXTMETRIC *lpntm,int FontType,LPARAM _dwDmy);

// �_�C�A���O �f�[�^
	//{{AFX_DATA(COptVisual)
	enum { IDD = IDD_OPT_VISUAL };
	CPaletStatic m_ctrlBorderPal;
	CPaletStatic m_ctrlTextPal;
	CPaletStatic m_ctrlBackPal;
	CComboBox	m_ctrlFontCombo;
	CString	m_strBackColor;
	CString	m_strTextColor;
	CString	m_strBorderColor;
	int		m_n;
	int		m_nScrollH;
	int		m_nScrollV;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(COptVisual)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	void setPalet(CEdit *edit,CPaletStatic *stPal);
	void setTextToPalet(CEdit *edit,CPaletStatic *stPal);
	int m_nMaxFont; 

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(COptVisual)
	afx_msg void OnOptBackColorPal();
	afx_msg void OnOptBorderColorPal();
	afx_msg void OnOptTextColorPal();
	virtual BOOL OnInitDialog();
	afx_msg void OnOptVsBrows();
	afx_msg void OnChangeOptTextColor();
	afx_msg void OnChangeOptBorderColor();
	afx_msg void OnChangeOptBackColor();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_OPTVISUAL_H__CE6482E8_6118_46DF_B05A_C898A00E9122__INCLUDED_)
