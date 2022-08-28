#pragma once
//=============================================================================
//
// メテオ処理 [meteor.h]
// Author : 山田隆徳
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_B_METEOR				(1)		// 小隕石の最大数
#define B_METEOR_SIZE				(100.0f) // 隕石の当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct B_METEOR
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float				speed;				// 移動スピード
	float				time;				// 時間
	float				size;
	XMFLOAT3			randMove;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitB_Meteor(void);
void UninitB_Meteor(void);
void UpdateB_Meteor(void);
void DrawB_Meteor(void);

void SetB_Meteor(XMFLOAT3 pos, float rot);

B_METEOR *GetB_Meteor(void);
#pragma once
#pragma once
