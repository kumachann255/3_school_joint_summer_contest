//=============================================================================
//
// Ođ [orbit.h]
// Author : FāV`O
//
//=============================================================================
#pragma once


//*****************************************************************************
// vg^Cvéū
//*****************************************************************************
HRESULT InitOrbit(void);
void UninitOrbit(void);
void UpdateOrbit(void);
void DrawOrbit(void);

int SetOrbit(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetOrbit(int nIdxParticle, XMFLOAT4 col);

