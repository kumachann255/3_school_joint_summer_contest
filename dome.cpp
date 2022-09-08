//=============================================================================
//
// ドーム処理 [dome.cpp]
// Author : 熊澤
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "dome.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_DOME_CITY			"data/MODEL/wakusei.obj"				// 読み込むモデル名
#define	MODEL_DOME_UNIVERSE		"data/MODEL/dome_universe.obj"			// 読み込むモデル名
#define	SKY_OFFSET_Y			(XM_PI/15000)							// 回転速度

#define DOME_SPEED				(1.0f)					// ドームが下さがる速度


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static DOME				g_Dome[MAX_DOME];						// 背景
static float			g_rot_y;

static BOOL				g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitDome(void)
{
	LoadModel(MODEL_DOME_CITY, &g_Dome[0].model);
	LoadModel(MODEL_DOME_UNIVERSE, &g_Dome[1].model);

	for (int i = 0; i < MAX_DOME; i++)
	{
		g_Dome[i].load = TRUE;

		g_Dome[i].use = TRUE;

		g_Dome[i].pos = XMFLOAT3(0.0f, -800.0f, 0.0f);
		g_Dome[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Dome[i].scl = XMFLOAT3(80.0f, 80.0f, 80.0f);

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Dome[0].model, &g_Dome[0].diffuse[0]);
	}
	
	g_rot_y = SKY_OFFSET_Y;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitDome(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_DOME; i++)
	{
		if (g_Dome[i].load)
		{
			UnloadModel(&g_Dome[i].model);
			g_Dome[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateDome(void)
{
	// ドームを下げる
	// プレイヤーが上に上がってるように見せる
	g_Dome[0].pos.y -= DOME_SPEED;

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawDome(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// フォグ無効
	SetFogEnable(FALSE);

	for (int i = 0; i < MAX_DOME; i++)
	{
		if (g_Dome[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Dome[i].scl.x, g_Dome[i].scl.y, g_Dome[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Dome[i].rot.x, g_Dome[i].rot.y, g_Dome[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Dome[i].pos.x, g_Dome[i].pos.y, g_Dome[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Dome[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Dome[i].model);
	
	}

	// フォグ設定を戻す
	SetFogEnable(FALSE);


	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

}

//=============================================================================
// エネミーの取得
//=============================================================================
DOME *GetDome()
{
	return &g_Dome[0];
}
