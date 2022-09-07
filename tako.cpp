//=============================================================================
//
// モデル処理 [tako.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"
#include "enemy.h"
#include "enemyHeli.h"
#include "tako.h"
#include "shadow.h"
#include "rockOn.h"
#include "target.h"
#include "collision.h"
#include "score.h"
#include "combo.h"
#include "sea_particle.h"
#include "tutorial.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_TAKO		"data/MODEL/tako_1.obj"			// 読み込むモデル名
#define	MODEL_TAKO_1	"data/MODEL/tako_2.obj"			// 読み込むモデル名
#define	MODEL_TAKO_2	"data/MODEL/tako_3.obj"			// 読み込むモデル名

#define	VALUE_MOVE			(0.1f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define TAKO_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define TAKO_OFFSET_Y		(7.0f)							// プレイヤーの足元をあわせる

#define TAKO_SCL_INIT		(0.1f)							// タコの大きさの初期値設定
#define TAKO_POS_Y			(50.0f)							// タコの高さ初期値設定
#define TAKO_POS_CATCH		(70.0f)							// タコのキャッチアクション発生時の高さ
#define TAKO_POS_ADDITION	(2.5f)							// タコのポジション加算用
#define VALUE_HOMING		(0.1f)							// タコ引き寄せ値
#define FRONT_COLLISION		(20.0f)							// 手前で消す位置の当たり判定値
#define TAKO_PARTICLE_POP	(5)								// タコパーティクルポップ間隔
#define POP_TIME			(20)							// パーティクルポップ時間

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
TAKO				g_Tako[MAX_TAKO];							// タコ
TAKO				g_Tako_middle[MAX_TAKO];					// タコ
TAKO				g_Tako_last[MAX_TAKO];						// タコ

static int pSetCount = 0;										// パーティクルセット用カウント

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTako(void)
{
	for (int i = 0; i < MAX_TAKO; i++)
	{
		LoadModel(MODEL_TAKO_1, &g_Tako[i].model);
		g_Tako[i].load = TRUE;

		g_Tako[i].pos = { 0.0f, TAKO_OFFSET_Y, 0.0f };
		g_Tako[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Tako[i].scl = { TAKO_SCL_INIT, TAKO_SCL_INIT, TAKO_SCL_INIT };

		g_Tako[i].angle = 0.0f;			// 移動スピードクリア
		g_Tako[i].size = TAKO_SIZE;	// 当たり判定の大きさ
		g_Tako[i].rockOnNum = 0;

		g_Tako[i].use = FALSE;

		g_Tako[i].mode = ZUZHO;

		//

		LoadModel(MODEL_TAKO_1, &g_Tako_middle[i].model);
		g_Tako_middle[i].load = TRUE;

		g_Tako_middle[i].pos = { 0.0f, TAKO_OFFSET_Y, 0.0f };
		g_Tako_middle[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Tako_middle[i].scl = { 1.0f, 1.0f, 1.0f };

		g_Tako_middle[i].angle = 0.0f;			// 移動スピードクリア
		g_Tako_middle[i].size = TAKO_SIZE;	// 当たり判定の大きさ

		g_Tako_middle[i].use = FALSE;

		LoadModel(MODEL_TAKO_2, &g_Tako_last[i].model);
		g_Tako_last[i].load = TRUE;

		g_Tako_last[i].pos = { 0.0f, TAKO_OFFSET_Y, 0.0f };
		g_Tako_last[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Tako_last[i].scl = { 1.0f, 1.0f, 1.0f };

		g_Tako_last[i].angle = 0.0f;			// 移動スピードクリア
		g_Tako_last[i].size = TAKO_SIZE;	// 当たり判定の大きさ

		g_Tako_last[i].use = FALSE;

	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTako(void)
{
	for (int i = 0; i < MAX_TAKO; i++)
	{
		// モデルの解放処理
		if (g_Tako[i].load)
		{
			UnloadModel(&g_Tako[i].model);
			g_Tako[i].load = FALSE;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTako(void)
{
	//return;

	CAMERA *cam = GetCamera();
	PLAYER *player = GetPlayer();
	ENEMY  *enemy = GetEnemy();
	ROCKON *rockOn = GetRockOn();

	for (int i = 0; i < MAX_TAKO; i++)
	{
		//if (g_Tako[i].use == FALSE)
		//	continue;
		if (g_Tako[i].use == TRUE)
		{
			//サメ爆弾　g_Takoには移動させるサイトを入れる。
			switch (g_Tako[i].mode)
			{
			//case WAIT:
			//	g_Tako[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
			//	g_Tako[i].pos.y += TAKO_POS_Y;
			//	g_Tako_middle[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
			//	g_Tako_middle[i].pos.y += TAKO_POS_Y;
			//	g_Tako_last[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
			//	g_Tako_last[i].pos.y += TAKO_POS_Y;

			//	if (GetKeyboardPress(DIK_SPACE) && player->rockOn == TRUE)
			//	{
			//		//g_Tako[i].scl = { TAKO_SCL_INIT, TAKO_SCL_INIT, TAKO_SCL_INIT };
			//		g_Tako_middle[i].use = TRUE;
			//		g_Tako[i].mode = ZUZHO;
			//	}
			//	break;

			case ZUZHO:

				g_Tako[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
				g_Tako[i].pos.y += TAKO_POS_CATCH;
				g_Tako_middle[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
				g_Tako_middle[i].pos.y += TAKO_POS_CATCH;
				g_Tako_last[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
				g_Tako_last[i].pos.y += TAKO_POS_CATCH;
				g_Tako[i].mode = CATCH;
				break;

			case CATCH:
				// エネミーの頭上から落ちていく
				g_Tako[i].pos.x = rockOn[g_Tako[i].rockOnNum].pos.x;
				g_Tako[i].pos.y -= TAKO_POS_ADDITION;
				g_Tako[i].pos.z = rockOn[g_Tako[i].rockOnNum].pos.z;
				g_Tako_middle[i].pos.x = rockOn[g_Tako[i].rockOnNum].pos.x;
				g_Tako_middle[i].pos.y -= TAKO_POS_ADDITION;
				g_Tako_middle[i].pos.z = rockOn[g_Tako[i].rockOnNum].pos.z;
				g_Tako_last[i].pos.x = rockOn[g_Tako[i].rockOnNum].pos.x;
				g_Tako_last[i].pos.y -= TAKO_POS_ADDITION;
				g_Tako_last[i].pos.z = rockOn[g_Tako[i].rockOnNum].pos.z;


				if (g_Tako[i].pos.y < 0.0f)
				{
					pSetCount = 0;
					g_Tako_middle[i].use = FALSE;
					g_Tako_last[i].use = TRUE;
					g_Tako[i].mode = WAIT;
				}
				break;

			case WAIT:
				// パーティクル発生用待機時間
				pSetCount++;
				if (pSetCount % TAKO_PARTICLE_POP == 0)
				{
					SetSeaParticleTako();
				}

				g_Tako[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
				g_Tako_last[i].pos = rockOn[g_Tako[i].rockOnNum].pos;

				if (pSetCount == POP_TIME)
				{
					pSetCount = 0;
					g_Tako[i].mode = RELEASE;
				}


			case RELEASE:
				XMVECTOR pos1 = XMLoadFloat3(&cam->pos);
				XMVECTOR pos2 = XMLoadFloat3(&g_Tako[i].pos);
				pos2 += (pos1 - pos2) * VALUE_HOMING;
				XMStoreFloat3(&g_Tako[i].pos, pos2);
				XMVECTOR pos3 = XMLoadFloat3(&cam->pos);
				XMVECTOR pos4 = XMLoadFloat3(&g_Tako_last[i].pos);
				pos2 += (pos1 - pos2) * VALUE_HOMING;
				XMStoreFloat3(&g_Tako_last[i].pos, pos2);

				if (g_Tako[i].pos.z < cam->pos.z + FRONT_COLLISION)
				{
					g_Tako_last[i].use = FALSE;
					g_Tako[i].use = FALSE;
					g_Tako[i].mode = ZUZHO;
					ResetRockOn();

					ReleaseShadow(enemy[i].shadowIdx);

					// スコアを足す
					//AddScore(100);

					// コンボを足す
					//AddCombo(1);
					ResetComboTime();

				}
				break;
			}

			// 当たった後エネミーの挙動処理
			{
				ENEMY *enemy = GetEnemy();		// エネミーのポインターを初期化
				ENEMY_HELI *enemyheli = GetEnemyHeli();		// エネミーのポインターを初期化

				// 他エネミーの処理

				// 敵とサメオブジェクト
				for (int j = 0; j < MAX_ENEMY; j++)
				{
					if (g_Tako[i].mode == RELEASE)
					{
						if (CollisionBC(g_Tako[i].pos, enemy[j].pos, g_Tako[i].size, enemy[j].size))
						{
							if (enemy[j].isHit == TRUE) break;
							// 敵キャラクターは倒される
							XMFLOAT3 pos = enemy[j].pos;
							SetSeaBonb(pos);
							enemy[j].use = FALSE;

							if (GetStage() == tutorial) SetTutorialEnemy(TRUE);

						}
					}
				}

				// ヘリの処理
				for (int j = 0; j < MAX_ENEMY_HELI; j++)
				{
					if (g_Tako[i].mode == RELEASE)
					{
						if (CollisionBC(g_Tako[i].pos, enemyheli[j].pos, g_Tako[i].size, enemyheli[j].size))
						{
							if (enemyheli[j].isHit == TRUE) break;
							// 敵キャラクターは倒される
							XMFLOAT3 pos = enemy[i].pos;
							SetSeaBonb(pos);
							enemyheli[j].use = FALSE;
						}
					}
				}
			}
		}
	}





#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Enemy:↑ → ↓ ←　Space\n");
	//PrintDebugProc("Enemy:X:%f Y:%f Z:%f\n", g_Tako[i].pos.x, g_Tako[i].pos.y, g_Tako[i].pos.z);
	//PrintDebugProc("Enemyangle%f\n", g_Tako[i].angle);

#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTako(void)
{
	for (int i = 0; i < MAX_TAKO; i++)
	{
		if (g_Tako[i].use == TRUE)
		{
			// カリング無効
			SetCullingMode(CULL_MODE_NONE);

			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Tako[i].scl.x, g_Tako[i].scl.y, g_Tako[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Tako[i].rot.x, g_Tako[i].rot.y + XM_PI, g_Tako[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Tako[i].pos.x, g_Tako[i].pos.y, g_Tako[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Tako[i].mtxWorld, mtxWorld);

			// モデル描画
			DrawModel(&g_Tako[i].model);

			// カリング設定を戻す
			SetCullingMode(CULL_MODE_BACK);
		}

		if (g_Tako_middle[i].use == TRUE)
		{
			// カリング無効
			SetCullingMode(CULL_MODE_NONE);

			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Tako_middle[i].scl.x, g_Tako_middle[i].scl.y, g_Tako_middle[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Tako_middle[i].rot.x, g_Tako_middle[i].rot.y + XM_PI, g_Tako_middle[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Tako_middle[i].pos.x, g_Tako_middle[i].pos.y, g_Tako_middle[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Tako_middle[i].mtxWorld, mtxWorld);

			// モデル描画
			DrawModel(&g_Tako_middle[i].model);

			// カリング設定を戻す
			SetCullingMode(CULL_MODE_BACK);
		}

		if (g_Tako_last[i].use == TRUE)
		{
			// カリング無効
			SetCullingMode(CULL_MODE_NONE);

			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Tako_last[i].scl.x, g_Tako_last[i].scl.y, g_Tako_last[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Tako_last[i].rot.x, g_Tako_last[i].rot.y + XM_PI, g_Tako_last[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Tako_last[i].pos.x, g_Tako_last[i].pos.y, g_Tako_last[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Tako_last[i].mtxWorld, mtxWorld);

			// モデル描画
			DrawModel(&g_Tako_last[i].model);

			// カリング設定を戻す
			SetCullingMode(CULL_MODE_BACK);
		}
	}
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
TAKO *GetTako(void)
{
	return &g_Tako[0];
}

//=============================================================================
//タコの発生
//=============================================================================
void SetTako(void)
{
	ROCKON *rockOn = GetRockOn();

	for (int i = 0; i < MAX_ROCKON; i++)
	{
		if (rockOn[i].use == TRUE)
		{

			//for (int j = 0; j < MAX_ROCKON; j++)
			//{
			//	if (rockOn[j].use == TRUE)
			//	{
			//		g_Tako[i].use = TRUE;
			//		g_Tako[i].pos = rockOn[j].pos;
			//		//g_Tako_middle[i].pos = rockOn[j].pos;
			//		//g_Tako_last[i].pos = rockOn[j].pos;
			//		break;
			//	}
			//}
			for (int j = 0; j < MAX_TAKO; j++)
			{
				if (g_Tako[j].use == FALSE)
				{
					// コンボを足す
					AddCombo(1);
					ResetComboTime();

					g_Tako[j].use = TRUE;
					g_Tako_middle[j].use = TRUE;
					g_Tako[j].pos = rockOn[i].pos;
					g_Tako[j].pos.y += 5.0f;
					g_Tako[j].rockOnNum = i;

					g_Tako_middle[i].pos = rockOn[j].pos;
					g_Tako_last[i].pos = rockOn[j].pos;
					break;
				}
			}
		}
	}
}
