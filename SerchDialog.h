/*----------------------------------------------------------
	Charu3
	�����_�C�A���O�w�b�_
----------------------------------------------------------*/
#if !defined(AFX_SERCHDIALOG_H__F054EA83_2A07_11D4_8AC7_0050DAC3512B__INCLUDED_)
#define AFX_SERCHDIALOG_H__F054EA83_2A07_11D4_8AC7_0050DAC3512B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SerchDialog.h : �w�b�_�[ �t�@�C��
//

//---------------------------------------------------
// CSerchDialog �_�C�A���O
//---------------------------------------------------
class CSearchDialog : public CDialog
{
// �R���X�g���N�V����
public:
	CSearchDialog(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CSerchDialog)
	enum { IDD = IDD_SEARCH };
	CEdit	m_ctrlSearchText;
	CButton m_ctrlSearchByName;
	CButton m_ctrlSearchByData;
	CButton m_ctrlSearchAnd;
	CButton m_ctrlSearchOr;
	CButton m_ctrlCaseInsensitive;
	//}}AFX_DATA

	int GetTarget();
	int GetSearchLogic();
	bool GetCaseInsensitive();
	CString GetSearchText();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CSerchDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CSerchDialog)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void OnFindNext();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SERCHDIALOG_H__F054EA83_2A07_11D4_8AC7_0050DAC3512B__INCLUDED_)
