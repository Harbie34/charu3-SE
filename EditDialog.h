/*----------------------------------------------------------
	EditDialog�N���X�w�b�_
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_EDITDIALOG_H__B4B68363_82C1_4D62_AC66_00D698DF05A4__INCLUDED_)
#define AFX_EDITDIALOG_H__B4B68363_82C1_4D62_AC66_00D698DF05A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditDialog.h : �w�b�_�[ �t�@�C��
//
#include "MyEditCtrl.h"
//---------------------------------------------------
// CEditDialog �_�C�A���O
//---------------------------------------------------
class CEditDialog : public CDialog
{
// �R���X�g���N�V����
public:
	CEditDialog(CWnd* pParent,STRING_DATA data);   // �W���̃R���X�g���N�^
	STRING_DATA m_data;
	void setMacroTempate(vector<MACRO_STRUCT> *pMacro,vector<MACRO_STRUCT> *pDataMacro){
		m_vctMacro = pMacro;
		m_vctDataMacro = pDataMacro;
	}
// �_�C�A���O �f�[�^
	//{{AFX_DATA(CEditDialog)
	enum { IDD = IDD_EDIT };
	CMyEditCtrl	m_ctrlDataEdit;
	CMyEditCtrl	m_ctrlMacroEdit;
	CComboBox	m_ctrlKindCombo;
	CComboBox	m_ctrlIconCombo;
	CButton	m_ctrlRirekiChk;
	CComboBox	m_ctrlMacro;
	CComboBox	m_ctrlDataMacro;
	CString	m_strDataName;
	CString	m_strDataMacro;
	CString	m_strData;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CEditDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	vector<MACRO_STRUCT>  *m_vctMacro;
	vector<MACRO_STRUCT>  *m_vctDataMacro;
	void pasteMacro(int nCtrlID,CString strString);
	CFont *m_cFontEdit,*m_cOlgFontEdit;
	CFont *m_cFontTitle,*m_cOlgFontTitle;

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CEditDialog)
	afx_msg void OnEditPasteFile();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeEditMacroCombo();
	afx_msg void OnSelchangeEditDataMacroCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_EDITDIALOG_H__B4B68363_82C1_4D62_AC66_00D698DF05A4__INCLUDED_)
