//=============================================================================
//
// サウンド処理 [sound.h]
// Author : 大塚勝亮
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	//// BGM
	// タイトル
	SOUND_LABEL_BGM_bgm_title_0,		// クリア後のタイトルBGM
	SOUND_LABEL_BGM_bgm_title_4,		// 最初のタイトルBGM

	// オープニング
	SOUND_LABEL_BGM_bgm_opening_1_2,	// オープニングBGM

	// チュートリアル
	SOUND_LABEL_BGM_bgm_tutorial,		// チュートリアルBGM

	// 街ステージ
	SOUND_LABEL_BGM_bgm_city_stage_1,	// 街ステージ１BGM
	SOUND_LABEL_BGM_bgm_city_stage_2,	// 街ステージ２BGM

	// 海ステージ
	SOUND_LABEL_BGM_bgm_sea_stage_1,	// 海ステージ１BGM
	SOUND_LABEL_BGM_bgm_sea_stage_3,	// 海ステージ２BGM

	// 空ステージ
	SOUND_LABEL_BGM_bgm_sky_stage_1,	// 空ステージ１BGM
	SOUND_LABEL_BGM_bgm_sky_stage_3,	// 空ステージ２BGM

	// リザルト
	SOUND_LABEL_BGM_bgm_result,			// リザルトBGM

	// エンドロール
	SOUND_LABEL_BGM_bgm_endroll,		// エンドロールBGM


	//// SE
	SOUND_LABEL_SE_booingSound01,		// ブーイング音
	SOUND_LABEL_SE_carHorn01,			// 車のクラクション音
	SOUND_LABEL_SE_cheers00,			// 歓声音0
	SOUND_LABEL_SE_cheers01,			// 歓声音1
	SOUND_LABEL_SE_cheers02,			// 歓声音2
	SOUND_LABEL_SE_cheers03,			// 歓声音3
	SOUND_LABEL_SE_cheers04,			// 歓声音4
	SOUND_LABEL_SE_cheers05,			// 歓声音5
	SOUND_LABEL_SE_collisionSound01,	// 車の衝突音
	SOUND_LABEL_SE_comboSound01,		// コンボの音（できれば爆弾破裂時の音と交換）
	SOUND_LABEL_SE_countdownSound01,	// 10秒前カウントダウン
	SOUND_LABEL_SE_damagedSound01,		// ダメージ音
	SOUND_LABEL_SE_explosionSound01,	// 粘着物の爆発音
	SOUND_LABEL_SE_gunShot01,			// 銃声1
	SOUND_LABEL_SE_gunShot02,			// 銃声2
	//SOUND_LABEL_SE_propellerSound01,	// プロペラ音
	SOUND_LABEL_SE_selectBomb01,		// タイトルの選択音
	SOUND_LABEL_SE_shrinkingSound01,	// 粘着物の収縮音
	//SOUND_LABEL_SE_siren01,				// パトカー音
	SOUND_LABEL_SE_stickingSound01,		// 粘着物の張り付き音
	SOUND_LABEL_SE_stirringSound01,		// 粘着かき混ぜ
	SOUND_LABEL_SE_throwingSound01,		// 粘着物投擲音
	SOUND_LABEL_SE_titleClick01,		// タイトルモード選択音1
	SOUND_LABEL_SE_titleClick02,		// タイトルモード選択音2
	SOUND_LABEL_SE_titleClick03,		// タイトルモード選択音3
	SOUND_LABEL_SE_titleClick04,		// タイトルモード選択音4
	SOUND_LABEL_SE_titleClick05,		// タイトルモード選択音4

	SOUND_LABEL_SE_logo,				// ロゴの環境音
	SOUND_LABEL_SE_logoD,				// ロゴの環境音
	SOUND_LABEL_SE_rhythm,				// リズムの音
	SOUND_LABEL_SE_timing_miss,			// ミスの音
	SOUND_LABEL_SE_timing_good,			// GOODの音

	// オープニング
	SOUND_LABEL_SE_nextPage,			// オープニングのページをめくる音

	// 空ステージ
	SOUND_LABEL_SE_enemy_pop,				// エネミーポップ音
	SOUND_LABEL_SE_meteor_shot,				// メテオ発射音
	SOUND_LABEL_SE_meteor_hit,				// メテオヒット音
	SOUND_LABEL_SE_skyEnemy_death_0,		// エネミー死亡音
	SOUND_LABEL_SE_skyEnemy_death_1,		// エネミー死亡音
	SOUND_LABEL_SE_skyEnemy_death_2,		// エネミー死亡音

	// 攻撃時の掛け声
	SOUND_LABEL_SE_legato,					// レガート
	SOUND_LABEL_SE_arpeggio,				// アルペジオ
	SOUND_LABEL_SE_tremolo,					// トレモロ
	SOUND_LABEL_SE_pizzicato,				// ピチカート
	SOUND_LABEL_SE_fortissimo,				// フォルテッシモ
	SOUND_LABEL_SE_glissando,				// グリッサンド

	SOUND_LABEL_SE_cupRotation,				// コーヒーカップの回る音

	SOUND_LABEL_SE_enemyPop_sea_0,			// 海ステージのエネミーのポップ音
	SOUND_LABEL_SE_enemyPop_sea_1,			// 海ステージのエネミーのポップ音

	SOUND_LABEL_SE_samePop,					// サメの音

	SOUND_LABEL_SE_piano_do00,				// ピアノ音 ド
	SOUND_LABEL_SE_piano_re,				// ピアノ音 レ
	SOUND_LABEL_SE_piano_mi,				// ピアノ音 ミ
	SOUND_LABEL_SE_piano_fa,				// ピアノ音 ファ
	SOUND_LABEL_SE_piano_so,				// ピアノ音 ソ
	SOUND_LABEL_SE_piano_ra,				// ピアノ音 ラ
	SOUND_LABEL_SE_piano_si,				// ピアノ音 シ
	SOUND_LABEL_SE_piano_do,				// ピアノ音 ド

	SOUND_LABEL_SE_titleVoice,				// タイトルのボイス
	SOUND_LABEL_SE_logoVoice,				// ロゴのボイス

	SOUND_LABEL_MAX,	// MAX
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);
void SetSouceVoice(int label, float volume);

void SetMasterVolume(float volume);
void SetSourceVolume(int label, float volume);
