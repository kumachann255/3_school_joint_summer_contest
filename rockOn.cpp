//=============================================================================
//
// ロックオンアイコン処理 [rockOn.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "rockOn.h"
#include "debugproc.h"
#include "target.h"
#include "sky_enemy.h"
#include "enemy.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ROCKON		"data/MODEL/rockOnIcon.obj"	// 読み込むモデル名

#define	ROCKON_ROT_SPEED	(0.01f)		// 回転速度


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static struct ROCKON	g_RockOn[MAX_ROCKON];
static float			g_rot_y;

static BOOL				g_Load = FALSE;
static int				g_max;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitRockOn(void)
{
	LoadModel(MODEL_ROCKON, &g_RockOn[0].model);

	for (int i = 0; i < MAX_ROCKON; i++)
	{
		g_RockOn[i].load = TRUE;

		g_RockOn[i].use = TRUE;

		g_RockOn[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_RockOn[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_RockOn[i].scl = XMFLOAT3(10.0f, 10.0f, 10.0f);

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_RockOn[i].model, &g_RockOn[i].diffuse[0]);

		g_RockOn[i].load = TRUE;

		g_RockOn[i].use = FALSE;

		g_RockOn[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_RockOn[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_RockOn[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_RockOn[i].model, &g_RockOn[i].diffuse[0]);

	}
	
	g_max = MAX_ROCKON;
	if (GetMode() == MODE_GAME_SEA) g_max = MAX_ROCKON_SEA;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitRockOn(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ROCKON; i++)
	{
		if (g_RockOn[i].load)
		{
			UnloadModel(&g_RockOn[i].model);
			g_RockOn[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateRockOn(void)
{
	TARGET *target = GetTarget();

	if (GetMode() != MODE_GAME_SKY)
	{
		ENEMY *enemy = GetEnemy();

		for (int i = 0; i < g_max; i++)
		{
			if (g_RockOn[i].use)
			{
				g_RockOn[i].pos = enemy[target[0].enemyNum[i]].pos;

				g_RockOn[i].rot.x += ROCKON_ROT_SPEED;
				g_RockOn[i].rot.z += ROCKON_ROT_SPEED;

				//if (!enemy[target[0].enemyNum[i]].use) g_aRockOn[i].bUse = FALSE;
			}
		}
	}
	else 
	{
		SKY_ENEMY *enemy = GetSkyEnemy();

		for (int i = 0; i < g_max; i++)
		{
			if (g_RockOn[i].use)
			{
				g_RockOn[i].pos = enemy[target[0].enemyNum[i]].pos;

				g_RockOn[i].rot.x += ROCKON_ROT_SPEED;
				g_RockOn[i].rot.z += ROCKON_ROT_SPEED;

				//if (!enemy[target[0].enemyNum[i]].use) g_aRockOn[i].bUse = FALSE;
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawRockOn(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < g_max; i++)
	{
		if (g_RockOn[i].use == FALSE) return;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_RockOn[i].scl.x, g_RockOn[i].scl.y, g_RockOn[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_RockOn[i].rot.x, g_RockOn[i].rot.y, g_RockOn[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_RockOn[i].pos.x, g_RockOn[i].pos.y, g_RockOn[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_RockOn[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_RockOn[0].model);
	}
}


//=============================================================================
// エネミーの取得
//=============================================================================
ROCKON *GetRockOn()
{
	return &g_RockOn[0];
}


void SetRockOn(void)
{
	for (int i = 0; i < g_max; i++)
	{
		if (!g_RockOn[i].use)
		{
			g_RockOn[i].use = TRUE;

			return;
		}
	}
}


// ロックオンターゲットの一括削除
void ResetRockOn(void)
{
	for (int i = 0; i < g_max; i++)
	{
		g_RockOn[i].use = FALSE;
	}
}