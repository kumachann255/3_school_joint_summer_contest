//=============================================================================
//
// 海パーティクル処理 [sea_particle.h]
// Author : 熊澤義弘 + 大塚勝亮
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************

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
	int				tex_No;			// 使用テクスチャ番号
	float			speed;			// 移動スピード
	float			time;			// 時間
	BOOL			use;			// 使用しているかどうか

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSeaParticle(void);
void UninitSeaParticle(void);
void UpdateSeaParticle(void);
void DrawSeaParticle(void);

void SetSeaParticleTako();
void SetSeaParticleSame();
void SetSeaBonb(XMFLOAT3 pos);

void SetColorSeaParticle(int nIdxSeaParticle, XMFLOAT4 col);

