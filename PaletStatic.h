/*----------------------------------------------------------
    PaletStatic�N���X�w�b�_
                                    2002/12/07 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_PALETSTATIC_H__52F92273_F258_4DFC_B200_6D3A5F68B707__INCLUDED_)
#define AFX_PALETSTATIC_H__52F92273_F258_4DFC_B200_6D3A5F68B707__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

//---------------------------------------------------
// CPaletStatic �E�B���h�E
//---------------------------------------------------
class CPaletStatic : public CStatic
{
    // �R���X�g���N�V����
public:
    CPaletStatic();

    // �A�g���r���[�g
public:

    // �I�y���[�V����
public:
    void SetColor(COLORREF color) {
        m_color = color;
    }

    // �I�[�o�[���C�h
    // ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
    //{{AFX_VIRTUAL(CPaletStatic)
    //}}AFX_VIRTUAL

    // �C���v�������e�[�V����
public:
    virtual ~CPaletStatic();

private:
    COLORREF m_color;

    // �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
    //{{AFX_MSG(CPaletStatic)
    afx_msg void OnPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_PALETSTATIC_H__52F92273_F258_4DFC_B200_6D3A5F68B707__INCLUDED_)
