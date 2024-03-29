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
#define NOTE_TEX_MAX		(4)			// 音符の種類
#define NOTE_TIMING_GOOD	(50.0f)		// 良いのタイミングの範囲
#define NOTE_TIMING_JUST	(20.0f)		// 最高のタイミングの範囲
#define NOTE_TIMING_MISS	(70.0f)		// ミスのタイミングの範囲

#define SENHU_X				(960.0f / 2.0f)	// 五線譜の表示位置
#define SENHU_Y				(370.0f)		// 五線譜の表示位置
#define SENHU_ALFA			(0.5f)			// 五線譜の表示位置

#define TARGET_X			(150.0f)	// ターゲットの表示位置
#define TARGET_Y			(SENHU_Y)	// ターゲットの表示位置

#define CONDUCTOR_POS_X				(850.0f)	// 指揮者の位置
#define CONDUCTOR_POS_Y				(SENHU_Y)	// 指揮者の位置
#define CONDUCTOR_TEXTURE_X			(170.0f)	// 指揮者のサイズ
#define CONDUCTOR_TEXTURE_Y			(170.0f)


// 音符の構造体
struct TIMINGNOTE
{
	XMFLOAT2	pos;
	BOOL		use;
	int			texNum;
	BOOL		seDid;

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

