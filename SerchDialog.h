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
	int m_nSearchByName;
	int m_nSearchByData;
	int m_nSearchLogic;
	CString	m_strSearchKeywords;
	//}}AFX_DATA

	int GetTarget();
	int GetSearchLogic();

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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SERCHDIALOG_H__F054EA83_2A07_11D4_8AC7_0050DAC3512B__INCLUDED_)
