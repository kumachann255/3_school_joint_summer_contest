//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticleBase(void);
void UninitParticleBase(void);
void UpdateParticleBase(void);
void DrawParticleBase(void);

int SetParticleBase(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetColorParticleBase(int nIdxParticle, XMFLOAT4 col);

