//=============================================================================
//
// コンボ処理 [combo.h]
// Author : 熊澤義弘
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define COMBO_MAX					(99)		// コンボ数の最大値
#define COMBO_DIGIT					(2)			// 桁数

#define COMBO_BONUS_0				(5)			// 何コンボからボーナスを加えるか
#define COMBO_BONUS_1				(10)		// 何コンボからボーナスを加えるか
#define COMBO_BONUS_2				(15)		// 何コンボからボーナスを加えるか

#define COMBO_BONUS_0_VALUE			(2)			// どのくらいボーナスを加えるか
#define COMBO_BONUS_1_VALUE			(3)			// どのくらいボーナスを加えるか
#define COMBO_BONUS_2_VALUE			(5)			// どのくらいボーナスを加えるか

#define COMBO_CHANGE_ACTION			(2)		// コンボいくつから攻撃法王を変えるか


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitCombo(void);
void UninitCombo(void);
void UpdateCombo(void);
void DrawCombo(void);

void AddCombo(int add);
int GetCombo(void);
void ResetComboTime(void);
void SetTimingHas(BOOL timing);


