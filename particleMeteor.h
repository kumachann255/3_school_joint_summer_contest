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
HRESULT InitParticleMeteor(void);
void UninitParticleMeteor(void);
void UpdateParticleMeteor(void);
void DrawParticleMeteor(void);

int SetParticleMeteor(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 scl, XMFLOAT4 col, int life);
void SetColorParticleMeteor(int nIdxParticle, XMFLOAT4 col);

