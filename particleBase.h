//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticleBase(void);
void UninitParticleBase(void);
void UpdateParticleBase(void);
void DrawParticleBase(void);

int SetParticleBase(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetColorParticleBase(int nIdxParticle, XMFLOAT4 col);

