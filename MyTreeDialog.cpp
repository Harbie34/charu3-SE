/*----------------------------------------------------------
	MyTreeDialog�N���X
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "MyTreeDialog.h"
#include "EditDialog.h"
#include "SerchDialog.h"
#include "General.h"
#include "Charu3.h"

#include <list>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCharu3App theApp;

namespace {
	//---------------------------------------------------
	//�֐���	DrawLline(CDC* pDC, CPoint* point, COLORREF color)
	//�@�\		����`��
	//---------------------------------------------------
	void DrawLline(CDC* pDC, CPoint* point, COLORREF color)
	{
		CPen pen(PS_SOLID, 1, color);
		CPen* old_pen = pDC->SelectObject(&pen);
		pDC->MoveTo(point[0]);
		pDC->LineTo(point[1]);
		pDC->LineTo(point[2]);
		pDC->SelectObject(old_pen);
	}

	//---------------------------------------------------
	//�֐���	DrawFrame(CDC* pDC, CRect& rect, COLORREF color)
	//�@�\		�g��`��
	//---------------------------------------------------
	void DrawFrame(CDC* pDC, CRect& rect, COLORREF color)
	{
		CPoint point[3];
		COLORREF colRD[5];
		COLORREF colLU[5];

		colRD[1] = colLU[0] = CGeneral::upDownLight(color, 1.2);
		colRD[2] = colLU[1] = color;
		colRD[0] = colLU[2] = CGeneral::upDownLight(color, 0.8);

		for (int i = 1; i <= 3; i++) {
			point[0].x = rect.left + i;
			point[0].y = rect.bottom - i;
			point[1].x = rect.right - i;
			point[1].y = rect.bottom - i;
			point[2].x = rect.right - i;
			point[2].y = rect.top + i - 1;
			DrawLline(pDC, point, colRD[i - 1]);
		}

		for (int i = 0; i <= 2; i++) {
			point[0].x = rect.left + i;
			point[0].y = rect.bottom - i - 1;
			point[1].x = rect.left + i;
			point[1].y = rect.top + i;
			point[2].x = rect.right - i;
			point[2].y = rect.top + i;
			DrawLline(pDC, point, colLU[i]);
		}
	}
} // anonymous namespace

//---------------------------------------------------
//�֐���	CMyTreeDialog
//�@�\		�R���X�g���N�^
//---------------------------------------------------
CMyTreeDialog::CMyTreeDialog(CWnd* pParent /*=NULL*/)
    : CDialog(CMyTreeDialog::IDD, pParent)
    , m_selectDataPtr(nullptr)
    , m_isInitOK(false)
    , m_isModal(true)
    , m_isAltDown(false)
    , m_dataPtrDbClick(nullptr)
    , m_hCopyData(nullptr)
    , m_dwStartTime(0)
    , m_hDLL(NULL)
    , m_pExStyle(NULL)
    , m_cOlgFont(nullptr)
    , m_strQuickKey("")
    , m_bCheckbox(false)
	{
	//{{AFX_DATA_INIT(CMyTreeDialog)
	//}}AFX_DATA_INIT
	m_brBack.m_hObject = NULL;
}

//---------------------------------------------------
//�֐���	DoDataExchange(CDataExchange* pDX)
//�@�\		�f�[�^�G�N�X�`�F���W
//---------------------------------------------------
void CMyTreeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyTreeDialog)
	if(GetDlgItem(IDC_MY_TREE))
		DDX_Control(pDX, IDC_MY_TREE, *m_pTreeCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyTreeDialog, CDialog)
	//{{AFX_MSG_MAP(CMyTreeDialog)
	ON_WM_SIZE()
	ON_COMMAND(IDML_EDIT, OnEdit)
	ON_COMMAND(IDML_DELETE, OnDelete)
	ON_COMMAND(IDML_RENAME, OnRename)
	ON_COMMAND(IDML_ICON_KEY, OnIconKey)
	ON_COMMAND(IDML_ICON_DATE, OnIconDate)
	ON_COMMAND(IDML_ICON_EXE, OnIconExe)
	ON_COMMAND(IDML_ICON_RELATE, OnIconRelate)
	ON_COMMAND(IDML_ICON_SELECT, OnIconSelect)
	ON_COMMAND(IDML_ICON_CLIP, OnIconClip)
	ON_COMMAND(IDML_ICON_PLUGIN, OnIconPlugin)
	ON_COMMAND(IDML_ICON_KEYMACRO, OnIconKeymacro)
	ON_COMMAND(IDML_MAKE_ONETIME, OnMakeOnetime)
	ON_COMMAND(IDML_MAKE_PERMANENT, OnMakePermanent)
	ON_COMMAND(IDML_FOLDER_CLEAR, OnFolderClear)
	ON_COMMAND(IDML_NEW_DATA, OnNewData)
	ON_COMMAND(IDML_NEW_FOLDER, OnNewFolder)
	ON_COMMAND(IDML_RESELECT_ICONS, OnReselectIcons)
	ON_COMMAND(IDML_CLEANUP_ONETIME, OnCleanupAllOnetime)
	ON_COMMAND(IDML_MAKE_PERMANENT_ALL, OnMakeAllOnetimePermanent)
	ON_COMMAND(IDML_CLOSE_ALL, OnCloseAll)
	ON_COMMAND(IDML_LIST_SERCH, OnListSearch)
	ON_COMMAND(IDML_CHECK_ITEM, OnCheckItem)
	ON_COMMAND(IDML_COPY_DATA, OnCopyData)
	ON_COMMAND(IDML_DATA_PASTE, OnDataPaste)
	ON_COMMAND(IDML_IMPORT, OnImport)
	ON_COMMAND(IDML_EXPORT, OnExport)
	ON_COMMAND(IDML_OPTION, OnOption)
	ON_NOTIFY(NM_RCLICK, IDC_MY_TREE, OnRclickMyTree)
	ON_NOTIFY(NM_CLICK, IDC_MY_TREE, OnClickMyTree)
	ON_NOTIFY(NM_KILLFOCUS, IDC_MY_TREE, OnKillfocusMyTree)
	ON_NOTIFY(TVN_KEYDOWN, IDC_MY_TREE, OnKeydownMyTree)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_MY_TREE, OnBeginlabeleditMyTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_MY_TREE, OnEndlabeleditMyTree)
	ON_WM_SHOWWINDOW()
	ON_WM_NCPAINT()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------
//�֐���	OnInitDialog()
//�@�\		�_�C�A���O�̏�����
//---------------------------------------------------
BOOL CMyTreeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_toolTip.Create(this, TTS_ALWAYSTIP);
	m_toolTip.SetMaxTipWidth(400);

	m_toolTip.Activate(TRUE);

	m_toolTip.AddTool(m_pTreeCtrl, _T(""));

	m_hDLL = ::LoadLibrary(_T("user32"));
	m_pExStyle = m_hDLL != 0 ? (PFUNC)::GetProcAddress(m_hDLL, "SetLayeredWindowAttributes") : NULL;
	m_PopupMenu.LoadMenu(MAKEINTRESOURCE(IDR_LISTMENU));//���j���[�N���X�Ƀ��j���[��ǂ�
	return TRUE;
}

//---------------------------------------------------
//�֐���	PopupMenu(CPoint point)
//�@�\		�|�b�v�A�b�v���j���[���o��
//---------------------------------------------------
void CMyTreeDialog::popupMenu(CPoint point)
{
	if(m_isModal || !m_isInitOK) return;

	CPoint HitPoint = point;
	ScreenToClient(&HitPoint);
	HTREEITEM hTreeItem = m_pTreeCtrl->HitTest(HitPoint);
	m_PopupMenu.EnableMenuItem(IDML_EDIT, hTreeItem ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.EnableMenuItem(IDML_DELETE, hTreeItem ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.EnableMenuItem(IDML_RENAME, hTreeItem ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.EnableMenuItem(IDML_CHECK_ITEM, m_bCheckbox ? MF_GRAYED : MF_ENABLED);
	m_PopupMenu.EnableMenuItem(IDML_COPY_DATA, hTreeItem ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.EnableMenuItem(IDML_DATA_PASTE, m_hCopyData ? MF_ENABLED : MF_GRAYED);
	bool isData = false;
	bool isFolder = false;
	if (hTreeItem) {
		m_pTreeCtrl->SelectItem(hTreeItem);
		STRING_DATA* dataPtr = m_pTreeCtrl->getDataPtr(hTreeItem);
		isData = (dataPtr->m_cKind & KIND_DATA_ALL) != 0;
		isFolder = (dataPtr->m_cKind & KIND_FOLDER_ALL) != 0;
		m_PopupMenu.EnableMenuItem(IDML_MAKE_PERMANENT, !isData || (dataPtr->m_cKind & KIND_LOCK) ? MF_GRAYED : MF_ENABLED);
		m_PopupMenu.EnableMenuItem(IDML_MAKE_ONETIME, !isData || (dataPtr->m_cKind & KIND_ONETIME) ? MF_GRAYED : MF_ENABLED);
	}
	else {
		m_PopupMenu.EnableMenuItem(IDML_MAKE_PERMANENT, MF_GRAYED);
		m_PopupMenu.EnableMenuItem(IDML_MAKE_ONETIME, MF_GRAYED);
	}
	m_PopupMenu.EnableMenuItem(IDML_ICON_KEY, isData ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.EnableMenuItem(IDML_ICON_DATE, isData ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.EnableMenuItem(IDML_ICON_EXE, isData ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.EnableMenuItem(IDML_ICON_RELATE, isData ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.EnableMenuItem(IDML_ICON_SELECT, isData ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.EnableMenuItem(IDML_ICON_CLIP, isData ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.EnableMenuItem(IDML_ICON_PLUGIN, isData ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.EnableMenuItem(IDML_ICON_KEYMACRO, isData ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.EnableMenuItem(IDML_FOLDER_CLEAR, isFolder ? MF_ENABLED : MF_GRAYED);
	m_PopupMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,point.x, point.y, this);
}

//---------------------------------------------------
//�֐���	showWindowPos(POINT pos,int nCmdShow)
//�@�\		�E�B���h�E���ړ����ĕ\��
//---------------------------------------------------
BOOL CMyTreeDialog::showWindowPos(POINT pos, POINT size, int nCmdShow, bool keepSelection, HTREEITEM hOpenItem)
{
	m_pTreeCtrl->OnWindowPosChanging(NULL);
	m_toolTip.SetDelayTime(theApp.m_ini.m_nToolTipDelay);
	m_toolTip.SetDelayTime(TTDT_AUTOPOP,theApp.m_ini.m_nToolTipTime);

	m_pTreeCtrl->SetBkColor(COLORREF(CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBackgroundColor)));
	m_pTreeCtrl->SetTextColor(COLORREF(CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nTextColor)));
	m_pTreeCtrl->SetInsertMarkColor(COLORREF(CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBorderColor)));
	m_pTreeCtrl->ModifyStyle(NULL,TVS_TRACKSELECT,NULL);
	if(theApp.m_ini.m_pop.m_bSingleExpand) {
		m_pTreeCtrl->ModifyStyle(NULL,TVS_SINGLEEXPAND,NULL);
	}
	else {
		m_pTreeCtrl->ModifyStyle(TVS_SINGLEEXPAND,NULL,NULL);
	}

#if false
	// I thought about displaying the pathname of the data above the data tree.But decided not to.
	CWnd* cpDataName = GetDlgItem(IDC_DATA_NAME);
	if (cpDataName) {
		TCHAR buf[MAX_PATH];
		TCHAR* pFileName;
		GetFullPathName(theApp.m_ini.m_strDataPath.GetString(), MAX_PATH, buf, &pFileName);
		cpDataName->SetWindowText(pFileName);
	}
#endif

	HTREEITEM hSelectItem = m_pTreeCtrl->GetSelectedItem();
	if (!keepSelection) {
		m_pTreeCtrl->SelectItem(m_pTreeCtrl->GetRootItem());
	}
	if (!theApp.m_ini.m_pop.m_bKeepFolders) {
		m_pTreeCtrl->closeFolder(m_pTreeCtrl->GetRootItem());
		if (keepSelection) {
			m_pTreeCtrl->SelectItem(hSelectItem);
		}
	}
	else if (hSelectItem && !hOpenItem) {
		RECT rSelItem;
		m_pTreeCtrl->GetItemRect(hSelectItem, &rSelItem, NULL);
		if (size.y < rSelItem.top || rSelItem.bottom < 0)
			m_pTreeCtrl->EnsureVisible(hSelectItem); //EnsureVisible����ƃt�H���_�͊J�����
	}
	if (hOpenItem) m_pTreeCtrl->Expand(hOpenItem, TVE_EXPAND);

	m_colFrame   = CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBorderColor);

	if(m_brBack.GetSafeHandle()) {
		m_brBack.DeleteObject();
		m_brBack.m_hObject = NULL;
	}
	if(m_brBack.m_hObject == NULL)
		m_brBack.CreateSolidBrush(COLORREF(CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBackgroundColor)));

	CGeneral::setAbsoluteForegroundWindow(theApp.m_pMainFrame->m_hWnd);
	m_pTreeCtrl->SetFocus();

	m_selectDataPtr = nullptr;
	m_isInitOK = true;
	m_isModal = false;
	m_isAltDown = false;
	m_dataPtrDbClick = nullptr;

	m_pTreeCtrl->m_ltCheckItems.clear();
	if(theApp.m_ini.m_visual.m_nToolTip == 2)	m_toolTip.Activate(FALSE);
	else	m_toolTip.Activate(TRUE);
	SetWindowPos(&wndTopMost,pos.x,pos.y,size.x,size.y,NULL);

	return ShowWindow(nCmdShow);
}

//---------------------------------------------------
// CMyTreeDialog ���b�Z�[�W �n���h��
//---------------------------------------------------
void CMyTreeDialog::OnNcPaint()
{
	CDialog::OnNcPaint();
	// Set scrollbars
	m_pTreeCtrl->setScrollBar();
	// Draw border
	{
		CRect rect;
		GetWindowRect(rect);
		CRect localRect = rect - rect.TopLeft();
		CDC* pDC = GetWindowDC();
		DrawFrame(pDC, localRect, m_colFrame);
		ReleaseDC(pDC);
	}
	// Workaround for the problem where the border is overwritten
	{
		DWMNCRENDERINGPOLICY policy = DWMNCRP_DISABLED;
		DwmSetWindowAttribute(m_hWnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof policy);
		BOOL allow = FALSE;
		DwmSetWindowAttribute(m_hWnd, DWMWA_ALLOW_NCPAINT, &allow, sizeof allow);
		// Even with this countermeasure, there are still occasional cases where the
		// border is hidden by overwriting. In such cases, I used the workaround of
		// calling RedrawWindow there.
	}
}

//---------------------------------------------------
//�֐���	OnSize(UINT nType, int cx, int cy)
//�@�\		���T�C�Y
//---------------------------------------------------
void CMyTreeDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(m_pTreeCtrl->m_hWnd) {
		m_pTreeCtrl->SetWindowPos(&wndTop,0,0,cx,cy,SWP_NOMOVE);
		m_pTreeCtrl->setScrollBar();
	}
}

//---------------------------------------------------
//�֐���	DestroyWindow()
//�@�\		�E�B���h�j��������
//---------------------------------------------------
BOOL CMyTreeDialog::DestroyWindow()
{
	m_toolTip.DelTool(m_pTreeCtrl);
	return CDialog::DestroyWindow();
}

//---------------------------------------------------
//�֐���	OnKeydownMyTree(NMHDR* pNMHDR, LRESULT* pResult)
//�@�\		�L�[��������
//---------------------------------------------------
void CMyTreeDialog::OnKeydownMyTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;

	*pResult = 0;
}

//---------------------------------------------------
//�֐���	OnRclickMyTree(NMHDR* pNMHDR, LRESULT* pResult)
//�@�\		�E�{�^����������
//---------------------------------------------------
void CMyTreeDialog::OnRclickMyTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	if(m_isModal || m_pTreeCtrl->IsDragging()) return;
	POINT point;
	::GetCursorPos(&point);
	popupMenu(point);
	*pResult = 0;
}

//---------------------------------------------------
//�֐���	OnClickMyTree(NMHDR* pNMHDR, LRESULT* pResult)
//�@�\		���{�^����������
//---------------------------------------------------
void CMyTreeDialog::OnClickMyTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	RECT TreeRect,ItemRect;
	POINT pCursolPos;
	HTREEITEM hClickItem;
	UINT Flags;

	m_pTreeCtrl->GetWindowRect(&TreeRect);
	GetCursorPos(&pCursolPos);

	pCursolPos.x -= TreeRect.left;
	pCursolPos.y -= TreeRect.top;

	hClickItem = m_pTreeCtrl->HitTest(pCursolPos,&Flags);
	if (hClickItem) {
		if (m_pTreeCtrl->GetItemRect(hClickItem,&ItemRect,true)) {
			//�A�C�R�����N���b�N����
			if(TVHT_ONITEMICON & Flags) {
				STRING_DATA* pData = m_pTreeCtrl->getDataPtr(hClickItem);
				if (pData->m_cKind & KIND_ONETIME) {
					pData->m_cKind = KIND_LOCK;
					m_pTreeCtrl->editData2(hClickItem);
				}
			}
			//�`�F�b�N�{�b�N�X
			if (TVHT_ONITEMSTATEICON & Flags) {
				m_pTreeCtrl->checkItem(hClickItem);
				m_pTreeCtrl->SetCheck(hClickItem,!m_pTreeCtrl->GetCheck(hClickItem));
			}
		}
		m_pTreeCtrl->SelectItem(hClickItem);
	}
	*pResult = 0;
}

//---------------------------------------------------
//�֐���	OnKillfocusMyTree(NMHDR* pNMHDR, LRESULT* pResult)
//�@�\		�c���[�̃t�H�[�J�X���O�ꂽ��B��
//---------------------------------------------------
void CMyTreeDialog::OnKillfocusMyTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);

	if (m_isInitOK && !m_isModal) {
		::PostMessage(theApp.getAppWnd(), WM_TREE_CLOSE, IDCANCEL, NULL);
		KillTimer(CHARU_QUICK_TIMER);
		m_isInitOK = false;
	}
	*pResult = 0;
}

//---------------------------------------------------
//�֐���	OnShowWindow(BOOL bShow, UINT nStatus)
//�@�\		�E�B���h�E�\��������
//---------------------------------------------------
void CMyTreeDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	if (bShow) {
		//����������
		LONG lExStyle = ::GetWindowLong(this->m_hWnd, GWL_EXSTYLE);
		if (m_pExStyle && theApp.m_ini.m_visual.m_nOpacity < 100) {
			SetWindowLong(this->m_hWnd, GWL_EXSTYLE, lExStyle | 0x80000);
			int nTansparent = 255 * theApp.m_ini.m_visual.m_nOpacity / 100;
			m_pExStyle(this->m_hWnd, 0, nTansparent, 2);
		}
		else {
			SetWindowLong(this->m_hWnd, GWL_EXSTYLE, lExStyle & 0xfff7ffff);
		}
		m_cOlgFont = m_pTreeCtrl->GetFont();
		m_cFont = new CFont;
		if (m_cFont) {
			m_cFont->CreatePointFont(theApp.m_ini.m_visual.m_nFontSize, theApp.m_ini.m_visual.m_strFontName);
			m_pTreeCtrl->SetFont(m_cFont, TRUE);
		}
	}
	else if (m_cFont) {
		RECT rect;
		GetWindowRect(&rect);
		theApp.m_ini.m_DialogSize.x = rect.right - rect.left;
		theApp.m_ini.m_DialogSize.y = rect.bottom - rect.top;
		//		if(m_cOlgFont) m_pTreeCtrl->SetFont(m_cOlgFont,FALSE);
		delete m_cFont;
	}
	CDialog::OnShowWindow(bShow, nStatus);
}

//---------------------------------------------------
//�֐���	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//�@�\		�w�i�F��ς���
//---------------------------------------------------
HBRUSH CMyTreeDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (m_brBack.GetSafeHandle()) hbr = m_brBack;
	return hbr;
}

//---------------------------------------------------
//�֐���	OnBeginlabeleditMyTree(NMHDR* pNMHDR, LRESULT* pResult)
//�@�\		���x���ҏW�J�n
//---------------------------------------------------
void CMyTreeDialog::OnBeginlabeleditMyTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	m_isModal = true;

	*pResult = 0;
}

//---------------------------------------------------
//�֐���	OnEndlabeleditMyTree(NMHDR* pNMHDR, LRESULT* pResult)
//�@�\		���x���ҏW�I��
//---------------------------------------------------
void CMyTreeDialog::OnEndlabeleditMyTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	//�ύX���c���[�ɔ��f
	m_pTreeCtrl->SetFocus();
	TV_ITEM* item = &(pTVDispInfo->item);
	if (item->pszText && *(item->pszText)) {
		HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
		STRING_DATA* pData = m_pTreeCtrl->getDataPtr(hTreeItem);
		pData->m_strTitle = item->pszText;
		m_pTreeCtrl->editData2(hTreeItem);
	}
	m_isModal = false;

	*pResult = 0;
}

//---------------------------------------------------
//�֐���	PreTranslateMessage(MSG* pMsg)
//�@�\		���b�Z�[�W�O����
//---------------------------------------------------
BOOL CMyTreeDialog::PreTranslateMessage(MSG* pMsg)
{
	m_toolTip.RelayEvent(pMsg);
	//����w�ߎ�M
	if (pMsg->message == WM_TREE_CLOSE) {
		closePopup();
		return TRUE;
	}
	//���{�^���_�u���N���b�N
	else if (pMsg->message == WM_LBUTTONDBLCLK && !m_pTreeCtrl->IsDragging()) {
		if (theApp.m_ini.m_pop.m_bSelectByTypingAutoPaste) KillTimer(CHARU_QUICK_TIMER);
		//�f�[�^�擾
		HTREEITEM hTreeItem;
		hTreeItem = m_pTreeCtrl->GetSelectedItem();
		if (hTreeItem) {
			STRING_DATA* dataPtr = m_pTreeCtrl->getDataPtr(hTreeItem);
			if (!(dataPtr->m_cKind & KIND_FOLDER_ALL)) {//�t�H���_����Ȃ���Ό���
				m_dataPtrDbClick = dataPtr;
			}
		}
	}
	else if (pMsg->message == WM_LBUTTONUP && m_dataPtrDbClick != nullptr && !m_pTreeCtrl->IsDragging()) {
		enterData(m_dataPtrDbClick);
		return TRUE;
	}
	//ALT�����j���[�L�[�|�b�v�A�b�v���j���[���o��
	if (pMsg->message == WM_SYSKEYDOWN) m_isAltDown = true;
	else if (((pMsg->message == WM_SYSKEYUP && m_isAltDown) || (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_APPS)) && !theApp.isCloseKey() && !m_pTreeCtrl->IsDragging()) {
		if (theApp.m_ini.m_pop.m_bSelectByTypingAutoPaste) KillTimer(CHARU_QUICK_TIMER);
		m_isAltDown = false;
		CPoint point;
		ZeroMemory(&point, sizeof(point));
		RECT rSelItem;
		HTREEITEM hSelectItem = m_pTreeCtrl->GetSelectedItem();//�I���ʒu���擾
		if (hSelectItem) {
			m_pTreeCtrl->GetItemRect(hSelectItem, &rSelItem, NULL);
			point.x = 0;
			point.y = rSelItem.bottom - 5;
		}
		ClientToScreen(&point);
		popupMenu(point);
		//		return true;
	}

	if (pMsg->message == WM_KEYDOWN) {
		if (!m_pTreeCtrl->IsDragging()) {
			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			msg.message = WM_LBUTTONDOWN;
			msg.hwnd = this->m_hWnd;
			m_toolTip.RelayEvent(&msg);

			::PostMessage(m_pTreeCtrl->m_hWnd, WM_MOUSEMOVE, 0, 0);
			::SetCursor(NULL);
		}
		//ESC�ŕ���
		if (pMsg->wParam == VK_ESCAPE && !m_isModal && !m_pTreeCtrl->IsDragging()) {
			closePopup();
			return TRUE;
		}
		//���x���ҏW���Ȃ�ҏW�I��
		else if (pMsg->wParam == VK_RETURN && m_isModal) {
			m_pTreeCtrl->SetFocus();
			return TRUE;
		}
		//���^�[���L�[���������猈��
		else if (pMsg->wParam == VK_RETURN && !m_pTreeCtrl->IsDragging()) {
			if (theApp.m_ini.m_pop.m_bSelectByTypingAutoPaste) KillTimer(CHARU_QUICK_TIMER);
			//�f�[�^�擾
			HTREEITEM hTreeItem;
			hTreeItem = m_pTreeCtrl->GetSelectedItem();
			if (hTreeItem) {
				STRING_DATA* dataPtr = m_pTreeCtrl->getDataPtr(hTreeItem);

				//�t�H���_���m�F
				if (!(dataPtr->m_cKind & KIND_FOLDER_ALL) || (m_pTreeCtrl->GetStyle() & TVS_CHECKBOXES && m_pTreeCtrl->GetCheck(hTreeItem))) {
					enterData(dataPtr);//�f�[�^������
				}
				else
					m_pTreeCtrl->Expand(hTreeItem, TVE_TOGGLE);
				return TRUE;
			}
		}
		//�X�y�[�X�L�[
		else if (pMsg->wParam == VK_SPACE && !m_isModal) {
			if (theApp.m_ini.m_pop.m_bSelectByTypingAutoPaste) KillTimer(CHARU_QUICK_TIMER);
			HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
			if (hTreeItem != NULL) {//�I������Ă邩?
				if (::GetKeyState(VK_CONTROL) < 0) {//CTRL��������Ă���
					HTREEITEM hSelItem = m_pTreeCtrl->GetSelectedItem();
					m_pTreeCtrl->checkItem(hSelItem);
					hSelItem = m_pTreeCtrl->GetNextVisibleItem(hTreeItem);
					if (hSelItem) hTreeItem = hSelItem;
				}
				else if (::GetKeyState(VK_SHIFT) < 0) {//�V�t�g��������Ă���
					hTreeItem = m_pTreeCtrl->GetPrevVisibleItem(hTreeItem);//�O�̃A�C�e�����擾
					if (!hTreeItem) {
						hTreeItem = m_pTreeCtrl->getLastVisibleItem();//��ԏ�܂ōs������Ō�Ƀ��[�v
					}
				}
				else {
					hTreeItem = m_pTreeCtrl->GetNextVisibleItem(hTreeItem);
					if (!hTreeItem) hTreeItem = m_pTreeCtrl->GetRootItem();
				}
				m_pTreeCtrl->SelectItem(hTreeItem);
			}
			else {//�I������ĂȂ�������0�Ԗڂ�I��
				m_pTreeCtrl->Select(m_pTreeCtrl->GetRootItem(), TVGN_FIRSTVISIBLE);
			}
			return true;
		}
		//�f���[�g�L�[(�f�[�^�폜)
		else if (pMsg->wParam == VK_DELETE && !m_pTreeCtrl->IsDragging() && !m_isModal) {
			if (theApp.m_ini.m_pop.m_bSelectByTypingAutoPaste) KillTimer(CHARU_QUICK_TIMER);
			OnDelete();
		}
		//F1�L�[ ���e�\��
		else if (pMsg->wParam == VK_F1 && !m_pTreeCtrl->IsDragging()) {
			CPoint point;
			ZeroMemory(&point, sizeof(point));
			RECT rSelItem;
			HTREEITEM hSelectItem = m_pTreeCtrl->GetSelectedItem();//�I���ʒu���擾
			if (hSelectItem) {
				m_pTreeCtrl->GetItemRect(hSelectItem, &rSelItem, true);
				point.x = rSelItem.left + ((rSelItem.right - rSelItem.left) / 2);
				point.y = rSelItem.top + ((rSelItem.bottom - rSelItem.top) / 2);

				MSG msg;
				ZeroMemory(&msg, sizeof(msg));
				msg.message = WM_MOUSEMOVE;
				msg.wParam = NULL;
				msg.lParam = point.y;
				msg.lParam = (msg.lParam << 16) + point.x;
				msg.hwnd = m_pTreeCtrl->m_hWnd;

				::PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
				ClientToScreen(&point);
				::SetCursorPos(point.x, point.y);
			}
		}
		// F2 : Rename
		else if (pMsg->wParam == VK_F2 && !m_pTreeCtrl->IsDragging()) {
			if (theApp.m_ini.m_pop.m_bSelectByTypingAutoPaste) KillTimer(CHARU_QUICK_TIMER);
			if (m_pTreeCtrl->GetSelectedItem())
				m_pTreeCtrl->EditLabel(m_pTreeCtrl->GetSelectedItem());
		}
		// F3 : Find Next
		else if (pMsg->wParam == VK_F3 && !m_pTreeCtrl->IsDragging() && !m_isModal) {
			HTREEITEM hSearchItem = m_pTreeCtrl->GetSelectedItem();
			hSearchItem = m_pTreeCtrl->searchItem(theApp.m_ini.m_nSearchTarget, theApp.m_ini.m_nSearchLogic, theApp.m_ini.m_strSearchKeywords, hSearchItem);
			if (hSearchItem) {
				m_pTreeCtrl->SelectItem(hSearchItem);
			}
		}
		// Ctrl+F : Open Find dialog
		else if (::GetKeyState(VK_CONTROL) < 0 && pMsg->wParam == 'F' && !m_pTreeCtrl->IsDragging() && !m_isModal) {
			OnListSearch();
			return true;
		}
		//TAB�L�[�Ń`�F�b�N
		else if (pMsg->wParam == VK_TAB && !m_pTreeCtrl->IsDragging() && !m_isModal) {
			HTREEITEM hSelItem = m_pTreeCtrl->GetSelectedItem();
			m_pTreeCtrl->checkItem(hSelItem);
			return true;
		}
		//�㉺
		else if (pMsg->wParam == VK_DOWN || pMsg->wParam == VK_UP && !m_isModal) {
			if (theApp.m_ini.m_pop.m_bSelectByTypingAutoPaste) KillTimer(CHARU_QUICK_TIMER);

			HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem(), hTreeItemTmp;
			if (hTreeItem != NULL && ::GetKeyState(VK_SHIFT) < 0) {//�I������Ă��āASHIFT�������Ă�
				do {
					hTreeItemTmp = hTreeItem;
					if (pMsg->wParam == VK_DOWN)	hTreeItem = m_pTreeCtrl->GetNextVisibleItem(hTreeItem);
					else							hTreeItem = m_pTreeCtrl->GetPrevVisibleItem(hTreeItem);
					if (m_pTreeCtrl->GetChildItem(hTreeItem)) break;
				} while (hTreeItem);
				if (hTreeItem) {
					m_pTreeCtrl->SelectItem(hTreeItem);
					return true;
				}
			}
		}
		//��
		else if (pMsg->wParam == VK_LEFT && !m_isModal) {
			if (theApp.m_ini.m_pop.m_bSelectByTypingAutoPaste) KillTimer(CHARU_QUICK_TIMER);
			if (::GetKeyState(VK_SHIFT) < 0) {
				if (::GetKeyState(VK_CONTROL) < 0) {
					m_pTreeCtrl->closeFolder(m_pTreeCtrl->GetRootItem()); // Ctrl+Shift+Left: collapse all
				}
				else {
					HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
					if (hTreeItem) {
						hTreeItem = m_pTreeCtrl->searchMyRoots(hTreeItem);
						if (hTreeItem) {
							m_pTreeCtrl->SelectItem(hTreeItem); // Shift+Left: collapse parent
						}
					}
				}
			}
		}
		// "select by typing"
		else if (!m_isModal && !m_pTreeCtrl->IsDragging()) {
			if (selectByTyping((UINT)pMsg->wParam)) return TRUE;
		}
	}
	// "select by typing" �m�菈��
	else if (pMsg->message == WM_TIMER && pMsg->wParam == CHARU_QUICK_TIMER && !m_pTreeCtrl->IsDragging()) {
		BOOL retval = FALSE;
		if (m_hQuickItem) {
			STRING_DATA* dataPtr = m_pTreeCtrl->getDataPtr(m_hQuickItem);
			if (dataPtr->m_cKind & KIND_DATA_ALL) {
				if (theApp.m_ini.m_pop.m_bSelectByTypingAutoPaste) {
					enterData(dataPtr);
				}
			}
			else if (dataPtr->m_cKind & KIND_FOLDER_ALL) {
				if (theApp.m_ini.m_pop.m_bSelectByTypingAutoExpand) {
					m_pTreeCtrl->Expand(m_hQuickItem, TVE_EXPAND);
				}
			}
			retval = TRUE;
		}
		this->KillTimer(CHARU_QUICK_TIMER);
		return retval;
	}
	//TIPS�̕ύX
	else if (WM_TIPS_CHANGE == pMsg->message) {
		HTREEITEM hTarget = (HTREEITEM)pMsg->wParam;
		if (hTarget) {
			STRING_DATA data = m_pTreeCtrl->getData(hTarget);
			changeTipString(data);
		}
		else {
			m_toolTip.Activate(FALSE);
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CMyTreeDialog::closePopup()
{
	::PostMessage(theApp.getAppWnd(), WM_TREE_CLOSE, IDCANCEL, NULL);
	m_isInitOK = false;
	KillTimer(CHARU_QUICK_TIMER);
}

//---------------------------------------------------
//�֐���	enterData(list<STRING_DATA>::iterator it)
//�@�\		�f�[�^�����肵�ă_�C�A���O���B��
//---------------------------------------------------
void CMyTreeDialog::enterData(STRING_DATA* dataPtr)
{
	m_selectDataPtr = dataPtr;
	::PostMessage(theApp.getAppWnd(), WM_TREE_CLOSE, IDOK, NULL);
	m_isInitOK = false;
	this->KillTimer(CHARU_QUICK_TIMER);
}

//---------------------------------------------------
//�֐���	OnEdit()
//�@�\		�I�����ڂ�ҏW
//---------------------------------------------------
void CMyTreeDialog::OnEdit()
{
	if (m_isModal) return;
	m_isModal = true;
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	CEditDialog dlgEdit(this, m_pTreeCtrl->getDataPtr(hTreeItem), false);
	if (dlgEdit.DoModal() == IDOK) {
		m_pTreeCtrl->editData2(hTreeItem);
	}
	m_isModal = false;
}

//---------------------------------------------------
//�֐���	OnDelete()
//�@�\		�I�����ڂ��폜
//---------------------------------------------------
void CMyTreeDialog::OnDelete()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (!hTreeItem) {
		return;
	}
	if (m_isModal) {
		return;
	}
	m_isModal = true;
	STRING_DATA data = m_pTreeCtrl->getData(hTreeItem);
	CString strMessage;
	strMessage.Format(APP_MES_DELETE_OK, data.m_strTitle);
	int nRet = AfxMessageBox(strMessage, MB_YESNO | MB_ICONEXCLAMATION | MB_APPLMODAL);
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	if (nRet == IDYES) {
		if ((data.m_cKind & KIND_FOLDER_ALL) && m_pTreeCtrl->ItemHasChildren(hTreeItem)) {
			strMessage.LoadString(APP_MES_DELETE_FOLDER);
			nRet = AfxMessageBox(strMessage, MB_YESNO | MB_ICONEXCLAMATION | MB_APPLMODAL);
			RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
		}
	}
	if (nRet == IDYES) {
		m_pTreeCtrl->deleteData(hTreeItem);
	}
	m_pTreeCtrl->SetFocus();
	m_isModal = false;
}

//---------------------------------------------------
//�֐���	OnRename()
//�@�\		�I�����ڂ̖��O�̕ύX
//---------------------------------------------------
void CMyTreeDialog::OnRename()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (!hTreeItem) return;

	m_pTreeCtrl->EditLabel(hTreeItem);
}

//---------------------------------------------------
//�֐���	OnIcon�`
//�@�\		�f�[�^�A�C�R����ύX
//---------------------------------------------------

//�N���b�v�{�[�h�}�N��
void CMyTreeDialog::OnIconClip()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem, KIND_CLIP);
}
//���t�}�N��
void CMyTreeDialog::OnIconDate()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem, KIND_DATE);
}
//�V�F���}�N��
void CMyTreeDialog::OnIconExe()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem, KIND_EXE);
}
//���ʍ���
void CMyTreeDialog::OnIconKey()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem, 0);
}
//�L�[�{�[�h�}�N��
void CMyTreeDialog::OnIconKeymacro()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem, KIND_KEY);
}
//�v���O�C��
void CMyTreeDialog::OnIconPlugin()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem, KIND_PLUG);
}
//�֘A�t�����s�}�N��
void CMyTreeDialog::OnIconRelate()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem, KIND_RELATE);
}
//�I���e�L�X�g�}�N��
void CMyTreeDialog::OnIconSelect()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem, KIND_SELECT);
}

//---------------------------------------------------
//�֐���	OnMakeOnetime()
//�@�\		�����^�C���f�[�^�ɕύX
//---------------------------------------------------
void CMyTreeDialog::OnMakeOnetime()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (hTreeItem) {
		STRING_DATA* pData = m_pTreeCtrl->getDataPtr(hTreeItem);
		pData->m_cKind = KIND_ONETIME;
		m_pTreeCtrl->editData2(hTreeItem);
	}
}

//---------------------------------------------------
//�֐���	OnMakePermanent()
//�@�\		�P�v�f�[�^�ɕύX
//---------------------------------------------------
void CMyTreeDialog::OnMakePermanent()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (hTreeItem) {
		STRING_DATA* pData = m_pTreeCtrl->getDataPtr(hTreeItem);
		pData->m_cKind = KIND_LOCK;
		m_pTreeCtrl->editData2(hTreeItem);
	}
}

//---------------------------------------------------
//�֐���	OnFolderClear()
//�@�\		�I���t�H���_���N���A
//---------------------------------------------------
void CMyTreeDialog::OnFolderClear()
{
	if (m_isModal) {
		return;
	}
	m_isModal = true;
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (hTreeItem) {
		//�t�H���_���ċA�ō폜
		CString strRes;
		strRes.LoadString(APP_MES_FOLDER_CLEAR);
		int nRet = AfxMessageBox(strRes, MB_YESNO | MB_ICONEXCLAMATION | MB_APPLMODAL);
		RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
		if (nRet == IDYES) {
			HTREEITEM hChildItem = m_pTreeCtrl->GetChildItem(hTreeItem);
			m_pTreeCtrl->clearFolder(hChildItem);
		}
	}
	m_pTreeCtrl->SetFocus();
	m_isModal = false;
}

//---------------------------------------------------
//�֐���	OnAdd()
//�@�\		�V�K�f�[�^��ǉ�
//---------------------------------------------------
void CMyTreeDialog::OnNewData()
{
	if (m_isModal) {
		return;
	}
	m_isModal = true;
	STRING_DATA data;
	CEditDialog editDialog(this, &data, true);
	if (editDialog.DoModal() == IDOK) {
		HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
		m_pTreeCtrl->SelectItem(theApp.m_pTree->addData(hTreeItem, data));
	}
	m_isModal = false;
}

//---------------------------------------------------
//�֐���	OnNewFolder()
//�@�\		�V�K�t�H���_��ǉ�
//---------------------------------------------------
void CMyTreeDialog::OnNewFolder()
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	CString strRes;
	strRes.LoadString(APP_INF_NEW_FOLDER);
	m_pTreeCtrl->EditLabel(m_pTreeCtrl->addNewFolder(hTreeItem, strRes));
}

void CMyTreeDialog::OnReselectIcons()
{
	if (m_isModal) {
		return;
	}
	m_isModal = true;
	CString strRes;
	strRes.LoadString(APP_MES_DECIDE_ICONS);
	int nRet = AfxMessageBox(strRes, MB_YESNO | MB_ICONEXCLAMATION | MB_APPLMODAL);
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	if (nRet == IDYES) {
		m_pTreeCtrl->allIconCheck();
	}
	m_isModal = false;
}

void CMyTreeDialog::OnCleanupAllOnetime()
{
	m_pTreeCtrl->cleanupOneTimeItems(m_pTreeCtrl->GetRootItem());
}

void CMyTreeDialog::OnMakeAllOnetimePermanent()
{
	m_pTreeCtrl->cleanupOneTimeItems(m_pTreeCtrl->GetRootItem(), KIND_LOCK);
}

void CMyTreeDialog::OnCloseAll()
{
	m_pTreeCtrl->closeFolder(m_pTreeCtrl->GetRootItem());
}

//---------------------------------------------------
//�֐���	OnListSearch()
//�@�\		��������
//---------------------------------------------------
void CMyTreeDialog::OnListSearch()
{
	if (m_isModal) {
		return;
	}
	m_isModal = true;
	CSearchDialog SearchDlg(this);
	SearchDlg.m_strSearchKeywords = theApp.m_ini.m_strSearchKeywords;
	int nRet = SearchDlg.DoModal();
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	if (nRet == IDOK) {
		theApp.m_ini.m_nSearchTarget = SearchDlg.GetTarget();
		theApp.m_ini.m_nSearchLogic = SearchDlg.GetSearchLogic();
		theApp.m_ini.m_strSearchKeywords = SearchDlg.m_strSearchKeywords;

		HTREEITEM hSearchItem = m_pTreeCtrl->searchItem(theApp.m_ini.m_nSearchTarget, theApp.m_ini.m_nSearchLogic, theApp.m_ini.m_strSearchKeywords, NULL);
		if (hSearchItem) {
			m_pTreeCtrl->SelectItem(hSearchItem);
		}
	}
	m_isModal = false;
}

void CMyTreeDialog::OnCheckItem()
{
	HTREEITEM hSelItem = m_pTreeCtrl->GetSelectedItem();
	m_pTreeCtrl->checkItem(hSelItem);
	m_bCheckbox = true;
}

//---------------------------------------------------
//�֐���	OnCopyData()
//�@�\		�I���f�[�^���R�s�[
//---------------------------------------------------
void CMyTreeDialog::OnCopyData()
{
	HTREEITEM hTreeItem;
	hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(hTreeItem) {
		m_hCopyData = hTreeItem;
	}
}

//---------------------------------------------------
//�֐���	OnDataPaste()
//�@�\		�R�s�[�f�[�^��\��t��
//---------------------------------------------------
void CMyTreeDialog::OnDataPaste()
{
	if(m_hCopyData && !m_isModal) {
		HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
		STRING_DATA data = m_pTreeCtrl->getData(m_hCopyData);
		CString strRes;
		strRes.LoadString(APP_INF_COPY_APPEND);
		data.m_strTitle = data.m_strTitle + strRes;
		if(m_pTreeCtrl->ItemHasChildren(m_hCopyData)) {
			if(!m_pTreeCtrl->checkMyChild(m_hCopyData,hTreeItem)) {
				hTreeItem = m_pTreeCtrl->addData(hTreeItem,data);
				m_pTreeCtrl->copyChildren(m_hCopyData,hTreeItem);
			}
			else {
				m_isModal = true;
				CString strRes;
				strRes.LoadString(APP_MES_CANT_COPY);
				AfxMessageBox(strRes,MB_OK|MB_ICONEXCLAMATION|MB_APPLMODAL);
				m_pTreeCtrl->SetFocus();
				m_isModal = false;
			}
		}
		else {
			m_pTreeCtrl->addData(hTreeItem,data);
		}
	}
}

//---------------------------------------------------
//�֐���	OnImport()
//�@�\		�C���|�[�g����
//---------------------------------------------------
void CMyTreeDialog::OnImport()
{
	if (m_isModal) {
		return;
	}
	m_isModal = true;

	CString strDisplay, strPattern;
	strDisplay.LoadString(APP_INF_FILE_FILTER_C3D_DISPLAY);
	strPattern.LoadString(APP_INF_FILE_FILTER_C3D_PATTERN);
	CString strFilter = strDisplay + _T('\0') + strPattern + _T('\0');
	for (std::vector<RW_PLUGIN>::iterator it = m_pTreeCtrl->m_rwPlugin.begin(); it != m_pTreeCtrl->m_rwPlugin.end(); it++) {
		CString strFormat, strDisplay, strPattern;
		strFormat.LoadString(APP_INF_FILE_FILTER_FMT_DISPLAY);
		strDisplay.Format(strFormat, it->m_strFormatName, it->m_strExtension);
		strFormat.LoadString(APP_INF_FILE_FILTER_FMT_PATTERN);
		strPattern.Format(strFormat, it->m_strExtension);
		strFilter = strFilter + strDisplay + _T('\0') + strPattern + _T('\0'); // NOTE: Don't use operator +=
	}
	strFilter = strFilter + _T('\0') + _T('\0'); // NOTE: Don't use operator +=

	OPENFILENAME param;
	ZeroMemory(&param, sizeof param);
	TCHAR tcPath[MAX_PATH] = _T("");
	param.lStructSize = sizeof param;
	param.hwndOwner = theApp.m_hSelfWnd;
	param.lpstrFilter = strFilter.GetBuffer();
	param.lpstrCustomFilter = NULL;
	param.nFilterIndex = 1;
	param.lpstrFile = tcPath;
	param.nMaxFile = MAX_PATH;
	param.lpstrFileTitle = NULL;
	param.lpstrInitialDir = NULL;
	param.lpstrTitle = NULL;
	param.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	param.nFileOffset = 0;
	param.nFileExtension = 0;
	param.lpstrDefExt = NULL;
	if (GetOpenFileName(&param)) {
		if (theApp.m_ini.m_bDebug) {
			CString strText;
			strText.Format(_T("OnImport \"%s\"\n"), tcPath);
			CGeneral::writeLog(theApp.m_ini.m_strDebugLog, strText, _ME_NAME_, __LINE__);
		}

		CString format = param.nFilterIndex < 2 ? DAT_FORMAT : m_pTreeCtrl->m_rwPlugin[param.nFilterIndex - 2].m_strFormatName;
		std::list<STRING_DATA> tmplist;
		m_pTreeCtrl->loadDataFile(CString(tcPath), format, &tmplist);
		HTREEITEM hTreeItem = m_pTreeCtrl->mergeTreeData(m_pTreeCtrl->GetSelectedItem(), &tmplist, false);

		CString strRes;
		CString strMessage;
		strRes.LoadString(APP_MES_IMPORT_OK);
		strMessage.Format(strRes, tmplist.size());
		AfxMessageBox(strMessage, MB_OK | MB_APPLMODAL);

		if (hTreeItem) m_pTreeCtrl->SelectItem(hTreeItem);
	}
	m_pTreeCtrl->SetFocus();
	m_isModal = false;
}

//---------------------------------------------------
//�֐���	OnExport()
//�@�\		�G�N�X�|�[�g����
//---------------------------------------------------
void CMyTreeDialog::OnExport()
{
	if (m_isModal) {
		return;
	}
	m_isModal = true;
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if (hTreeItem) {
		CString file = theApp.NewFile();
		if (file != _T("")) {
			if (!m_pTreeCtrl->saveDataToFile(file, DAT_FORMAT, hTreeItem)) {
				CString strRes;
				strRes.LoadString(APP_MES_FAILURE_DATA_SAVE);
				AfxMessageBox(strRes, MB_ICONEXCLAMATION, 0);
			}
		}
	}
	m_pTreeCtrl->SetFocus();
	m_isModal = false;
}

//---------------------------------------------------
//�֐���	OnOption()
//�@�\		�ݒ�
//---------------------------------------------------
void CMyTreeDialog::OnOption()
{
	if(m_isModal) return;
	m_isModal = true;
	theApp.OnOption();
	m_isModal = false;
}

//---------------------------------------------------
//�֐���	selectByTyping(UINT uKeyCode)
//�@�\		"select by typing" ����
//---------------------------------------------------
bool CMyTreeDialog::selectByTyping(UINT uKeyCode)
{
	bool isRet = false;
	BYTE byteKey[256];
	GetKeyboardState(byteKey);//�L�[�{�[�h�̏�Ԃ��擾

	char strbuff[16];
	//�L�[�𕶎��ɕϊ� API�g�p
	if(ToAsciiEx(uKeyCode,0,byteKey,(LPWORD)strbuff,0,theApp.m_ini.m_keyLayout) == 1) {
		strbuff[1] = NULL;
		DWORD span = timeGetTime() - m_dwStartTime;
		m_dwStartTime = timeGetTime();
		if(span >= static_cast<DWORD>(theApp.m_ini.m_pop.m_nSelectByTypingFinalizePeriod)) {
			m_strQuickKey = "";//�L�[�A�^�C�}�[���Đݒ�
			if (theApp.m_ini.m_pop.m_bSelectByTypingAutoPaste || theApp.m_ini.m_pop.m_bSelectByTypingAutoExpand) {
				this->KillTimer(CHARU_QUICK_TIMER);
				this->SetTimer(CHARU_QUICK_TIMER, theApp.m_ini.m_pop.m_nSelectByTypingFinalizePeriod, NULL);//�^�C�}�[�Z�b�g
			}
		}
		CString strKey;
		strKey = strbuff;
		if (theApp.m_ini.m_pop.m_bSelectByTypingCaseInsensitive) {
			strKey.MakeLower();
		}
		m_strQuickKey = m_strQuickKey + strKey;
		m_hQuickItem = m_pTreeCtrl->GetSelectedItem();//�I���ʒu����Ɍ���
		if(!m_hQuickItem) m_hQuickItem = m_pTreeCtrl->GetRootItem();

		if(m_hQuickItem) {
			m_hQuickItem = m_pTreeCtrl->searchTitle(m_hQuickItem, m_strQuickKey, theApp.m_ini.m_pop.m_bSelectByTypingCaseInsensitive);//����
			if(m_hQuickItem) {
				m_pTreeCtrl->SelectItem(m_hQuickItem);
				isRet = true;//�W���̃C���N�������^���T�[�`���L�����Z��
			}
		}
	}
	return isRet;
}

//---------------------------------------------------
//�֐���	changeTipString(CString strData)
//�@�\		�����̃e�L�X�g���c�[���`�b�v�ɐݒ�
//---------------------------------------------------
void CMyTreeDialog::changeTipString(STRING_DATA data)
{
	CString strTip = _T("");
	CString strRes;
	bool gap = false;

	m_toolTip.Activate(FALSE);
	if (theApp.m_ini.m_visual.m_nToolTip == 0) {
		(void)strRes.LoadString(APP_INF_TIP_DATA01);
		CString s = data.m_strTitle;
		int max = 100;
		if (s.GetLength() > max) {
			s = s.Left(max - 3) + _T("...");
		}
		strTip += strRes + s;
		gap = true;
	}
	if (theApp.m_ini.m_visual.m_nToolTip != 2) {
		if (data.m_strData != _T("")) {
			if (gap) strTip += _T("\n\n");
			CString s = data.m_strData;
			(void)s.Replace(_T("\t"), _T(" "));
			int max = 500;
			if (s.GetLength() > max) {
				s = s.Left(max - 3) + _T("...");
			}
			strTip += s;
			gap = true;
		}
		if (data.m_strMacro != _T("")) {
			(void)strRes.LoadString(APP_INF_TIP_DATA02);
			CString s = data.m_strMacro;
			(void)s.Replace(_T("\t"), _T(" "));
			(void)s.Replace(_T("\n"), _T("\n  "));
			int max = 300;
			if (s.GetLength() > max) {
				s = s.Left(max - 3) + _T("...");
			}
			if (gap) strTip += _T("\n");
			strTip += strRes + s;
			gap = true;
		}
	}
	if (theApp.m_ini.m_visual.m_nToolTip == 0) {
		if (gap) strTip += _T("\n");
		(void)strRes.LoadString(APP_INF_TIP_DATA03);
		strTip += strRes + CTime(data.m_timeCreate).Format(_T("%x %X"));
		(void)strRes.LoadString(APP_INF_TIP_DATA04);
		strTip += strRes + CTime(data.m_timeEdit).Format(_T("%x %X"));
	}
	m_toolTip.UpdateTipText(strTip, m_pTreeCtrl); // NOTE: Experiments have shown that if strTip exceeds 1024 characters, the app will crash after MFC fails with Debug Assertion Failed.
	m_toolTip.Activate(TRUE);
}
