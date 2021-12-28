// InputDialog.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "input.h"
#include "InputDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputDialog �_�C�A���O


CInputDialog::CInputDialog(TCHAR *szTitle,CWnd* pParent /*=NULL*/)
	: CDialog(CInputDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputDialog)
	m_strInput = _T("");
	//}}AFX_DATA_INIT
	m_szTitle = szTitle;
}


void CInputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputDialog)
	DDX_Text(pDX, IDC_EDIT, m_strInput);
	//}}AFX_DATA_MAP

	this->SetWindowText(m_szTitle);
}


BEGIN_MESSAGE_MAP(CInputDialog, CDialog)
	//{{AFX_MSG_MAP(CInputDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDialog ���b�Z�[�W �n���h��

BOOL CInputDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	this->SetForegroundWindow();
	GetDlgItem(IDC_EDIT)->ModifyStyleEx(NULL,SS_LEFTNOWORDWRAP,NULL);

	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}
