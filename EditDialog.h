/*----------------------------------------------------------
    EditDialog�N���X�w�b�_
                                    2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_EDITDIALOG_H__B4B68363_82C1_4D62_AC66_00D698DF05A4__INCLUDED_)
#define AFX_EDITDIALOG_H__B4B68363_82C1_4D62_AC66_00D698DF05A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

#include <vector>

#include "MyEditCtrl.h"
#include "Charu3Tree.h"
#include "Init.h"
#include "resource.h"

//---------------------------------------------------
// CEditDialog �_�C�A���O
//---------------------------------------------------
class CEditDialog : public CDialog
{
public:
    CEditDialog(CWnd* pParent, STRING_DATA* pData, bool newData);
    STRING_DATA* m_pData;
    void setMacroTempate(std::vector<MACRO_STRUCT>* pMacro, std::vector<MACRO_STRUCT>* pDataMacro) {
        m_vctMacro = pMacro;
        m_vctDataMacro = pDataMacro;
    }
    // �_�C�A���O �f�[�^
        //{{AFX_DATA(CEditDialog)
    enum { IDD = IDD_EDIT };
    CMyEditCtrl	m_ctrlDataEdit;
    CMyEditCtrl m_ctrlNameEdit;
    CMyEditCtrl	m_ctrlMacroEdit;
    CComboBox	m_ctrlKindCombo;
    CComboBox	m_ctrlIconCombo;
    CComboBox	m_ctrlMacro;
    CComboBox	m_ctrlDataMacro;
    CButton m_ctrlPasteFile;
    //}}AFX_DATA


// �I�[�o�[���C�h
    // ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
    //{{AFX_VIRTUAL(CEditDialog)
public:
    virtual BOOL DestroyWindow();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
    //}}AFX_VIRTUAL

protected:
    std::vector<MACRO_STRUCT>* m_vctMacro;
    std::vector<MACRO_STRUCT>* m_vctDataMacro;

    // �������ꂽ���b�Z�[�W �}�b�v�֐�
    //{{AFX_MSG(CEditDialog)
    afx_msg void OnEditPasteFile();
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnSelchangeEditMacroCombo();
    afx_msg void OnSelchangeEditDataMacroCombo();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnKindChanged();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    void InsertText(CEdit& ctrl, CString strString);
    void ResetExtendedSettingItems();

    bool m_bNewData;
    int m_nKind;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_EDITDIALOG_H__B4B68363_82C1_4D62_AC66_00D698DF05A4__INCLUDED_)
