//=============================================================================
//
// メテオのパーティクル処理 [particleMeteor.h]
// Author : 熊澤義弘
//
//=============================================================================
#pragma once

//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticleSky(void);
void UninitParticleSky(void);
void UpdateParticleSky(void);
void DrawParticleSky(void);

int SetParticleSky(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 scl, XMFLOAT4 col, int life);
void SetColorParticleSky(int nIdxParticle, XMFLOAT4 col);

