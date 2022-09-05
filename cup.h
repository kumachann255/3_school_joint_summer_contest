//=============================================================================
//
// カップと当たり判定処理 [cup.h]
// Author : エナ
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_CUP			(1)
#define	CUP_SIZE			(50.0f)				// 当たり判定の大きさ

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct CUP
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float				size;				// 当たり判定の大きさ
	int					life;				// 爆破の残り時間
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitCup(void);
void UninitCup(void);
void UpdateCup(void);
void DrawCup(void);

CUP *GetCup(void);
void SetCup(void);

BOOL GetCupParticleOn(void);

BOOL GetCameraSwitch(void);
void SetCameraSwitch(BOOL data);
int GetMorphing(void);
int GetStopTime(void);
