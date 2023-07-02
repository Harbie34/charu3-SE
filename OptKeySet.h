/*----------------------------------------------------------
	�L�[�ݒ�N���X�w�b�_
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_OPTKEYSET_H__A388CCDA_003B_4044_99AF_3B563BF12272__INCLUDED_)
#define AFX_OPTKEYSET_H__A388CCDA_003B_4044_99AF_3B563BF12272__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptKeySet.h : �w�b�_�[ �t�@�C��
#include "resource.h"
#include "Init.h"
#include "MyHotkeyCtrl.h"

#include <list>
#include <afxwin.h>

//---------------------------------------------------
// COptKeySet �_�C�A���O
//---------------------------------------------------
class COptKeySet : public CDialog
{
// �R���X�g���N�V����
public:
	COptKeySet(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^
	CHANGE_KEY windouToKeyOption();
// �_�C�A���O �f�[�^
	//{{AFX_DATA(COptKeySet)
	enum { IDD = IDD_SETTINGS_05_KEYS };
	CEdit	m_ctrlCopyWaitEdit;
	CEdit	m_ctrlPasteWaitEdit;
	CMyHotkeyCtrl	m_ctrlPasteKey;
	CListCtrl	m_ctrlIniList;
	CMyHotkeyCtrl	m_ctrlCopyKey;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(COptKeySet)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	void SetListData(CHANGE_KEY sKeyData, CHANGE_KEY *dataPtr, bool isSet, int nLine);
	std::list<CHANGE_KEY>::iterator findData(CHANGE_KEY* dataPtr);

	std::list<CHANGE_KEY> m_MyKeyList;//�L�[�ݒ�f�[�^���X�g
	CHANGE_KEY *m_dataPtrSelect;
	int m_nSelItem;

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(COptKeySet)
	virtual BOOL OnInitDialog();
	afx_msg void OnOptKeyAdd();
	afx_msg void OnOptKeyDalete();
	afx_msg void OnItemchangedOptKeyIniList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOptKeyEdit();
	afx_msg void OnDblclkOptKeyIniList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_OPTKEYSET_H__A388CCDA_003B_4044_99AF_3B563BF12272__INCLUDED_)
