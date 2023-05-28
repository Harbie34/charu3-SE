/*----------------------------------------------------------
	ClipBord�N���X
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "Charu3.h"
#include "ClipBord.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//---------------------------------------------------
//�֐���	ClipBord
//�@�\		�R���X�g���N�^
//---------------------------------------------------
CClipBord::CClipBord()
{
	m_hParentWnd = NULL;
}

//---------------------------------------------------
//�֐���	~ClipBord
//�@�\		�f�X�g���N�^
//---------------------------------------------------
CClipBord::~CClipBord()
{

}

//---------------------------------------------------
//�֐���	setClipboardText(const CString& sData)
//�@�\		�N���b�v�{�[�h�Ƀe�L�X�g��ݒ�
//---------------------------------------------------
BOOL CClipBord::setClipboardText(const CString sData)
{
	size_t len = sData.GetLength();
	if (0 == len) return true; // do not set empty text
	++len; // for terminator
	TCHAR* newText = new TCHAR[len];
	_tcscpy_s(newText, len, sData);

	bool isRet = false;
	UINT uFormat;
	#ifdef _UNICODE
		uFormat = CF_UNICODETEXT;
	#else
		uFormat = CF_TEXT;
	#endif

	if (::OpenClipboard(m_hParentWnd)) {
		HGLOBAL hMem = ::GlobalAlloc(GHND, len * sizeof(TCHAR));
		if (hMem) {
			LPVOID pPtr = ::GlobalLock(hMem);
			if (pPtr) {
				_tcscpy_s(static_cast<TCHAR*>(pPtr), len, newText);
			}
			else {
				::GlobalFree(hMem);
				return isRet;
			}
			::GlobalUnlock(hMem);
			::EmptyClipboard();
			HANDLE hRet = ::SetClipboardData(uFormat, hMem);
			if (hRet) {
				isRet = true;
			}
		}
		::CloseClipboard();
	}
	return isRet;
}

//---------------------------------------------------
//�֐���	getClipboardText(CString& sData)
//�@�\		�N���b�v�{�[�h�̃e�L�X�g���擾
//---------------------------------------------------
BOOL CClipBord::getClipboardText(CString& sData)
{
	bool isRet = false;

	UINT uFormat;
#ifdef _UNICODE
	uFormat = CF_UNICODETEXT;
#else
	uFormat = CF_TEXT;
#endif
	if (::IsClipboardFormatAvailable(uFormat)) {
		if (::OpenClipboard(m_hParentWnd)) {
			HGLOBAL hGet = ::GetClipboardData(uFormat);
			if (hGet) {
				LPVOID pPtr = ::GlobalLock(hGet);
				if (pPtr) {
					sData = CString(static_cast<TCHAR*>(pPtr));
					isRet = true;
					::GlobalUnlock(hGet);
				}
			}
			::CloseClipboard();
		}
	}
	return isRet;
}

