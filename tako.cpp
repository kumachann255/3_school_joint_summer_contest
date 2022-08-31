//=============================================================================
//
// モデル処理 [enemy.cpp]
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
#include "rockOn.h"

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
#define TAKO_POS_ADDITION	(0.5f)							// タコのポジション加算用
#define VALUE_HOMING		(0.1f)							// タコ引き寄せ値
#define FRONT_COLLISION		(20.0f)							// 手前で消す位置の当たり判定値
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
TAKO				g_Tako[MAX_TAKO];							// プレイヤー
TAKO				g_Tako_middle[MAX_TAKO];					// プレイヤー
TAKO				g_Tako_last[MAX_TAKO];					// プレイヤー


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTako(void)
{
	for (int i = 0; i < MAX_TAKO; i++)
	{
		LoadModel(MODEL_TAKO, &g_Tako[i].model);
		g_Tako[i].load = TRUE;

		g_Tako[i].pos = { 0.0f, TAKO_OFFSET_Y, 0.0f };
		g_Tako[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Tako[i].scl = { TAKO_SCL_INIT, TAKO_SCL_INIT, TAKO_SCL_INIT };

		g_Tako[i].angle = 0.0f;			// 移動スピードクリア
		g_Tako[i].size = TAKO_SIZE;	// 当たり判定の大きさ

		g_Tako[i].use = FALSE;

		g_Tako[i].mode = WAIT;

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
	CAMERA *cam = GetCamera();
	PLAYER *player = GetPlayer();
	ENEMY  *enemy = GetEnemy();
	ROCKON *rockOn = GetRockOn();

	for (int i = 0; i < MAX_TAKO; i++)
	{
		if (g_Tako[i].use == TRUE || g_Tako_middle[i].use == TRUE || g_Tako_last[i].use == TRUE)
		{
			//サメ爆弾　g_Takoには移動させるサイトを入れる。
			switch (g_Tako[i].mode)
			{
			case WAIT:
				g_Tako[i].scl = { TAKO_SCL_INIT, TAKO_SCL_INIT, TAKO_SCL_INIT };
				g_Tako[i].pos = enemy[i].pos;
				g_Tako[i].pos.y += TAKO_POS_Y;
				g_Tako_middle[i].pos = enemy[i].pos;
				g_Tako_middle[i].pos.y += TAKO_POS_Y;
				g_Tako_last[i].pos = enemy[i].pos;
				g_Tako_last[i].pos.y += TAKO_POS_Y;

				if (GetKeyboardTrigger(DIK_RETURN))
				{
					g_Tako[i].use = TRUE;
				}

				if (GetKeyboardPress(DIK_SPACE))
				{
					g_Tako[i].use = FALSE;
					g_Tako_middle[i].use = TRUE;
					g_Tako[i].mode = ZUZHO;
				}
				break;

			case ZUZHO:

				g_Tako[i].pos = enemy[i].pos;
				g_Tako[i].pos.y += TAKO_POS_CATCH;
				g_Tako_middle[i].pos = enemy[i].pos;
				g_Tako_middle[i].pos.y += TAKO_POS_CATCH;
				g_Tako_last[i].pos = enemy[i].pos;
				g_Tako_last[i].pos.y += TAKO_POS_CATCH;
				g_Tako[i].mode = CATCH;
				break;

			case CATCH:
				g_Tako[i].pos.x = enemy[i].pos.x;
				g_Tako[i].pos.y -= TAKO_POS_ADDITION;
				g_Tako[i].pos.z = enemy[i].pos.z;
				g_Tako_middle[i].pos.x = enemy[i].pos.x;
				g_Tako_middle[i].pos.y -= TAKO_POS_ADDITION;
				g_Tako_middle[i].pos.z = enemy[i].pos.z;
				g_Tako_last[i].pos.x = enemy[i].pos.x;
				g_Tako_last[i].pos.y -= TAKO_POS_ADDITION;
				g_Tako_last[i].pos.z = enemy[i].pos.z;


				if (g_Tako[i].pos.y < 0.0f)
				{
					g_Tako_middle[i].use = FALSE;
					g_Tako_last[i].use = TRUE;
					enemy[i].takoHit = TRUE;
					g_Tako[i].mode = RELEASE;
				}
				break;

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
					g_Tako[i].mode = WAIT;
					enemy[i].use = FALSE;
					rockOn[i].use = FALSE;
				}
				break;
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
	for (int i = 0; i < MAX_TAKO; i++)
	{
		if (g_Tako[i].use == FALSE && g_Tako[i].mode == WAIT)
		{
			ENEMY *enemy = GetEnemy();
			ROCKON *rockOn = GetRockOn();

			for (int j = 0; j < MAX_ENEMY; j++)
			{
				g_Tako[i].use = TRUE;
				if (rockOn[i].use == TRUE)
				{
					g_Tako[i].pos = enemy[j].pos;
					g_Tako_middle[i].pos = enemy[j].pos;
					g_Tako_last[i].pos = enemy[j].pos;
				}
			}
			return;
		}
	}
}
