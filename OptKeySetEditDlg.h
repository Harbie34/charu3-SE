#if !defined(AFX_OPTKEYSETEDITDLG_H__A0D37959_215D_4FCE_856F_A470CFAB6F9A__INCLUDED_)
#define AFX_OPTKEYSETEDITDLG_H__A0D37959_215D_4FCE_856F_A470CFAB6F9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

#include "MyHotkeyCtrl.h"
#include "Init.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// COptKeySetEditDlg �_�C�A���O

class COptKeySetEditDlg : public CDialog
{
    // �R���X�g���N�V����
public:
    COptKeySetEditDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

    // �_�C�A���O �f�[�^
private:
    //{{AFX_DATA(COptKeySetEditDlg)
    enum { IDD = IDD_KEYSET_EXT };
    CMyHotkeyCtrl m_pasteKey;
    CMyHotkeyCtrl m_copyKey;
    //}}AFX_DATA

    // �I�[�o�[���C�h
    // ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
protected:
    //{{AFX_VIRTUAL(COptKeySetEditDlg)
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
    //}}AFX_VIRTUAL

    // �C���v�������e�[�V����
public:
    void SetKeyInfo(CHANGE_KEY keyInfo) { m_key = keyInfo; }
    CHANGE_KEY GetKeyInfo() const { return m_key; };

    CString m_pasteMessage;
    CString m_copyMessage;
    int m_keyAction;
    int m_matchCombo;
    CString m_caption;
    int m_copyWait;
    int m_pasteWait;
    int m_nHistoryLimit;

private:
    CHANGE_KEY m_key;

    // �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
    //{{AFX_MSG(COptKeySetEditDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_OPTKEYSETEDITDLG_H__A0D37959_215D_4FCE_856F_A470CFAB6F9A__INCLUDED_)
