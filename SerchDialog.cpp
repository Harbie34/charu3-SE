/*----------------------------------------------------------
	Charu3
	�����_�C�A���O
----------------------------------------------------------*/

#include "stdafx.h"
#include "Charu3.h"
#include "SerchDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// �A�v���P�[�V�����I�u�W�F�N�g
extern	CCharu3App theApp;

//---------------------------------------------------
// CSerchDialog �_�C�A���O
//---------------------------------------------------
BEGIN_MESSAGE_MAP(CSerchDialog, CDialog)
	//{{AFX_MSG_MAP(CSerchDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------
//�R���X�g���N�^
//---------------------------------------------------
CSerchDialog::CSerchDialog(CWnd* pParent /*=NULL*/)	: CDialog(CSerchDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSerchDialog)
	m_nSerchKind = -1;
	m_nSerchLogic = -1;
	m_strSerchKey = _T("");
	//}}AFX_DATA_INIT
}

//---------------------------------------------------
//�f�[�^�G�N�X�`�F���W
//---------------------------------------------------
void CSerchDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	m_nSerchKind = theApp.m_ini.m_nSerchKind;
	m_nSerchLogic = theApp.m_ini.m_nSerchLogic;
	//{{AFX_DATA_MAP(CSerchDialog)
	if(GetDlgItem(IDC_RADIO_NAME))
		DDX_Radio(pDX, IDC_RADIO_NAME, m_nSerchKind);
	if(GetDlgItem(IDC_RADIO_AND))
		DDX_Radio(pDX, IDC_RADIO_AND, m_nSerchLogic);
	if(GetDlgItem(IDC_SERCH_TEXT))
		DDX_Text(pDX, IDC_SERCH_TEXT, m_strSerchKey);
	//}}AFX_DATA_MAP
}


//---------------------------------------------------
// CSerchDialog ���b�Z�[�W �n���h��
//---------------------------------------------------

void CSerchDialog::OnOK() 
{
	CDialog::OnOK();
}








