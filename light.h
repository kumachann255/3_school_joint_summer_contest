//=============================================================================
//
// ライト処理 [light.h]
// Author : 熊澤義弘
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitLight(void);
void UpdateLight(void);

void SetLightData(int index, LIGHT *light);
void SetFogData(FOG *fog);
LIGHT *GetLightData(int index);

