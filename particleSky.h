//=============================================================================
//
// ���e�I�̃p�[�e�B�N������ [particleMeteor.h]
// Author : �F�V�`�O
//
//=============================================================================
#pragma once

//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticleSky(void);
void UninitParticleSky(void);
void UpdateParticleSky(void);
void DrawParticleSky(void);

int SetParticleSky(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 scl, XMFLOAT4 col, int life);
void SetColorParticleSky(int nIdxParticle, XMFLOAT4 col);

