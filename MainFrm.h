/*----------------------------------------------------------
	MainFrm�N���X�w�b�_
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_MAINFRM_H__86B0B4A1_0092_48D1_B396_31664A4314DB__INCLUDED_)
#define AFX_MAINFRM_H__86B0B4A1_0092_48D1_B396_31664A4314DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ChildView.h"

#define _ME_NAME_ _T("MainFrm.cpp")

//---------------------------------------------------
// CMainFrame�N���X��`
//---------------------------------------------------
class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
	void setMenu();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// �A�g���r���[�g
public:

// �I�y���[�V����
public:
	bool checkTrayPos();
	void changeTrayIcon(bool isStock);

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CChildView    m_wndView;

// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnExit();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnQueryEndSession();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	HICON		m_hIcon,m_hStopIcon;
	NOTIFYICONDATA m_nIcon;
	CMenu m_PopupMenu;
	HWND m_hActive;

	void changeClip();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_MAINFRM_H__86B0B4A1_0092_48D1_B396_31664A4314DB__INCLUDED_)
