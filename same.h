//=============================================================================
//
// サメ処理 [same.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_SAME		(1)					// サメの数
#define	SAME_SIZE		(10.0f)				// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
enum
{
	GURUGURU,
	DOWN_FIRST,
	PACKNCYO,
	DOWN_LAST
};



struct SAME
{
	XMFLOAT3		pos;		// ポリゴンの位置
	XMFLOAT3		rot;		// ポリゴンの向き(回転)
	XMFLOAT3		scl;		// ポリゴンの大きさ(スケール)

	XMFLOAT4X4		mtxWorld;	// ワールドマトリックス

	BOOL			load;
	DX11_MODEL		model;		// モデル情報

	float			angle;
	float			spd;		// 移動スピード
	float			dir;		// 向き
	float			size;		// 当たり判定の大きさ
	int				shadowIdx;	// 影のIndex
	BOOL			use;

	int				mode;

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSame(void);
void UninitSame(void);
void UpdateSame(void);
void DrawSame(void);

SAME *GetSame(void);
void SetSame(void);

