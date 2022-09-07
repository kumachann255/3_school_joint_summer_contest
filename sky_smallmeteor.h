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
#define	MAX_S_METEOR				(25)		// 小隕石の最大数
#define S_METEOR_SIZE				(70.0f) // 隕石の当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct S_METEOR
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

	int					ModelPattern;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitS_Meteor(void);
void UninitS_Meteor(void);
void UpdateS_Meteor(void);
void DrawS_Meteor(void);

void SetS_Meteor(XMFLOAT3 pos, float rot);

S_METEOR *GetS_Meteor(void);
#pragma once
