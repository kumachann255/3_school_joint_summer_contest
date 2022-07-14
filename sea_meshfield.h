//=============================================================================
//
// �C�̃��b�V���n�ʂ̏��� [sea_meshfield.h]
// Author : �F�V�`�O
//
//=============================================================================
#pragma once

#define FIELD_SPEED			(5.0f)		// �n�ʂ̓������x

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSeaMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitSeaMeshField(void);
void UpdateSeaMeshField(void);
void DrawSeaMeshField(void);

BOOL RayHitSeaField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);

