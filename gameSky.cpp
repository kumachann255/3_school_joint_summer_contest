﻿//=============================================================================
//
// ゲーム画面処理 [gameSky.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "gameSky.h"

#include "player.h"
#include "sky_enemy.h"
#include "shadow.h"
#include "sky_smallmeteor.h"

#include "score.h"
#include "particle.h"
#include "collision.h"
#include "debugproc.h"
#include "timeUI.h"
#include "damageEF.h"
#include "combo.h"
#include "playerHP.h"
#include "gameUI.h"
#include "speech.h"
#include "tutorial.h"
#include "dome.h"
#include "timingBar.h"
#include "timingtext.h"
#include "timingEffect.h"
#include "target.h"
#include "targetObj.h"
#include "rockOn.h"
#include "fieldobj.h"
#include "particleMeteor.h"
#include "particleSky.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHitSky(void);



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// ポーズON/OFF
static BOOL g_DeathParticl;

static int					g_Stage;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGameSky(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	//// フィールドの初期化
	//InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 1, 7, 700.0f, 400.0f);
	////InitMeshField(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 1, 1, 5000.0f, 5000.0f);

	//InitMeshField2(XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 1, 2, 2000.0f, 2000.0f);

	// ライトを有効化	// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();


	// 空ステージのエネミーの初期化
	InitSkyEnemy();

	//// 攻撃範囲の初期化
	//InitAttackR();

	//// ダイナマイトの初期化
	//InitBom();

	//// 爆破オブジェクトの初期化
	//InitBlast();

	//// 壁の初期化
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	//// 壁(裏側用の半透明)
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	//// 弾の初期化
	//InitBullet();

	// スモールメテオの初期化
	InitS_Meteor();

	// スコアの初期化
	InitScore();

	//// コンボの初期化
	InitCombo();

	// UIの初期化
	InitGameUI();

	// 制限時間の初期化
	InitTime();

	// プレイヤーのHPの初期化
	InitPlayerHP();

	// パーティクルの初期化
	InitParticle();

	//// 軌跡の初期化
	//InitOrbit();

	// ダメージエフェクトの初期化
	InitDamageEF();

	// 吹き出しの初期化
	InitSpeech();

	// ドームの初期化
	InitDome();

	// チュートリアルの初期化
	InitTutorial();

	// タイミングバーの初期化
	InitTImingBar();

	// タイミングテキストの初期化
	InitTimingText();

	// タイミングエフェクトの初期化
	InitTimingEffect();

	// ターゲットアイコンの初期化
	InitTarget();

	// ターゲットオブジェクトの初期化
	InitTargetObj();

	// ロックオンアイコンの初期化
	InitRockOn();

	// メテオのパーティクルの初期化
	InitParticleMeteor();

	// 空ステージのエネミーを倒したときのパーティクルの初期化
	InitParticleSky();

	g_Stage = GetStage();

	// 連続再生のSEを再生
	//PlaySound(SOUND_LABEL_SE_stirringSound01);


	switch (g_Stage)
	{

	case stage0:
		PlaySound(SOUND_LABEL_BGM_bgm_sky_stage_3);
		break;
	case stage1:
		// BGM再生
		PlaySound(SOUND_LABEL_BGM_bgm_sky_stage_1);
		break;
	}

	g_DeathParticl = FALSE;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGameSky(void)
{
	// 空ステージのエネミーを倒したときのパーティクルの終了処理
	UninitParticleSky();

	// メテオのパーティクルの終了処理
	UninitParticleMeteor();

	// タイミングエフェクトの終了処理
	UninitTimingEffect();

	// ロックオンアイコンの終了処理
	UninitRockOn();

	// ターゲットオブジェクトの終了処理
	UninitTargetObj();

	// ターゲットアイコンの終了処理
	UninitTarget();

	// タイミングテキストの終了処理
	UninitTimingText();

	// タイミングバーの終了処理
	UninitTImingBar();

	// ドームの終了処理
	UninitDome();

	// チュートリアルの終了処理
	UninitTutorial();

	// 吹き出しの終了処理
	UninitSpeech();

	// ダメージエフェクトの終了処理
	UninitDamageEF();

	//// 軌跡の終了処理
	//UninitOrbit();

	// パーティクルの終了処理
	UninitParticle();

	// プレイヤーのHPの終了処理
	UninitPlayerHP();

	// 時間の終了処理
	UninitTime();

	// UIの終了処理
	UninitGameUI();

	//// コンボの終了処理
	//UninitCombo();

	// スコアの終了処理
	UninitScore();

	// スモールメテオの終了処理
	UninitS_Meteor();

	// 空ステージのエネミーの終了処理
	UninitSkyEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGameSky(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}


#endif

	if(g_bPause == FALSE)
		return;

	// プレイヤーの更新処理
	UpdatePlayer();

	// 空ステージのエネミーの更新処理
	UpdateSkyEnemy();

	// ドームの更新処理
	UpdateDome();

	// スモールメテオの更新処理
	UpdateS_Meteor();

	// パーティクルの更新処理
	UpdateParticle();

	// 影の更新処理
	UpdateShadow();

	// 当たり判定処理
	CheckHitSky();

	// スコアの更新処理
	UpdateScore();

	// コンボの更新処理
	UpdateCombo();

	// UIの更新処理
	UpdateGameUI();

	// 時間の更新処理
	UpdateTime();

	// プレイヤーのHPの更新処理
	UpdatePlayerHP();

	// ダメージエフェクトの更新処理
	UpdateDamageEF();

	// 吹き出しの更新処理
	UpdateSpeech();

	// チュートリアルの更新処理
	UpdateTutorial();

	// タイミングバーの更新処理
	UpdateTImingBar();

	// タイミングテキストの更新処理
	UpdateTimingText();

	// タイミングエフェクトの更新処理
	UpdateTimingEffect();

	// ターゲットアイコンの更新処理
	UpdateTarget();

	// ターゲットオブジェクトの更新処理
	UpdateTargetObj();

	// ロックオンアイコンの更新処理
	UpdateRockOn();

	// メテオのパーティクルの更新処理
	UpdateParticleMeteor();

	// 空ステージのエネミーを倒したときのパーティクルの更新処理
	UpdateParticleSky();

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGameSky0(void)
{
	// 影の描画処理
	DrawShadow();

	// 空ステージのエネミーの描画処理
	DrawSkyEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	// スモールメテオの描画処理
	DrawS_Meteor();


	//// 壁の描画処理
	//DrawMeshWall();

	// 木の描画処理
	//DrawFieldObj();

	// スカイドームの描画処理
	//DrawSky();

	// ドームの描画処理
	DrawDome();

	// パーティクルの描画処理
	DrawParticle();

	// ターゲットオブジェクトの描画処理
	DrawTargetObj();

	// ロックオンアイコンの描画処理
	DrawRockOn();

	// メテオのパーティクルの描画処理
	DrawParticleMeteor();

	// 空ステージのエネミーを倒したときのパーティクルの描画処理
	DrawParticleSky();



	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	// ターゲットアイコンの描画処理
	DrawTarget();

	// スコアの描画処理
	DrawScore();

	// コンボの描画処理
	DrawCombo();

	// 時間の描画処理
	DrawTime();

	// プレイヤーのHPの描画処理
	DrawPlayerHP();

	// ダメージエフェクトの描画処理
	DrawDamageEF();

	// UIの描画処理
	DrawGameUI();

	// 吹き出しの描画処理
	DrawSpeech();

	// チュートリアルの描画処理
	DrawTutorial();

	// タイミングエフェクトの描画処理
	DrawTimingEffect();

	// タイミングバーの描画処理
	DrawTImingBar();

	// タイミングテキストの描画処理
	DrawTimingText();

	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);
}


void DrawGameSky(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// プレイヤー視点
	pos = GetPlayer()->pos;
	pos.y += 30.0f;			// カメラ酔いを防ぐためにクリアしている
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGameSky0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGameSky0();

		// エネミー視点
		pos = GetSkyEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGameSky0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGameSky0();

		// エネミー視点
		pos = GetSkyEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGameSky0();
		break;

	}
}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHitSky(void)
{

	PLAYER *player = GetPlayer();	// プレイヤーのポインターを初期化
	SKY_ENEMY *sky_enemy = GetSkyEnemy();	// エネミーのポインターを初期化
	S_METEOR *s_meteor = GetS_Meteor();	// スモールメテオのポインターを初期化


	//==========================================
	// スモールメテオとエネミーの当たり判定処理
	//==========================================

	for (int i = 0; i < MAX_S_METEOR; i++)
	{
		if (s_meteor[i].use == FALSE)	continue;

		for (int j = 0; j < MAX_SKY_ENEMY; j ++)
		{
			if (sky_enemy[j].use == FALSE)	continue;


			if (CollisionBC(s_meteor[i].pos,sky_enemy[j].pos,s_meteor[i].size,sky_enemy[j].size))
			{
				PlaySound(SOUND_LABEL_SE_meteor_hit);

				sky_enemy[j].use = FALSE;

				// スコアを足す
				AddScore(100);

				switch (sky_enemy[i].EnemyType)
				{
				case 0:
					PlaySound(SOUND_LABEL_SE_skyEnemy_death_1);
					break;

				case 1:
					PlaySound(SOUND_LABEL_SE_skyEnemy_death_2);
					break;

				case 2:
					PlaySound(SOUND_LABEL_SE_skyEnemy_death_0);
					break;

				default:
					break;
				}
				sky_enemy[j].particleOn = TRUE;

				g_DeathParticl = TRUE;

				// スコアを足す
				AddScore(10000);

				// コンボを足す
				AddCombo(1);
				ResetComboTime();
			}
		}
	}

	// プレイヤーのHPが0でゲームオーバー
	// リザルト画面へ遷移
	// 最終的にはゲームオーバー画面からリザルト画面へ遷移
	if (player->hp <= 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}
}


BOOL GetDeathPartical(void)
{
	return g_DeathParticl;
}
