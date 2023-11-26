/*----------------------------------------------------------
    ����FIFO�ݒ�N���X�w�b�_
                                    2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_OPTFIFO_H__63AED525_A07B_429F_B89E_2A8964C05622__INCLUDED_)
#define AFX_OPTFIFO_H__63AED525_A07B_429F_B89E_2A8964C05622__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

#include "resource.h"

//---------------------------------------------------
// COptFifo �_�C�A���O
//---------------------------------------------------

class COptFifo : public CDialog
{
    // �R���X�g���N�V����
public:
    COptFifo(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^
    virtual BOOL DestroyWindow();

    // �_�C�A���O �f�[�^
    //{{AFX_DATA(COptFifo)
    enum { IDD = IDD_SETTINGS_04_STOCKMODE };
    // ����: ClassWizard �͂��̈ʒu�Ƀf�[�^ �����o��ǉ����܂��B
    //}}AFX_DATA

    // �I�[�o�[���C�h
    // ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
    //{{AFX_VIRTUAL(COptFifo)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
    //}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
    // �������ꂽ���b�Z�[�W �}�b�v�֐�
    //{{AFX_MSG(COptFifo)
    afx_msg void OnPlayCopy();
    afx_msg void OnPlayPaste();
    afx_msg void OnFileCopy();
    afx_msg void OnFilePaste();
    afx_msg void OnNoSoundCopy();
    afx_msg void OnNoSoundPaste();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    void Play(int id);
    void File(int id);

    int m_nAutoOff;
    int m_nCleanup;
    int m_nDontSaveSameDataAsLast;
    CString m_strCopySound;
    CString m_strPasteSound;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_OPTFIFO_H__63AED525_A07B_429F_B89E_2A8964C05622__INCLUDED_)
