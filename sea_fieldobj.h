//=============================================================================
//
// 海オブジェクトの処理 [sea_fieldobj.h]
// Author : 大塚勝亮
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_SEA_FOBJ_SIGN	(5)						// オブジェクトの数
#define MAX_SEA_WAVE		(2)						// オブジェクトの数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct SEA_FOBJ
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色
	int					moveFlag;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSeaFieldObj(void);
void UninitSeaFieldObj(void);
void UpdateSeaFieldObj(void);
void DrawSeaFieldObj(void);