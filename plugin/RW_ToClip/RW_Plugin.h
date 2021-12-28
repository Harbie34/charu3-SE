
#include <list>
using namespace std;

//�f�[�^���
#define KIND_ONETIME 0x01	//�����^�C������
#define KIND_LOCK    0x02	//�m�[�}������
#define KIND_FOLDER  0x04	//�t�H���_����
#define KIND_RIREKI  0x08	//�����t�H���_

#define KIND_DATE	1	//���t
#define KIND_EXE    2	//���s
#define KIND_RELATE 3	//�֘A�t��
#define KIND_SELECT 4	//�I��
#define KIND_CLIP   5	//�N���b�v�{�[�h
#define KIND_PLUG   6   //�v���O�C��
#define KIND_KEY    7   //�L�[�}�N��

//�A�C�R���ԍ�
#define ICON_FOLDER	0
#define ICON_RIREKI_FOLDER 2

#define ICON_KEY	4
#define ICON_ONETIME 6
#define ICON_DATE	8
#define ICON_EXE	10
#define ICON_RELATE	12
#define ICON_SELECT	14
#define ICON_CLIP	16
#define ICON_PLUG	18
#define ICON_KEYM	20

#define ROOT -1

struct STRING_DATA
{
	char	m_cKIND;		//�f�[�^���
	char	m_cIcon;		//�A�C�R�����
	int		m_nMyID;		//�f�[�^��ID
	int		m_nParentID;	//�e�f�[�^��ID
	time_t	m_timeCreate;	//�쐬����
	time_t	m_timeEdit;		//�ύX����
	CString  m_strTitle;	//�ݒ�f�[�^�^�C�g��
	CString  m_strData;		//�ݒ�f�[�^������
	CString  m_strMacro;		//�g���p������f�[�^
};

