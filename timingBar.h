//=============================================================================
//
// �^�C�~���O�o�[�̏��� [timingBar.h]
// Author : �F�V�`�O
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define NOTE_MAX			(10)		// �����̍ő吔
#define NOTE_DIS			(10)		// �����̊Ԋu(�b)
#define NOTE_TEX_MAX		(4)			// �����̎��
#define NOTE_TIMING_GOOD	(50.0f)		// �ǂ��̃^�C�~���O�͈̔�
#define NOTE_TIMING_JUST	(20.0f)		// �ō��̃^�C�~���O�͈̔�
#define NOTE_TIMING_MISS	(70.0f)		// �~�X�̃^�C�~���O�͈̔�

#define SENHU_X				(960.0f / 2.0f)	// �ܐ����̕\���ʒu
#define SENHU_Y				(370.0f)		// �ܐ����̕\���ʒu
#define SENHU_ALFA			(0.5f)			// �ܐ����̕\���ʒu

#define TARGET_X			(150.0f)	// �^�[�Q�b�g�̕\���ʒu
#define TARGET_Y			(SENHU_Y)	// �^�[�Q�b�g�̕\���ʒu



// �����̍\����
struct TIMINGNOTE
{
	XMFLOAT2	pos;
	BOOL		use;
	int			texNum;
	BOOL		seDid;

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTImingBar(void);
void UninitTImingBar(void);
void UpdateTImingBar(void);
void DrawTImingBar(void);

void SetNote(void);
int GetNoteTiming(void);

