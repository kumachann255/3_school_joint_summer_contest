//=============================================================================
//
// ロックオンアイコン処理 [rockOn.h]
// Author : 
//
//=============================================================================
#pragma once
#define MAX_ROCKON			(20)		// 最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct ROCKON
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	BOOL				use;				// 使うかどうか
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRockOn(void);
void UninitRockOn(void);
void UpdateRockOn(void);
void DrawRockOn(void);

ROCKON *GetRockOn();
void SetRockOn(void);
void ResetRockOn(void);


