/*----------------------------------------------------------
	ChildView�N���X
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "ChildView.h"
#include "Charu3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------
//�֐���	CChildView
//�@�\		�R���X�g���N�^
//---------------------------------------------------
CChildView::CChildView()
{
}

//---------------------------------------------------
//�֐���	~CChildView
//�@�\		�f�X�g���N�^
//---------------------------------------------------
CChildView::~CChildView()
{
}

BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------
// CChildView ���b�Z�[�W �n���h��
//---------------------------------------------------

//---------------------------------------------------
//�֐���	PreCreateWindow(CREATESTRUCT& cs)
//�@�\		�E�B���h�E�쐬�O����
//---------------------------------------------------
BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

//---------------------------------------------------
//�֐���	OnPaint()
//�@�\		�`�揈��
//---------------------------------------------------
void CChildView::OnPaint()
{
	CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g

	// TODO: ���b�Z�[�W �n���h���̃R�[�h�������ɒǉ����Ă��������B

	// �`��̂��߂� CWnd::OnPaint ���Ăяo���Ă͂����܂���B
}
