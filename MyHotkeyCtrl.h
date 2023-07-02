#if !defined(AFX_MYHOTKEYCTRL_H__A6BC19D4_358F_498B_A7DF_CA317C017F1C__INCLUDED_)
#define AFX_MYHOTKEYCTRL_H__A6BC19D4_358F_498B_A7DF_CA317C017F1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyHotkeyCtrl.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CMyHotkeyCtrl �E�B���h�E

class CMyHotkeyCtrl : public CHotKeyCtrl
{
// �R���X�g���N�V����
public:
	CMyHotkeyCtrl();

// �A�g���r���[�g
public:

// �I�y���[�V����
public:
	void SetHotKey( WORD wVirtualKeyCode, WORD wModifiers ) {
		if(wVirtualKeyCode >= 0x60 || wVirtualKeyCode == 0x1d || wVirtualKeyCode == 0x1c || wVirtualKeyCode == 0x20)
			CHotKeyCtrl::SetHotKey(wVirtualKeyCode,wModifiers);
		else
			CHotKeyCtrl::SetHotKey(wVirtualKeyCode,wModifiers | HOTKEYF_EXT);
	};

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CMyHotkeyCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	virtual ~CMyHotkeyCtrl();

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(CMyHotkeyCtrl)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	WORD m_lastKey,m_mod;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_MYHOTKEYCTRL_H__A6BC19D4_358F_498B_A7DF_CA317C017F1C__INCLUDED_)
