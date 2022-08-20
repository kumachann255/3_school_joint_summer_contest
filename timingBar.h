//=============================================================================
//
// タイミングバーの処理 [timingBar.h]
// Author : 熊澤義弘
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NOTE_MAX			(10)		// 音符の最大数
#define NOTE_DIS			(10)		// 音符の間隔(秒)
#define NOTE_TEX_MAX		(8)			// 音符の種類
#define NOTE_TIMING_GOOD	(50.0f)		// 良いのタイミングの範囲
#define NOTE_TIMING_JUST	(20.0f)		// 最高のタイミングの範囲
#define NOTE_TIMING_MISS	(70.0f)		// ミスのタイミングの範囲

#define TARGET_X					(150.0f)	// ターゲットの表示位置
#define TARGET_Y					(70.0f)		// ターゲットの表示位置



// 音符の構造体
struct TIMINGNOTE
{
	XMFLOAT2	pos;
	BOOL		use;
	int			texNum;

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTImingBar(void);
void UninitTImingBar(void);
void UpdateTImingBar(void);
void DrawTImingBar(void);

void SetNote(void);
int GetNoteTiming(void);

