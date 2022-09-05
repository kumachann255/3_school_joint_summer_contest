//=============================================================================
//
// 海パーティクル処理 [sea_particle.h]
// Author : 熊澤義弘 + 大塚勝亮
//
//=============================================================================
#pragma once

//*****************************************************************************
// 構造体定義
//*****************************************************************************

// パーティクルの種類
enum
{
	PARTICLE_TYPE_TAKO,			// タコ用
	PARTICLE_TYPE_SAME,			// サメ用
	PARTICLE_TYPE_MAX,
};


struct SEA_PARTICLE
{
	XMFLOAT3		pos;			// 位置
	XMFLOAT3		rot;			// 回転
	XMFLOAT3		scl;			// スケール
	XMFLOAT3		move;			// 移動量
	
	MATERIAL		material;		// マテリアル
	
	int				type;			// タイプ
	int				life;			// 寿命
	int				pop;			// 表示タイミング
	
	BOOL			use;			// 使用しているかどうか

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSeaParticle(void);
void UninitSeaParticle(void);
void UpdateSeaParticle(void);
void DrawSeaParticle(void);

//int SetSeaParticleSetSeaParticle(int type, XMFLOAT3 pos, int life);
//void SetColorSeaParticle(int nIdxSeaParticle, XMFLOAT4 col);

