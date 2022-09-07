//=============================================================================
//
// チュートリアル処理 [tutorial.h]
// Author : 熊澤義弘
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
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
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTutorial(void);
void UninitTutorial(void);
void UpdateTutorial(void);
void DrawTutorial(void);

void SetTutorialEnemy(BOOL data);
BOOL GetTutorialUse(void);
