//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : 熊澤義弘
//
//=============================================================================
#pragma once

//*****************************************************************************
// 構造体定義
//*****************************************************************************

// パーティクルの種類
enum
{
	PARTICLE_TYPE_CUP,			// カップ用
	PARTICLE_TYPE_CUP_NOTE,		// カップ音符用
	PARTICLE_TYPE_BLAST,		// ネバネバ用
	PARTICLE_TYPE_MAX,
};


struct PARTICLE
{
	XMFLOAT3		pos;			// 位置
	XMFLOAT3		rot;			// 回転
	XMFLOAT3		scl;			// スケール
	XMFLOAT3		move;			// 移動量
	
	MATERIAL		material;		// マテリアル

	int				nIdxShadow;		// 影ID
	
	int				texNo;			// テクスチャ番号
	int				type;			// タイプ
	int				life;			// 寿命
	
	float			pop;			// 表示タイミング
	
	BOOL			use;			// 使用しているかどうか

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

int SetParticle(int type, int texNo, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 move, XMFLOAT4 col, float pop, int life);
void SetColorParticle(int nIdxParticle, XMFLOAT4 col);

