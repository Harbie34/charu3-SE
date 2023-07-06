/*----------------------------------------------------------
	Charu3Tree�N���X�w�b�_
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_CHARU3TREE_H__246081ED_82BD_4EFE_AA98_067AC36E58E6__INCLUDED_)
#define AFX_CHARU3TREE_H__246081ED_82BD_4EFE_AA98_067AC36E58E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Charu3Tree.h : �w�b�_�[ �t�@�C��
//

#include <list>
#include <vector>

//�f�[�^���
#define KIND_ONETIME 0x01	//�����^�C������
#define KIND_LOCK    0x02	//�m�[�}������
#define KIND_FOLDER  0x04	//�t�H���_����
#define KIND_RIREKI  0x08	//�����t�H���_

#define KIND_DATA_ALL	 0x03	//�f�[�^
#define KIND_FOLDER_ALL  0x0C	//�t�H���_

#define FOLDER_OPEN 0x10	//�t�H���_�I�[�v���}�X�N

//�f�[�^�A�C�R�����
#define KIND_DEFAULT 0  //�A�C�R���w�薳��

#define KIND_DATE	1	//���t
#define KIND_EXE    2	//���s
#define KIND_RELATE 3	//�֘A�t��
#define KIND_SELECT 4	//�I��
#define KIND_CLIP   5	//�N���b�v�{�[�h
#define KIND_PLUG   6   //�v���O�C��
#define KIND_KEY    7   //�L�[�}�N��

//---------------------------------------------------
// �v���O�C���\����
//---------------------------------------------------
struct RW_PLUGIN
{
	CString m_strPluginFilePath; // Pathname of the plugin
	CString m_strFormatName; // Data format name
	CString m_strExtension; // Extension of data file
	RW_PLUGIN(CString path, TCHAR* format, TCHAR* ext) : m_strPluginFilePath(CString(path)), m_strFormatName(CString(format)), m_strExtension(CString(ext)) {}
	RW_PLUGIN() {}
};

//---------------------------------------------------
// �f�[�^�\����
//---------------------------------------------------
struct STRING_DATA
{
	char	m_cKind;		//�f�[�^���
	char	m_cIcon;		//�A�C�R�����
	int		m_nMyID;		//�f�[�^��ID
	int		m_nParentID;	//�e�f�[�^��ID
	time_t	m_timeCreate;	//�쐬���� 32bit
	time_t	m_timeEdit;		//�ύX���� 32bit
	CString  m_strTitle;	//�ݒ�f�[�^�^�C�g��
	CString  m_strData;		//�ݒ�f�[�^������
	CString  m_strMacro;	//�g���p������f�[�^

	STRING_DATA() : m_cKind(KIND_LOCK), m_cIcon(0), m_nMyID(0), m_nParentID(0), m_timeCreate(0), m_timeEdit(0)
		, m_strTitle(_T("")), m_strData(_T("")), m_strMacro(_T("")) {}
};

typedef bool (*writeDataFile)(CString strFileName, std::list<STRING_DATA> *pList);
typedef std::list<STRING_DATA> * (*readDataFile)(CString strFileName,bool isImport);
typedef bool (*convertMacro)(TCHAR *szSource,TCHAR *szRet,int nSize,TCHAR *szSelect,TCHAR *szClip);
typedef void (*getExtension)(TCHAR *szBuff,int nSize);
typedef void (*getFormatName)(TCHAR *szBuff,int nSize);
typedef void (*endDLL)();
typedef void (*initDLL)();
typedef bool (*isUnicode)();

//---------------------------------------------------
// CCharu3Tree �E�B���h�E
//---------------------------------------------------
class CCharu3Tree : public CTreeCtrl
{
// �R���X�g���N�V����
public:
	CCharu3Tree();
	virtual ~CCharu3Tree();

public:
	void setPlugin(CString strPath);
	bool getPlugin(CString strName,RW_PLUGIN* pPlugin);
	void setImageList(POINT posSize,CString strFileName,CString strPath);

	HTREEITEM addNewFolder(HTREEITEM hTreeItem, CString strName);
	HTREEITEM addData(HTREEITEM hTreeItem, STRING_DATA Data, bool isNewID = true, bool isChild = false);
	void editData(HTREEITEM hTreeItem, STRING_DATA Data);
	void editData2(HTREEITEM hTreeItem);
	void deleteData(HTREEITEM hTreeItem);
	void clearFolder(HTREEITEM hItem);
	void closeFolder(HTREEITEM hStartItem);
	void checkFolder(HTREEITEM hStartItem, bool isCheck, std::list<HTREEITEM> *listItem);
	void cleanupOneTimeItems(HTREEITEM hStartItem,int nKind = 0);
	void ClearChecks();

	void changeIcon(HTREEITEM hTreeItem,int nID);
	void tree2List(HTREEITEM hStartItem, std::list<STRING_DATA> *tmplist, bool isAll = false);
	void data2TreeStruct(TV_INSERTSTRUCT *pTreeCtrlItem, std::list<STRING_DATA>::iterator it);
	std::list<STRING_DATA>::iterator findData(STRING_DATA* dataPtr);

	STRING_DATA getData(HTREEITEM hTreeItem);
	STRING_DATA* getDataPtr(HTREEITEM hTreeItem);

	HTREEITEM searchItem(int nKind,int nLogic,CString strKey,HTREEITEM hStartItem);
	HTREEITEM searchTitle(HTREEITEM hStartItem, CString strKey, bool makeLower = false);
	HTREEITEM getTrueNextItem(HTREEITEM hTreeItem);
	HTREEITEM getLastVisibleItem();
	HTREEITEM getFirstFolder(HTREEITEM hStartItem);
	HTREEITEM getLastChild(HTREEITEM hStartItem);

	void copyChildren(HTREEITEM hFromItem,HTREEITEM hToItem);
	void moveChildren(HTREEITEM hFromItem,HTREEITEM hToItem);

	int  getIconNumber(char cKind,char cIcon);
	int  getChildCount(HTREEITEM hTreeItem,bool isBrotherOnly = false);
	char getDatakind(HTREEITEM hTreeItem){
		char cRet = 0;
		if(hTreeItem) {
			STRING_DATA data = getData(hTreeItem);
			cRet = data.m_cKind;
		}
		return cRet;
	}

	void deleteExcessChildren(HTREEITEM hTreeItem,int *nCount);
	void copyData(int nParentID,HTREEITEM hParentTreeItem, std::list<STRING_DATA> *pList);
	HTREEITEM mergeTreeData(HTREEITEM hTreeItem, std::list<STRING_DATA> *pList, bool isRoot);
	int mergeList(std::list<STRING_DATA> *pMainList, std::list<STRING_DATA> *pList, int nParent);

	bool loadDataFileDef(CString strFileName,CString strPlugin);
	bool LoadDataWithPlugin(CString strFileName, CString strPlugin, std::list<STRING_DATA> *tmplist);
	bool loadDataFile(CString strFileName, CString strPlugin, std::list<STRING_DATA> *tmplist);
	void normalizationID(std::list<STRING_DATA>* pList, int nParentID);
	bool convertMacroPlugin(STRING_DATA *SourceData,CString *strRet,CString strSelect,CString strClip,CString strSoftName);

	bool SaveDataWithPlugin(CString strFileName, CString strPlugin, std::list<STRING_DATA> *tmplist);
	bool saveDataToFile(CString strFileName,CString strPlugin,HTREEITEM hStartItem);
	int  makeNewID(){
		(*m_nMaxID)++;
		while(checkRedundancyID(*m_nMaxID)) {
			(*m_nMaxID) += 10;
		}
		return *m_nMaxID;
	}

	void setInitInfo(int *nMaxID,int *nSelectID,int *nRecNumber){
		m_nMaxID = nMaxID;
		m_nSelectID = nSelectID;
		m_nRecNumber = nRecNumber;
	}
	void setSelectID(int nID){*m_nSelectID = nID;}

	bool checkRedundancyID(int nID);
	bool checkMyChild(HTREEITEM hMeItem,HTREEITEM hChildItem);
	HTREEITEM searchMyRoots(HTREEITEM hStartItem);
	HTREEITEM searchParentOption(HTREEITEM hStartItem,CString strOption);

	void allIconCheck();
	char decideIcon(CString strData);
	CString makeTitle(CString strData,int nTitleLength = 32);
	HTREEITEM getOneTimeText(int nType);

	int getDataOption(CString strData,CString strKind);
	DWORD getDataOptionHex(CString strData,CString strKind);
	CString getDataOptionStr(CString strData,CString strKind);
	void addDataToRecordFolder(STRING_DATA data,CString strClipBkup);
	HTREEITEM moveFolderTop(HTREEITEM hTreeItem);
	void classHistoryFolder(HTREEITEM hTreeItem,int nRirekiCount);
	void checkItem(HTREEITEM hItem);
	void checkOut(HTREEITEM hItem);

	std::list<STRING_DATA> m_MyStringList;//�f�[�^���X�g
	std::vector<RW_PLUGIN>  m_rwPlugin;
	std::list<HTREEITEM> m_ltCheckItems;
	void setScrollBar();
	bool IsDragging() { return m_dragState != DRAGSTATE_NOT_DRAGGING; }

protected:
	int *m_nMaxID;
	int *m_nRecNumber;

	// �I�y���[�V����
public:
	CWnd *m_pParent;
	// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CCharu3Tree)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

private:
	enum {
		DRAGSTATE_NOT_DRAGGING,
		DRAGSTATE_INSERT_BEFORE,
		DRAGSTATE_PLACE_INSIDE,
		DRAGSTATE_INSERT_AFTER
	} m_dragState;
	ULONGLONG m_nHoverTick;
	HTREEITEM m_hDragItem;
	HTREEITEM m_hDragTarget;
	HTREEITEM m_hPrevTarget;
	CImageList* m_pDragImage;

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	CImageList *m_ImageList;//�C���[�W���X�g
	int *m_nSelectID;

	//{{AFX_MSG(CCharu3Tree)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_CHARU3TREE_H__246081ED_82BD_4EFE_AA98_067AC36E58E6__INCLUDED_)
