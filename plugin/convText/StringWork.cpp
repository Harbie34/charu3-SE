/*----------------------------------------------------------
	�����񏈗��N���X
									2001/11/20 (c)Wizard
----------------------------------------------------------*/

#include "stdafx.h"
#include "StringWork.h"

//---------------------------------------------------
//�֐���	trim(char *strSource)		[public]
//�@�\		������̑O��̃X�y�[�X�����
//---------------------------------------------------
int UStringWork::trim(char *strSource)
{
	int nRet = 0;

	nRet = trimBack(strSource);
	nRet = trimFront(strSource);

	return nRet;
}


//---------------------------------------------------
//�֐���	trimFront(char *strSource)	[public]
//�@�\		������̑O�̃X�y�[�X�����
//---------------------------------------------------
int UStringWork::trimFront(char *strSource)
{
	int nRet = 0;
	bool isSpace = false;
	char *szStart = strSource;

	for( ; *strSource != (char)NULL; strSource++) {
		if(!isSpace && *strSource != ' ' && *strSource != (char)0x0d && *strSource != (char)0x0a) {
			isSpace = true;
		}
		if(isSpace) {
			*szStart = *strSource;
			szStart++;
			nRet++;
		}
	}
	*szStart = (char)NULL;
	return nRet;
}

//---------------------------------------------------
//�֐���	trimBack(char *strSource)	[public]
//�@�\		������̌��̃X�y�[�X�����
//---------------------------------------------------
int UStringWork::trimBack(char *strSource)
{
	int nRet = 0;
	bool isSpace = false;
	char *szEnd = strSource + strlen(strSource)-1;

	for(; *szEnd == ' ' || *szEnd == (char)0x0d || *szEnd == (char)0x0a; szEnd--) {
		if(szEnd == strSource) break;
	}
	szEnd++;
	*szEnd = (char)NULL;
	return strlen(szEnd);
}

//---------------------------------------------------
//�֐���	splitString(char split, ...)	[public]
//�@�\		�w�蕶���ŕ�����𕪊�����
//---------------------------------------------------
int UStringWork::splitString(char *strSource,char split, ...)
{
	va_list marker;
	char **strTarget;
	int nRet = 0;

	va_start(marker,split);//�ϐ����X�g�̏�����
	strTarget = va_arg( marker, char**);//�擪���R�s�[

	while(strTarget != (char**)NULL) {//�ϐ����X�g�̍Ō�܂�
		if(*strSource)	nRet++;
		*strTarget = *strSource ? strSource: (char*)NULL;
		for(; *strSource; strSource++) {
			if(isSJIS(strSource)) {
				*strSource++;
				continue;
			}
			if(*strSource == split) {//����������܂�
				*strSource = (char)NULL;//�����������NULL�����ă|�C���^��i�߂�
				*strSource++;
				break;
			}
		}
		strTarget = va_arg( marker, char**);//���̕ϐ����X�g���擾
	}
	va_end( marker );

	return nRet;
}

//---------------------------------------------------
//�֐���	splitStringSame(char split, ...)[public]
//�@�\		�w�蕶���ŕ�����𕪊�����
//---------------------------------------------------
int UStringWork::splitStringSame(char *strSource,char split, ...)
{
	va_list marker;
	char **strTarget;
	int nRet = 0;
	bool isSame = false;

	va_start(marker,split);//�ϐ����X�g�̏�����
	strTarget = va_arg( marker, char**);//�擪���R�s�[

	while(strTarget != (char**)NULL) {//�ϐ����X�g�̍Ō�܂�
		if(*strSource)	nRet++;
		*strTarget = *strSource ? strSource: (char*)NULL;
		for(; *strSource; strSource++) {
			if(isSJIS(strSource)) {
				*strSource++;
				continue;
			}
			if(*strSource == split && !isSame) {//����������܂�
				*strSource = (char)NULL;//�����������NULL�����ă|�C���^��i�߂�
				*strSource++;
				isSame = true;
				break;
			}
			else if(isSame && *strSource != split) isSame = false;
		}
		strTarget = va_arg( marker, char**);//���̕ϐ����X�g���擾
	}
	va_end( marker );

	return nRet;
}

//---------------------------------------------------
//�֐���	strtime2TimeT_S(char *strSource)[public]
//�@�\		������̎��Ԃ�ϊ�
//---------------------------------------------------
time_t UStringWork::strtime2TimeT_S(char *strSource)
{
	//YYYY/MM/DD-HH:MM:SS�`����ϊ�
	tm timeptr;
	time_t timet = (time_t)NULL;
	char *szDate,*szTime;
	char *szTmp[3];

	memset(&timeptr,0x00,sizeof(timeptr));

	if(UStringWork::splitString(strSource,'-',&szDate,&szTime,NULL) != 2) return timet;

	//���t��ϊ�
	if(splitString(szDate,'/',&szTmp[0],&szTmp[1],&szTmp[2],NULL) != 3) return timet;
	timeptr.tm_year = atoi(szTmp[0]);
	timeptr.tm_mon  = atoi(szTmp[1]);
	timeptr.tm_mday = atoi(szTmp[2]);

	//���Ԃ�ϊ�
	if(splitString(szTime,':',&szTmp[0],&szTmp[1],&szTmp[2],NULL) != 3) return timet;
	timeptr.tm_hour = atoi(szTmp[0]);
	timeptr.tm_min  = atoi(szTmp[1]);
	timeptr.tm_sec  = atoi(szTmp[2]);

	timeptr.tm_year -= 1900;
	timet = mktime(&timeptr);

	return timet;
}

//---------------------------------------------------
//�֐���	time_t strtime2TimeT_A(char *strTime)
//�@�\		���Ԃ�ϊ�
//---------------------------------------------------
time_t UStringWork::strtime2TimeT_A(char *strTime)
{
	//Oct 15 15:49:38�`����ϊ�
	char *strMonthList[] = {"Jan","Feb","Mar","Apr","May","Jun",
							"Jul","Aug","Sep","Oct","Nov","Dec"};
	tm timeptr,*nowTm;
	time_t timet,nowTime;
	char *szMonth,*szDay,*szTime;
	char *szTmp[3];

	//���݂̔N�����
	time(&nowTime);
	nowTm = localtime(&nowTime);

	memset(&timeptr,0x00,sizeof(timeptr));
	memset(&timet,0x00,sizeof(timet));

	//���t�𕪊�
	if(splitString(strTime,' ',&szMonth,&szDay,&szTime,NULL) != 3) return timet;

	//���Ԃ𕪊�
	if(splitString(szTime,':',&szTmp[0],&szTmp[1],&szTmp[2],NULL) != 3) return timet;

	timeptr.tm_mday = atoi(szDay);

	timeptr.tm_hour = atoi(szTmp[0]);
	timeptr.tm_min  = atoi(szTmp[1]);
	timeptr.tm_sec  = atoi(szTmp[2]);

	int nMonth = -1;
	for(int i = 0; i <= 12; i++){
		if(strcmp(strMonthList[i],szMonth) == 0) {
			nMonth = i+1;
			break;
		}
	}
	if(nMonth == -1) {
		return -1;
	};
	timeptr.tm_mon = nMonth;
	timeptr.tm_year = nowTm->tm_year;
	timet = mktime(&timeptr);

	return timet;
}

//---------------------------------------------------
//�֐���	awk(char *strSource,int nAwk,char cSplit)
//�@�\		�w�蕶����؂�ŕ������؂�o��
//---------------------------------------------------
char* UStringWork::awk(char *strSource,char *strRet,int nSize,int nAwk,char cSplit /* = 0x20*/)
{
	bool isSpace = false;

	if(nSize < 1 || nAwk < 1 || *strSource == NULL) return strSource;
	while(*strSource != NULL) {

		if(!isSJIS(strSource)) {
			if(!isSpace) {
				if(*strSource == cSplit) {//�X�v���b�^����������
					isSpace = true;
				}
			}
			else if(isSpace) {
				if(*strSource != cSplit) {//�X�v���b�^���ᖳ���Ȃ���
					nAwk--;//�J�E���^�����炷
					isSpace = false;
				}
			}
		}
		else if(isSpace){
			nAwk--;//�J�E���^�����炷
			isSpace = false;
		}
		if(nAwk == 1){
			if(*strSource == 0x0a || *strSource == 0x0d || *strSource == cSplit ||
				*strSource == 0x1a|| *strSource == NULL) {
				break;
			}
			*strRet = *strSource;
			if(isSJIS(strSource)) {
				strSource++;//�|�C���^��i�߂�
				strRet++;
				*strRet = *strSource;
			}
			strRet++;
		}
		else if(isSJIS(strSource))	strSource++;//�|�C���^��i�߂�
		strSource++;//�|�C���^��i�߂�
	}
	*strRet = (char)NULL;

	return strSource;
}

//---------------------------------------------------
//�֐���	serchString(char *strSerch,char *strFound,int nFoundSize)
//�@�\		���C���h�J�[�h�t�������񌟍�
//			*�Ȃ�ł�OK ?�Ȃ�ł�1���� {xxx}���ʓ��̕�����Ȃ�
//---------------------------------------------------
char* UStringWork::serchString(char *strSource,char *strSerch,int *nFoundSize,int nStart)
{
	char *szRet = NULL,*szSerch;

	szRet = NULL;
	*nFoundSize = 0;
	szSerch = strSerch;

	strSource += (char)nStart;
	bool isFirstSame = false;

	while(*strSource && *strSerch && strcmp(strSerch,"*")){
		//�����ꂪ�S�p
		if(UStringWork::isSJIS(strSerch)){
			if(strncmp(strSource,strSerch,2) == 0) {//��v����
				if(!szRet)	szRet = strSource;//�����ʒu���L�^
				strSource += 2;
				strSerch += 2;
				(*nFoundSize) += 2;
			}
			else {//�����
				if(UStringWork::isSJIS(strSource)) strSource += 2;
				else strSource++;//�\�[�X�̃|�C���^��i�߂�
				strSerch = szSerch;//��������ŏ��ɖ߂�
				szRet = NULL;
				(*nFoundSize) = 0;
				continue;
			}
		}
		//�����ꂪ���p
		else if(*strSerch == '*' || *strSerch == '?' || *strSerch == '{'){//���C���h�J�[�h
			if(!szRet)	szRet = strSource;//�����ʒu���L�^
			if(!UStringWork::isSJIS(strSerch) && *strSerch == '{') {//�����N���X�Ȃ̂ŁA�N���X���ŏƍ�
				char *szClass;
				bool isSame = false;

				szClass = strSerch + 1;
				while(*szClass && *szClass != '}') {// ]�ɓ����邩�A������̍Ō�܂Ō���
					//�����N���X�A�\�[�X�̂ǂ��炩���S�p�Ȃ�2�o�C�g�ƍ�
					if((UStringWork::isSJIS(strSource) || UStringWork::isSJIS(szClass))
						&& strncmp(strSource,szClass,2) == 0)
						isSame = true;
					//�������p�Ȃ�1�o�C�g�ƍ�
					else if(!UStringWork::isSJIS(strSource) && !UStringWork::isSJIS(szClass) && *strSource == *szClass)	
						isSame = true;

					//�����N���X�̃|�C���^��i�߂�
					if(UStringWork::isSJIS(szClass))	szClass += 2;	//�S�p
					else								szClass ++;		//���p
				}
				//�ƍ����s
				if(!isSame) {
					//�����N���X�̒��ŁA1����ƍ��������Ȃ�����
					if(!isFirstSame) {
						szRet = NULL;//ʹ�݃f�[�^������
						*nFoundSize = 0;
						strSerch = szSerch;//��������ŏ��ɖ߂�
						if(UStringWork::isSJIS(strSource)) strSource += 2;
						else strSource++;//�\�[�X�̃|�C���^��i�߂�
					}
					else strSerch = szClass + 1;
					isFirstSame = false;
					continue;
				}
				isFirstSame = true;//���Ȃ��Ƃ�1�����͏ƍ�����
				//�\�[�X�̃|�C���^��i�߂�
				if(UStringWork::isSJIS(strSource)){
					if(!szRet)	szRet = strSource;//�����ʒu���L�^
					strSource += 2;//�\�[�X�̃|�C���^��i�߂�
					if(!*strSource)	strSerch = szClass + 1;//�\�[�X���I���Ȃ猟������i�߂�
					(*nFoundSize) += 2;
				}
				else {
					if(!szRet)	szRet = strSource;//�����ʒu���L�^
					strSource++;//�\�[�X�̃|�C���^��i�߂�
					(*nFoundSize)++;
				}
				continue;
			}
			//?�̏ꍇ
			if(!UStringWork::isSJIS(strSerch) && *strSerch == '?') {
				strSerch++;//�������1�����i�߂�
				if(UStringWork::isSJIS(strSource)) {//�Ώۂ�1�����i�߂�
					(*nFoundSize) += 2;
					strSource += 2;
				}
				else {
					(*nFoundSize)++;
					strSource ++;
				}
			}
			//*�ŁA�\�[�X�����p��*�͈͂��I���̏ꍇ�͌������i�߂�
			else if(strlen(strSerch) > 1 && !UStringWork::isSJIS(strSerch) && *strSerch == '*' && !UStringWork::isSJIS(strSource) && *(strSerch+1) == *strSource) {
				strSerch++;
			}
			//*�ŁA�\�[�X���S�p��*�͈͂��I���̏ꍇ�͌������i�߂�
			else if(strlen(strSerch) > 1 && !UStringWork::isSJIS(strSerch) && *strSerch == '*' && UStringWork::isSJIS(strSource) && strncmp(strSerch+1,strSource,2) == 0) {
				strSerch++;
			}
			else if(UStringWork::isSJIS(strSource)) {
					strSource += 2;
					if(szRet)	(*nFoundSize) += 2;
			}
			else {
					strSource++;//�\�[�X�̃|�C���^��i�߂�
					if(szRet)	(*nFoundSize)++;
			}
		}
		else if(*strSource == *strSerch && !UStringWork::isSJIS(strSource)){//���p������ň�v
			if(!szRet)	szRet = strSource;//�����ʒu���L�^
			strSource++;
			strSerch++;
			(*nFoundSize)++;
		}
		else {//�S�R�Ⴄ
			if(UStringWork::isSJIS(strSource)) strSource += 2;
			else strSource++;//�\�[�X�̃|�C���^��i�߂�
			strSerch = szSerch;//��������ŏ��ɖ߂�
			szRet = NULL;		//ʹ�ݏ����N���A
			*nFoundSize = 0;
			continue;
		}
	}
	if(strcmp(strSerch,"*") == 0)	*nFoundSize = strlen(szRet);//�Ōオ*�������ꍇ�͕��������ăJ�E���g
	else if(strlen(strSerch) > 0)	szRet = NULL;//�����ꂪ�I�肫���ĂȂ��ꍇ�͕s��v

	return szRet;
}

//---------------------------------------------------
//�֐���	isSJIS(char *szSource)
//�@�\		S-JIS���ǂ����𔻕�
//---------------------------------------------------
bool UStringWork::isSJIS(char *szSource)
{
	bool isRet = false;
	
	if(((unsigned char)*szSource >= 0x81 && (unsigned char)*szSource <= 0x9f) ||
		((unsigned char)*szSource >= 0xe0 && (unsigned char)*szSource <= 0xef)) 
		if(((unsigned char)*(szSource + 1) >= 0x40 && (unsigned char)*(szSource + 1) <= 0x7e) ||
			((unsigned char)*szSource >= 0x80 && (unsigned char)*szSource <= 0xfc)) 
			isRet = true;

	return isRet;
}

//---------------------------------------------------
//�֐���	convertKansuuji(char *strSource,char *strDst,int nSize)
//�@�\		��������ϊ�
//---------------------------------------------------
void UStringWork::convertKansuuji(char *strSource,char *strDst,int nSize)
{
	memset(strDst,0x00,nSize);

	static vector<KANSUUJI> kanjiData;
	KANSUUJI kanji;

	if(kanjiData.size() < 1) {
		kanjiData.reserve(20);
		//�ϊ��p�f�[�^��ݒ�
		strcpy(kanji.strKANJI,"��");kanji.nNumber = 1;			kanjiData[0] = kanji;
		strcpy(kanji.strKANJI,"��");							kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"��");kanji.nNumber = 2;			kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"��");							kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"�O");kanji.nNumber = 3;			kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"�Q");							kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"�l");kanji.nNumber = 4;			kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"��");kanji.nNumber = 5;			kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"�Z");kanji.nNumber = 6;			kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"��");kanji.nNumber = 7;			kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"��");kanji.nNumber = 8;			kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"��");kanji.nNumber = 9;			kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"�\");kanji.nNumber = 10;			kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"�E");							kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"�S");kanji.nNumber = 100;		kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"��");kanji.nNumber = 1000;		kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"��");kanji.nNumber = 10000;		kanjiData.push_back(kanji);
		strcpy(kanji.strKANJI,"��");kanji.nNumber = 100000000;	kanjiData.push_back(kanji);
	}
	int nNumber = 0;
	int nTmpNum = 0;
	bool isNumber = false,isNumberMode = false;

	char *sz;
	sz = strDst;

	while(*strSource) {
		vector<KANSUUJI>::iterator it;
		isNumber = false;
		for( it = kanjiData.begin(); it != kanjiData.end(); it++) {
			if(strncmp(strSource,it->strKANJI,2) == 0) {//��������T��
				isNumber = true;
				strSource += 2;
				break;
			}
		}
		if(isNumber) {
			if(it->nNumber > 9) {//10�ȏ�̐����̏ꍇ�͌�����������
				if(nTmpNum == 0) nTmpNum = 1;
				nNumber += nTmpNum * it->nNumber;//�|����
				nTmpNum = 0;
			}
			else				nTmpNum = nTmpNum + it->nNumber;//10�����̏ꍇ�͑�������
			isNumberMode = true;
		}
		else if(!isNumber && isNumberMode) {//�������̊��������I������
			if(nTmpNum) nNumber += nTmpNum;//������10�����̏ꍇ�̏���
			strDst = UStringWork::addNumberString(nNumber,strDst);
			nNumber = 0;
			nTmpNum = 0;
			isNumberMode = false;
		}
		else {				//����������Ȃ�����
			*strDst = *strSource;
			strDst++; strSource++;
		}
	}
	if(isNumberMode) {//�����������ɏI�����̂ŁA�Ō�̐���������
		strDst = UStringWork::addNumberString(nNumber+nTmpNum,strDst);
	}

	*strDst = (char)NULL;
}


//---------------------------------------------------
//�֐���	addNumberString(int nNum,char *szDst)
//�@�\		���p������S�p�ɂ��ĕ�����ɒǉ�
//---------------------------------------------------
char* UStringWork::addNumberString(int nNum,char *szDst)
{
	char strBuff[32],strBuffZen[64],*szZenPos;
	const char szZenSuu[] = "�O�P�Q�R�S�T�U�V�W�X";
	
	itoa(nNum,strBuff,10);

	//�S�p�����ɂ���
	memset(strBuffZen,0x00,sizeof(strBuffZen));
	szZenPos = strBuffZen;
	for(int i = 0; i < strlen(strBuff); i++) {
		strncpy(szZenPos,&szZenSuu[((strBuff[i] - '0') * 2)],2);
		szZenPos += 2;
	}

	//�������A��
	strcat(szDst,strBuffZen);
	szDst += strlen(strBuffZen);

	return szDst;
}

//---------------------------------------------------
//�֐���	GetIPv4String(char* overSize16Buffer, int nIPv4)
//�@�\		���[�U��`�̃o�b�t�@(16�o�C�g�ȏ�K�v)��
//			xxx.xxx.xxx.xxx�`���̕����������B
//			nIPv4 �� !(32�ȉ� 0�ȏ�) �̐���
//---------------------------------------------------
char* UStringWork::GetIPv4String(char* overSize16Buffer, int nIPv4)
{
	BYTE* b = (BYTE*)&nIPv4;
	_snprintf(overSize16Buffer, 16, "%d.%d.%d.%d", b[3], b[2], b[1], b[0]);
	return overSize16Buffer;
}


//---------------------------------------------------
//�֐���	GetIPv4Mask(char* overSize16Buffer, int under32)
//�@�\		���[�U��`�̃o�b�t�@(16�o�C�g�ȏ�K�v)�� 
//			xxx.xxx.xxx.xxx�`���̕����������B
//			under32 �� 32�ȉ� 0�ȏ� �̐���
//---------------------------------------------------
char* UStringWork::GetIPv4MaskString(char* overSize16Buffer, int under32)
{
	DWORD mask = -1;
	if(under32)
		mask <<= 32-under32;
	else
		mask = 0;
	BYTE* b = (BYTE*)&mask;
	_snprintf(overSize16Buffer, 16, "%d.%d.%d.%d", b[3], b[2], b[1], b[0]);
	return overSize16Buffer;
}

//---------------------------------------------------
//�֐���	URLEncode(const char *pszSrc,int nSrcLength,char *pszBuf)
//�@�\		URL�G���R�[�h������
//---------------------------------------------------
int UStringWork::URLEncode(const char *pszSrc,int nSrcLength,char *pszBuf)
{
	int n=0,nOut=0;
	char cData;
	unsigned char byData;
	char cHex[]={
		'0','1','2','3',
		'4','5','6','7',
		'8','9','A','B',
		'C','D','E','F'
	};

	while(pszSrc[n]!='\0' && ( nSrcLength<0 ? !0 : nSrcLength>n )) {
		cData=pszSrc[n];

		if((cData>='0' && cData<='9') ||
			(cData>='A' && cData<='Z') ||
			(cData>='a' && cData<='z') ||
			cData=='_' || cData=='-' || cData=='.' || cData=='*'){
			// ���̂܂ܐ��ꗬ���ɂł��镶���̏ꍇ
			nOut++;
			if(pszBuf) {
				*pszBuf=cData;
				pszBuf++;
			}
		}
		else {
			if(cData==' ') {
				// �󔒕�����'+'�ɒu������K��
				// '+'�́A%XX�Ȃǂƕϊ������̂ŏd���͂��Ȃ�
				if(pszBuf) {
					*pszBuf='+';
					pszBuf++;
				}
				nOut++;
			}
			else {
				// %XX�`���ւ̕ϊ�
				if(pszBuf) {
					*pszBuf='%';pszBuf++;
					byData=(unsigned char)cData;
					*pszBuf=cHex[byData / 0x10];pszBuf++;
					*pszBuf=cHex[byData % 0x10];pszBuf++;
				}
				nOut+=3;
			}
		}
		n++;
	}

	if(pszBuf) {
		*pszBuf='\0';
	}
	nOut++;

	return nOut;
}

//---------------------------------------------------
//�֐���	HexToDec(char c)
//�@�\		16�i���ϊ�
//---------------------------------------------------
int UStringWork::HexToDec(char c)
{
	if(c>='0' && c<='9')	return c-'0';
	if(c>='A' && c<='F')	return c-'A'+10;
	if(c>='a' && c<='f')	return c-'a'+10;

	return 0;
}

//---------------------------------------------------
//�֐���	URLDecode(const char *pszSrc,char *pszBuf)
//�@�\		URL�f�R�[�h������
//---------------------------------------------------
int UStringWork::URLDecode(const char *pszSrc,char *pszBuf)
{
	int n=0;
	int nOut=0;
	char cData;
	unsigned char byData;
	
	while(pszSrc[n]!='\0') {
		cData=pszSrc[n];
		if(cData=='%') {
			// %12
			n++;byData=(HexToDec(pszSrc[n]))*0x10;	// 1
			n++;byData+=(HexToDec(pszSrc[n]));		// 2

			if(pszBuf) {
				*pszBuf=byData;
				pszBuf++;
			}
			nOut++;
		}
		else {
			if(cData=='+')
				cData=' ';

			byData=cData;
			
			if(pszBuf) {
				*pszBuf=byData;
				pszBuf++;
			}
			nOut++;
		}
		
		n++;
	}

	if(pszBuf) {
		*pszBuf='\0';
	}
	nOut++;

	return nOut;
}
