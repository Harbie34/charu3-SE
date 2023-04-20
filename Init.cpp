/*----------------------------------------------------------
	�ݒ�Ǘ��N���X
									2002/11/16 (c)Keizi
----------------------------------------------------------*/

#include "stdafx.h"
#include "Charu3.h"
#include "Init.h"
#include "StringWork.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//---------------------------------------------------
//�֐���	CInit
//�@�\		�R���X�g���N�^
//---------------------------------------------------
CInit::CInit()
{
	m_nSelectID = 0;		//�I�����Ă�ID
	m_nOptionPage = 0;
	m_DialogSize.x = 100;	//�_�C�A���O�̃T�C�Y
	m_DialogSize.y = 100;	//�_�C�A���O�̃T�C�Y

	m_nSerchKind = 0;		//�������
	m_nSerchLogic = 0;		//�������@

	m_strIniFile = "";
	m_nOptionPage = 0;
	m_nRecNumber = 0;

	m_IconSize.x = ICON_SIZE;
	m_IconSize.y = ICON_SIZE;

	m_hHookDLL = NULL;
}

//---------------------------------------------------
//�֐���	~CInit
//�@�\		�f�X�g���N�^
//---------------------------------------------------
CInit::~CInit()
{
	if(m_hHookDLL) unHook();
}

//---------------------------------------------------
//�֐���	initialize()
//�@�\		������
//---------------------------------------------------
void CInit::initialize()
{
	CString StringBuff,StringBuff2;
	TCHAR strBuff[MAX_PATH];
	TCHAR *cpName;

	//���[�U�[�����擾
	DWORD dwSize = _countof(strBuff);
	m_strUserName = _T("Charu3User");
	if(::GetUserName(strBuff,&dwSize)) {
		m_strUserName = strBuff;
	}

	//���s�p�X���擾�A�쐬
	GetModuleFileName(NULL,strBuff,_countof(strBuff));
	cpName = _tcsrchr(strBuff,_T('\\'));
	cpName++;
	*cpName = NULL;
	m_strAppPath = strBuff;
}

//---------------------------------------------------
//�֐���	readAllInitData()
//�@�\		�ǂݍ���
//---------------------------------------------------
void CInit::readAllInitData()
{
	if(m_strIniFile == "") return;

	int i;
	int nKetsetCount;
	CString strKeyBuff,StringBuff;
	CString strRes;
	CHANGE_KEY key;
	TCHAR strBuff[1024],*szName,*szKind,*szMacro;

	//��ʐݒ�	
	m_etc.m_nMulchUser    = setIniFileInt(REGKEY_ETC,_T("MulchUser"),1);//�}���`���[�U�[�Ŏg�����ǂ���

	m_etc.m_nToClip    = setIniFileInt(REGKEY_ETC,_T("ToClipBord"),1);//���X�g����I�񂾕�����̓N���b�v�{�[�h�ɂ������
	m_etc.m_nToolTip   = setIniFileInt(REGKEY_ETC,_T("FixToolTip"),0);//�^�X�N�g���C�A�C�R���̃c�[���`�b�v���Œ�ɂ���
	m_etc.m_nIconClick = setIniFileInt(REGKEY_ETC,_T("IconClickAct"),0);//�A�C�R�����N���b�N�����Ƃ��̓���
	m_etc.m_nWordBugSW = setIniFileInt(REGKEY_ETC,_T("WordBug"),0);	//Word�̃o�O�ɑΉ�����
	m_etc.m_nSelfDiagnosisTime = setIniFileInt(REGKEY_ETC,_T("SelfDiagnosisTime"),10000);	//���Ȑf�f�^�C�}�[
	m_etc.m_nActiveTime = setIniFileInt(REGKEY_ETC,_T("ActiveTime"),400);	//�Ď��^�C�}�[
	m_etc.m_nMinimization = setIniFileInt(REGKEY_ETC,_T("Minimization"),1);	//�ŏ�������
	m_etc.m_nDontSave = setIniFileInt(REGKEY_ETC,_T("DontSave"),0);
	m_etc.m_nCopyWaitCnt = setIniFileInt(REGKEY_ETC,_T("CopyWaitCnt"),5);
	m_etc.m_nPasteWaitCnt = setIniFileInt(REGKEY_ETC,_T("PasteWaitCnt"),10);

	

	//�|�b�v�A�b�v�ݒ�	
	m_pop.m_nSelectSW  = setIniFileInt(REGKEY_POPUP,_T("HoldSelect"),1);//�e�L�X�g���X�g�̑I���ʒu��ێ�����
	m_pop.m_nFolderSW  = setIniFileInt(REGKEY_POPUP,_T("HoldFolder"),1);//�e�L�X�g���X�g�̃t�H���_��Ԃ�ێ�����
	m_pop.m_nCornerPopup = setIniFileInt(REGKEY_POPUP,_T("CornerPopup"),0);//�}�E�X�J�[�\������ʂ̒[�ɐG�ꂽ��|�b�v�A�b�v����
	m_pop.m_nCornerPopupTime = setIniFileInt(REGKEY_POPUP,_T("CornerPopupTime"),400);
	m_pop.m_nCornerPopupPix  = setIniFileInt(REGKEY_POPUP,_T("CornerPopupPix"),2);

	m_pop.m_nDoubleKeyPOP = setIniFileInt(REGKEY_POPUP,_T("PopupKeyDC"),0);
	m_pop.m_uVK_Pouup   = setIniFileInt(REGKEY_POPUP,_T("PopupKey"),88);		//�|�b�v�A�b�v�L�[
	m_pop.m_uMod_Pouup  = setIniFileInt(REGKEY_POPUP,_T("PopupKeyM"),MOD_ALT);	//�|�b�v�A�b�v����L�[
	m_pop.m_nDCKeyPopTime = setIniFileInt(REGKEY_POPUP,_T("DCPopupKeyTime"),250);
	
	m_pop.m_nDoubleKeyFIFO = setIniFileInt(REGKEY_POPUP,_T("FifoKeyDC"),0);
	m_pop.m_uVK_Fifo  = setIniFileInt(REGKEY_POPUP,_T("FifoKey"),29);			//����FIFO�؂�ւ��L�[
	m_pop.m_uMod_Fifo = setIniFileInt(REGKEY_POPUP,_T("FifoKeyM"),MOD_CONTROL);	//����FIFO�؂�ւ�����L�[
	m_pop.m_nDCKeyFifoTime = setIniFileInt(REGKEY_POPUP,_T("DCFifoKeyTime"),250);

	m_pop.m_nPopupPos     = setIniFileInt(REGKEY_POPUP,_T("PopupPos"),0);	//�|�b�v�A�b�v�o���ʒu
	m_pop.m_posCaretHosei.x = setIniFileInt(REGKEY_POPUP,_T("CaretHoseiX"),-20);	//�L�����b�g�ʒu�̕␳�l
	m_pop.m_posCaretHosei.y = setIniFileInt(REGKEY_POPUP,_T("CaretHoseiY"),0);	//�L�����b�g�ʒu�̕␳�l
	m_pop.m_nOutRevice      = setIniFileInt(REGKEY_POPUP,_T("ReviseOutOfDesktop"),1);

	m_pop.m_nQuickEnter = setIniFileInt(REGKEY_POPUP,_T("QuickEnter"),0);	//�N�C�b�N�A�N�Z�X�I�����̓\��t��
	m_pop.m_nQuickTime  = setIniFileInt(REGKEY_POPUP,_T("QuickTime"),400);		//�N�C�b�N�A�N�Z�X�m�莞��
	m_pop.m_nQuickChar  = setIniFileInt(REGKEY_POPUP,_T("QuickChar"),0);
	m_pop.m_nAutoImeOff  = setIniFileInt(REGKEY_POPUP,_T("AutoImeOff"),0);
	m_pop.m_nSingleExpand = setIniFileInt(REGKEY_POPUP,_T("SingleExpand"),0);

	//����FIFO�ݒ�	
	m_fifo.m_nFifo = setIniFileInt(REGKEY_FIFO,_T("FifoSW"),1);			//���𓮍쒆��Ctrl+V�œ\��t���镶����������o���ɂ��� 0NON 1FIFO 2RIFO
	m_fifo.m_nCopySound = setIniFileInt(REGKEY_FIFO,_T("CopySound"),1);	//�����L�^���̓R�s�[���Ɍ��ʉ���炷
	m_fifo.m_nPasteSound = setIniFileInt(REGKEY_FIFO,_T("PasteSound"),0);	//�\��t�����ɂ�����炷�X�C�b�`
	m_fifo.m_strWaveFile = setIniFileString(REGKEY_FIFO,_T("WaveName"),_T("pu.wav"));//���ʉ��̃t�@�C����
	m_fifo.m_nOffClear = setIniFileInt(REGKEY_FIFO,_T("OffClear"),0);		//����OFF���Ɉꎞ���ڂ��N���A
	m_fifo.m_nNoClear = setIniFileInt(REGKEY_FIFO,_T("NoClear"),1);		//�������[�h�ōŌ�ɋ�ɂ��Ȃ��X�C�b�`
	m_fifo.m_nAllClearOff = setIniFileInt(REGKEY_FIFO,_T("AllClearOff"),1);//�ꎞ��������ɂȂ����痚�����[�h��OFF�ɂ���
	m_fifo.m_nDuplication = setIniFileInt(REGKEY_FIFO,_T("Duplication"),1);

	//�r�W���A���ݒ�
	m_visual.m_nTransparentSW = setIniFileInt(REGKEY_VISUAL,_T("TransparentSW"),0);	//�����@�\���g�����X�C�b�`
	m_visual.m_nSemitransparent = setIniFileInt(REGKEY_VISUAL,_T("Semitransparent"),50);	//�����x
	strRes.LoadString(APP_INF_DEF_FONT);
	m_visual.m_strFontName = setIniFileString(REGKEY_VISUAL,_T("FontName"),strRes);//�t�H���g��
	m_visual.m_nFontSize = setIniFileInt(REGKEY_VISUAL,_T("FontSize"),100);					//�t�H���g�T�C�Y
	strRes.LoadString(APP_INF_DEF_ICON);
	m_visual.m_strResourceName = setIniFileString(REGKEY_VISUAL,_T("ResourceName"),strRes);//���\�[�X��
	m_visual.m_nBorderColor = setIniFileInt(REGKEY_VISUAL,_T("BorderColor"),0xff9900);	//�g�F
	m_visual.m_nBackColor = setIniFileInt(REGKEY_VISUAL,_T("BackColor"),0xffffe0);		//�w�i�F
	m_visual.m_nTextColor = setIniFileInt(REGKEY_VISUAL,_T("TextColor"),0x663300);		//�e�L�X�g�F
	m_visual.m_nResetTree = setIniFileInt(REGKEY_VISUAL,_T("ResetTree"),1);	//�c���[�̎����č\�z
	m_visual.m_nScrollbar = setIniFileInt(REGKEY_VISUAL,_T("Scrollbar"),SB_BOTH);
	m_visual.m_nToolTip = setIniFileInt(REGKEY_VISUAL,_T("ToolTip"),0);

	//���z�L�[�ݒ�
	WINDOWS_MESSAGE winMessage;
	m_key.m_defKeySet.m_nMessage = setIniFileInt(REGKEY_KEY,_T("isMessage"),0);		//���b�Z�[�W�����t���O
	m_key.m_defKeySet.m_uMod_Paste = setIniFileInt(REGKEY_KEY,_T("ModPaste"),MOD_CONTROL);	//�\��t������L�[
	m_key.m_defKeySet.m_uVK_Paste  = setIniFileInt(REGKEY_KEY,_T("VKPaste"),'V');			//�\��t���L�[
	m_key.m_defKeySet.m_uMod_Copy  = setIniFileInt(REGKEY_KEY,_T("ModCopy"),MOD_CONTROL);	//�R�s�[����L�[
	m_key.m_defKeySet.m_uVK_Copy   = setIniFileInt(REGKEY_KEY,_T("VkCopy"),'C');			//�R�s�[�L�[
	m_key.m_defKeySet.m_nCopyWait  = setIniFileInt(REGKEY_KEY,_T("CopyWait"),50);		//�R�s�[�҂�����
	m_key.m_defKeySet.m_nPasteWait = setIniFileInt(REGKEY_KEY,_T("PasteWait"),50);		//�y�[�X�g�҂�����
	//���b�Z�[�W
	strKeyBuff = setIniFileString(REGKEY_KEY,_T("CopyMessage"),_T("301,0,0"));
	_stscanf_s(LPCTSTR(strKeyBuff),_T("%x,%x,%x"),&winMessage.Msg,&winMessage.wParam,&winMessage.lParam);
	m_key.m_defKeySet.m_copyMessage = winMessage;

	strKeyBuff = setIniFileString(REGKEY_KEY,_T("PasteMessage"),_T("302,0,0"));
	_stscanf_s(LPCTSTR(strKeyBuff),_T("%x,%x,%x"),&winMessage.Msg,&winMessage.wParam,&winMessage.lParam);
	m_key.m_defKeySet.m_pasteMessage = winMessage;
	//�N���b�v�{�[�h�e�ʐ���
	m_key.m_nHistoryLimit = setIniFileInt(REGKEY_KEY,_T("HistoryLimit"),-1);	

	//�ŗL�L�[�ݒ��ǂ�
	nKetsetCount = setIniFileInt(REGKEY_KEY,_T("KeySetCount"),0);
	m_key.m_KeyList.clear();
	for(i = 0; i < nKetsetCount; i++){
		StringBuff.Format(_T("KeySetTitle_%d"),i);
		//�E�B���h�E�L���v�V����
		key.m_strTitle = setIniFileString(REGKEY_KEY,StringBuff,_T(""));
		//�}�b�`���@
		StringBuff.Format(_T("KeySetMatch_%d"),i);
		key.m_nMatch = setIniFileInt(REGKEY_KEY,StringBuff,0);
		//�L�[�R�[�h
		StringBuff.Format(_T("KeySetCode_%d"),i);
		strKeyBuff = setIniFileString(REGKEY_KEY,StringBuff,"");
		_stscanf_s(LPCTSTR(strKeyBuff),_T("%d,%d,%d,%d")
			,&key.m_sCopyPasteKey.m_uMod_Copy,&key.m_sCopyPasteKey.m_uVK_Copy
			,&key.m_sCopyPasteKey.m_uMod_Paste,&key.m_sCopyPasteKey.m_uVK_Paste);
		StringBuff.Format(_T("KeyCopyWait_%d"),i);
		key.m_sCopyPasteKey.m_nCopyWait  = setIniFileInt(REGKEY_KEY,StringBuff,100);		//�R�s�[�҂�����
		StringBuff.Format(_T("KeyPasteWait_%d"),i);
		key.m_sCopyPasteKey.m_nPasteWait = setIniFileInt(REGKEY_KEY,StringBuff,100);		//�y�[�X�g�҂�����

		StringBuff.Format(_T("KeyMessage_%d"),i);
		key.m_sCopyPasteKey.m_nMessage = setIniFileInt(REGKEY_KEY,StringBuff,0);		//����

		//���b�Z�[�W
		StringBuff.Format(_T("CopyMessage_%d"),i);
		strKeyBuff = setIniFileString(REGKEY_KEY,StringBuff,_T("301,0,0"));
		_stscanf_s(LPCTSTR(strKeyBuff),_T("%x,%x,%x"),&winMessage.Msg,&winMessage.wParam,&winMessage.lParam);
		key.m_sCopyPasteKey.m_copyMessage = winMessage;
	
		StringBuff.Format(_T("PasteMessage_%d"),i);
		strKeyBuff = setIniFileString(REGKEY_KEY,StringBuff,_T("302,0,0"));
		_stscanf_s(LPCTSTR(strKeyBuff),_T("%x,%x,%x"),&winMessage.Msg,&winMessage.wParam,&winMessage.lParam);
		key.m_sCopyPasteKey.m_pasteMessage = winMessage;
		//�N���b�v�{�[�h�e�ʐ���
		StringBuff.Format(_T("HistoryLimit_%d"),i);
		key.m_nHistoryLimit = setIniFileInt(REGKEY_KEY,StringBuff,-1);	
		
		m_key.m_KeyList.insert(m_key.m_KeyList.end(),key);//�ݒ�ɒǉ�
	}	

	//��ʊ��ݒ�
	m_strDataFile = setIniFileString(REGKEY_ENV,_T("DataFile"),m_strAppPath + m_strUserName + _T("\\") + DAT_FILE);

	m_strRwPluginFolder = setIniFileString(REGKEY_ENV,_T("RwPluginFolder"),m_strAppPath + _T("RW_Plugin"));
	m_strPluginName = setIniFileString(REGKEY_ENV,_T("DataFormat"),DAT_FORMAT);
	m_strMacroPluginName = setIniFileString(REGKEY_ENV,_T("MacroPlugin"),DAT_FORMAT);
	m_nSelectID     = setIniFileInt(REGKEY_ENV,_T("SelectID"),-1);
	time_t lTime;
	m_nTreeID       = setIniFileInt(REGKEY_ENV,_T("TreeID"),time(&lTime));
	m_nRecNumber    = setIniFileInt(REGKEY_ENV,_T("RecNumber"),0);
	m_strBBS        = setIniFileString(REGKEY_ENV,_T("SupportBBS"),_T("http://8537.teacup.com/keiziweb/bbs"));
	m_nToolTipTime  = setIniFileInt(REGKEY_ENV,_T("ToolTipTime"),30000);
	m_nToolTipDelay = setIniFileInt(REGKEY_ENV,_T("ToolTipDelay"),300);
	m_nDebug		= setIniFileInt(REGKEY_ENV,_T("Debug"),0);
	m_strDebugLog	= setIniFileString(REGKEY_ENV,_T("LogfileName"),_T("charu3.log"));


	//�E�B���h�E�ݒ�
	m_DialogSize.x = setIniFileInt(REGKEY_WINDOW,_T("PopupSizeX"),250);
	m_DialogSize.y = setIniFileInt(REGKEY_WINDOW,_T("PopupSizeY"),350);

	//�����ݒ�
	m_nSerchKind  = setIniFileInt(REGKEY_SERCH,_T("SerchKind"),0);		//�������
	m_nSerchLogic = setIniFileInt(REGKEY_SERCH,_T("SerchLogic"),0);		//�������@
	m_strSerchKey  = setIniFileString(REGKEY_SERCH,_T("SerchKey"),_T(""));	//�����L�[

	//�}�N���e���v���[�g
	for(i = 0; i <= 99; i++) {
		strRes.LoadString(APP_INF_MACRO_TEMPLATE01+i);
		if(strRes == _T("end")) break;
	}
	int nStringCnt = i;
	MACRO_STRUCT macroData;
	CString strMacro;
	m_vctMacro.clear();
	m_vctMacro.reserve(99);
	for(i = 1; i <= 99; i++) {
		StringBuff.Format(_T("Macro_%02d"),i);
		::GetPrivateProfileString(REGKEY_MACRO,StringBuff,_T(""),strBuff,_countof(strBuff),m_strIniFile);
		if (_tcsclen(strBuff) == 0 && i <= nStringCnt) {
			strRes.LoadString(APP_INF_MACRO_TEMPLATE01 + i -1);
			writeProfileString(REGKEY_MACRO,StringBuff,strRes);
			strMacro = strRes;
			_tcscpy_s(strBuff,LPCTSTR(strRes));
		}
		if(_tcsclen(strBuff) == 0) break;
		if(UStringWork::splitString(strBuff,_T('@'),&szName,&szKind,&szMacro,NULL) == 3) {
			macroData.m_strName = szName;
			if(*szKind == _T('D'))		macroData.m_cKind = KIND_DATA_ALL;
			else if(*szKind == _T('F'))	macroData.m_cKind = KIND_FOLDER_ALL;
			else					macroData.m_cKind = 0xff;
			macroData.m_strMacro = szMacro;
			m_vctMacro.push_back(macroData);
		}
	}
	
	//�g���}�N���e���v���[�g R�����t�H���_ F�t�H���_ A�S�� D�f�[�^
	for(i = 0; i <= 99; i++) {
		strRes.LoadString(APP_INF_EXMACRO_TEMPLATE01+i);
		if(strRes == "end") break;
	}
	nStringCnt = i;
	m_vctDataMacro.clear();
	m_vctDataMacro.reserve(99);
	for(i = 1; i <= 99; i++) {
		StringBuff.Format(_T("DataMacro_%02d"),i);
		::GetPrivateProfileString(REGKEY_MACRO,StringBuff,_T(""),strBuff,_countof(strBuff),m_strIniFile);
		if (_tcsclen(strBuff) == 0 && i <= nStringCnt) {
			strRes.LoadString(APP_INF_EXMACRO_TEMPLATE01 + i -1);
			writeProfileString(REGKEY_MACRO,StringBuff,strRes);
			strMacro = strRes;
			_tcscpy_s(strBuff,LPCTSTR(strRes));
		}
		if(_tcsclen(strBuff) == 0) break;
		if(UStringWork::splitString(strBuff,'@',&szName,&szKind,&szMacro,NULL) == 3) {
			macroData.m_strName = szName;
			if(*szKind == 'D')		macroData.m_cKind = KIND_DATA_ALL;
			else if(*szKind == 'R')	macroData.m_cKind = KIND_RIREKI;
			else if(*szKind == 'F')	macroData.m_cKind = KIND_FOLDER_ALL;
			else					macroData.m_cKind = 0xff;
			macroData.m_strMacro = szMacro;
			m_vctDataMacro.push_back(macroData);
		}
	}
}

//---------------------------------------------------
//�֐���	writeAllInitData()
//�@�\		��������
//---------------------------------------------------
void CInit::writeAllInitData()
{
	if(m_strIniFile == "") return;

	//��ʐݒ�	
	writeProfileInt(REGKEY_ETC,_T("ToClipBord"),m_etc.m_nToClip);		//���X�g����I�񂾕�����̓N���b�v�{�[�h�ɂ������
	writeProfileInt(REGKEY_ETC,_T("FixToolTip"),m_etc.m_nToolTip);		//�^�X�N�g���C�A�C�R���̃c�[���`�b�v���Œ�ɂ���
	writeProfileInt(REGKEY_ETC,_T("IconClickAct"),m_etc.m_nIconClick);	//�A�C�R�����N���b�N�����Ƃ��̓���
	writeProfileInt(REGKEY_ETC,_T("WordBug"),m_etc.m_nWordBugSW);		//Word�̃o�O�ɑΉ�����
	writeProfileInt(REGKEY_ETC,_T("SelfDiagnosisTime"),m_etc.m_nSelfDiagnosisTime);	//���Ȑf�f�^�C�}�[
	writeProfileInt(REGKEY_ETC,_T("ActiveTime"),m_etc.m_nActiveTime);	//�Ď��^�C�}�[
	writeProfileInt(REGKEY_ETC,_T("Minimization"),m_etc.m_nMinimization);	//�ŏ�������
	writeProfileInt(REGKEY_ETC,_T("DontSave"),m_etc.m_nDontSave);
	
	//�|�b�v�A�b�v�ݒ�	
	writeProfileInt(REGKEY_POPUP,_T("HoldSelect"),m_pop.m_nSelectSW);	//�e�L�X�g���X�g�̑I���ʒu��ێ�����
	writeProfileInt(REGKEY_POPUP,_T("HoldFolder"),m_pop.m_nFolderSW);	//�e�L�X�g���X�g�̃t�H���_��Ԃ�ێ�����
	writeProfileInt(REGKEY_POPUP,_T("CornerPopup"),m_pop.m_nCornerPopup);	//�}�E�X�J�[�\������ʂ̒[�ɐG�ꂽ��|�b�v�A�b�v����

	writeProfileInt(REGKEY_POPUP,_T("PopupKeyDC"),m_pop.m_nDoubleKeyPOP);
	writeProfileInt(REGKEY_POPUP,_T("PopupKey"),m_pop.m_uVK_Pouup);			//�|�b�v�A�b�v�L�[
	writeProfileInt(REGKEY_POPUP,_T("PopupKeyM"),m_pop.m_uMod_Pouup);		//�|�b�v�A�b�v����L�[
	writeProfileInt(REGKEY_POPUP,_T("FifoKeyDC"),m_pop.m_nDoubleKeyFIFO);
	writeProfileInt(REGKEY_POPUP,_T("FifoKey"),m_pop.m_uVK_Fifo);			//����FIFO�؂�ւ��L�[
	writeProfileInt(REGKEY_POPUP,_T("FifoKeyM"),m_pop.m_uMod_Fifo);		//����FIFO�؂�ւ�����L�[

	writeProfileInt(REGKEY_POPUP,_T("PopupPos"),m_pop.m_nPopupPos);	//�|�b�v�A�b�v�o���ʒu
	writeProfileInt(REGKEY_POPUP,_T("CaretHoseiX"),m_pop.m_posCaretHosei.x);	//�L�����b�g�ʒu�̕␳�l
	writeProfileInt(REGKEY_POPUP,_T("CaretHoseiY"),m_pop.m_posCaretHosei.y);	//�L�����b�g�ʒu�̕␳�l
	writeProfileInt(REGKEY_POPUP,_T("ReviseOutOfDesktop"),m_pop.m_nOutRevice);

	writeProfileInt(REGKEY_POPUP,_T("QuickEnter"),m_pop.m_nQuickEnter);//�N�C�b�N�A�N�Z�X�I�����̓\��t��
	writeProfileInt(REGKEY_POPUP,_T("QuickTime"),m_pop.m_nQuickTime);		//�N�C�b�N�A�N�Z�X�m�莞��
	writeProfileInt(REGKEY_POPUP,_T("QuickChar"),m_pop.m_nQuickChar);
	writeProfileInt(REGKEY_POPUP,_T("AutoImeOff"),m_pop.m_nAutoImeOff);
	writeProfileInt(REGKEY_POPUP,_T("SingleExpand"),m_pop.m_nSingleExpand);

	//����FIFO�ݒ�	
	writeProfileInt(REGKEY_FIFO,_T("FifoSW"),m_fifo.m_nFifo);			//���𓮍쒆��Ctrl+V�œ\��t���镶����������o���ɂ��� 0NON 1FIFO 2RIFO
	writeProfileInt(REGKEY_FIFO,_T("CopySound"),m_fifo.m_nCopySound);	//�����L�^���̓R�s�[���Ɍ��ʉ���炷
	writeProfileInt(REGKEY_FIFO,_T("PasteSound"),m_fifo.m_nPasteSound);//�\��t�����ɂ�����炷�X�C�b�`
	writeProfileString(REGKEY_FIFO,_T("WaveName"),m_fifo.m_strWaveFile);	//���ʉ��̃t�@�C����
	writeProfileInt(REGKEY_FIFO,_T("OffClear"),m_fifo.m_nOffClear);	//����OFF���Ɉꎞ���ڂ��N���A
	writeProfileInt(REGKEY_FIFO,_T("NoClear"),m_fifo.m_nNoClear);		//�������[�h�ōŌ�ɋ�ɂ��Ȃ��X�C�b�`
	writeProfileInt(REGKEY_FIFO,_T("AllClearOff"),m_fifo.m_nAllClearOff);//�ꎞ��������ɂȂ����痚�����[�h��OFF�ɂ���
	writeProfileInt(REGKEY_FIFO,_T("Duplication"),m_fifo.m_nDuplication);

	//�r�W���A���ݒ�
	writeProfileInt(REGKEY_VISUAL,_T("TransparentSW"),m_visual.m_nTransparentSW);	//�����@�\���g�����X�C�b�`
	writeProfileInt(REGKEY_VISUAL,_T("Semitransparent"),m_visual.m_nSemitransparent);//�����x
	writeProfileString(REGKEY_VISUAL,_T("FontName"),m_visual.m_strFontName);		//�t�H���g��
	writeProfileInt(REGKEY_VISUAL,_T("FontSize"),m_visual.m_nFontSize);				//�t�H���g�T�C�Y
	writeProfileString(REGKEY_VISUAL,_T("ResourceName"),m_visual.m_strResourceName);//���\�[�X��
	writeProfileInt(REGKEY_VISUAL,_T("BorderColor"),m_visual.m_nBorderColor);		//�g�F
	writeProfileInt(REGKEY_VISUAL,_T("BackColor"),m_visual.m_nBackColor);			//�w�i�F
	writeProfileInt(REGKEY_VISUAL,_T("TextColor"),m_visual.m_nTextColor);			//�e�L�X�g�F
	writeProfileInt(REGKEY_VISUAL,_T("ResetTree"),m_visual.m_nResetTree);	//�c���[�̎����č\�z
	writeProfileInt(REGKEY_VISUAL,_T("Scrollbar"),m_visual.m_nScrollbar);
	writeProfileInt(REGKEY_VISUAL,_T("ToolTip"),m_visual.m_nToolTip);

	//���z�L�[�ݒ�
	CString strBuff,strKeyBuff;
	writeProfileInt(REGKEY_KEY,_T("isMessage"),m_key.m_defKeySet.m_nMessage);	//���b�Z�[�W�����t���O
	writeProfileInt(REGKEY_KEY,_T("ModPaste"),m_key.m_defKeySet.m_uMod_Paste);		//�\��t������L�[
	writeProfileInt(REGKEY_KEY,_T("VKPaste"),m_key.m_defKeySet.m_uVK_Paste);		//�\��t���L�[
	writeProfileInt(REGKEY_KEY,_T("ModCopy"),m_key.m_defKeySet.m_uMod_Copy);		//�R�s�[����L�[
	writeProfileInt(REGKEY_KEY,_T("VkCopy"),m_key.m_defKeySet.m_uVK_Copy);			//�R�s�[�L�[
	writeProfileInt(REGKEY_KEY,_T("CopyWait"),m_key.m_defKeySet.m_nCopyWait);		//�R�s�[�҂�����
	writeProfileInt(REGKEY_KEY,_T("PasteWait"),m_key.m_defKeySet.m_nPasteWait);			//�y�[�X�g�҂�����
	//���b�Z�[�W
	strKeyBuff.Format(_T("%x,%x,%x"),m_key.m_defKeySet.m_copyMessage.Msg,
		m_key.m_defKeySet.m_copyMessage.wParam,
		m_key.m_defKeySet.m_copyMessage.lParam);
	writeProfileString(REGKEY_KEY,_T("CopyMessage"),strKeyBuff);

	strKeyBuff.Format(_T("%x,%x,%x"),m_key.m_defKeySet.m_pasteMessage.Msg,
		m_key.m_defKeySet.m_pasteMessage.wParam,
		m_key.m_defKeySet.m_pasteMessage.lParam);
	writeProfileString(REGKEY_KEY,_T("PasteMessage"),strKeyBuff);
	//�N���b�v�{�[�h�e�ʐ���
	writeProfileInt(REGKEY_KEY,_T("HistoryLimit"),m_key.m_nHistoryLimit);	

	//�ŗL�L�[�ݒ������
	list<CHANGE_KEY>::iterator it;
	int i,nKetsetCount = m_key.m_KeyList.size();
	writeProfileInt(REGKEY_KEY,_T("KeySetCount"),nKetsetCount);
	for(it = m_key.m_KeyList.begin(),i = 0; it != m_key.m_KeyList.end(); it++,i++) {
		//�E�B���h�E�L���v�V����
		strBuff.Format(_T("KeySetTitle_%d"),i);
		writeProfileString(REGKEY_KEY,strBuff,it->m_strTitle);

		//�}�b�`���@
		strBuff.Format(_T("KeySetMatch_%d"),i);
		writeProfileInt(REGKEY_KEY,strBuff,it->m_nMatch);

		//�L�[�R�[�h
		strBuff.Format(_T("KeySetCode_%d"),i);
		strKeyBuff.Format(_T("%d,%d,%d,%d")
			,it->m_sCopyPasteKey.m_uMod_Copy,it->m_sCopyPasteKey.m_uVK_Copy
			,it->m_sCopyPasteKey.m_uMod_Paste,it->m_sCopyPasteKey.m_uVK_Paste);
		writeProfileString(REGKEY_KEY,strBuff,strKeyBuff);

		strBuff.Format(_T("KeyCopyWait_%d"),i);
		writeProfileInt(REGKEY_KEY,strBuff,it->m_sCopyPasteKey.m_nCopyWait);		//�R�s�[�҂�����
		strBuff.Format(_T("KeyPasteWait_%d"),i);
		writeProfileInt(REGKEY_KEY,strBuff,it->m_sCopyPasteKey.m_nPasteWait);		//�y�[�X�g�҂�����

		strBuff.Format(_T("KeyMessage_%d"),i);
		writeProfileInt(REGKEY_KEY,strBuff,it->m_sCopyPasteKey.m_nMessage);		//����

		//���b�Z�[�W
		strBuff.Format(_T("CopyMessage_%d"),i);
		strKeyBuff.Format(_T("%x,%x,%x"),it->m_sCopyPasteKey.m_copyMessage.Msg,
			it->m_sCopyPasteKey.m_copyMessage.wParam,
			it->m_sCopyPasteKey.m_copyMessage.lParam);
		writeProfileString(REGKEY_KEY,strBuff,strKeyBuff);
	
		strBuff.Format(_T("PasteMessage_%d"),i);
		strKeyBuff.Format(_T("%x,%x,%x"),it->m_sCopyPasteKey.m_pasteMessage.Msg,
			it->m_sCopyPasteKey.m_pasteMessage.wParam,
			it->m_sCopyPasteKey.m_pasteMessage.lParam);
		writeProfileString(REGKEY_KEY,strBuff,strKeyBuff);
	
		//�N���b�v�{�[�h�e�ʐ���
		strKeyBuff.Format(_T("HistoryLimit_%d"),i);
		writeProfileInt(REGKEY_KEY,strKeyBuff,it->m_nHistoryLimit);	
	}

	writeEnvInitData();
}


//---------------------------------------------------
//�֐���	writeEnvInitData()
//�@�\		����񂾂���������
//---------------------------------------------------
void CInit::writeEnvInitData()
{
	if(m_strIniFile == "") return;

	//��ʊ��ݒ�
	writeProfileString(REGKEY_ENV,_T("DataFile"),m_strDataFile);
	writeProfileString(REGKEY_ENV,_T("RwPluginFolder"),m_strRwPluginFolder);
	writeProfileString(REGKEY_ENV,_T("DataFormat"),m_strPluginName);
	writeProfileString(REGKEY_ENV,_T("MacroPlugin"),m_strMacroPluginName);

	writeProfileInt(REGKEY_ENV,_T("SelectID"),m_nSelectID);
	writeProfileInt(REGKEY_ENV,_T("TreeID"),m_nTreeID);
	writeProfileInt(REGKEY_ENV,_T("RecNumber"),m_nRecNumber);

	//�E�B���h�E�ݒ�
	writeProfileInt(REGKEY_WINDOW,_T("PopupSizeX"),m_DialogSize.x);
	writeProfileInt(REGKEY_WINDOW,_T("PopupSizeY"),m_DialogSize.y);

	//�����ݒ�
	writeProfileInt(REGKEY_SERCH,_T("SerchKind"),m_nSerchKind);		//�������
	writeProfileInt(REGKEY_SERCH,_T("SerchLogic"),m_nSerchLogic);	//�������@
	writeProfileString(REGKEY_SERCH,_T("SerchKey"),m_strSerchKey);	//�����L�[
}


//---------------------------------------------------
//�֐���	setIniFileInt()
//�@�\		�ݒ�t�@�C����ǂ�
//---------------------------------------------------
int CInit::setIniFileInt(const TCHAR* szSection,const TCHAR* szKey,int nDefault)
{
	int n = (int)::GetPrivateProfileInt(szSection,szKey,-1,m_strIniFile);
	if(n == -1) {
		n = nDefault;
		writeProfileInt(szSection,szKey,n);
	}
	return n;
} 

//---------------------------------------------------
//�֐���	setIniFileString()
//�@�\		�ݒ�t�@�C����ǂ�
//---------------------------------------------------
CString CInit::setIniFileString(const TCHAR* szSection,const TCHAR* szKey,CString strDefault)
{
	TCHAR strBuff[1024];
	CString strRet;
	::GetPrivateProfileString(szSection,szKey,_T(""),strBuff,_countof(strBuff),m_strIniFile);
	strRet = strBuff;
	if (strRet == "")	{
		// �ݒ肪�����ꍇ�̓f�t�H���g(�A�v���P�[�V�����p�X)�ō쐬
		writeProfileString(szSection,szKey,strDefault);
		strRet = strDefault;
	}
	return strRet;
} 

//---------------------------------------------------
//�֐���	writeProfileInt()
//�@�\		�ݒ�t�@�C������������
//---------------------------------------------------
void CInit::writeProfileInt(const TCHAR* szSection,const TCHAR* szKey,int nValue)
{
	TCHAR strBuff[1024];
	_stprintf_s(strBuff,_T("%d"),nValue);	
	WritePrivateProfileString(szSection,szKey,strBuff,m_strIniFile);
}

//---------------------------------------------------
//�֐���	writeProfileString()
//�@�\		�ݒ�t�@�C������������
//---------------------------------------------------
void CInit::writeProfileString(const TCHAR* szSection,const TCHAR* szKey,CString strValue)
{
	WritePrivateProfileString(szSection,szKey,_T("\"") + strValue + _T("\""),m_strIniFile);
}

//---------------------------------------------------
//�֐���	getPasteHotKey
//�@�\		�\��t���n�L�[�ݒ���擾
//---------------------------------------------------
void CInit::getPasteHotKey(UINT *uKey,UINT *uMod,UINT *uCopyKey,UINT *uCopyMod)
{
	*uMod = m_key.m_defKeySet.m_uMod_Paste;
	*uKey = m_key.m_defKeySet.m_uVK_Paste;

	*uCopyMod = m_key.m_defKeySet.m_uMod_Copy;
	*uCopyKey = m_key.m_defKeySet.m_uVK_Copy;

}
//---------------------------------------------------
//�֐���	setPasteHotkey()
//�@�\		�z�b�g�L�[�ݒ���Z�b�g
//---------------------------------------------------
void CInit::setPasteHotkey(UINT uKey,UINT uMod,UINT uCopyKey,UINT uCopyMod)
{
	m_key.m_defKeySet.m_uMod_Paste = uMod;
	m_key.m_defKeySet.m_uVK_Paste = uKey;
	m_key.m_defKeySet.m_uMod_Copy = uCopyMod;
	m_key.m_defKeySet.m_uVK_Copy = uCopyKey;
}
//---------------------------------------------------
//�֐���	setHotkey()
//�@�\		�z�b�g�L�[�ݒ���Z�b�g
//---------------------------------------------------
void CInit::setHotkey(UINT uPopKey,UINT uPopMod,UINT uFifoKey,UINT uFifoMod)
{
	m_pop.m_uMod_Pouup = uPopMod;
	m_pop.m_uVK_Pouup = uPopKey;
	m_pop.m_uMod_Fifo = uFifoMod;
	m_pop.m_uVK_Fifo = uFifoKey;
}
//---------------------------------------------------
//�֐���	getHotkey()
//�@�\		�z�b�g�L�[�ݒ���擾
//---------------------------------------------------
void CInit::getHotKey(UINT *uPopKey,UINT *uPopMod,UINT *uFifoKey,UINT *uFifoMod)
{
	*uPopMod = m_pop.m_uMod_Pouup;
	*uPopKey = m_pop.m_uVK_Pouup;
	*uFifoMod = m_pop.m_uMod_Fifo;
	*uFifoKey = m_pop.m_uVK_Fifo;
}

//---------------------------------------------------
//�֐���	setHookKey()
//�@�\		�L�[�t�b�N��ݒ�
//---------------------------------------------------
bool CInit::setHookKey(HWND hWnd)
{
	bool isRet = false;
	if(!isWindows98()) return true;
	setHook pSetHook;

	if(!m_hHookDLL) {
		m_hHookDLL = LoadLibrary(m_strAppPath + _T("\\HookSet.dll"));
		if(!m_hHookDLL) return isRet;
		pSetHook = (setHook)GetProcAddress(m_hHookDLL,"SetHook");
		if(!pSetHook) return isRet;
		pSetHook(hWnd, WM_KEY_HOOK);

		isRet = true;
	}
	return isRet;
}

//---------------------------------------------------
//�֐���	unHookKey()
//�@�\		�t�b�N������
//---------------------------------------------------
void CInit::unHookKey()
{
	if(!isWindows98()) return;
	FARPROC pUnHook;
	//�t�b�N�̉���
	if(m_hHookDLL) {
		pUnHook = ::GetProcAddress(m_hHookDLL, "UnHook");
		if(pUnHook)	pUnHook();
		FreeLibrary(m_hHookDLL);
		m_hHookDLL = NULL;
	}
}


//---------------------------------------------------
//�֐���	getAppendKeyInit()
//�@�\		�ǉ��L�[�ݒ���擾
//---------------------------------------------------
COPYPASTE_KEY CInit::getAppendKeyInit(CString strWinName,int nNumber)
{
	bool isMatch = false;

	//�L�[�ݒ胊�X�g����K��������̂�T��
	list<CHANGE_KEY>::iterator it;
	CHANGE_KEY key;
	COPYPASTE_KEY ret;
	int nFindPoint;

	ret.m_nMessage = -1;
	for(it = m_key.m_KeyList.begin(); it !=  m_key.m_KeyList.end(); it++) {
		key = *it;
		if(key.m_nMatch != MATCH_PHRASE) {
			nFindPoint = strWinName.Find(key.m_strTitle);
		}
		
		//�O����v �����炠������OK
		if(key.m_nMatch == MATCH_FORWORD) {
			if(nFindPoint == 0)	isMatch = true;
		}
		//�����v�@�Ō�ɂ�������OK
		else if(key.m_nMatch == MATCH_BACK && nFindPoint >= 0) {
			if(nFindPoint == (strWinName.GetLength() - key.m_strTitle.GetLength()))	isMatch = true;
		}
		//�C���N���[�h�@�܂܂�Ă���OK
		else if(key.m_nMatch == MATCH_INCLUDE) {
			if(nFindPoint >= 0)	isMatch = true;
		}
		//�t���[�Y�@���S��v
		else if(key.m_nMatch == MATCH_PHRASE) {
			if(strWinName == key.m_strTitle)	isMatch = true;
		}
		
		if(isMatch) {		//��������
			if(nNumber > 0) {
				nNumber--;
				isMatch = false;
			}
			else {
				ret = key.m_sCopyPasteKey;
				break;
			}
		}
	}
//	if(!isMatch)		ret = m_key.m_defKeySet;//�f�t�H���g�L�[�ݒ�

	return ret;
}


//---------------------------------------------------
//�֐���	getAppendKeyInit()
//�@�\		�ǉ��L�[�ݒ���擾
//---------------------------------------------------
CHANGE_KEY CInit::getAppendKeyInit2(CString strWinName)
{
	bool isMatch = false;

	//�L�[�ݒ胊�X�g����K��������̂�T��
	list<CHANGE_KEY>::iterator it;
	CHANGE_KEY key;
	int nFindPoint;

	for(it = m_key.m_KeyList.begin(); it !=  m_key.m_KeyList.end(); it++) {
		key = *it;
		if(key.m_nMatch != MATCH_PHRASE) {
			nFindPoint = strWinName.Find(key.m_strTitle);
		}
		
		//�O����v �����炠������OK
		if(key.m_nMatch == MATCH_FORWORD) {
			if(nFindPoint == 0)	isMatch = true;
		}
		//�����v�@�Ō�ɂ�������OK
		else if(key.m_nMatch == MATCH_BACK && nFindPoint >= 0) {
			if(nFindPoint == (strWinName.GetLength() - key.m_strTitle.GetLength()))	isMatch = true;
		}
		//�C���N���[�h�@�܂܂�Ă���OK
		else if(key.m_nMatch == MATCH_INCLUDE) {
			if(nFindPoint >= 0)	isMatch = true;
		}
		//�t���[�Y�@���S��v
		else if(key.m_nMatch == MATCH_PHRASE) {
			if(strWinName == key.m_strTitle)	isMatch = true;
		}
		
		if(isMatch) {		//��������
			return key;
		}
	}

	key.m_nMatch = -1;
	return key;
}

