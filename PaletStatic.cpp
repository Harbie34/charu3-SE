/*----------------------------------------------------------
	PaletStatic�N���X
									2002/12/07 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "charu3.h"
#include "PaletStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------
//�֐���	CPaletStatic
//�@�\		�R���X�g���N�^
//---------------------------------------------------
CPaletStatic::CPaletStatic()
{
	m_colMyColor = 0;
}

//---------------------------------------------------
//�֐���	~CPaletStatic
//�@�\		�f�X�g���N�^
//---------------------------------------------------
CPaletStatic::~CPaletStatic()
{
}


BEGIN_MESSAGE_MAP(CPaletStatic, CStatic)
	//{{AFX_MSG_MAP(CPaletStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------
// CPaletStatic ���b�Z�[�W �n���h��
//---------------------------------------------------

//---------------------------------------------------
//�֐���	OnPaint()
//�@�\		�`��
//---------------------------------------------------
void CPaletStatic::OnPaint() 
{
	CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g
	
	RECT rect;
	GetClientRect(&rect);
	dc.IntersectClipRect(&rect);
	dc.FillSolidRect(&rect,m_colMyColor);
}
