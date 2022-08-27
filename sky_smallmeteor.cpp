//=============================================================================
//
// メテオ処理 [meteor.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include <time.h>
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "sky_smallmeteor.h"
#include "shadow.h"
#include "player.h"
#include "attackRange.h"
#include "blast.h"

#include "sound.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BOM			"data/MODEL/m.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量

#define S_METEOR_SPEED			(0.015f)						// ボムの速度



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static S_METEOR			g_sMeteor[MAX_S_METEOR];				// 攻撃範囲

static BOOL				g_Load = FALSE;

static XMFLOAT3			control0, control1, control2;	// 制御点

static XMFLOAT3			rot;	// 回転

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitS_Meteor(void)
{
	//srand(time(NULL));

	for (int i = 0; i < MAX_S_METEOR; i++)
	{
		LoadModel(MODEL_BOM, &g_sMeteor[i].model);
		g_sMeteor[i].load = TRUE;

		g_sMeteor[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_sMeteor[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_sMeteor[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);

		g_sMeteor[i].speed = S_METEOR_SPEED;			// 移動スピードクリア
		g_sMeteor[i].time = 0.0f;

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_sMeteor[i].model, &g_sMeteor[i].diffuse[0]);

		// 色を少し変える
		XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 0.5f };

		// 色をセット
		SetModelDiffuse(&g_sMeteor[i].model, 0, color);

		g_sMeteor[i].size = S_METEOR_SIZE;
		g_sMeteor[i].randMove = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_sMeteor[i].use = FALSE;			// TRUE:生きてる


		g_Load = TRUE;
	}
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitS_Meteor(void)
{
	for (int i = 0; i < MAX_S_METEOR; i++)
	{
		if (g_Load == FALSE) return;

		if (g_sMeteor[i].load)
		{
			UnloadModel(&g_sMeteor[i].model);
			g_sMeteor[i].load = FALSE;
		}
	}
	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateS_Meteor(void)
{
	for (int i = 0; i < MAX_S_METEOR; i++)
	{
		if (g_sMeteor[i].use == TRUE)			// この攻撃範囲が使われている？
		{									// Yes

			g_sMeteor[i].pos.x += g_sMeteor[i].randMove.x;
			g_sMeteor[i].pos.y -= g_sMeteor[i].randMove.y;
			g_sMeteor[i].pos.z += g_sMeteor[i].randMove.z;


			if (g_sMeteor[i].pos.y < -300.0f)
			{
				g_sMeteor[i].use = FALSE;
			}


			// 回転処理
			g_sMeteor[i].rot.x += rot.x;
			g_sMeteor[i].rot.y += rot.y;



		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawS_Meteor(void)
{

	for (int i = 0; i < MAX_S_METEOR; i++)
	{

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// カリング無効
		SetCullingMode(CULL_MODE_NONE);

		if (g_sMeteor[i].use == FALSE) return;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_sMeteor[i].scl.x, g_sMeteor[i].scl.y, g_sMeteor[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_sMeteor[i].rot.x, g_sMeteor[i].rot.y + XM_PI, g_sMeteor[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_sMeteor[i].pos.x, g_sMeteor[i].pos.y, g_sMeteor[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_sMeteor[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_sMeteor[i].model);

		// カリング設定を戻す
		SetCullingMode(CULL_MODE_BACK);
	}


}


void SetS_Meteor(XMFLOAT3 pos, float rot)
{
	for (int i = 0; i < MAX_S_METEOR; i++)
	{
		if (!g_sMeteor[i].use)
		{
			g_sMeteor[i].use = TRUE;

			g_sMeteor[i].randMove.x = RamdomFloat(2, 2.0f, -2.0f);
			g_sMeteor[i].randMove.z = RamdomFloat(2, 2.0f, -2.0f);
			g_sMeteor[i].randMove.y = RamdomFloat(2, 6.0f, 4.0f);

			g_sMeteor[i].pos.y = 300.0f;


			//				どの向きに　どれくらいの距離で		さらにそこからランダムでどのくらい移動するか

			g_sMeteor[i].pos.x = (pos.x - sinf(rot) * 100.0f) + RamdomFloat(2, 50.0f, -50.0f);
			g_sMeteor[i].pos.z = (pos.z + cosf(rot) * 100.0f) + RamdomFloat(2, 50.0f, -50.0f);

			//return;
		}

	}
}


S_METEOR *GetS_Meteor(void)
{
	return &g_sMeteor[0];
}