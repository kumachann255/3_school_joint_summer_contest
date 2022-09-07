//=============================================================================
//
// サメ処理 [same.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "same.h"
#include "attackRange.h"
#include "player.h"
#include "enemy.h"
#include "shadow.h"
#include "enemyHeli.h"
#include "tutorial.h"
#include "collision.h"
#include "score.h"
#include "combo.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_SAME		"data/MODEL/shark_hire.obj"			// 読み込むモデル名
#define	MODEL_SAME_1	"data/MODEL/shark_1.obj"			// 読み込むモデル名
#define	MODEL_SAME_2	"data/MODEL/shark_2.obj"			// 読み込むモデル名

#define	VALUE_MOVE			(0.1f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define SAME_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define SAME_OFFSET_Y		(7.0f)							// プレイヤーの足元をあわせる

#define HIRE_ROT_PI			(100)							// ヒレの回転用半径の長さ
#define HIRE_ROT_ADDTION	(0.1f)							// ヒレのY軸回転加算量
#define DOWN_ADDTION_1		(0.9f)							// 下降加算量①
#define DOWN_ADDTION_2		(0.8f)							// 下降加算量②
#define UP_ADDTION			(2.0f)							// 上昇加算量
#define MAX_UP				(40.0f)							// 上昇最高値
#define MAX_DOWN			(-50.0f)						// 下降最低値
#define KUCHI_ROT_ADDTION	(0.05f)							// 下顎回転量
#define MAX_KUCHI_ROT		(XM_PI * 0.5f)					// 下顎最大回転量


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
SAME				g_Same;							// プレイヤー
SAME				g_sAtama;
SAME				g_sKuchi;

static float		g_radius;						// エネミー回転用
static float		g_rot;							// エネミー回転用
static BOOL			g_particleOn;					// TRUE:爆発エフェクト発生

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSame(void)
{
	LoadModel(MODEL_SAME, &g_Same.model);
	g_Same.load = TRUE;

	g_Same.pos = { 0.0f, SAME_OFFSET_Y, 0.0f };
	g_Same.rot = { 0.0f, 0.0f, 0.0f };
	g_Same.scl = { 10.0f, 10.0f, 10.0f };

	g_Same.angle = 0.0f;			// 移動スピードクリア
	g_Same.size = SAME_SIZE;	// 当たり判定の大きさ

	g_Same.use = FALSE;

	g_Same.mode = GURUGURU;

	//
	LoadModel(MODEL_SAME_1, &g_sAtama.model);
	g_sAtama.load = TRUE;

	g_sAtama.pos = { 0.0f, SAME_OFFSET_Y, 0.0f };
	g_sAtama.rot = { -0.77f, 0.0f, 0.0f };
	g_sAtama.scl = { 4.0f, 4.0f, 4.0f };

	g_sAtama.angle = 0.0f;			// 移動スピードクリア
	g_sAtama.size = SAME_SIZE;	// 当たり判定の大きさ

	g_sAtama.use = FALSE;

	g_sAtama.mode = PACKNCYO;

	LoadModel(MODEL_SAME_2, &g_sKuchi.model);
	g_sKuchi.load = TRUE;

	g_sKuchi.pos = { 0.0f, SAME_OFFSET_Y, 0.0f };
	g_sKuchi.rot = { 0.0f, 0.0f, 0.0f };
	g_sKuchi.scl = { 4.0f, 4.0f, 4.0f };

	g_sKuchi.angle = 0.0f;			// 移動スピードクリア
	g_sKuchi.size = SAME_SIZE;	// 当たり判定の大きさ

	g_sKuchi.use = FALSE;

	g_sKuchi.mode = DOWN_LAST;
	//

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSame(void)
{
	// モデルの解放処理
	if (g_Same.load)
	{
		UnloadModel(&g_Same.model);
		g_Same.load = FALSE;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSame(void)
{
	CAMERA *cam = GetCamera();
	ATTACKRANGE *attackR = GetAttackR();
	PLAYER *player = GetPlayer();

	if (g_Same.use == TRUE)
	{
		// サメ爆弾　g_Sameには移動させるサイトを入れる。
// switch文のため、g_Sameの引数をベースに管理する。
		switch (g_Same.mode)
		{
		case GURUGURU:
			g_Same.pos.x = attackR->pos.x - HIRE_ROT_PI * cosf(g_Same.angle);
			g_Same.pos.y = attackR->pos.y;
			g_Same.pos.z = attackR->pos.z - HIRE_ROT_PI * sinf(g_Same.angle);

			g_sAtama.pos = attackR->pos;
			g_sKuchi.pos = attackR->pos;

			g_Same.rot.y -= HIRE_ROT_ADDTION; // ヒレのY軸での回転
			// サークルの中心から半径100をぐるぐる回るための加算
			g_Same.angle += VALUE_MOVE;

			if (g_Same.angle > XM_2PI)
			{
				// 一周したら別モードへ遷移
				g_Same.angle = 0.0f;
				g_Same.mode = DOWN_FIRST;
			}
			break;

			//g_Same.pos.x = attackR->pos.x - HIRE_ROT_PI * cosf(g_Same.angle);
			//g_Same.pos.y = attackR->pos.y;
			//g_Same.pos.z = attackR->pos.z - HIRE_ROT_PI * sinf(g_Same.angle);

			//g_sAtama.pos = attackR->pos;
			//g_sKuchi.pos = attackR->pos;

			//// ヒレの移動
			//if (GetKeyboardPress(DIK_SPACE) && player->rockOn == FALSE)
			//{
			//	g_Same.use = TRUE;
			//	g_Same.rot.y -= HIRE_ROT_ADDTION; // ヒレのY軸での回転
			//	// サークルの中心から半径100をぐるぐる回るための加算
			//	g_Same.angle += VALUE_MOVE;

			//	if (g_Same.angle > XM_2PI)
			//	{
			//		// 一周したら別モードへ遷移
			//		g_Same.angle = 0.0f;
			//		g_Same.mode = DOWN_FIRST;
			//	}
			//}
			//break;


		case DOWN_FIRST:
			// 下降処理
			g_Same.pos.x = attackR->pos.x - HIRE_ROT_PI * cosf(g_Same.angle);
			g_Same.pos.y -= DOWN_ADDTION_1;
			g_Same.pos.z = attackR->pos.z - HIRE_ROT_PI * sinf(g_Same.angle);

			g_sAtama.pos.x = attackR->pos.x;
			g_sAtama.pos.y -= DOWN_ADDTION_1;
			g_sAtama.pos.z = attackR->pos.z;
			g_sKuchi.pos.x = attackR->pos.x;
			g_sKuchi.pos.y -= DOWN_ADDTION_1;
			g_sKuchi.pos.z = attackR->pos.z;

			if (g_Same.pos.y < MAX_DOWN)
			{
				g_Same.pos.x = attackR->pos.x;
				g_Same.pos.z = attackR->pos.z;

				// 下降最低値までいったらヒレのモデルはFALSEにするがswitch用に他の情報は残す
				g_Same.rot.y = 0.0f;
				g_Same.mode = PACKNCYO;
				//g_Same.use = FALSE;

				g_sAtama.use = TRUE;
				g_sKuchi.use = TRUE;
			}
			break;

		case PACKNCYO:
			// 上昇噛みつき処理(上昇してからはその位置で固定)
			//g_Same.pos.x = attackR->pos.x;
			g_Same.pos.y += UP_ADDTION;
			//g_Same.pos.z = attackR->pos.z;

			//g_sAtama.pos.x = attackR->pos.x;
			g_sAtama.pos.y += UP_ADDTION;
			//g_sAtama.pos.z = attackR->pos.z;

			g_sKuchi.rot.x -= KUCHI_ROT_ADDTION;
			//g_sKuchi.pos.x = attackR->pos.x;
			g_sKuchi.pos.y += UP_ADDTION;
			//g_sKuchi.pos.z = attackR->pos.z;


			if (g_Same.pos.y > MAX_UP)
			{
				// 最大上昇値までいったらシーン遷移
				g_sKuchi.rot.x = -MAX_KUCHI_ROT;	// 下顎の回転を止めて下顎の回転最大値値を代入
				g_Same.mode = DOWN_LAST;
			}
			break;

		case DOWN_LAST:
			// 下降処理
			g_Same.pos.y -= DOWN_ADDTION_2;
			g_sAtama.pos.y -= DOWN_ADDTION_2;
			g_sKuchi.pos.y -= DOWN_ADDTION_2;

			if (g_Same.pos.y < MAX_DOWN)
			{
				// 頭と口のモデルをFALSEにする
				g_Same.use = FALSE;
				g_sAtama.use = FALSE;
				g_sKuchi.use = FALSE;
				g_sKuchi.rot.x = 0.0f; // 下顎の回転量の初期化
				g_Same.mode = GURUGURU;
			}
			break;
		}

		// 当たった後エネミーの挙動処理
		{
			ENEMY *enemy = GetEnemy();		// エネミーのポインターを初期化
			ENEMY_HELI *enemyheli = GetEnemyHeli();		// エネミーのポインターを初期化

			// 他エネミーの処理

			// 敵とサメオブジェクト
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Same.mode == PACKNCYO)
				{
					if (CollisionBC(g_Same.pos, enemy[i].pos, g_Same.size, enemy[i].size))
					{
						if (enemy[i].isHit == TRUE) break;
						if (enemy[i].use == TRUE)
						{
							// スコアを足す
							AddScore(100);

							// コンボを足す
							AddCombo(1);
							ResetComboTime();
						}
						// 敵キャラクターは倒される
						enemy[i].use = FALSE;
					}
				}
			}

			// ヘリの処理
			for (int j = 0; j < MAX_ENEMY_HELI; j++)
			{
				if (g_Same.mode == PACKNCYO)
				{
					if (CollisionBC(g_Same.pos, enemyheli[j].pos, g_Same.size, enemyheli[j].size))
					{
						if (enemyheli[j].isHit == TRUE) break;
						if (enemyheli[j].use == TRUE)
						{
							// スコアを足す
							AddScore(100);

							// コンボを足す
							AddCombo(1);
							ResetComboTime();
						}
						// 敵キャラクターは倒される
						enemyheli[j].use = FALSE;
					}
				}
			}
		}

	}

#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("g_Same:↑ → ↓ ←　Space\n");
	//PrintDebugProc("g_Same:X:%f Y:%f Z:%f\n", g_Same.pos.x, g_Same.pos.y, g_Same.pos.z);
	//PrintDebugProc("g_Sameangle%f\n", g_Same.angle);

#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSame(void)
{

	if (g_Same.use == TRUE)
	{
		// カリング無効
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Same.scl.x, g_Same.scl.y, g_Same.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Same.rot.x, g_Same.rot.y + XM_PI, g_Same.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Same.pos.x, g_Same.pos.y, g_Same.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Same.mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Same.model);

		// カリング設定を戻す
		SetCullingMode(CULL_MODE_BACK);
	}
	if (g_sAtama.use == TRUE)
	{
		// カリング無効
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_sAtama.scl.x, g_sAtama.scl.y, g_sAtama.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_sAtama.rot.x, g_sAtama.rot.y + XM_PI, g_sAtama.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_sAtama.pos.x, g_sAtama.pos.y, g_sAtama.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_sAtama.mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_sAtama.model);

		// カリング設定を戻す
		SetCullingMode(CULL_MODE_BACK);
	}
	if (g_sKuchi.use == TRUE)
	{
		// カリング無効
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_sKuchi.scl.x, g_sKuchi.scl.y, g_sKuchi.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_sKuchi.rot.x, g_sKuchi.rot.y + XM_PI, g_sKuchi.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_sKuchi.pos.x, g_sKuchi.pos.y, g_sKuchi.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_sKuchi.mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_sKuchi.model);

		// カリング設定を戻す
		SetCullingMode(CULL_MODE_BACK);
	}
}


//=============================================================================
// サメ情報を取得
//=============================================================================
SAME *GetSame(void)
{
	return &g_Same;
}

//=============================================================================
// サメの発生
//=============================================================================
void SetSame(void)
{
	if (g_Same.use == FALSE && g_Same.mode == GURUGURU)
	{
		g_Same.use = TRUE;

		ATTACKRANGE *attackR = GetAttackR();

		g_Same.pos.x = attackR->pos.x - HIRE_ROT_PI * cosf(g_Same.angle);
		g_Same.pos.y = attackR->pos.y;
		g_Same.pos.z = attackR->pos.z - HIRE_ROT_PI * sinf(g_Same.angle);

		g_sAtama.pos = attackR->pos;
		g_sKuchi.pos = attackR->pos;

		return;
	}
}
