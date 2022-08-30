//=============================================================================
//
// タイミングエフェクトの処理 [timingEffect.h]
// Author : 熊澤義弘
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEFFECT_MAX			(5)		// タイミングエフェクトの最大数
#define TEFFECT_TYPE_MAX	(3)		// エフェクトの種類


// 音符の構造体
struct TIMINGEFFECT
{
	XMFLOAT2	pos;
	XMFLOAT2	size;
	BOOL		use;
	int			texNum;
	int			life;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTimingEffect(void);
void UninitTimingEffect(void);
void UpdateTimingEffect(void);
void DrawTimingEffect(void);

void SetTimingEffect(int text);
