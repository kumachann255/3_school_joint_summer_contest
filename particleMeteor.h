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
HRESULT InitParticleMeteor(void);
void UninitParticleMeteor(void);
void UpdateParticleMeteor(void);
void DrawParticleMeteor(void);

int SetParticleMeteor(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 scl, XMFLOAT4 col, int life);
void SetColorParticleMeteor(int nIdxParticle, XMFLOAT4 col);

