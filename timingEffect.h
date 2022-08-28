//=============================================================================
//
// �^�C�~���O�G�t�F�N�g�̏��� [timingEffect.h]
// Author : �F�V�`�O
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEFFECT_MAX			(5)		// �^�C�~���O�G�t�F�N�g�̍ő吔
#define TEFFECT_TYPE_MAX	(3)		// �G�t�F�N�g�̎��


// �����̍\����
struct TIMINGEFFECT
{
	XMFLOAT2	pos;
	XMFLOAT2	size;
	BOOL		use;
	int			texNum;
	int			life;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTimingEffect(void);
void UninitTimingEffect(void);
void UpdateTimingEffect(void);
void DrawTimingEffect(void);

void SetTimingEffect(int text);
