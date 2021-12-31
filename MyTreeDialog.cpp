/*----------------------------------------------------------
	MyTreeDialog�N���X
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "Charu3.h"
#include "MyTreeDialog.h"
#include "AddDialog.h"
#include "EditDialog.h"
#include "General.h"
#include "MyFileDialog.h"
#include "SerchDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCharu3App theApp;

//---------------------------------------------------
//�֐���	CMyTreeDialog
//�@�\		�R���X�g���N�^
//---------------------------------------------------
CMyTreeDialog::CMyTreeDialog(CWnd* pParent /*=NULL*/) : CDialog(CMyTreeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyTreeDialog)
	//}}AFX_DATA_INIT
	m_selectIT_valid = false;
	m_brBack.m_hObject = NULL;

	m_isInitOK = false;
	m_isModal = true;
	m_isAltDown = false;
	m_itDbClick_valid = false;
	m_hCopyData = nullptr;
	m_dwStartTime = 0;

	m_hDLL = NULL;
	m_pExStyle = NULL;
	m_cOlgFont = nullptr;

	m_strQuickKey = "";
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
	ON_COMMAND(IDML_ADD, OnAdd)
	ON_COMMAND(IDML_CHANGE_ONETIME, OnChangeOnetime)
	ON_COMMAND(IDML_CHANGE_LOCK, OnChangeLock)
	ON_COMMAND(IDML_COPY_DATA, OnCopyData)
	ON_COMMAND(IDML_DATA_PASTE, OnDataPaste)
	ON_COMMAND(IDML_DELETE, OnDelete)
	ON_COMMAND(IDML_EDIT, OnEdit)
	ON_COMMAND(IDML_EXPORT, OnExport)
	ON_COMMAND(IDML_FOLDER_CLEAR, OnFolderClear)
	ON_COMMAND(IDML_ICON_CLIP, OnIconClip)
	ON_COMMAND(IDML_ICON_DATE, OnIconDate)
	ON_COMMAND(IDML_ICON_EXE, OnIconExe)
	ON_COMMAND(IDML_ICON_KEY, OnIconKey)
	ON_COMMAND(IDML_ICON_KEYMACRO, OnIconKeymacro)
	ON_COMMAND(IDML_ICON_PLUGIN, OnIconPlugin)
	ON_COMMAND(IDML_ICON_RELATE, OnIconRelate)
	ON_COMMAND(IDML_ICON_SELECT, OnIconSelect)
	ON_COMMAND(IDML_IMPORT, OnImport)
	ON_COMMAND(IDML_LIST_SERCH, OnListSerch)
	ON_COMMAND(IDML_NEW_FOLDER, OnNewFolder)
	ON_COMMAND(IDML_OPTION, OnOption)
	ON_NOTIFY(NM_RCLICK, IDC_MY_TREE, OnRclickMyTree)
	ON_NOTIFY(NM_CLICK, IDC_MY_TREE, OnClickMyTree)
	ON_NOTIFY(TVN_KEYDOWN, IDC_MY_TREE, OnKeydownMyTree)
	ON_NOTIFY(NM_KILLFOCUS, IDC_MY_TREE, OnKillfocusMyTree)
	ON_WM_SHOWWINDOW()
    ON_WM_NCPAINT()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_MY_TREE, OnBeginlabeleditMyTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_MY_TREE, OnEndlabeleditMyTree)
	ON_COMMAND(IDML_CHECK_ITEM, OnCheckItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------
//�֐���	PouupMenu(CPoint point)
//�@�\		�|�b�v�A�b�v���j���[���o��
//---------------------------------------------------
void CMyTreeDialog::pouupMenu(CPoint point)
{
	if(m_isModal || !m_isInitOK) return;
	CPoint HitPoint;

	UINT tmp;
	HTREEITEM hTreeItem;

	//�J�[�\���ʒu�̍��ڂ�I��
	HitPoint = point;
	this->ScreenToClient(&HitPoint);
	hTreeItem = m_pTreeCtrl->HitTest(HitPoint,&tmp);

	//���j���[���o��
	if(hTreeItem) {
		list<STRING_DATA>::iterator it;
		m_pTreeCtrl->SelectItem(hTreeItem);
		m_pTreeCtrl->getData(hTreeItem,&it);

		m_PopupMenu.EnableMenuItem(IDML_CHANGE_LOCK,!(it->m_cKind & KIND_ONETIME));
		m_PopupMenu.EnableMenuItem(IDML_CHANGE_ONETIME,!(it->m_cKind & KIND_LOCK));
		m_PopupMenu.EnableMenuItem(IDML_FOLDER_CLEAR,!(it->m_cKind & KIND_FOLDER_ALL));
		m_PopupMenu.EnableMenuItem(IDML_EDIT,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_DELETE,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_LIST_SERCH,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_EXPORT,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_KEY,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_DATE,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_EXE,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_RELATE,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_SELECT,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_CLIP,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_PLUGIN,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_KEYMACRO,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_COPY_DATA,FALSE);
		m_PopupMenu.EnableMenuItem(IDML_DATA_PASTE,(BOOL)(!m_hCopyData));
		m_PopupMenu.EnableMenuItem(IDML_CHECK_ITEM,FALSE);
	}
	else {
		m_PopupMenu.EnableMenuItem(IDML_CHANGE_LOCK,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_CHANGE_ONETIME,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_EDIT,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_DELETE,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_FOLDER_CLEAR,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_LIST_SERCH,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_EXPORT,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_KEY,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_DATE,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_EXE,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_RELATE,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_SELECT,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_CLIP,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_PLUGIN,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_ICON_KEYMACRO,TRUE);

		m_PopupMenu.EnableMenuItem(IDML_COPY_DATA,TRUE);
		m_PopupMenu.EnableMenuItem(IDML_DATA_PASTE,(BOOL)m_hCopyData);
		m_PopupMenu.EnableMenuItem(IDML_CHECK_ITEM,TRUE);
	}
	m_PopupMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,point.x, point.y, this);
}

//---------------------------------------------------
//�֐���	ShowWindowPos(POINT pos,int nCmdShow)
//�@�\		�E�B���h�E���ړ����ĕ\��
//---------------------------------------------------
BOOL CMyTreeDialog::showWindowPos(POINT pos,POINT size,int nCmdShow,bool isSelect,HTREEITEM hOpenItem)
{
	m_pTreeCtrl->OnWindowPosChanging(NULL);
	m_toolTip.SetDelayTime(theApp.m_ini.m_nToolTipDelay);
	m_toolTip.SetDelayTime(TTDT_AUTOPOP,theApp.m_ini.m_nToolTipTime);
	
	m_pTreeCtrl->SetBkColor(COLORREF(CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBackColor)));
	m_pTreeCtrl->SetTextColor(COLORREF(CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nTextColor)));
	m_pTreeCtrl->SetInsertMarkColor(COLORREF(CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBorderColor)));
	m_pTreeCtrl->ModifyStyle(NULL,TVS_TRACKSELECT,NULL);
	if(theApp.m_ini.m_pop.m_nSingleExpand) {
		m_pTreeCtrl->ModifyStyle(NULL,TVS_SINGLEEXPAND,NULL);
	}
	else {
		m_pTreeCtrl->ModifyStyle(TVS_SINGLEEXPAND,NULL,NULL);
	}


	HTREEITEM hSelectItem = m_pTreeCtrl->GetSelectedItem();
	if(!isSelect)	m_pTreeCtrl->SelectItem(m_pTreeCtrl->GetRootItem());
	if(!theApp.m_ini.m_pop.m_nFolderSW)	{
		m_pTreeCtrl->closeFolder(m_pTreeCtrl->GetRootItem());
		if(isSelect) m_pTreeCtrl->SelectItem(hSelectItem);
	}
	else if(hSelectItem && !hOpenItem) {
		RECT rSelItem;
		m_pTreeCtrl->GetItemRect(hSelectItem, &rSelItem,NULL);
		if(size.y < rSelItem.top  || rSelItem.bottom < 0)
			m_pTreeCtrl->EnsureVisible(hSelectItem);//EnsureVisible����ƃt�H���_�͊J�����
	}
	if(hOpenItem) m_pTreeCtrl->Expand(hOpenItem,TVE_EXPAND);

	m_colFrame   = CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBorderColor);
	m_colFrameL  = CGeneral::upDownLight(CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBorderColor),1.2);
	m_colFrameD  = CGeneral::upDownLight(CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBorderColor),0.8);

	if(m_brBack.GetSafeHandle()) {
		m_brBack.DeleteObject();
		m_brBack.m_hObject = NULL;
	}
	if(m_brBack.m_hObject == NULL)
		m_brBack.CreateSolidBrush(COLORREF(CGeneral::RGB2BGR(theApp.m_ini.m_visual.m_nBackColor)));
	
	CGeneral::setAbsoluteForegroundWindow(theApp.m_pMainFrame->m_hWnd);
	m_pTreeCtrl->SetFocus();

	m_selectIT_valid = false;
	m_isInitOK = true;
	m_isModal = false;
	m_isAltDown = false;
	m_itDbClick_valid = false;

	m_pTreeCtrl->m_ltCheckItems.clear();
	if(theApp.m_ini.m_visual.m_nToolTip == 2)	m_toolTip.Activate(FALSE);
	else	m_toolTip.Activate(TRUE);
	SetWindowPos(&wndTopMost,pos.x,pos.y,size.x,size.y,NULL);
	m_pTreeCtrl->setScrollBar();

	return ShowWindow(nCmdShow);
}


//---------------------------------------------------
//�֐���	changeTipString(CString strData)
//�@�\		�����̃e�L�X�g���c�[���`�b�v�ɐݒ�
//---------------------------------------------------
void CMyTreeDialog::changeTipString(STRING_DATA data) 
{
	CString strTip,strData;
	CString strRes;
	CTime timeC(data.m_timeCreate),timeE(data.m_timeEdit);
	
	m_toolTip.Activate(FALSE);

	if(theApp.m_ini.m_visual.m_nToolTip == 0) {
		strData = data.m_strTitle.Left(1024);
		strData.Replace(_T("	"),_T(""));
		strRes.LoadString(APP_INF_TIP_DATA01);
		strTip = strRes + strData + _T("\n\n");
	}
	if(theApp.m_ini.m_visual.m_nToolTip != 2) {
		strData = data.m_strData.Left(4096);
		strData.Replace(_T("	"),_T(""));
		strData.TrimLeft();
		strData.TrimRight();
		if(strData.GetLength() > 512) strTip = strTip + strData.Left(511) + _T("\n");
		else strTip = strTip + strData + _T("\n");
		if(data.m_strMacro != _T("")) {
			data.m_strMacro.Replace(_T("\n"),_T("\n  "));
			strRes.LoadString(APP_INF_TIP_DATA02);
			strTip = strTip + strRes + data.m_strMacro;
		}
	}
	if(theApp.m_ini.m_visual.m_nToolTip == 0) {
		strRes.LoadString(APP_INF_TIP_DATA03);
		strTip = strTip + strRes;
		strTip = strTip + timeC.Format(_T("%Y/%m/%d-%H:%M:%S"));
		strRes.LoadString(APP_INF_TIP_DATA04);
		strTip = strTip + strRes;
		strTip = strTip + timeE.Format(_T("%Y/%m/%d-%H:%M:%S"));
	}
	m_toolTip.UpdateTipText(strTip,m_pTreeCtrl);
	m_toolTip.Activate(TRUE);
}


//---------------------------------------------------
// CMyTreeDialog ���b�Z�[�W �n���h��
//---------------------------------------------------
void CMyTreeDialog::OnNcPaint(void)
{
    CDialog::OnNcPaint();

    CRect rect;
    GetWindowRect(rect);
    rect -= rect.TopLeft();

    CWindowDC dc(this);
    drawFrame(&dc, rect);
}

//---------------------------------------------------
//�֐���	drawFrame(CDC* pDC, CRect& rect)
//�@�\		�g��`��
//---------------------------------------------------
void CMyTreeDialog::drawFrame(CDC* pDC, CRect& rect)
{
	CPoint point[3];
	COLORREF colRD[5];
	COLORREF colLU[5];

	colLU[0] = m_colFrameL;
	colLU[1] = m_colFrame;
	colLU[2] = m_colFrameD;
	
	colRD[0] = m_colFrameD;
	colRD[1] = m_colFrameL;
	colRD[2] = m_colFrame;
	for(int i = 1; i <= 3; i++) {
		point[0].x = rect.left + i;
		point[0].y = rect.bottom - i;
		point[1].x = rect.right - i;
		point[1].y = rect.bottom - i;
		point[2].x = rect.right - i;
		point[2].y = rect.top + i - 1;
	    drawLline(pDC, point, colRD[i-1]);
	}

	for(int i = 0; i <= 2; i++) {
		point[0].x = rect.left + i;
		point[0].y = rect.bottom - i - 1;
		point[1].x = rect.left + i;
		point[1].y = rect.top + i;
		point[2].x = rect.right - i;
		point[2].y = rect.top + i;
	    drawLline(pDC, point, colLU[i]);
	}
}

//---------------------------------------------------
//�֐���	drawLline(CDC* pDC, CPoint* point, COLORREF col)
//�@�\		����`��
//---------------------------------------------------
void CMyTreeDialog::drawLline(CDC* pDC, CPoint* point, COLORREF col)
{
    CPen pen(PS_SOLID, 1, col);
    CPen* old_pen = pDC->SelectObject(&pen);
    pDC->MoveTo(point[0]);
    pDC->LineTo(point[1]);
    pDC->LineTo(point[2]);
    pDC->SelectObject(old_pen);
}

//---------------------------------------------------
//�֐���	OnSize(UINT nType, int cx, int cy)
//�@�\		���T�C�Y
//---------------------------------------------------
void CMyTreeDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);	
	if(m_pTreeCtrl->m_hWnd) {
		RECT rect;
		this->GetWindowRect(&rect);
		m_pTreeCtrl->SetWindowPos(&wndTop,0,0,cx,cy,SWP_NOMOVE);
		m_pTreeCtrl->setScrollBar();
	}
}

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
	m_pExStyle = (PFUNC)::GetProcAddress(m_hDLL,"SetLayeredWindowAttributes");
	m_PopupMenu.LoadMenu(MAKEINTRESOURCE(IDR_LISTMENU));//���j���[�N���X�Ƀ��j���[��ǂ�
	return TRUE;
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
	if(m_isModal || m_pTreeCtrl->isDrag()) return;
	POINT point;
	::GetCursorPos(&point);
	pouupMenu(point);	
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
	if(hClickItem) {
		if(m_pTreeCtrl->GetItemRect(hClickItem,&ItemRect,true)) {
			//�A�C�R�����N���b�N����
			if(TVHT_ONITEMICON & Flags) {
				STRING_DATA data = m_pTreeCtrl->getData(hClickItem);
				if(data.m_cKind & KIND_ONETIME) data.m_cKind = KIND_LOCK;
				m_pTreeCtrl->editData(hClickItem,data);
			}
			//�`�F�b�N�{�b�N�X
			if(TVHT_ONITEMSTATEICON & Flags) {
				m_pTreeCtrl->checkItem(hClickItem);
				m_pTreeCtrl->SetCheck(hClickItem,!m_pTreeCtrl->GetCheck(hClickItem));
			}
		}
		m_pTreeCtrl->SelectItem(hClickItem);
	}
	*pResult = 0;
}

//---------------------------------------------------
//�֐���	enterData(list<STRING_DATA>::iterator it)
//�@�\		�f�[�^�����肵�ă_�C�A���O���B��
//---------------------------------------------------
void CMyTreeDialog::enterData(list<STRING_DATA>::iterator it)
{		
	m_selectIT = it;
	m_selectIT_valid = true;
	::PostMessage(theApp.getAppWnd(),WM_TREE_CLOSE,IDOK,NULL);
	m_isInitOK = false;
	this->KillTimer(CHARU_QUICK_TIMER);
}

void CMyTreeDialog::closePopup()
{
	::PostMessage(theApp.getAppWnd(),WM_TREE_CLOSE,IDCANCEL,NULL);
	m_isInitOK = false;
	KillTimer(CHARU_QUICK_TIMER);
}

//---------------------------------------------------
//�֐���	OnAdd()
//�@�\		�f�[�^�ǉ�����
//---------------------------------------------------
void CMyTreeDialog::OnAdd() 
{
	if(m_isModal) return;
	m_isModal = true;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();

	CAddDialog addDialog(this,hTreeItem);
	addDialog.setMacroTempate(&theApp.m_ini.m_vctMacro,&theApp.m_ini.m_vctDataMacro);

	//�ǉ��_�C�A���O���J��
	int nRet = addDialog.DoModal();
	m_isModal = false;		
}

//---------------------------------------------------
//�֐���	OnChangeOnetime()
//�@�\		�ꎞ���ڂɕύX
//---------------------------------------------------
void CMyTreeDialog::OnChangeOnetime() 
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(hTreeItem) {
		STRING_DATA data = m_pTreeCtrl->getData(hTreeItem);

		data.m_cKind = KIND_ONETIME;
		m_pTreeCtrl->editData(hTreeItem,data);
	}
}

//---------------------------------------------------
//�֐���	OnChangeLock()
//�@�\		���b�N���ڂɕύX
//---------------------------------------------------
void CMyTreeDialog::OnChangeLock() 
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(hTreeItem) {
		STRING_DATA data = m_pTreeCtrl->getData(hTreeItem);

		data.m_cKind = KIND_LOCK;
		m_pTreeCtrl->editData(hTreeItem,data);
	}
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
//�֐���	OnDelete()
//�@�\		�I���f�[�^���폜
//---------------------------------------------------
void CMyTreeDialog::OnDelete() 
{
	list<STRING_DATA>::iterator it;
	STRING_DATA data;
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(!hTreeItem) return;

	if(m_isModal) return;
	m_isModal = true;

	data = m_pTreeCtrl->getData(hTreeItem);
	CString strMessage;
	strMessage.Format(APP_MES_DELETE_OK,data.m_strTitle);
	int nRet = AfxMessageBox(strMessage,MB_YESNO|MB_ICONEXCLAMATION|MB_APPLMODAL);

	if(nRet == IDYES) {
		if(data.m_cKind & KIND_FOLDER_ALL) {
			//�t�H���_���ċA�ō폜
			CString strRes;
			strRes.LoadString(APP_MES_DELETE_FOLDER);
			int nRet = AfxMessageBox(strRes,MB_YESNO|MB_ICONEXCLAMATION|MB_APPLMODAL);
			if(nRet != IDYES) {
				m_pTreeCtrl->SetFocus();
				m_isModal = false;		
				return;
			}
		}
		m_pTreeCtrl->deleteData(hTreeItem);
	}
	m_pTreeCtrl->SetFocus();
	m_isModal = false;		
}

//---------------------------------------------------
//�֐���	OnEdit()
//�@�\		�I���f�[�^��ҏW
//---------------------------------------------------
void CMyTreeDialog::OnEdit() 
{
	if(m_isModal) return;
	m_isModal = true;
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	STRING_DATA data = m_pTreeCtrl->getData(hTreeItem);

	CEditDialog dlgEdit(this,data);
	dlgEdit.setMacroTempate(&theApp.m_ini.m_vctMacro,&theApp.m_ini.m_vctDataMacro);

	if(dlgEdit.DoModal() == IDOK) {
		data = dlgEdit.m_data;
		m_pTreeCtrl->editData(hTreeItem,data);
	}
	m_isModal = false;		
}

//---------------------------------------------------
//�֐���	OnImport()
//�@�\		�C���|�[�g����
//---------------------------------------------------
void CMyTreeDialog::OnImport() 
{
	if(m_isModal) return;
	m_isModal = true;
	
	CString strFilter,strBuff,strPluginName;

	strFilter = "";
	RW_PLUGIN plugin;
	vector<RW_PLUGIN>::iterator it;
	CString strRes;
	strRes.LoadString(APP_INF_FILE_FILTER2);
	for(it = m_pTreeCtrl->m_rwPlugin.begin(); it != m_pTreeCtrl->m_rwPlugin.end(); it++) {
		strBuff.Format(strRes,it->m_strSoftName,it->m_strExtension,it->m_strExtension);
		strFilter = strFilter + strBuff;
	}

	CMyFileDialog *pFileDialog;
	strRes.LoadString(APP_INF_FILE_FILTER);
	pFileDialog = new CMyFileDialog(TRUE,_T("c3d"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strRes + _T("|") + strFilter,NULL,TRUE);

	if(pFileDialog) {
		pFileDialog->m_ofn.lpstrInitialDir = theApp.m_ini.m_strAppPath;
		strRes.LoadString(APP_INF_CHECKBOX_IMPORT);
		if(IDOK == pFileDialog->DoModal(FALSE,strRes)) {
			//�K���v���O�C��������
			bool isFound = false;
			OPENFILENAME fileName;
			list<STRING_DATA> tmplist;
			CString strMessage;

			//�\�t�g����ݒ�
			fileName = pFileDialog->getFileName();
			if(fileName.nFilterIndex == 1)
				strPluginName = DAT_FORMAT;
			else
				strPluginName = m_pTreeCtrl->m_rwPlugin[fileName.nFilterIndex - 2].m_strSoftName;

			m_pTreeCtrl->loadDataFile(pFileDialog->GetPathName(),strPluginName,&tmplist);//�ǂݍ���

			//���X�g�ƃc���[�ɒǉ�����
			HTREEITEM hTreeItem = m_pTreeCtrl->mergeTreeData(m_pTreeCtrl->GetSelectedItem(),&tmplist,pFileDialog->m_isAutoMacro);
			strRes.LoadString(APP_MES_IMPORT_OK);
			strMessage.Format(strRes,tmplist.size());
			AfxMessageBox(strMessage,MB_OK|MB_APPLMODAL);
			if(hTreeItem) 	m_pTreeCtrl->SelectItem(hTreeItem);//�Ώۂ�I��
		}
		delete pFileDialog;
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
	if(m_isModal) return;
	m_isModal = true;
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	//�I���A�C�e���̃n���h�������
	if(hTreeItem) {
		CFileDialog *pFileDialog;
		CString strFilter;
		strFilter.LoadString(APP_INF_FILE_FILTER);
		pFileDialog = new CFileDialog(FALSE,_T("c3d"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilter + _T("||"));

		if(pFileDialog) {
			pFileDialog->m_ofn.lpstrInitialDir = theApp.m_ini.m_strAppPath;
			//�t�@�C���I���_�C�A���O���o��
			if(IDOK == pFileDialog->DoModal()) {
				m_pTreeCtrl->saveDataFile(pFileDialog->GetPathName(),DAT_FORMAT,hTreeItem);	
			}
			delete pFileDialog;
		}
	}	
	m_pTreeCtrl->SetFocus();
	m_isModal = false;
}

//---------------------------------------------------
//�֐���	OnFolderClear()
//�@�\		�I���t�H���_���N���A
//---------------------------------------------------
void CMyTreeDialog::OnFolderClear() 
{
	if(m_isModal) return;
	m_isModal = true;
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(hTreeItem) {
		//�t�H���_���ċA�ō폜
		CString strRes;
		strRes.LoadString(APP_MES_FOLDER_CLEAR);
		int nRet = AfxMessageBox(strRes,MB_YESNO|MB_ICONEXCLAMATION|MB_APPLMODAL);
		if(nRet == IDYES) {
			HTREEITEM hChildItem = m_pTreeCtrl->GetChildItem(hTreeItem);
			m_pTreeCtrl->clearFolder(hChildItem);
		}
	}
	m_pTreeCtrl->SetFocus();
	m_isModal = false;
}

//---------------------------------------------------
//�֐���	OnIcon�`
//�@�\		�f�[�^�A�C�R����ύX
//---------------------------------------------------


//�N���b�v�{�[�h�}�N��
void CMyTreeDialog::OnIconClip() 
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem,KIND_CLIP);
}
//���t�}�N��
void CMyTreeDialog::OnIconDate() 
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem,KIND_DATE);
}
//�V�F���}�N��
void CMyTreeDialog::OnIconExe() 
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem,KIND_EXE);
}
//���ʍ���
void CMyTreeDialog::OnIconKey() 
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem,0);
}
//�L�[�{�[�h�}�N��
void CMyTreeDialog::OnIconKeymacro() 
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem,KIND_KEY);
}
//�v���O�C��
void CMyTreeDialog::OnIconPlugin() 
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem,KIND_PLUG);
}
//�֘A�t�����s�}�N��
void CMyTreeDialog::OnIconRelate() 
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem,KIND_RELATE);
}
//�I���e�L�X�g�}�N��
void CMyTreeDialog::OnIconSelect() 
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if(hTreeItem)	m_pTreeCtrl->changeIcon(hTreeItem,KIND_SELECT);
}

//---------------------------------------------------
//�֐���	OnListSerch()
//�@�\		��������
//---------------------------------------------------
void CMyTreeDialog::OnListSerch() 
{
	if(m_isModal) return;
	m_isModal = true;
	CSerchDialog SerchDlg(this);

	SerchDlg.m_strSerchKey = theApp.m_ini.m_strSerchKey;
	int nRet = SerchDlg.DoModal();
	if(nRet == IDOK) {
		theApp.m_ini.m_nSerchKind  = SerchDlg.m_nSerchKind;
		theApp.m_ini.m_nSerchLogic = SerchDlg.m_nSerchLogic;
		theApp.m_ini.m_strSerchKey = SerchDlg.m_strSerchKey;

		HTREEITEM hSerchItem = m_pTreeCtrl->serchItem(theApp.m_ini.m_nSerchKind,theApp.m_ini.m_nSerchLogic,theApp.m_ini.m_strSerchKey,NULL);
		if(hSerchItem) {
			m_pTreeCtrl->SelectItem(hSerchItem);
		}
	}
	m_isModal = false;
}

//---------------------------------------------------
//�֐���	OnNewFolder()
//�@�\		�V�����t�H���_�����
//---------------------------------------------------
void CMyTreeDialog::OnNewFolder() 
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	CString strRes;
	strRes.LoadString(APP_INF_NEW_FOLDER);
	m_pTreeCtrl->EditLabel(m_pTreeCtrl->addNewFolder(hTreeItem,strRes));
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
//�֐���	OnKillfocusMyTree(NMHDR* pNMHDR, LRESULT* pResult)
//�@�\		�c���[�̃t�H�[�J�X���O�ꂽ��B��
//---------------------------------------------------
void CMyTreeDialog::OnKillfocusMyTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_isInitOK && !m_isModal) {
	
		::PostMessage(theApp.getAppWnd(),WM_TREE_CLOSE,IDCANCEL,NULL);
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
	if(bShow) {
		//����������
		LONG lExStyle = ::GetWindowLong(this->m_hWnd,GWL_EXSTYLE);
		if(m_pExStyle && theApp.m_ini.m_visual.m_nTransparentSW) {
			SetWindowLong(this->m_hWnd,GWL_EXSTYLE,lExStyle|0x80000);
			int nTansparent = 255 * theApp.m_ini.m_visual.m_nSemitransparent / 100;
			m_pExStyle(this->m_hWnd,0,nTansparent,2);
		}
		else {
			SetWindowLong(this->m_hWnd,GWL_EXSTYLE,lExStyle & 0xfff7ffff);
		}
		m_cOlgFont = m_pTreeCtrl->GetFont();
		m_cFont = new CFont;
		if(m_cFont) {
			m_cFont->CreatePointFont(theApp.m_ini.m_visual.m_nFontSize,theApp.m_ini.m_visual.m_strFontName);
			m_pTreeCtrl->SetFont(m_cFont,TRUE);
		}
	}
	else if(m_cFont) {
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
//�֐���	PreTranslateMessage(MSG* pMsg)
//�@�\		���b�Z�[�W�O����
//---------------------------------------------------
BOOL CMyTreeDialog::PreTranslateMessage(MSG* pMsg) 
{
	m_toolTip.RelayEvent(pMsg);
	//����w�ߎ�M
	if(pMsg->message == WM_TREE_CLOSE) {
		closePopup();
		return TRUE;
	}
	//���{�^���_�u���N���b�N
	else if(pMsg->message == WM_LBUTTONDBLCLK && !m_pTreeCtrl->isDrag()){
		if(theApp.m_ini.m_pop.m_nQuickEnter) KillTimer(CHARU_QUICK_TIMER);
		//�f�[�^�擾
		HTREEITEM hTreeItem;
		list<STRING_DATA>::iterator it;
		hTreeItem = m_pTreeCtrl->GetSelectedItem();
		if(hTreeItem) {
			STRING_DATA data;
			data = m_pTreeCtrl->getData(hTreeItem,&it);
			if(!(it->m_cKind & KIND_FOLDER_ALL)) {//�t�H���_����Ȃ���Ό���
				m_itDbClick = it;
				m_itDbClick_valid = true;
			}
		}
	}
	else if(pMsg->message == WM_LBUTTONUP && m_itDbClick_valid && !m_pTreeCtrl->isDrag()) {
		enterData(m_itDbClick);
		return TRUE;
	}
	//ALT�����j���[�L�[�|�b�v�A�b�v���j���[���o��
	if(pMsg->message == WM_SYSKEYDOWN) m_isAltDown = true;
	else if(((pMsg->message == WM_SYSKEYUP && m_isAltDown) || (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_APPS)) && !theApp.isCloseKey() && !m_pTreeCtrl->isDrag()) {
		if(theApp.m_ini.m_pop.m_nQuickEnter) KillTimer(CHARU_QUICK_TIMER);
		m_isAltDown = false;
		CPoint point;
		ZeroMemory(&point,sizeof(point));
		RECT rSelItem;
		HTREEITEM hSelectItem = m_pTreeCtrl->GetSelectedItem();//�I���ʒu���擾
		if (hSelectItem) {
			m_pTreeCtrl->GetItemRect(hSelectItem, &rSelItem,NULL);
			point.x = 0;
			point.y = rSelItem.bottom - 5;
		}
		ClientToScreen(&point);
		pouupMenu(point);
//		return true;
	}

	if( pMsg->message == WM_KEYDOWN) {
		if(!m_pTreeCtrl->isDrag()) {
			MSG msg;
			ZeroMemory(&msg,sizeof(msg));
			msg.message =WM_LBUTTONDOWN;
			msg.hwnd = this->m_hWnd;
			m_toolTip.RelayEvent(&msg);

			::PostMessage(m_pTreeCtrl->m_hWnd,WM_MOUSEMOVE,0,0);
			::SetCursor(NULL);
		}
		//ESC�ŕ���
		if(pMsg->wParam == VK_ESCAPE && !m_isModal && !m_pTreeCtrl->isDrag()) {
			closePopup();
			return TRUE;
		}
		//���x���ҏW���Ȃ�ҏW�I��
		else if(pMsg->wParam == VK_RETURN && m_isModal) {
			m_pTreeCtrl->SetFocus();
			return TRUE;
		}
		//���^�[���L�[���������猈��
		else if(pMsg->wParam == VK_RETURN && !m_pTreeCtrl->isDrag()) {
			if(theApp.m_ini.m_pop.m_nQuickEnter) KillTimer(CHARU_QUICK_TIMER);
			//�f�[�^�擾
			HTREEITEM hTreeItem;
			hTreeItem = m_pTreeCtrl->GetSelectedItem();
			if(hTreeItem) {
				list<STRING_DATA>::iterator it;
				m_pTreeCtrl->getData(hTreeItem,&it);

				//�t�H���_���m�F
				if(!(it->m_cKind & KIND_FOLDER_ALL) || (m_pTreeCtrl->GetStyle() & TVS_CHECKBOXES && m_pTreeCtrl->GetCheck(hTreeItem))) {
					enterData(it);//�f�[�^������
				}
				else 
					m_pTreeCtrl->Expand(hTreeItem,TVE_TOGGLE);
				return TRUE;
			}
		}
		//�X�y�[�X�L�[
		else if(pMsg->wParam == VK_SPACE && !m_isModal) {
			if(theApp.m_ini.m_pop.m_nQuickEnter) KillTimer(CHARU_QUICK_TIMER);
			HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
			if(hTreeItem != NULL) {//�I������Ă邩?
				if(::GetKeyState(VK_CONTROL) < 0) {//CTRL��������Ă���
					HTREEITEM hSelItem  = m_pTreeCtrl->GetSelectedItem();
					m_pTreeCtrl->checkItem(hSelItem);
					hSelItem = m_pTreeCtrl->GetNextVisibleItem(hTreeItem);
					if(hSelItem) hTreeItem = hSelItem;
				}
				else if(::GetKeyState(VK_SHIFT) < 0) {//�V�t�g��������Ă���
					hTreeItem = m_pTreeCtrl->GetPrevVisibleItem(hTreeItem);//�O�̃A�C�e�����擾
					if(!hTreeItem) {
						hTreeItem = m_pTreeCtrl->getLastVisibleItem();//��ԏ�܂ōs������Ō�Ƀ��[�v
					}
				}
				else {
					hTreeItem = m_pTreeCtrl->GetNextVisibleItem(hTreeItem);
					if(!hTreeItem) hTreeItem = m_pTreeCtrl->GetRootItem();
				}
				m_pTreeCtrl->SelectItem(hTreeItem);
			}
			else {//�I������ĂȂ�������0�Ԗڂ�I��
				m_pTreeCtrl->Select(m_pTreeCtrl->GetRootItem(),TVGN_FIRSTVISIBLE);
			}
			return true;
		}
		//�f���[�g�L�[(�f�[�^�폜)
		else if(pMsg->wParam ==  VK_DELETE && !m_pTreeCtrl->isDrag() && !m_isModal) {
			if(theApp.m_ini.m_pop.m_nQuickEnter) KillTimer(CHARU_QUICK_TIMER);
			OnDelete();
		}
		//F1�L�[ ���e�\��
		else if(pMsg->wParam ==  VK_F1 && !m_pTreeCtrl->isDrag()) {
			CPoint point;
			ZeroMemory(&point,sizeof(point));
			RECT rSelItem;
			HTREEITEM hSelectItem = m_pTreeCtrl->GetSelectedItem();//�I���ʒu���擾
			if (hSelectItem) {
				m_pTreeCtrl->GetItemRect(hSelectItem, &rSelItem,true);
				point.x = rSelItem.left + ((rSelItem.right - rSelItem.left) / 2);
				point.y = rSelItem.top + ((rSelItem.bottom - rSelItem.top) / 2);

				MSG msg;
				ZeroMemory(&msg,sizeof(msg));
				msg.message = WM_MOUSEMOVE;
				msg.wParam = NULL;
				msg.lParam = point.y;
				msg.lParam = (msg.lParam << 16) + point.x;
				msg.hwnd = m_pTreeCtrl->m_hWnd;

				::PostMessage(msg.hwnd,msg.message,msg.wParam,msg.lParam);
				ClientToScreen(&point);
				::SetCursorPos(point.x,point.y);
			}
		}
		//F2�L�[ ���l�[��
		else if(pMsg->wParam ==  VK_F2 && !m_pTreeCtrl->isDrag()) {
			if(theApp.m_ini.m_pop.m_nQuickEnter) KillTimer(CHARU_QUICK_TIMER);
			if(m_pTreeCtrl->GetSelectedItem())
				m_pTreeCtrl->EditLabel(m_pTreeCtrl->GetSelectedItem());
		}
		//F3�L�[
		else if(pMsg->wParam ==  VK_F3 && !m_pTreeCtrl->isDrag() && !m_isModal) {
			if(::GetKeyState(VK_SHIFT) < 0) {
				OnListSerch();
				return true;
			}
			else {
				HTREEITEM hSerchItem = m_pTreeCtrl->GetSelectedItem();
				hSerchItem = m_pTreeCtrl->serchItem(theApp.m_ini.m_nSerchKind,theApp.m_ini.m_nSerchLogic,theApp.m_ini.m_strSerchKey,hSerchItem);
				if(hSerchItem) 	m_pTreeCtrl->SelectItem(hSerchItem);
			}
		}
		//TAB�L�[�Ń`�F�b�N
		else if(pMsg->wParam ==  VK_TAB && !m_pTreeCtrl->isDrag() && !m_isModal) {
			HTREEITEM hSelItem  = m_pTreeCtrl->GetSelectedItem();
			m_pTreeCtrl->checkItem(hSelItem);
			return true;
		}
		//�㉺
		else if(pMsg->wParam ==  VK_DOWN || pMsg->wParam ==  VK_UP && !m_isModal) {
			if(theApp.m_ini.m_pop.m_nQuickEnter) KillTimer(CHARU_QUICK_TIMER);

			HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem(),hTreeItemTmp;
			if(hTreeItem != NULL && ::GetKeyState(VK_SHIFT) < 0) {//�I������Ă��āASHIFT�������Ă�
				do {
					hTreeItemTmp = hTreeItem;
					if(pMsg->wParam ==  VK_DOWN)	hTreeItem = m_pTreeCtrl->GetNextVisibleItem(hTreeItem);
					else							hTreeItem = m_pTreeCtrl->GetPrevVisibleItem(hTreeItem);
					if(m_pTreeCtrl->GetChildItem(hTreeItem)) break;
				}while(hTreeItem);
				if(hTreeItem) {
					m_pTreeCtrl->SelectItem(hTreeItem);
					return true;
				}
			}
		}
		//��
		else if(pMsg->wParam ==  VK_LEFT && !m_isModal) {
			if(theApp.m_ini.m_pop.m_nQuickEnter) KillTimer(CHARU_QUICK_TIMER);

			HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
			if(hTreeItem  && ::GetKeyState(VK_SHIFT) < 0) {//�I������Ă��āASHIFT�������Ă�
				hTreeItem = m_pTreeCtrl->serchMyRoots(hTreeItem);
				if(hTreeItem) m_pTreeCtrl->SelectItem(hTreeItem);
			}
		}
		//�N�C�b�N�A�N�Z�X����
		else if(!m_isModal && !m_pTreeCtrl->isDrag()){
			if(quickAccess((UINT)pMsg->wParam)) return TRUE;
		}
	}
	//�N�C�b�N�m�菈��
	else if(pMsg->message == WM_TIMER && pMsg->wParam == CHARU_QUICK_TIMER && !m_pTreeCtrl->isDrag()){
		if(!m_hQuickItem) {
			this->KillTimer(CHARU_QUICK_TIMER);
			return FALSE;
		}
		//�f�[�^�擾
		list<STRING_DATA>::iterator it;
		STRING_DATA data = m_pTreeCtrl->getData(m_hQuickItem,&it);
		if(!(data.m_cKind & KIND_FOLDER_ALL)) {//�t�H���_����Ȃ����
			enterData(it);
		}
		else {
			m_pTreeCtrl->Expand(m_hQuickItem,TVE_EXPAND);
			this->KillTimer(CHARU_QUICK_TIMER);
		}
		return TRUE;
	}
	//TIPS�̕ύX
	else if(WM_TIPS_CHANGE == pMsg->message) {
		HTREEITEM hTarget = (HTREEITEM)pMsg->wParam;
		if(hTarget) {
			STRING_DATA data = m_pTreeCtrl->getData(hTarget);
			changeTipString(data);
		}
		else {
			m_toolTip.Activate(FALSE);
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

//---------------------------------------------------
//�֐���	quickAccess(UINT uKeyCode)
//�@�\		�N�C�b�N�A�N�Z�X����
//---------------------------------------------------
bool CMyTreeDialog::quickAccess(UINT uKeyCode)
{
	bool isRet = false;
	BYTE byteKey[256];
	GetKeyboardState(byteKey);//�L�[�{�[�h�̏�Ԃ��擾

	char strbuff[16];
	//�L�[�𕶎��ɕϊ� API�g�p
	if(ToAsciiEx(uKeyCode,0,byteKey,(LPWORD)strbuff,0,theApp.m_ini.m_keyLayout) == 1) {
		strbuff[1] = NULL;
		DWORD dwNow = timeGetTime();
		dwNow -= m_dwStartTime;
		m_dwStartTime = timeGetTime();
		if(dwNow > (DWORD)theApp.m_ini.m_pop.m_nQuickTime) {//�m�莞�Ԃ��߂���
			m_strQuickKey = "";//�L�[�A�^�C�}�[���Đݒ�
			if(theApp.m_ini.m_pop.m_nQuickEnter) {
				this->KillTimer(CHARU_QUICK_TIMER);
				this->SetTimer(CHARU_QUICK_TIMER,theApp.m_ini.m_pop.m_nQuickTime,NULL);//�^�C�}�[�Z�b�g
			}
		}
		CString strKey;
		strKey = strbuff;
		if(theApp.m_ini.m_pop.m_nQuickChar) {//�啶������������ʂ��Ȃ�
			strKey.MakeLower();//�������ɂ���
		}
		m_strQuickKey = m_strQuickKey + strKey;
		m_hQuickItem = m_pTreeCtrl->GetSelectedItem();//�I���ʒu����Ɍ���
		if(!m_hQuickItem) m_hQuickItem = m_pTreeCtrl->GetRootItem();

		if(m_hQuickItem) {
			m_hQuickItem = m_pTreeCtrl->serchTitle(m_hQuickItem,m_strQuickKey,theApp.m_ini.m_pop.m_nQuickChar);//����
			if(m_hQuickItem) {
				m_pTreeCtrl->SelectItem(m_hQuickItem);
				isRet = true;//�W���̃C���N�������^���T�[�`���L�����Z��
			}
		}
	}
	return isRet;
}


//---------------------------------------------------
//�֐���	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//�@�\		�w�i�F��ς���
//---------------------------------------------------
HBRUSH CMyTreeDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if(m_brBack.GetSafeHandle()) hbr = m_brBack;
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
	TV_ITEM* item = &(pTVDispInfo->item );
	if(item->pszText && *(item->pszText)) {
		HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
		STRING_DATA Data = m_pTreeCtrl->getData(hTreeItem);
		Data.m_strTitle = item->pszText;
		m_pTreeCtrl->editData(hTreeItem,Data);
	}
	m_isModal = false;		
	
	*pResult = 0;
}

void CMyTreeDialog::OnCheckItem() 
{
	HTREEITEM hSelItem  = m_pTreeCtrl->GetSelectedItem();
	m_pTreeCtrl->checkItem(hSelItem);
}
