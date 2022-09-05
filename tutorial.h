//=============================================================================
//
// �`���[�g���A������ [tutorial.h]
// Author : �F�V�`�O
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
enum {
	tutorial00 = 1,
	tutorial01,
	tutorial02,
	howto0,
	tutorial03,
	tutorial04,
	tutorial05,
	tutorial00sea,
	tutorial01sea,
	howto1,
	tutorial02sea,
	tutorialMax,
};


struct TUTORIAL
{
	BOOL		use;
	XMFLOAT3	pos;
	float		w;
	float		h;
	int			texNo;


};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTutorial(void);
void UninitTutorial(void);
void UpdateTutorial(void);
void DrawTutorial(void);

void SetTutorialEnemy(BOOL data);
BOOL GetTutorialUse(void);
