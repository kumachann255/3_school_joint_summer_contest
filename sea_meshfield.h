//=============================================================================
//
// 海のメッシュ地面の処理 [sea_meshfield.h]
// Author : 熊澤義弘
//
//=============================================================================
#pragma once

#define FIELD_SPEED			(5.0f)		// 地面の動く速度

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSeaMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitSeaMeshField(void);
void UpdateSeaMeshField(void);
void DrawSeaMeshField(void);

BOOL RayHitSeaField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);

