/*----------------------------------------------------------
    ChildView�N���X�w�b�_
                                    2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_CHILDVIEW_H__F8B13439_ACED_4C46_87CF_498ACB16F2B8__INCLUDED_)
#define AFX_CHILDVIEW_H__F8B13439_ACED_4C46_87CF_498ACB16F2B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//---------------------------------------------------
// CChildView �E�B���h�E
//---------------------------------------------------
class CChildView : public CWnd
{
    // �R���X�g���N�^
public:
    CChildView();

    // �A�g���r���[�g
public:

    // �I�y���[�V����
public:

    // �I�[�o�[���C�h
        // ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
        //{{AFX_VIRTUAL(CChildView)
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
    virtual ~CChildView();

    // �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
    //{{AFX_MSG(CChildView)
    afx_msg void OnPaint();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ ���V������`��������ꍇ�ɂ́A���̍s�����O�ɒǉ����܂��B

#endif // !defined(AFX_CHILDVIEW_H__F8B13439_ACED_4C46_87CF_498ACB16F2B8__INCLUDED_)
