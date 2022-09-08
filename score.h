//=============================================================================
//
// スコア処理 [score.h]
// Author : 熊澤義弘
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCORE_MAX			(99999)		// スコアの最大値
#define SCORE_DIGIT			(5)			// ステージ桁数
#define COMBO_MAX_DIGIT		(3)			// 最大コンボの桁数

#define CITY_STAGE0_BORDER	(4000)		// ステージ0のクリアスコア
#define CITY_STAGE1_BORDER	(8000)		// ステージ1のクリアスコア
#define SEA_STAGE0_BORDER	(4000)		// ステージ0のクリアスコア
#define SEA_STAGE1_BORDER	(8000)		// ステージ1のクリアスコア
#define SKY_STAGE0_BORDER	(4000)		// ステージ0のクリアスコア
#define SKY_STAGE1_BORDER	(8000)		// ステージ1のクリアスコア

//#define SCORE_STAGE0_BORDER	(200)		// ステージ0のクリアスコア
//#define SCORE_STAGE1_BORDER	(200)		// ステージ1のクリアスコア
//#define SCORE_STAGE2_BORDER	(200)		// ステージ2のクリアスコア
//#define SCORE_STAGE3_BORDER	(200)		// ステージ3のクリアスコア


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

void AddScore(int add);
int GetScore(void);


void Seta(float a);
