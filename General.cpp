/*----------------------------------------------------------
    General�N���X
                                    2002/12/07 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#include "General.h"

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
bool CGeneral::loadBitmapFile(CString strFileName, CBitmap* BitMap)
{
    HBITMAP hBitmap;
    BitMap->Detach();
    hBitmap = (HBITMAP)::LoadImage(NULL, strFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    if (hBitmap == NULL) 	return false;
    else {
        BitMap->Attach(hBitmap);
    }
    return true;
}

//---------------------------------------------------
//�֐���	ConvertWcharToCString(wchar_t *szUnicodeBuff)
//�@�\		���C�h������MBCS�ɂ���CString�ɓ����
//---------------------------------------------------
CString CGeneral::ConvertWcharToCString(wchar_t* szUnicodeBuff)
{
    char* szMbcsBuff;
    CString strRet;
    int nDataSize = ::WideCharToMultiByte(CP_ACP, 0, szUnicodeBuff, -1, NULL, 0, NULL, NULL);
    szMbcsBuff = new char[nDataSize];
    if (szMbcsBuff) {
        ::WideCharToMultiByte(CP_ACP, 0, szUnicodeBuff, -1, szMbcsBuff, nDataSize, "", NULL);
        strRet = szMbcsBuff;
        delete[] szMbcsBuff;
    }
    return strRet;
}

CStringA CGeneral::ConvertUnicodeToUTF8(const CStringW& uni)
{
    if (uni.IsEmpty()) return "";
    CStringA utf8;
    int cc = 0;
    // get length (cc) of the new multibyte string excluding the \0 terminator first
    if ((cc = WideCharToMultiByte(CP_UTF8, 0, uni, -1, NULL, 0, 0, 0) - 1) > 0) {
        char* buf = utf8.GetBuffer(cc);
        if (buf) WideCharToMultiByte(CP_UTF8, 0, uni, -1, buf, cc, 0, 0);
        utf8.ReleaseBuffer();
    }
    return utf8;
}

//---------------------------------------------------
//�֐���	getLastErrorMessage()
//�@�\		lasterror�����b�Z�[�W�ɕϊ�
//---------------------------------------------------
CString CGeneral::getLastErrorMessage()
{
    LPTSTR lpMsgBuf = {};
    CString strRet;
    strRet = _T("");
    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
        GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL)) {
        strRet = lpMsgBuf;
        LocalFree((void*)lpMsgBuf);
    }
    return strRet;
}

//---------------------------------------------------
//�֐���	convertWareki(long lDate)
//�@�\		�����a��ɕϊ�
//---------------------------------------------------
CString CGeneral::convertWareki(long lDate)
{
    CString strRet;
    strRet = "";

    const TCHAR* szGengou[] = { _T("���v"),_T("����"),_T("�c��"),_T("����"),_T("�吳"),_T("���a"),_T("����") };
    const long lDateTbl[] = { 18640220,18650407,18680908,19120730,19261225,19890108,999991231 };

    for (int i = 0; i <= 6; i++) {
        long lYear, lMonth, lDay, lGannen;
        DWORD dwComp, dwNow;
        //�����̋�؂�
        setCTime(lDateTbl[i], &lYear, &lMonth, &lDay);
        dwComp = countDays(lYear, lMonth, lDay);
        //��
        setCTime(lDate, &lYear, &lMonth, &lDay);
        dwNow = countDays(lYear, lMonth, lDay);

        if (dwComp > dwNow) {
            if (i > 0)	setCTime(lDateTbl[i - 1], &lGannen, &lMonth, &lDay);
            else lGannen = 1861;
            lYear = lYear - lGannen + 1;
            if (lYear == 1)	strRet.Format(_T("%s��"), szGengou[i]);
            else			strRet.Format(_T("%s%2d"), szGengou[i], lYear);
            break;
        }
    }
    return strRet;
}

//---------------------------------------------------
//�֐���	countDays(long lYear,long lMonth,long lDay)
//�@�\		�����̃J�E���g
//---------------------------------------------------
long CGeneral::countDays(long lYear, long lMonth, long lDay)
{
    long lRet = 0;
    //   1, 2, 3, 4, 5, 6  7, 8, 9,10,11,12
    int nMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    //365�|���āA4�N�Ɉ�x�͂��邤�N�ŁA�ł�100�N�Ɉ�x�͂��邤�N�łȂ��B�Ƃ��낪400�N�Ɉ�x�͂��邤�N
    lRet += ((lYear - 1) * 365) + (int)((lYear - 1) / 4) - (int)((lYear - 1) / 100) + (int)((lYear - 1) / 400);

    //���̓����𑫂�
    for (int i = 0; i < lMonth - 1; i++) {
        lRet += nMonth[i];
    }
    if ((lYear % 4 == 0 && lYear % 100 != 0 || lYear % 400 == 0) && lMonth >= 3)	lRet++;

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

    time(&long_time);
    localtime_s(&newtime, &long_time);

    strDate.Format(_T("%04d/%02d/%02d"), newtime.tm_year + 1900, newtime.tm_mon + 1, newtime.tm_mday);
    return strDate;
}

//---------------------------------------------------
//�֐���	setCTime(long lDate,long *lYear,long *lMonth,long *lDay)
//�@�\		���t�f�[�^�̕ϊ�
//����		20021003�@���t����
//---------------------------------------------------
bool CGeneral::setCTime(long lDate, long* lYear, long* lMonth, long* lDay)
{
    bool isRet = false;

    //�N������o��
    *lYear = lDate / 10000;
    if (*lYear <= 0) return isRet;

    //��������o��
    *lMonth = (lDate - (*lYear) * 10000) / 100;
    if (*lMonth <= 0) return isRet;

    //��������o��
    *lDay = (lDate - ((*lYear) * 10000 + (*lMonth) * 100));
    if (*lDay <= 0) return isRet;

    isRet = true;
    return isRet;
}
