/*----------------------------------------------------------
    �����񏈗��N���X
                                    2002/11/20 (c)keizi
----------------------------------------------------------*/

#include "stdafx.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#include "StringWork.h"

//---------------------------------------------------
//�֐���	awk(char *strSource,int nAwk,char cSplit)
//�@�\		�w�蕶����؂�ŕ������؂�o��
//---------------------------------------------------
TCHAR* UStringWork::awk(TCHAR* strSource, TCHAR* strRet, int nSize, int nAwk, TCHAR cSplit /* = 0x20*/)
{
    bool isSpace = false;
    TCHAR* szStart = NULL;

    if (nSize < 1 || nAwk < 1 || *strSource == NULL) return strSource;
    while (*strSource != NULL) {

        if (!isSJIS(strSource)) {
            if (!isSpace) {
                if (*strSource == cSplit) {//�X�v���b�^����������
                    isSpace = true;
                }
            }
            else if (isSpace) {
                if (*strSource != cSplit) {//�X�v���b�^���ᖳ���Ȃ���
                    nAwk--;//�J�E���^�����炷
                    isSpace = false;
                }
            }
        }
        else if (isSpace) {
            nAwk--;//�J�E���^�����炷
            isSpace = false;
        }
        if (nAwk == 1) {
            if (!szStart)	szStart = strSource - 1;

            if (*strSource == 0x0a || *strSource == 0x0d || *strSource == cSplit ||
                *strSource == 0x1a || *strSource == NULL) {
                break;
            }
            if (nSize <= 1) break;
            *strRet = *strSource;
            if (isSJIS(strSource)) {
                strSource++;//�|�C���^��i�߂�
                strRet++;
                *strRet = *strSource;
            }
            nSize--;
            strRet++;
        }
        else if (isSJIS(strSource))	strSource++;//�|�C���^��i�߂�
        strSource++;//�|�C���^��i�߂�
    }
    *strRet = (TCHAR)NULL;

    return szStart;
}

//---------------------------------------------------
//�֐���	isSJIS(char *szSource)
//�@�\		S-JIS���ǂ����𔻕�
//---------------------------------------------------
bool UStringWork::isSJIS(TCHAR* szSource)
{
    bool isRet = false;
#ifdef _UNICODE
    return false;
#endif
    if (*szSource == NULL) return isRet;
    if (((unsigned char)*szSource >= 0x81 && (unsigned char)*szSource <= 0x9f) ||
        ((unsigned char)*szSource >= 0xe0 && (unsigned char)*szSource <= 0xef))
        if (((unsigned char)*(szSource + 1) >= 0x40 && (unsigned char)*(szSource + 1) <= 0x7e) ||
            ((unsigned char)*szSource >= 0x80 && (unsigned char)*szSource <= 0xfc))
            isRet = true;

    return isRet;
}

//---------------------------------------------------
//�֐���	compressionSpace(TCHAR *szSource)
//�@�\		�����̃X�y�[�X����ɓZ�߂�
//---------------------------------------------------
int UStringWork::compressionSpace(TCHAR* szSource)
{
    int i;
    for (i = 0; *szSource; i++, szSource++) {
        if (*szSource == _T(' ')) {
            TCHAR* szSpaceEnd;
            szSpaceEnd = szSource;
            int j;
            for (j = 0; *szSpaceEnd && *szSpaceEnd == _T(' '); j++, szSpaceEnd++);
            UStringWork::moveForward(szSpaceEnd, j - 1);
            szSource++;
        }
    }
    return i;
}

//---------------------------------------------------
//�֐���	moveForward(TCHAR *szMovePos,int nMove)
//�@�\		�������O�Ɉړ�(�������͈͂ɒ���)
//---------------------------------------------------
int UStringWork::moveForward(TCHAR* szMovePos, int nMove)
{
    //������ړ�
    int i;
    for (i = 0; *szMovePos; i++, szMovePos++) {
        *(szMovePos - nMove) = *szMovePos;
    }
    *(szMovePos - nMove) = NULL;
    return i;
}

//---------------------------------------------------
//�֐���	ConvertWcharToCString(wchar_t *szUnicodeBuff)
//�@�\		���C�h������MBCS�ɂ���CString�ɓ����
//---------------------------------------------------
CString UStringWork::ConvertWcharToCString(wchar_t* szUnicodeBuff)
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

CStringA UStringWork::ConvertUnicodeToUTF8(const CStringW& uni)
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
