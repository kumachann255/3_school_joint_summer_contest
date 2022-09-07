//=============================================================================
//
// タイトル画面処理 [target.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_TARGET		(2)
#define MAX_ROCKON		(20)
#define MAX_ROCKON_SEA	(20)

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct TARGET
{
	XMFLOAT3				pos;		// ポリゴンの座標
	XMFLOAT3				rot;		// ポリゴンの座標
	float					w, h;		// 幅と高さ	float
	float					spd;		// 移動スピード
	int						TexNo;		// テクスチャ番号
	BOOL					use;		// TRUE:使っている  FALSE:未使用
	int						count;		// 同時に何体ロックオンしているか
	int						enemyNum[MAX_ROCKON];	// ロックオンしたエネミーの番号
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTarget(void);
void UninitTarget(void);
void UpdateTarget(void);
void DrawTarget(void);

TARGET *GetTarget(void);
BOOL GetTargetArea(int type);
