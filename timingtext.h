//=============================================================================
//
// �^�C�~���O�e�L�X�g�̏��� [timingtext.h]
// Author : �F�V�`�O
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TTEXT_MAX		(5)		// �^�C�~���O�e�L�X�g�̍ő吔



// �����̍\����
struct TIMINGTEXT
{
	XMFLOAT2	pos;
	BOOL		use;
	int			texNum;
	int			life;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTimingText(void);
void UninitTimingText(void);
void UpdateTimingText(void);
void DrawTimingText(void);

void SetTimingText(int text);

void SetNoteMove(float onpu_speed);
