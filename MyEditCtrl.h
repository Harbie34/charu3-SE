#if !defined(AFX_MYEDITCTRL_H__A57B3A10_CCBB_48D2_9A46_A4D36FE50D26__INCLUDED_)
#define AFX_MYEDITCTRL_H__A57B3A10_CCBB_48D2_9A46_A4D36FE50D26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyEditCtrl.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CMyEditCtrl �E�B���h�E

class CMyEditCtrl : public CEdit
{
// �R���X�g���N�V����
public:
	CMyEditCtrl();

// �A�g���r���[�g
public:

// �I�y���[�V����
public:

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CMyEditCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	virtual ~CMyEditCtrl();

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(CMyEditCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_MYEDITCTRL_H__A57B3A10_CCBB_48D2_9A46_A4D36FE50D26__INCLUDED_)
