/*----------------------------------------------------------
	�ݒ�Ǘ��N���X�w�b�_
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_INIT_H__EA6EFDDA_9745_4690_B6A9_8B0533E4AB56__INCLUDED_)
#define AFX_INIT_H__EA6EFDDA_9745_4690_B6A9_8B0533E4AB56__INCLUDED_

#include "Charu3.h"
#include "MyHotkeyCtrl.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;


//�ݒ�Z�N�V������`
#define REGKEY_ENV		_T("Environment")
#define REGKEY_WINDOW	_T("Windows")
#define REGKEY_SERCH	_T("Serch")

#define REGKEY_ETC		_T("EtcSetting")
#define REGKEY_POPUP	_T("PouupSetting")
#define REGKEY_FIFO		_T("FifoSetting")
#define REGKEY_VISUAL	_T("VisualSetting")
#define REGKEY_KEY		_T("KeySetting")

#define REGKEY_PLUGIN	_T("PluginSetting")
#define REGKEY_MACRO	_T("MacroTemplate")

//---------------------------------------------------
// ��ʐݒ�\����
//---------------------------------------------------
struct OPTION_ETC
{
	int m_nMulchUser;//�}���`���[�U�[�Ŏg�����ǂ��� // No longer used

	int m_nToClip;		//���X�g����I�񂾕�����̓N���b�v�{�[�h�ɂ������
	int m_nToolTip;	//�^�X�N�g���C�A�C�R���̃c�[���`�b�v���Œ�ɂ���
	int m_nMinimization;	//�ŏ�������

	int m_nIconClick;		//�A�C�R�����N���b�N�����Ƃ��̓���
	int m_nWordBugSW;		//Word�̃o�O�ɑΉ�����

	int m_nSelfDiagnosisTime;//���Ȑf�f�^�C�}�[
	int m_nActiveTime;		//�Ď��^�C�}�[

	int m_nDontSave;

	int m_nCopyWaitCnt;
	int m_nPasteWaitCnt;
};

//---------------------------------------------------
// �|�b�v�A�b�v�ݒ�\����
//---------------------------------------------------
struct OPTION_POPUP
{
	int  m_nSelectSW;		//�e�L�X�g���X�g�̑I���ʒu��ێ�����
	int  m_nFolderSW;		//�e�L�X�g���X�g�̃t�H���_��Ԃ�ێ�����
	int  m_nCornerPopup;		//�}�E�X�J�[�\������ʂ̒[�ɐG�ꂽ��|�b�v�A�b�v����
	int  m_nCornerPopupTime;	//�}�E�X�J�[�\������ʂ̒[�ɐG�ꂽ��|�b�v�A�b�v����
	int  m_nCornerPopupPix;	//�}�E�X�J�[�\������ʂ̒[�ɐG�ꂽ��|�b�v�A�b�v����

	int  m_nDoubleKeyPOP;
	UINT m_uVK_Pouup;	//�|�b�v�A�b�v�L�[
	UINT m_uMod_Pouup;	//�|�b�v�A�b�v����L�[
	int  m_nDCKeyPopTime;

	int  m_nDoubleKeyFIFO;
	UINT m_uVK_Fifo;	//����FIFO�؂�ւ��L�[
	UINT m_uMod_Fifo;	//����FIFO�؂�ւ�����L�[
	int  m_nDCKeyFifoTime;

	int  m_nPopupPos;		//�|�b�v�A�b�v�o���ʒu
	POINT m_posCaretHosei;	//�L�����b�g�ʒu�̕␳�l
	int  m_nOutRevice;

	int  m_nQuickEnter;		//�N�C�b�N�A�N�Z�X�I�����̓\��t��
	int  m_nQuickChar;		//�N�C�b�N�A�N�Z�X�ő啶������������ʂ��Ȃ�
	int  m_nQuickTime;		//�N�C�b�N�A�N�Z�X�m�莞��
	int  m_nAutoImeOff;

	int  m_nSingleExpand;
};

//---------------------------------------------------
// �X�g�b�N���[�h�ݒ�\����
//---------------------------------------------------
struct OPTION_FIFO
{
	int  m_nFifo;			//���𓮍쒆��Ctrl+V�œ\��t���镶����������o���ɂ���
	int  m_nCopySound;		//�����L�^���̓R�s�[���Ɍ��ʉ���炷
	CString m_strWaveFile;	//���ʉ��̃t�@�C����
	int  m_nOffClear;		//����OFF���Ɉꎞ���ڂ��N���A
	int  m_nNoClear;		//�������[�h�ōŌ�ɋ�ɂ��Ȃ��X�C�b�`
	int  m_nPasteSound;		//�\��t�����ɂ�����炷�X�C�b�`
	int  m_nAllClearOff;	//�ꎞ��������ɂȂ����痚�����[�h��OFF�ɂ���
	int  m_nDuplication;	//�d���`�F�b�N
};

//---------------------------------------------------
// �r�W���A���ݒ�\����
//---------------------------------------------------
struct OPTION_VISUAL
{
	int m_nSemitransparent;
	int m_nScrollbar;

	CString m_strFontName;//�t�H���g��
	int m_nFontSize;		//�t�H���g�T�C�Y

	CString m_strResourceName;//���\�[�X��
	uint32_t m_nBorderColor;		//�g�F
	uint32_t m_nBackColor;		//�w�i�F
	uint32_t m_nTextColor;		//�e�L�X�g�F
	int m_nResetTree;

	int m_nToolTip;			//�c�[���`�b�v�̕\������
};

struct WINDOWS_MESSAGE
{
	UINT Msg; // message to send
	WPARAM wParam; // first message parameter
	LPARAM lParam; // second message parameter
};

//---------------------------------------------------
// �L�[�ݒ�\����
//---------------------------------------------------
struct COPYPASTE_KEY
{
	int  m_nMessage;	//���b�Z�[�W�����t���O
	UINT m_uMod_Paste;	//�\��t������L�[
	UINT m_uVK_Paste;	//�\��t���L�[
	
	UINT m_uMod_Copy;	//�R�s�[����L�[
	UINT m_uVK_Copy;	//�R�s�[�L�[

	WINDOWS_MESSAGE m_copyMessage;
	WINDOWS_MESSAGE m_pasteMessage;

	int  m_nCopyWait;		//�R�s�[�҂�����
	int  m_nPasteWait;		//�y�[�X�g�҂�����
};
//---------------------------------------------------
// �����L�[�ύX�\����
//---------------------------------------------------
struct CHANGE_KEY
{
	CString m_strTitle;	//�^�C�g��������
	int		m_nMatch;
	COPYPASTE_KEY m_sCopyPasteKey;
	int  m_nHistoryLimit;	//�N���b�v�{�[�h�e�ʐ���
};
//---------------------------------------------------
// �L�[�ݒ�\����
//---------------------------------------------------
struct OPTION_KEYSET
{
	int  m_nHistoryLimit;	//�N���b�v�{�[�h�e�ʐ���
	COPYPASTE_KEY	m_defKeySet;
	list<CHANGE_KEY> m_KeyList;//�L�[�ݒ�f�[�^���X�g
};

//---------------------------------------------------
// �}�N���ݒ�\����
//---------------------------------------------------
struct MACRO_STRUCT
{
	int  m_cKind;
	CString m_strName;
	CString m_strMacro;
};

//---------------------------------------------------
// �N���X��`
//---------------------------------------------------
class CInit  
{
public:
	CInit();
	virtual ~CInit();
	void initialize();
	void setIniFileName(CString strFileName) {
		m_strIniFile = strFileName;
	}

	void readAllInitData();
	CString setIniFileString(const TCHAR* szSection,const TCHAR* szKey,CString strDefault);
	int setIniFileInt(const TCHAR* szSection,const TCHAR* szKey,int nDefault);

	void writeAllInitData();
	void writeEnvInitData();
	void writeProfileInt(const TCHAR* szSection,const TCHAR* szKey,int nValue);
	void writeProfileString(const TCHAR* szection,const TCHAR* szKey,CString strValue);

	COPYPASTE_KEY getAppendKeyInit(CString strWinName,int nNumber = 0);
	CHANGE_KEY getAppendKeyInit2(CString strWinName);
	void setHotkey(UINT uPopKey,UINT uPopMod,UINT uFifoKey,UINT uFifoMod);
	void getHotKey(UINT *uPopKey,UINT *uPopMod,UINT *uFifoKey,UINT *uFifoMod);
	void setPasteHotkey(UINT uKey,UINT uMod,UINT uCopyKey,UINT uCopyMod);
	void getPasteHotKey(UINT *uKey,UINT *uMod,UINT *uCopyKey,UINT *uCopyMod);

	bool setHookKey(HWND hWnd);
	void unHookKey();

	//OS�̎푰����(NT40��false)
	bool isNotNt40() {
		// Windows 9x and its variants are no longer supported.
		return false;
	}
	bool isNt40() {
		// Assume running on Windows NT 4.0 or later.
		return true;
	}
	bool isWindows98() {
		// Windows 9x and its variants are no longer supported.
		return false;
	}

	//��{���
	CString m_strAppPath;	//���s�p�X
	CString m_strUserName;	//���[�U�[��
	CString m_strIniFile;	//ini�t�@�C����
	CString m_strDataFile;	//�f�[�^�t�@�C����
	CString m_strRwPluginFolder;//�v���O�C���t�H���_
	CString m_strPluginName;//�t�H�[�}�b�g��
	CString m_strMacroPluginName;//�}�N���`����
	CString m_strBBS;//�T�|�[�gBBSURL
	HKL     m_keyLayout;

	int  m_nSelectID;		//�I�����Ă�ID
	int  m_nTreeID;
	int  m_nOptionPage;
	int  m_nRecNumber;
	int  m_nToolTipTime,m_nToolTipDelay;
	POINT m_DialogSize,m_IconSize;	//�_�C�A���O�̃T�C�Y

	int  m_nSerchKind;		//�������
	int  m_nSerchLogic;		//�������@
	CString m_strSerchKey;//�����L�[

	int m_nDebug;
	CString m_strDebugLog;

	vector<MACRO_STRUCT>  m_vctMacro;
	vector<MACRO_STRUCT>  m_vctDataMacro;

	//�ݒ���
	OPTION_ETC		m_etc;
	OPTION_POPUP	m_pop;
	OPTION_FIFO		m_fifo;
	OPTION_VISUAL	m_visual;
	OPTION_KEYSET	m_key;

	HMODULE m_hHookDLL;
};

#endif // !defined(AFX_INIT_H__EA6EFDDA_9745_4690_B6A9_8B0533E4AB56__INCLUDED_)
