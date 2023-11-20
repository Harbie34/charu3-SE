#include "stdafx.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#include "key.h"

//---------------------------------------------------
//�֐���	HotkeyToMod(UINT hotkey)
//�@�\		�L�[�R�[�h�ϊ�
//---------------------------------------------------
UINT KeyHelper::HotkeyToMod(UINT hotkey)
{
    UINT mod = 0;
    if (hotkey & HOTKEYF_ALT)     mod |= MOD_ALT;
    if (hotkey & HOTKEYF_CONTROL) mod |= MOD_CONTROL;
    if (hotkey & HOTKEYF_SHIFT)	  mod |= MOD_SHIFT;
    return mod;
}

//---------------------------------------------------
//�֐���	ModToHotkey(UINT mod)
//�@�\		�L�[�R�[�h�ϊ�
//---------------------------------------------------
UINT KeyHelper::ModToHotkey(UINT mod)
{
    UINT hotkey = 0;
    if (mod & MOD_ALT)     hotkey |= HOTKEYF_ALT;
    if (mod & MOD_CONTROL) hotkey |= HOTKEYF_CONTROL;
    if (mod & MOD_SHIFT)   hotkey |= HOTKEYF_SHIFT;
    return hotkey;
}

//---------------------------------------------------
//�֐���	ModToVK(UINT uMod)
//�@�\		�L�[�R�[�h�ϊ�
//---------------------------------------------------
UINT KeyHelper::ModToVK(UINT mod)
{
    UINT vk = 0;
    if (mod & MOD_ALT)     vk = VK_MENU;
    if (mod & MOD_CONTROL) vk = VK_CONTROL;
    if (mod & MOD_SHIFT)   vk = VK_SHIFT;
    return vk;
}

//---------------------------------------------------
//�֐���	GetKeyName(UINT uVkCode,HKL keyLayout)
//�@�\		���z�L�[�̖��O�𓾂�
//---------------------------------------------------
CString KeyHelper::GetKeyName(UINT uVkCode, HKL keyLayout)
{
    CString strRet(_T(""));
    int nExtFlag = 0;
    int nScanCode = MapVirtualKeyEx(uVkCode, 0, keyLayout) << 16;
    if (nScanCode) {
        if (uVkCode == VK_APPS || uVkCode == VK_PRIOR || uVkCode == VK_NEXT || uVkCode == VK_END ||
            uVkCode == VK_HOME || uVkCode == VK_LEFT || uVkCode == VK_UP || uVkCode == VK_RIGHT ||
            uVkCode == VK_DOWN || uVkCode == VK_INSERT || uVkCode == VK_DELETE || uVkCode == VK_NUMLOCK) {
            nExtFlag = 1 << 24;
        }
        TCHAR strKeyName[256];
        GetKeyNameText(nScanCode | nExtFlag, strKeyName, _countof(strKeyName));
        strRet = strKeyName;
    }
    return strRet;
}
