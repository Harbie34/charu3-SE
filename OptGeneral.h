/*----------------------------------------------------------
    ��ʐݒ�N���X�w�b�_
                                    2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_OPTGENERAL_H__0D4DFA1E_8391_4DA0_A8C2_1C86D2C0C460__INCLUDED_)
#define AFX_OPTGENERAL_H__0D4DFA1E_8391_4DA0_A8C2_1C86D2C0C460__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

#include "MyHotkeyCtrl.h"
#include "resource.h"

//---------------------------------------------------
// COptGeneral �_�C�A���O
//---------------------------------------------------
class COptGeneral : public CDialog
{
    // �R���X�g���N�V����
public:
    COptGeneral(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

    // �_�C�A���O �f�[�^
private:
    //{{AFX_DATA(COptGeneral)
    enum { IDD = IDD_SETTINGS_01_GENERAL };
    CMyHotkeyCtrl m_ctrlPopupKey;
    CMyHotkeyCtrl m_ctrlFifoKey;
    //}}AFX_DATA

    // �I�[�o�[���C�h
    // ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
public:
    //{{AFX_VIRTUAL(COptGeneral)
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL DestroyWindow();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
    //}}AFX_VIRTUAL

    // �C���v�������e�[�V����
private:
    int m_nPutBackClipboard;
    int m_nShowClipboardInNotifyIconTooltip;

    // �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
    //{{AFX_MSG(COptGeneral)
    virtual BOOL OnInitDialog();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void HotkeyEnablePOP();
    afx_msg void HotkeyDisablePOP();
    afx_msg void HotkeyEnableFIFO();
    afx_msg void HotkeyDisableFIFO();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_OPTGENERAL_H__0D4DFA1E_8391_4DA0_A8C2_1C86D2C0C460__INCLUDED_)
