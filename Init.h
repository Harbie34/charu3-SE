/*----------------------------------------------------------
    �ݒ�Ǘ��N���X�w�b�_
                                    2002/11/16 (c)Keizi
----------------------------------------------------------*/

#if !defined(AFX_INIT_H__EA6EFDDA_9745_4690_B6A9_8B0533E4AB56__INCLUDED_)
#define AFX_INIT_H__EA6EFDDA_9745_4690_B6A9_8B0533E4AB56__INCLUDED_

// TODO: Consistent naming

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <vector>
#include "nlomann/json.hpp"
#include "jsonHelper.h"

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

#define SETTINGS_SECTION_GENERAL "GeneralSettings"
#define SETTINGS_SECTION_STYLE "StyleSettings"
#define SETTINGS_SECTION_TREEVIEW "TreeViewSettings"

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
    int  m_nMessage;	//���b�Z�[�W����
    UINT m_uMod_Paste;	//�\��t������L�[
    UINT m_uVK_Paste;	//�\��t���L�[
    UINT m_uMod_Copy;	//�R�s�[����L�[
    UINT m_uVK_Copy;	//�R�s�[�L�[

    WINDOWS_MESSAGE m_copyMessage;
    WINDOWS_MESSAGE m_pasteMessage;

    int  m_nCopyWait;		//�R�s�[�҂�����
    int  m_nPasteWait;		//�y�[�X�g�҂�����

    COPYPASTE_KEY()
        : m_nMessage(0)
        , m_uMod_Paste(MOD_CONTROL)
        , m_uVK_Paste('V')
        , m_uMod_Copy(MOD_CONTROL)
        , m_uVK_Copy('C')
        , m_copyMessage(WINDOWS_MESSAGE())
        , m_pasteMessage(WINDOWS_MESSAGE())
        , m_nCopyWait(50)
        , m_nPasteWait(50)
    {
    }

    COPYPASTE_KEY(nlohmann::json& obj)
        : m_nMessage(0)
        , m_uMod_Paste(MOD_CONTROL)
        , m_uVK_Paste('V')
        , m_uMod_Copy(MOD_CONTROL)
        , m_uVK_Copy('C')
        , m_copyMessage(WINDOWS_MESSAGE())
        , m_pasteMessage(WINDOWS_MESSAGE())
        , m_nCopyWait(50)
        , m_nPasteWait(50)
    {
        if (obj.is_object()) {
            m_nMessage = static_cast<int>(jsonHelper::GetNumberProperty(obj, "method", 0));
            m_uVK_Copy = static_cast<UINT>(jsonHelper::GetNumberProperty(obj, "copy.keyCode", 'C'));
            m_uMod_Copy = static_cast<UINT>(jsonHelper::GetNumberProperty(obj, "copy.keyModifier", MOD_CONTROL));
            m_nCopyWait = static_cast<int>(jsonHelper::GetNumberProperty(obj, "copy.delay", 50));
            m_uVK_Paste = static_cast<UINT>(jsonHelper::GetNumberProperty(obj, "paste.keyCode", 'V'));
            m_uMod_Paste = static_cast<UINT>(jsonHelper::GetNumberProperty(obj, "paste.keyModifier", MOD_CONTROL));
            m_nPasteWait = static_cast<int>(jsonHelper::GetNumberProperty(obj, "paste.delay", 50));

            nlohmann::json msg;
            msg = obj["copy.message"];
            if (msg.is_object()) {
                m_copyMessage.Msg = static_cast<UINT>(jsonHelper::GetNumberProperty(msg, "id", 0));
                m_copyMessage.wParam = static_cast<WPARAM>(jsonHelper::GetNumberProperty(msg, "param1", 0));
                m_copyMessage.lParam = static_cast<LPARAM>(jsonHelper::GetNumberProperty(msg, "param2", 0));
            }
            msg = obj["paste.message"];
            if (msg.is_object()) {
                m_pasteMessage.Msg = static_cast<UINT>(jsonHelper::GetNumberProperty(msg, "id", 0));
                m_pasteMessage.wParam = static_cast<WPARAM>(jsonHelper::GetNumberProperty(msg, "param1", 0));
                m_pasteMessage.lParam = static_cast<LPARAM>(jsonHelper::GetNumberProperty(msg, "param2", 0));
            }
        }
    }

    nlohmann::json ToJson() {
        nlohmann::json obj;
        obj["method"] = m_nMessage;
        obj["copy.keyCode"] = m_uVK_Copy;
        obj["copy.keyModifier"] = m_uMod_Copy;
        obj["copy.delay"] = m_nCopyWait;
        obj["paste.keyCode"] = m_uVK_Paste;
        obj["paste.keyModifier"] = m_uMod_Paste;
        obj["paste.delay"] = m_nPasteWait;

        nlohmann::json msg;
        msg["id"] = m_copyMessage.Msg;
        msg["param1"] = m_copyMessage.wParam;
        msg["param2"] = m_copyMessage.lParam;
        obj["copy.message"] = msg;
        msg["id"] = m_pasteMessage.Msg;
        msg["param1"] = m_pasteMessage.wParam;
        msg["param2"] = m_pasteMessage.lParam;
        obj["paste.message"] = msg;

        return obj;
    }
};

//---------------------------------------------------
// �����L�[�ύX�\����
//---------------------------------------------------
struct CHANGE_KEY
{
    CString m_strTitle;	//�^�C�g��������
    int m_nMatch{};

    COPYPASTE_KEY m_sCopyPasteKey;
    int m_nHistoryLimit{};	//�N���b�v�{�[�h�e�ʐ���
};

//---------------------------------------------------
// �}�N���ݒ�\����
//---------------------------------------------------
struct MACRO_STRUCT
{
    int  m_cKind{};
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
    ~CInit();
    void initialize();

    void writeAllInitData();
    void writeEnvInitData();

    COPYPASTE_KEY getAppendKeyInit(CString strWinName, int nNumber = 0);
    CHANGE_KEY getAppendKeyInit2(CString strWinName);
    void setHotkey(UINT uPopKey, UINT uPopMod, UINT uFifoKey, UINT uFifoMod);
    void getHotKey(UINT* uPopKey, UINT* uPopMod, UINT* uFifoKey, UINT* uFifoMod);
    void setPasteHotkey(UINT uKey, UINT uMod, UINT uCopyKey, UINT uCopyMod);
    void getPasteHotKey(UINT* uKey, UINT* uMod, UINT* uCopyKey, UINT* uCopyMod);

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

    //
    // environment
    //
    CString m_strUserName;
    CString m_strAppPath;
    CString m_strRwPluginFolder;
    CString m_strAppDataPath;
    POINT m_IconSize;
    CString m_strStateFile;
    CString m_strSettingsFile;
    CString m_strDebugLog;
    HKL m_keyLayout;

    //
    // state (reset on every startup)
    //
    int m_nOptionPage;

    //
    // state (to be saved)
    //
    nlohmann::json m_state;  // json with all of the following for load/save

    CString m_strDataPath;  // data file path
    CString m_strDataFormat;  // data format
    bool m_bReadOnly{};  // whether access to data files should be read-only
    POINT m_treeviewSize{};
    POINT m_editWindowSize{};
    int m_nSearchTarget{};
    int m_nSearchLogic{};
    bool m_bSearchCaseInsensitive{};
    CString m_strSearchKeywords;
    int m_nSelectID{};
    int m_nTreeID{};
    int m_nRecNumber{};

    //
    // settings
    //
    nlohmann::json m_settings;  // json with all of the following for load/save

    // no setting UI
    bool m_bDebug{};
    int m_nToolTipTime{};
    int m_nToolTipDelay{};
    int m_nDCKeyPopTime{};
    int m_nDCKeyFifoTime{};

    // General
    int  m_nDoubleKeyPOP{};
    UINT m_uVK_Pouup{};	//�|�b�v�A�b�v�L�[
    UINT m_uMod_Pouup{};	//�|�b�v�A�b�v����L�[
    int  m_nDoubleKeyFIFO{};
    UINT m_uVK_Fifo{};	//����FIFO�؂�ւ��L�[
    UINT m_uMod_Fifo{};	//����FIFO�؂�ւ�����L�[
    int m_nIconClick{};		// �ʒm�̈�A�C�R�����N���b�N�����Ƃ��̓���
    bool m_bPutBackClipboard{}; // �f�[�^�c���[����̃R�s�[���y�[�X�g��A�N���b�v�{�[�h�ɈȑO�̓��e���Z�b�g������
    bool m_bShowClipboardInTooltipOfNofifyIcon{}; // �ʒm�̈�A�C�R���̃c�[���`�b�v�ŃN���b�v�{�[�h�̓��e��\������

    // Style
    CString m_strFontName;
    int m_nFontSize{};
    CString m_strResourceName;
    uint32_t m_nBorderColor{};
    uint32_t m_nBackgroundColor{};
    uint32_t m_nTextColor{};
    int m_nOpacity{};

    // Data Tree View
    int  m_nPopupPos{};		//�|�b�v�A�b�v�o���ʒu
    POINT m_posCaretHosei{};	//�L�����b�g�ʒu�̕␳�l
    int m_nSelectByTypingFinalizePeriod{};
    bool m_bSelectByTypingCaseInsensitive{};
    bool m_bSelectByTypingAutoPaste{};
    bool m_bSelectByTypingAutoExpand{};
    int m_nWheelBehavior;
    int m_nToolTip{};
    bool m_bScrollbarVertical{};
    bool m_bScrollbarHorizontal{};
    bool m_bSingleExpand{};
    bool m_bSingleEnter{};
    bool m_bKeepSelection{};  // Keep selection on data tree view
    bool m_bKeepFolders{};  // Keep expanded/collapsed status of folders

    // Stock Mode
    int  m_nFifo{};			//���𓮍쒆��Ctrl+V�œ\��t���镶����������o���ɂ���
    CString m_strCopySound;	// �R�s�[���ɍĐ�����T�E���h�̃p�X��
    CString m_strPasteSound;	// �\��t�����ɍĐ�����T�E���h�̃p�X��
    bool m_bCleanupAtTurnOff{};  // Remove all one-time items when turned off
    bool m_bAutoOff{};  // Turn off stock mode when one-time item is gone
    bool m_bDontSaveSameDataAsLast{};

    // Key
    COPYPASTE_KEY m_defKeySet;
    int m_nHistoryLimit{};  //�N���b�v�{�[�h�e�ʐ���
    std::list<CHANGE_KEY> m_KeyList;

    // Advancced
    int m_nClipboardOpenDelay{};
    int m_nClipboardRetryInterval{};
    int m_nClipboardRetryTimes{};
    int m_nWindowCheckInterval{};
    std::vector<CString> m_stealth;

    //
    // Snippets
    //
    std::vector<MACRO_STRUCT>  m_vctMacro;

    //
    // Option properties
    //
    std::vector<MACRO_STRUCT>  m_vctDataMacro;

private:
    CString m_locale;
    HMODULE m_hHookDLL;
    bool m_bIsPortableMode{};

    void SaveSettings();
};

#endif // !defined(AFX_INIT_H__EA6EFDDA_9745_4690_B6A9_8B0533E4AB56__INCLUDED_)
