/*----------------------------------------------------------
	General�N���X
									2002/12/07 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "General.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//---------------------------------------------------
//�֐���	CGeneral
//�@�\		�R���X�g���N�^
//---------------------------------------------------
CGeneral::CGeneral()
{

}

//---------------------------------------------------
//�֐���	~CGeneral
//�@�\		�f�X�g���N�^
//---------------------------------------------------
CGeneral::~CGeneral()
{

}

//---------------------------------------------------
//�֐���	loadBitmapFile
//�@�\		BMP�t�@�C����ǂݍ���
//---------------------------------------------------
bool CGeneral::loadBitmapFile(CString strFileName,CBitmap *BitMap)
{
	HBITMAP hBitmap;
	BitMap->Detach();
	hBitmap = (HBITMAP)::LoadImage(NULL,strFileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

	if(hBitmap == NULL) 	return false;
	else {
		BitMap->Attach(hBitmap);
	}
	return true;
}

//---------------------------------------------------
//�֐���	wideCharToCString(wchar_t *szUnicodeBuff)
//�@�\		���C�h������MBCS�ɂ���CString�ɓ����
//---------------------------------------------------
CString CGeneral::wideCharToCString(wchar_t *szUnicodeBuff)
{
	char *szMbcsBuff;
	CString strRet;
	int nDataSize = ::WideCharToMultiByte(CP_ACP, 0, szUnicodeBuff,-1,NULL,0,NULL,NULL);
	szMbcsBuff = new char[nDataSize];
	if(szMbcsBuff) {
		::WideCharToMultiByte(CP_ACP,0,szUnicodeBuff,-1,szMbcsBuff,nDataSize,"",NULL);
		strRet = szMbcsBuff;
		delete[] szMbcsBuff;
	}

	return strRet;
}

bool CGeneral::getPrefNumber(nlohmann::json& j, const char* key, double& result)
{
	if (j[key].is_number()) {
		nlohmann::json::iterator it = j.find(key);
		if (it != j.end() && it.value().is_number()) {
			result = it.value().get<double>();
			return true;
		}
	}
	return false;
}

std::string CGeneral::getPrefString(nlohmann::json& j, const char* key)
{
	if (j[key].is_string()) {
		nlohmann::json::iterator it = j.find(key);
		if (it != j.end() && it.value().is_string()) {
			return it.value().get<std::string>();
		}
	}
	return std::string();
}

CString CGeneral::getPrefCString(nlohmann::json& j, const char* key)
{
	if (j[key].is_string()) {
		nlohmann::json::iterator it = j.find(key);
		if (it != j.end() && it.value().is_string()) {
			auto s = it.value().get<std::string>();
			const char* cptr = s.c_str();
			int size = MultiByteToWideChar(CP_UTF8, 0, cptr, -1, nullptr, 0);
			wchar_t* wbuf = new wchar_t[size];
			if (wbuf) {
				MultiByteToWideChar(CP_UTF8, 0, cptr, -1, wbuf, size);
				CString cs = CString(wbuf);
				delete[] wbuf;
				return cs;
			}
		}
	}
	return CString();
}

void CGeneral::flatSB_UpdateMetrics(HWND hWnd)
{
	FlatSB_SetScrollProp(hWnd, WSB_PROP_CXVSCROLL, GetSystemMetrics(SM_CXVSCROLL), TRUE);
	FlatSB_SetScrollProp(hWnd, WSB_PROP_CXHSCROLL, GetSystemMetrics(SM_CXHSCROLL), TRUE);
	FlatSB_SetScrollProp(hWnd, WSB_PROP_CYVSCROLL, GetSystemMetrics(SM_CYVSCROLL), TRUE);
	FlatSB_SetScrollProp(hWnd, WSB_PROP_CYHSCROLL, GetSystemMetrics(SM_CYHSCROLL), TRUE);
	FlatSB_SetScrollProp(hWnd, WSB_PROP_CXHTHUMB, GetSystemMetrics(SM_CXHTHUMB), TRUE);
	FlatSB_SetScrollProp(hWnd, WSB_PROP_CYVTHUMB, GetSystemMetrics(SM_CYVTHUMB), TRUE);
}

//---------------------------------------------------
//�֐���	mod2VK(UINT uMod)
//�@�\		�L�[�R�[�h�ϊ�
//---------------------------------------------------
UINT CGeneral::mod2VK(UINT uMod)
{
	UINT uVK = 0;
	if(uMod & MOD_ALT)		uVK = VK_MENU;
	if(uMod & MOD_CONTROL)	uVK = VK_CONTROL;
	if(uMod & MOD_SHIFT)	uVK = VK_SHIFT;
	return uVK;
}

//---------------------------------------------------
//�֐���	mod2Hotkey(UINT uMod)
//�@�\		�L�[�R�[�h�ϊ�
//---------------------------------------------------
UINT CGeneral::mod2Hotkey(UINT uMod)
{
	WORD wMod = 0;
	if(uMod & MOD_ALT)		wMod |= HOTKEYF_ALT;
	if(uMod & MOD_CONTROL)	wMod |= HOTKEYF_CONTROL;
	if(uMod & MOD_SHIFT)	wMod |= HOTKEYF_SHIFT;
	return (UINT)wMod;
}

//---------------------------------------------------
//�֐���	hotkey2MOD(WORD wModS)
//�@�\		�L�[�R�[�h�ϊ�
//---------------------------------------------------
WORD CGeneral::hotkey2MOD(WORD wModS)
{
	WORD wMod = 0;
	if(wModS & HOTKEYF_ALT)		wMod |= MOD_ALT;
	if(wModS & HOTKEYF_CONTROL)	wMod |= MOD_CONTROL;
	if(wModS & HOTKEYF_SHIFT)	wMod |= MOD_SHIFT;
	return wMod;
}

//---------------------------------------------------
//�֐���	RGB2BGR(COLORREF col)
//�@�\		RGB��BGR�ɕϊ�
//---------------------------------------------------
COLORREF CGeneral::RGB2BGR(COLORREF col)
{
	COLORREF ret;

	int R,G,B;

	R = 0x00ff0000 & col;
	R = R >> 16;
	G = 0x0000ff00 & col;
	B = 0x000000ff & col;
	B = B << 16;

	ret = (R + G + B);
	return ret;
}
//---------------------------------------------------
//�֐���	BGR2RGB(COLORREF col)
//�@�\		BGR��RGB�ɕϊ�
//---------------------------------------------------
COLORREF CGeneral::BGR2RGB(COLORREF col)
{
	COLORREF ret;

	int R,G,B;

	B = 0x00ff0000 & col;
	B = B >> 16;
	G = 0x0000ff00 & col;
	R = 0x000000ff & col;
	R = R << 16;

	ret = (R + G + B);
	return ret;
}

//---------------------------------------------------
//�֐���	upDownLight(COLORREF col,float fLight)
//�@�\		RGB�̖��邳��ύX
//---------------------------------------------------
COLORREF CGeneral::upDownLight(COLORREF col,double fLight)
{
	COLORREF ret;

	int R,G,B;

	R = 0x00ff0000 & col;
	R = R >> 16;
	R = (int)((double)R * fLight);
	if(R > 0xff) R = 0xff;

	G = 0x0000ff00 & col;
	G = G >> 8;
	G = (int)((double)G * fLight);
	if(G > 0xff) G = 0xff;

	B = 0x000000ff & col;
	B = (int)((double)B * fLight);
	if(B > 0xff) B = 0xff;

	ret = ((R << 16) + (G << 8) + B);
	return ret;
}

//---------------------------------------------------
//�֐���	GetCaretPos(POINT *CaretPos)
//�@�\		�A�N�e�B�u�ȃA�v���̃L�����b�g�ʒu���擾
//---------------------------------------------------
void CGeneral::getCaretPos(POINT *CaretPos,FOCUS_INFO *focusInfo)
{
	//�L�����b�g�ʒu
	DWORD dwFrom,dwTo;
	HWND hActiveWnd,hForground;

	if(focusInfo) {
		focusInfo->m_hActiveWnd = NULL;
		focusInfo->m_hFocusWnd = NULL;
	}

	dwFrom = ::GetCurrentThreadId();//�A�N�e�B�u�ȃX���b�h��ID���擾
	hForground = ::GetForegroundWindow();
	if(focusInfo) focusInfo->m_hActiveWnd = hForground;
	if(hForground != NULL)
		dwTo = ::GetWindowThreadProcessId(hForground, NULL);//�v���Z�XID���擾
	else
		dwTo = NULL;


	if (dwFrom && dwTo) {
		BOOL isGetGuiInfo = false;
		GUITHREADINFO gui;
		ZeroMemory(&gui,sizeof(gui));
		gui.cbSize = sizeof(gui);
		isGetGuiInfo = GetGUIThreadInfo(dwTo,&gui);
		if(!isGetGuiInfo) {
			if (::AttachThreadInput(dwFrom, dwTo, TRUE)) {//�A�^�b�`
				::GetCaretPos(CaretPos);//�L�����b�g���W���擾
				hActiveWnd = ::GetFocus();//�t�H�[�J�X�E�B���h�E�n���h�����擾
				if(focusInfo) focusInfo->m_hFocusWnd = hActiveWnd;
				ClientToScreen(hActiveWnd,CaretPos);
				::AttachThreadInput(dwFrom,dwTo, FALSE);//�f�^�b�`
			}
		}
		else {
			hActiveWnd = gui.hwndFocus;//�t�H�[�J�X�E�B���h�E�n���h�����擾
			if(focusInfo) focusInfo->m_hFocusWnd = hActiveWnd;
			CaretPos->x = gui.rcCaret.left;
			CaretPos->y = gui.rcCaret.top;

			ClientToScreen(hActiveWnd,CaretPos);
			CaretPos->y += gui.rcCaret.bottom - gui.rcCaret.top;  
		}
	}
//	if(CaretPos->x < 0)	CaretPos->x = 0;
//	if(CaretPos->y < 0)	CaretPos->y = 0;
	if(focusInfo->m_hFocusWnd == NULL) focusInfo->m_hFocusWnd =  focusInfo->m_hActiveWnd;
}

//---------------------------------------------------
//�֐���	SetAbsoluteForegroundWindow(HWND hWnd)
//�@�\		�E�B���h�E�������A�N�e�B�u
//---------------------------------------------------
BOOL CGeneral::setAbsoluteForegroundWindow(HWND hWnd)
{
	int nTargetID = NULL, nForegroundID = NULL;
	UINT nTimeout;
	BOOL isRet = false;

	if(GetForegroundWindow() && hWnd) {
		nForegroundID = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
		nTargetID = GetWindowThreadProcessId(hWnd, NULL);
		AttachThreadInput(nTargetID, nForegroundID, TRUE);

		SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &nTimeout, 0);
		SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (PVOID)0, 0);

		isRet = SetForegroundWindow(hWnd);

		SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (PVOID)nTimeout, 0);

		AttachThreadInput(nTargetID, nForegroundID, FALSE);
		isRet = true;
	}
	return isRet;
}

//---------------------------------------------------
//�֐���	setFocus(const HWND hActiveWnd, const HWND hFocusWnd)
//�@�\		�t�H�[�J�X�Z�b�g
//---------------------------------------------------
void CGeneral::setFocus(const HWND hActiveWnd, const HWND hFocusWnd)
{
	if(hFocusWnd && hActiveWnd){
		DWORD dwID = GetWindowThreadProcessId(hActiveWnd, NULL);
		if(dwID) {
			AttachThreadInput(dwID, GetCurrentThreadId(), TRUE);
			if(GetFocus() != hFocusWnd) {
				HWND nRet = ::SetFocus(hFocusWnd);
			}
			dwID = GetWindowThreadProcessId(hActiveWnd, NULL);
			if(dwID) AttachThreadInput(dwID, GetCurrentThreadId(), FALSE);
		}
	}
}

//---------------------------------------------------
//�֐���	setFocusInfo(const FOCUS_INFO *focusInfo)
//�@�\		�t�H�[�J�X�Z�b�g
//---------------------------------------------------
void CGeneral::setFocusInfo(const FOCUS_INFO *focusInfo)
{
	//�A�N�e�B�u�E�B���h�E�̐ݒ�
	if(focusInfo->m_hActiveWnd) {
		setAbsoluteForegroundWindow(focusInfo->m_hActiveWnd);
		if(focusInfo->m_hActiveWnd) ::SendMessage(focusInfo->m_hActiveWnd, WM_NCACTIVATE, (WPARAM)TRUE, 0);
		//�t�H�[�J�X�̐ݒ�
		setFocus(focusInfo->m_hActiveWnd, focusInfo->m_hFocusWnd);
	}
}
	
//---------------------------------------------------
//�֐���	getFocusInfo(FOCUS_INFO *focusInfo)
//�@�\		�t�H�[�J�X���擾
//---------------------------------------------------
void CGeneral::getFocusInfo(FOCUS_INFO *focusInfo,HWND hForeground)
{
	//�t�H�[�J�X�����E�B���h�E�̎擾
	if(!hForeground)
		focusInfo->m_hActiveWnd = GetForegroundWindow();
	else 
		focusInfo->m_hActiveWnd = hForeground;

	DWORD dwID = GetWindowThreadProcessId(focusInfo->m_hActiveWnd, NULL);
	if(dwID) {
		AttachThreadInput(dwID, GetCurrentThreadId(), TRUE); 
		focusInfo->m_hFocusWnd = GetFocus();
		dwID = GetWindowThreadProcessId(focusInfo->m_hActiveWnd, NULL);
		if(dwID)	AttachThreadInput(dwID, GetCurrentThreadId(), FALSE);
	}
	if(focusInfo->m_hFocusWnd == NULL) focusInfo->m_hFocusWnd =  focusInfo->m_hActiveWnd;
}

//---------------------------------------------------
//�֐���	getKeyName(UINT uVkCode,HKL keyLayout)
//�@�\		���z�L�[�̖��O�𓾂�
//---------------------------------------------------
CString CGeneral::getKeyName(UINT uVkCode,HKL keyLayout)
{
	CString strRet;
	strRet = "";
	int nExtFlag = 0;

	TCHAR strKeyName[256];
	int nScanCode = MapVirtualKeyEx(uVkCode,0,keyLayout) << 16;
	if(nScanCode) {
		if(uVkCode == VK_APPS || uVkCode == VK_PRIOR || uVkCode == VK_NEXT || uVkCode == VK_END ||
			uVkCode == VK_HOME || uVkCode == VK_LEFT || uVkCode == VK_UP || uVkCode == VK_RIGHT ||
			uVkCode == VK_DOWN || uVkCode == VK_INSERT || uVkCode == VK_DELETE || uVkCode == VK_NUMLOCK)
				nExtFlag = 1 << 24;

		GetKeyNameText(nScanCode | nExtFlag,strKeyName,_countof(strKeyName));
		strRet = strKeyName;
	}
	return strRet;
}

//---------------------------------------------------
//�֐���	getWindowTitle(HWND hWnd)
//�@�\		�E�B���h�E�^�C�g�����擾
//---------------------------------------------------
CString CGeneral::getWindowTitle(HWND hWnd)
{
	TCHAR strWindowName[1024];
	*strWindowName = (TCHAR)NULL;
	CString strWinName;
	bool isRet = false;

	strWinName = "";
	if(!GetWindowText(hWnd,strWindowName,_countof(strWindowName))) return strWinName;//�L���v�V�������擾

	strWinName = strWindowName;
	return 	strWinName;
}


//---------------------------------------------------
//�֐���	getLastErrorMessage()
//�@�\		lasterror�����b�Z�[�W�ɕϊ�
//---------------------------------------------------
CString CGeneral::getLastErrorMessage()
{
	LPTSTR lpMsgBuf;
	CString strRet;
	strRet = _T("");
	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,NULL,
		GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR)&lpMsgBuf,0,NULL)) {
		strRet = lpMsgBuf;
		LocalFree( (void*)lpMsgBuf );
	}
	return strRet;
}

//---------------------------------------------------
//�֐���	writeLog(CString strFileName,strLogText)
//�@�\		���O���t�@�C���ɏ����o��
//---------------------------------------------------
void CGeneral::writeLog(CString strFileName,CString strLogText,CString strSourceFile,int nLine)
{
	FILE *outPut = nullptr;
	CString strWrite,strTest;

#ifdef _UNICODE
	char *szMbcsBuff = NULL;
	int nDataSize;
	errno_t err = -1;

	nDataSize = ::WideCharToMultiByte(CP_ACP, 0, strFileName,-1,NULL,0,NULL,NULL);
	szMbcsBuff = new char[nDataSize+1];
	if(szMbcsBuff) {
		::WideCharToMultiByte(CP_ACP,0,strFileName,-1,szMbcsBuff,nDataSize,"",NULL);
		szMbcsBuff[nDataSize] = NULL;
		err = fopen_s(&outPut, szMbcsBuff, "a");
		delete[] szMbcsBuff;
	}
	if(0 == err) {
		strWrite.Format(_T("%s %s:%d %s"),getDateTimeString().GetString(),strSourceFile.GetString(),nLine,strLogText.GetString());
		nDataSize = ::WideCharToMultiByte(CP_ACP, 0, strWrite,-1,NULL,0,NULL,NULL);
		szMbcsBuff = new char[nDataSize + 1];
		if(szMbcsBuff) {
			::WideCharToMultiByte(CP_ACP,0,strWrite,-1,szMbcsBuff,nDataSize,"",NULL);
			szMbcsBuff[nDataSize] = NULL;
			fputs(szMbcsBuff,outPut);
			delete[] szMbcsBuff;
		}
		fflush(outPut);
		OutputDebugString(strWrite);
		fclose(outPut);
	}
#else
	if((outPut = fopen(strFileName, "a")) != NULL) {
		strWrite.Format("%s %s:%d %s",getDateTimeString().GetString(),strSourceFile.GetString(),nLine,strLogText.GetString());
		fputs(strWrite,outPut);
		fflush(outPut);
		OutputDebugString(strWrite);
		fclose(outPut);
	}
#endif
}


//---------------------------------------------------
//�֐���	convertWareki(long lDate)
//�@�\		�����a��ɕϊ�
//---------------------------------------------------
CString CGeneral::convertWareki(long lDate)
{
	CString strRet;
	strRet = "";

	const TCHAR *szGengou[] = {_T("���v"),_T("����"),_T("�c��"),_T("����"),_T("�吳"),_T("���a"),_T("����")};
	const long lDateTbl[]  = {18640220,18650407,18680908,19120730,19261225,19890108,999991231};

	for(int i = 0; i <= 6; i++) {
		long lYear,lMonth,lDay,lGannen;
		DWORD dwComp,dwNow;
		//�����̋�؂�
		setCTime(lDateTbl[i],&lYear,&lMonth,&lDay);
		dwComp = countDays(lYear,lMonth,lDay);
		//��
		setCTime(lDate,&lYear,&lMonth,&lDay);
		dwNow = countDays(lYear,lMonth,lDay);
	
		if(dwComp > dwNow) {
			if(i > 0)	setCTime(lDateTbl[i-1],&lGannen,&lMonth,&lDay);
			else lGannen = 1861;
			lYear = lYear - lGannen + 1;
			if(lYear == 1)	strRet.Format(_T("%s��"),szGengou[i]);
			else			strRet.Format(_T("%s%2d"),szGengou[i],lYear);
			break;
		}
	}
	return strRet;
}

//---------------------------------------------------
//�֐���	countDays(long lYear,long lMonth,long lDay)
//�@�\		�����̃J�E���g
//---------------------------------------------------
long CGeneral::countDays(long lYear,long lMonth,long lDay)
{
	long lRet = 0;
				//   1, 2, 3, 4, 5, 6  7, 8, 9,10,11,12
	int nMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};

	//365�|���āA4�N�Ɉ�x�͂��邤�N�ŁA�ł�100�N�Ɉ�x�͂��邤�N�łȂ��B�Ƃ��낪400�N�Ɉ�x�͂��邤�N
	lRet += ((lYear-1) * 365) + (int)((lYear-1) / 4) - (int)((lYear-1) / 100) + (int)((lYear-1) / 400);

	//���̓����𑫂�
	for(int i = 0; i < lMonth-1; i++) {
		lRet += nMonth[i];
	}
	if((lYear % 4 == 0 && lYear % 100 != 0 || lYear % 400 == 0) && lMonth >= 3)	lRet++;

	//���𑫂�
	lRet += lDay;

	return lRet;
}

//---------------------------------------------------
//�֐���	getDateString()
//�@�\		���t����������
//---------------------------------------------------
CString CGeneral::getDateString()
{
	CString strDate;

	struct tm newtime;
	time_t long_time;

	time( &long_time );
	localtime_s(&newtime, &long_time );

	strDate.Format(_T("%04d/%02d/%02d"),newtime.tm_year + 1900,newtime.tm_mon + 1,newtime.tm_mday);
	return strDate;
}

//---------------------------------------------------
//�֐���	getDateTimeString()
//�@�\		���t���ԕ���������
//---------------------------------------------------
CString CGeneral::getDateTimeString()
{
	CString strDate;

	struct tm newtime;
	time_t long_time;

	time( &long_time );
	localtime_s(&newtime, &long_time);

	strDate.Format(_T("%04d/%02d/%02d-%02d:%02d:%02d"),newtime.tm_year + 1900,newtime.tm_mon + 1,newtime.tm_mday,
		newtime.tm_hour,newtime.tm_min,newtime.tm_sec);
	return strDate;
}

//---------------------------------------------------
//�֐���	setCTime(long lDate,long *lYear,long *lMonth,long *lDay)
//�@�\		���t�f�[�^�̕ϊ�
//����		20021003�@���t����
//---------------------------------------------------
bool CGeneral::setCTime(long lDate,long *lYear,long *lMonth,long *lDay)
{
	bool isRet = false;

	//�N������o��
	*lYear = lDate / 10000;
	if(*lYear <= 0) return isRet;

	//��������o��
	*lMonth = (lDate - (*lYear) * 10000) / 100;
	if(*lMonth <= 0) return isRet;

	//��������o��
	*lDay = (lDate - ((*lYear) * 10000 + (*lMonth) * 100));
	if(*lDay <= 0) return isRet;
	
	isRet = true;
	return isRet;
}

