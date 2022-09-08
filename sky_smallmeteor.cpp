//=============================================================================
//
// メテオ処理 [meteor.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include <time.h>
#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "sky_smallmeteor.h"
#include "shadow.h"
#include "player.h"
#include "attackRange.h"
#include "blast.h"
#include "target.h"
#include "sky_enemy.h"
#include "sound.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_METEOR_1			"data/MODEL/meteo_01.obj"		// 読み込むモデル名
#define	MODEL_METEOR_2			"data/MODEL/meteo_02.obj"		// 読み込むモデル名
#define	MODEL_METEOR_3			"data/MODEL/meteo_03.obj"		// 読み込むモデル名
#define	MODEL_METEOR_4			"data/MODEL/meteo_04.obj"		// 読み込むモデル名
#define	MODEL_METEOR_5			"data/MODEL/meteo_05.obj"		// 読み込むモデル名
#define	MODEL_METEOR_6			"data/MODEL/meteo_06.obj"		// 読み込むモデル名


#define	VALUE_MOVE				(5.0f)				// 移動量

#define S_METEOR_SPEED			(0.015f)			// ボムの速度

#define S_METEOR_SPEED_MAX		(40.0f)			// 何フレームでメテオがエネミーにぶつかるか
#define S_METEOR_SPEED_MIN		(30.0f)				// 何フレームでメテオがエネミーにぶつかるか
#define S_METEOR_POP_DISTANCE	(100.0f)
#define	S_METEOR_POP_RAND		(50.0f)

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

	LoadModel(MODEL_METEOR_1, &g_sMeteor[0].model);
	LoadModel(MODEL_METEOR_2, &g_sMeteor[1].model);
	LoadModel(MODEL_METEOR_3, &g_sMeteor[2].model);
	LoadModel(MODEL_METEOR_4, &g_sMeteor[3].model);
	LoadModel(MODEL_METEOR_5, &g_sMeteor[4].model);
	LoadModel(MODEL_METEOR_6, &g_sMeteor[5].model);

	for (int i = 0; i < MAX_S_METEOR; i++)
	{
		g_sMeteor[i].load = TRUE;

		g_sMeteor[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_sMeteor[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_sMeteor[i].scl = XMFLOAT3(4.0f, 4.0f, 4.0f);

		g_sMeteor[i].speed = S_METEOR_SPEED;			// 移動スピードクリア
		g_sMeteor[i].time = 0.0f;

		// モデルのディフューズを保存しておく。色変え対応の為。
		//GetModelDiffuse(&g_sMeteor[i].model, &g_sMeteor[i].diffuse[0]);

		//// 色を少し変える
		//XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 0.5f };

		//// 色をセット
		//SetModelDiffuse(&g_sMeteor[i].model, 0, color);

		g_sMeteor[i].size = S_METEOR_SIZE;
		g_sMeteor[i].randMove = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_sMeteor[i].ModelPattern = 0;

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
	TARGET *target = GetTarget();

	for (int i = 0; i < MAX_S_METEOR; i++)
	{
		if (g_sMeteor[i].use == TRUE)			// この攻撃範囲が使われている？
		{									// Yes

			g_sMeteor[i].pos.x += g_sMeteor[i].randMove.x;
			g_sMeteor[i].pos.z += g_sMeteor[i].randMove.z;
			g_sMeteor[i].pos.y += g_sMeteor[i].randMove.y;

			//g_sMeteor[i].pos.x += (target[0].targetPos[i].x - g_sMeteor[i].pos.x) * 0.9f;
			//g_sMeteor[i].pos.x += (target[0].targetPos[i].z - g_sMeteor[i].pos.z) * 0.9f;
			//g_sMeteor[i].pos.x += (target[0].targetPos[i].y - g_sMeteor[i].pos.y) * 0.9f;

			//g_sMeteor[i].pos.x = target[0].targetPos[i].x;
			//g_sMeteor[i].pos.x = target[0].targetPos[i].z;
			//g_sMeteor[i].pos.x = target[0].targetPos[i].y;

			if (g_sMeteor[i].pos.y < -300.0f)
			{
				g_sMeteor[i].use = FALSE;
			}


			// 回転処理
			g_sMeteor[i].rot.x += rot.x;
			g_sMeteor[i].rot.y += rot.y;


#ifdef _DEBUG	// デバッグ情報を表示する
			PrintDebugProc("g_sMeteor[%d]:X:%f Y:%f Z:%f\n", i, g_sMeteor[i].pos.x, g_sMeteor[i].pos.y, g_sMeteor[i].pos.z);
#endif
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
		DrawModel(&g_sMeteor[g_sMeteor[i].ModelPattern].model);

		// カリング設定を戻す
		SetCullingMode(CULL_MODE_BACK);
	}


}


void SetS_Meteor(XMFLOAT3 pos, float rot)
{
	TARGET *target = GetTarget();
	SKY_ENEMY *enemy = GetSkyEnemy();

	for (int i = 0; i < target->count; i++)
	{
		if (!g_sMeteor[i].use)
		{
			g_sMeteor[i].use = TRUE;

			PlaySound(SOUND_LABEL_SE_meteor_shot);

			g_sMeteor[i].ModelPattern = rand() % 2;

			// 発生位置を規定位置から少しランダム要素を加える
			g_sMeteor[i].pos.x = (pos.x - sinf(rot) * S_METEOR_POP_DISTANCE) + RamdomFloat(2, S_METEOR_POP_RAND, -S_METEOR_POP_RAND);
			g_sMeteor[i].pos.z = (pos.z + cosf(rot) * S_METEOR_POP_DISTANCE) + RamdomFloat(2, S_METEOR_POP_RAND, -S_METEOR_POP_RAND);
			g_sMeteor[i].pos.y = 300.0f;

			//g_sMeteor[i].randMove.x = RamdomFloat(2, 2.0f, -2.0f);
			//g_sMeteor[i].randMove.z = RamdomFloat(2, 2.0f, -2.0f);
			//g_sMeteor[i].randMove.y = RamdomFloat(2, 6.0f, 4.0f);

			float time = RamdomFloat(2, S_METEOR_SPEED_MAX, S_METEOR_SPEED_MIN);
			g_sMeteor[i].randMove.x = (enemy[target[0].enemyNum[i]].pos.x - g_sMeteor[i].pos.x) / time;
			g_sMeteor[i].randMove.z = (enemy[target[0].enemyNum[i]].pos.z - g_sMeteor[i].pos.z) / time;
			g_sMeteor[i].randMove.y = (enemy[target[0].enemyNum[i]].pos.y - g_sMeteor[i].pos.y) / time;
			//g_sMeteor[i].randMove.y = RamdomFloat(2, 6.0f, 4.0f);
			//g_sMeteor[i].randMove.y = RamdomFloat(2, 2.0f, 1.0f);



			//return;
		}

	}

	target[0].count = 0;
	//target[0].use = FALSE;
}


S_METEOR *GetS_Meteor(void)
{
	return &g_sMeteor[0];
}