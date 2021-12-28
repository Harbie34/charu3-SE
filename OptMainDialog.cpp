/*----------------------------------------------------------
	�ݒ胁�C���N���X
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "Charu3.h"
#include "OptMainDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCharu3App theApp;

//---------------------------------------------------
//�֐���	COptMainDialog
//�@�\		�R���X�g���N�^
//---------------------------------------------------
COptMainDialog::COptMainDialog(CWnd* pParent /*=NULL*/,int nPage) : CDialog(COptMainDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptMainDialog)
	//}}AFX_DATA_INIT
	m_nPage = nPage;
}


//---------------------------------------------------
//�֐���	DoDataExchange(CDataExchange* pDX)
//�@�\		�f�[�^�G�N�X�`�F���W
//---------------------------------------------------
void COptMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptMainDialog)
	if(GetDlgItem(IDC_OPT_TAB))
		DDX_Control(pDX, IDC_OPT_TAB, m_ctrlTab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptMainDialog, CDialog)
	//{{AFX_MSG_MAP(COptMainDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_OPT_TAB, OnSelchangeOptTab)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(TCN_KEYDOWN, IDC_OPT_TAB, OnKeydownOptTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------
// COptMainDialog ���b�Z�[�W �n���h��
//---------------------------------------------------

//---------------------------------------------------
//�֐���	OnInitDialog()
//�@�\		������
//---------------------------------------------------
BOOL COptMainDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	TC_ITEM    item;
	int i;
    item.mask = TCIF_TEXT;

	//�^�u��ݒ�
	CString strTabText;
	//{_T("���"),_T("�X�g�b�N���[�h"),_T("�|�b�v�A�b�v"),_T("�r�W���A��"),_T("�L�[�ݒ�")};

	for(i = 0; i <= MAX_OPT_PAGE; i++) {
		strTabText.LoadString(APP_INF_TABNAME_01 + i);
	    item.pszText = (TCHAR*)LPCTSTR(strTabText);
	    m_ctrlTab.InsertItem(i, &item);
	}

    //�^�u�R���̃T�C�Y���擾
    CRect rect;
    m_ctrlTab.GetWindowRect(&rect);
    m_ctrlTab.AdjustRect(FALSE, &rect);
    ScreenToClient(&rect);

	//�y�[�W���\�z
	int nDialogID[] = {IDD_OPT_ETC,IDD_OPT_FIFO,IDD_OPT_POPUP,IDD_OPT_VISUAL,IDD_OPT_KEYSET};

	m_OptionPage[0] = &m_EtcPage;
	m_OptionPage[1] = &m_FifoPage;
	m_OptionPage[2] = &m_PopupPage;
	m_OptionPage[3] = &m_VisualPage;
	m_OptionPage[4] = &m_KeysetPage;

	//�����ݒ�
	for(i = 0; i <= MAX_OPT_PAGE; i++) {
	    m_OptionPage[i]->Create(nDialogID[i],this);
	    m_OptionPage[i]->MoveWindow(&rect);
	    m_OptionPage[i]->ShowWindow(SW_HIDE);
	}
	m_nPage = theApp.m_ini.m_nOptionPage;
    m_OptionPage[m_nPage]->ShowWindow(SW_SHOW);
	m_ctrlTab.SetCurFocus(m_nPage);	
	
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

//---------------------------------------------------
//�֐���	OnSelchangeOptTab(NMHDR* pNMHDR, LRESULT* pResult)
//�@�\		�^�u�؂�ւ�����
//---------------------------------------------------
void COptMainDialog::OnSelchangeOptTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	for(int i = 0; i <= MAX_OPT_PAGE; i++)
	    m_OptionPage[i]->ShowWindow(SW_HIDE);

	m_nPage = m_ctrlTab.GetCurSel();
    m_OptionPage[m_nPage]->ShowWindow(SW_SHOW);
  	
	*pResult = 0;
}

//---------------------------------------------------
//�֐���	DestroyWindow()
//�@�\		�E�B���h�E�j������
//---------------------------------------------------
BOOL COptMainDialog::DestroyWindow() 
{
	for(int i = 0; i <= MAX_OPT_PAGE; i++) {
		m_OptionPage[i]->UpdateData();
	    m_OptionPage[i]->DestroyWindow();
	}
	theApp.m_ini.m_nOptionPage = m_nPage;

	
	return CDialog::DestroyWindow();
}

void COptMainDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	
}

//---------------------------------------------------
// ���b�Z�[�W�O����
//---------------------------------------------------
BOOL COptMainDialog::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN) {
		if(pMsg->wParam == VK_PRIOR) {
			if(m_nPage < 4) m_nPage++;
			else			m_nPage = 0;
			m_ctrlTab.SetCurFocus(m_nPage);
		}
		else if(pMsg->wParam == VK_NEXT) {
			if(m_nPage > 0) m_nPage--;
			else			m_nPage = 4;
			m_ctrlTab.SetCurFocus(m_nPage);
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}


void COptMainDialog::OnKeydownOptTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TC_KEYDOWN* pTCKeyDown = (TC_KEYDOWN*)pNMHDR;
	if(pTCKeyDown->wVKey == VK_TAB && ::GetKeyState(VK_CONTROL) < 0){
		if(m_nPage < 4) m_nPage++;
		else			m_nPage = 0;
		m_ctrlTab.SetCurFocus(m_nPage);
	}
	*pResult = 0;
}
