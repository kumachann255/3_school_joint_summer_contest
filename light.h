//=============================================================================
//
// ���C�g���� [light.h]
// Author : �F�V�`�O
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitLight(void);
void UpdateLight(void);

void SetLightData(int index, LIGHT *light);
void SetFogData(FOG *fog);
LIGHT *GetLightData(int index);
