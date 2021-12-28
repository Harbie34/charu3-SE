/*----------------------------------------------------------
	AddDialog�N���X�w�b�_
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_ADDDIALOG_H__9B7892F6_DF6E_4022_8FFA_45EF89D77EC9__INCLUDED_)
#define AFX_ADDDIALOG_H__9B7892F6_DF6E_4022_8FFA_45EF89D77EC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Charu3.h"
#include "Charu3Tree.h"
#include <list>
using namespace std;

//---------------------------------------------------
// CAddDialog �_�C�A���O
//---------------------------------------------------
class CAddDialog : public CDialog
{
// �R���X�g���N�V����
public:
	CAddDialog(CWnd* pParent,HTREEITEM hTreeItem);   // �W���̃R���X�g���N�^
	void setMacroTempate(vector<MACRO_STRUCT> *pMacro,vector<MACRO_STRUCT> *pDataMacro){
		m_vctMacro = pMacro;
		m_vctDataMacro = pDataMacro;
	}

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CAddDialog)
	enum { IDD = IDD_ADD_DATA };
	CComboBox	m_ctrlDataMacro;
	CComboBox	m_ctrlMacro;
	CComboBox	m_comboIcon;
	//}}AFX_DATA

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CAddDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	HTREEITEM m_hTreeItem;
	vector<MACRO_STRUCT>  *m_vctMacro;
	vector<MACRO_STRUCT>  *m_vctDataMacro;
	void pasteMacro(int nCtrlID,CString strString);
	CFont *m_cFontEdit,*m_cOlgFontEdit;
	CFont *m_cFontTitle,*m_cOlgFontTitle;
	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CAddDialog)
	afx_msg void OnAddOk();
	afx_msg void OnAddClose();
	afx_msg void OnAddPasteFile();
	afx_msg void OnMacroPaste();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAddMacroCombo();
	afx_msg void OnSelchangeEditDataMacroCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_ADDDIALOG_H__9B7892F6_DF6E_4022_8FFA_45EF89D77EC9__INCLUDED_)
