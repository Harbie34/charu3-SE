/*----------------------------------------------------------
	AddDialog�N���X�w�b�_
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_ADDDIALOG_H__9B7892F6_DF6E_4022_8FFA_45EF89D77EC9__INCLUDED_)
#define AFX_ADDDIALOG_H__9B7892F6_DF6E_4022_8FFA_45EF89D77EC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

//---------------------------------------------------
// CAddDialog �_�C�A���O
//---------------------------------------------------
class CAddDialog : public CDialog
{
// �R���X�g���N�V����
public:
	CAddDialog(CWnd* pParent, STRING_DATA* pData, bool newData);

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CAddDialog)
	enum { IDD = IDD_ADD_DATA };
private:
	CComboBox	m_ctrlMacro;
	CComboBox	m_ctrlDataMacro;
	CComboBox	m_ctrlIcon;
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
	std::vector<MACRO_STRUCT>  *m_vctMacro;
	std::vector<MACRO_STRUCT>  *m_vctDataMacro;
	CFont *m_cFontEdit,*m_cOlgFontEdit;
	CFont *m_cFontTitle,*m_cOlgFontTitle;

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CAddDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPasteFile();
	afx_msg void OnSelchangeMacroCombo();
	afx_msg void OnSelchangeDataMacroCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void PasteMacro(int nCtrlID, CString strString);

	STRING_DATA* m_pData;
	bool m_bNewData;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_ADDDIALOG_H__9B7892F6_DF6E_4022_8FFA_45EF89D77EC9__INCLUDED_)
