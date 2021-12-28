/*----------------------------------------------------------
	SJIS��EUC�ɕϊ�
									2001/08/04 (c)C+ Factory
----------------------------------------------------------*/

#include "stdafx.h"
#include "charu2plugin.h"

#define SJIS1(c)	((c) >= 0x81 && (c) <= 0x9F || (c) >= 0xE0 && (c) <= 0xF0)
#define SJIS2(c2)	((c2) >= 0x40 && (c2) <= 0xFC && (c2) != 0x7F)
void Sjis2Euc(char *EucStr,char *SjisBuff,int nSize);//�ϊ��N

extern "C" __declspec (dllexport) bool Charu2PlugIn
	(char *strSource,char *strResult,int nSize,OPTION_DATA *ini,STRING_DATA *data,void *pVoid)
{
	bool isRet = false;
	*strResult = NULL;
	if((unsigned int)nSize > strlen(strSource)) return isRet;
	//---------------��������R�[�h�������Ƃ����ł�---------------------
	Sjis2Euc(strResult,strSource,nSize+1);
	//---------------�����܂ŃR�[�h�������Ƃ����ł�---------------------
	isRet = true;
	return isRet;
}

//--------------------------------------------------
//�֐���	Sjis2Euc(char *EucStr,char *SjisBuff)
//�@�\		�n���ꂽS-JIS�������EUC�ɕϊ����܂�
//����		�i�[�敶����(char *EucStr)
//			�ϊ���������(char *SjisBuff)
//--------------------------------------------------
void Sjis2Euc(char *EucStr,char *SjisBuff,int nSize)
{
	unsigned int i;
	unsigned char hib, lob;
	char *Buff;

//	Buff = new char[nSize];
//	if(!Buff) return;
	Buff = EucStr;
	strcpy(Buff,SjisBuff);
	//�����珈��
	for(i = 0; i <= strlen(Buff) + 1; i++) {
		hib = (unsigned char)Buff[i];
		if(SJIS1(hib)) {//��1�o�C�g����
			i++;
			lob = (unsigned char)Buff[i];
			if(SJIS2(lob)) {//��2�o�C�g����
				hib -= (hib <= 0x9f) ? 0x71 : 0xb1;//�ȉ��ϊ�
				hib = (hib << 1) + 1;
				if (lob >= 0x9e) {
					lob -= 0x7e;
					hib++;
				}
				else if (lob > 0x7f) lob -= 0x20;
				else lob -= 0x1f;
				hib |= 0x80;
				lob |= 0x80;
				Buff[i - 1] = (char)hib;
				Buff[i] = (char)lob;
			}
		}
	}
	strcpy(EucStr,Buff);//�ϊ���փR�s�[
//	if(Buff) delete [] Buff;
	return;
}

