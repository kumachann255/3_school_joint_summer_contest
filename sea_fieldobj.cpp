//=============================================================================
//
// 海オブジェクトの処理 [sea_fieldobj.cpp]
// Author : 大塚勝亮
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "debugproc.h"
#include "model.h"
#include "sea_fieldobj.h"
#include "meshfield.h"
#include "same.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_NAMI			"data/MODEL/kuzira_nami.obj"	// 読み込むモデル名
#define	MODEL_KUZIRA		"data/MODEL/kuzira.obj"			// 読み込むモデル名
#define	MODEL_SHIO			"data/MODEL/kuzira_sio00.obj"	// 読み込むモデル名
#define	MODEL_HORAGAI		"data/MODEL/kuzira_horage.obj"	// 読み込むモデル名
#define	MODEL_SHIMA			"data/MODEL/sima.obj"			// 読み込むモデル名
#define	MODEL_HITO			"data/MODEL/enemy08.obj"		// 読み込むモデル名

#define VALUE_NAMI				(5.0f)						// 波加算量
#define VALUE_DOWN_POSY			(0.3f)						// 下降加算量
#define VALUE_FRONT				(1.0f)						// 前後加算量
#define	VALUE_ROTATE			(0.01f)						// 回転加算量
#define	VALUE_POSY				(5.0f)						// 上下加算量
#define	MAX_FRONT_ROT			(0.574f)					// 最大前回転値
#define	MAX_BACK_ROT			(-0.65f)					// 最大後回転値
#define	MAX_LOWPOS_Y			(-100.0f)					// 最大下降値
#define	MAX_LOWPOS_KUZIRA_Y		(50.0f)						// 最大下降値
#define MAX_LOW_NAMI			(-25.0f)					// 波の最大下降値
#define KUZIRA_POP_COUNT		(300)						// くじらの出現間隔
#define KUZIRA_POP_Z			(550)						// くじらの初期ポップ位置(z座標)
#define KUZIRA_POP_X			(700)						// くじらの初期ポップの範囲(x座標)
#define MAX_HUNSYA_TIME			(25)						// 潮の最大噴射時間

enum {
	wait,
	move,
	hunsya,
	up,
	down,
	left,
	light
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetNami(int i, int mode);
void SetShio(XMFLOAT3 pos);
void SetKuzira(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static SEA_FOBJ				g_Nami[MAX_SEA_WAVE];			// 波
static SEA_FOBJ				g_Kuzira[MAX_SEA_FOBJ_SIGN];	// クジラ
static SEA_FOBJ				g_Shio[MAX_SEA_FOBJ_SIGN];		// 潮
static SEA_FOBJ				g_Horagai;						// ほら貝
static SEA_FOBJ				g_Shima;						// 島
static SEA_FOBJ				g_Hito;							// 人

static BOOL				g_Load = FALSE;

static int popCount = 0;
static int hunsyaTime = 0;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSeaFieldObj(void)
{
	LoadModel(MODEL_KUZIRA, &g_Kuzira[0].model);
	LoadModel(MODEL_NAMI, &g_Nami[0].model);
	LoadModel(MODEL_SHIO, &g_Shio[0].model);
	LoadModel(MODEL_HORAGAI, &g_Horagai.model);
	LoadModel(MODEL_SHIMA, &g_Shima.model);
	LoadModel(MODEL_HITO, &g_Hito.model);

	for (int i = 0; i < MAX_SEA_WAVE; i++)
	{
		LoadModel(MODEL_NAMI, &g_Nami[i].model);
		g_Nami[i].load = TRUE;
		g_Nami[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Nami[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Nami[i].scl = XMFLOAT3(1.0f, 3.0f, 3.0f);
		g_Nami[i].moveFlag = wait;
		g_Nami[i].use = FALSE;			// TRUE:生きてる
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Nami[i].model, &g_Nami[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		LoadModel(MODEL_KUZIRA, &g_Kuzira[i].model);
		g_Kuzira[i].load = TRUE;
		g_Kuzira[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Kuzira[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Kuzira[i].scl = XMFLOAT3(3.5f - 0.5f * i, 3.5f - 0.5f * i, 3.5f - 0.5f * i);
		g_Kuzira[i].moveFlag = hunsya;
		g_Kuzira[i].use = FALSE;			// TRUE:生きてる
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Kuzira[i].model, &g_Kuzira[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		LoadModel(MODEL_SHIO, &g_Shio[i].model);
		g_Shio[i].load = TRUE;
		g_Shio[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Shio[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Shio[i].scl = XMFLOAT3(3.0f, 6.0f, 3.0f);
		g_Shio[i].moveFlag = up;
		g_Shio[i].use = FALSE;			// TRUE:生きてる
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Shio[i].model, &g_Shio[i].diffuse[0]);
	}

	g_Horagai.load = TRUE;
	g_Horagai.pos = XMFLOAT3(20.0f, 10.0f, 20.0f);
	g_Horagai.rot = XMFLOAT3(0.754f, 1.57f, 0.0f);
	g_Horagai.scl = XMFLOAT3(0.5f, 0.5f, 0.5f);
	g_Horagai.moveFlag = wait;
	g_Horagai.use = TRUE;			// TRUE:生きてる

	g_Shima.load = TRUE;
	g_Shima.pos = XMFLOAT3(-10.0f, 0.0f, 10.0f);
	g_Shima.rot = XMFLOAT3(0.0f, 0.754f, 0.0f);
	g_Shima.scl = XMFLOAT3(1.125f, 1.125f, 1.125f);
	g_Shima.moveFlag = wait;
	g_Shima.use = TRUE;			// TRUE:生きてる

	g_Hito.load = TRUE;
	g_Hito.pos = XMFLOAT3(0.0f, 20.0f, 10.0f);
	g_Hito.rot = XMFLOAT3(0.0f, 3.14f, 0.0f);
	g_Hito.scl = XMFLOAT3(0.75f, 0.75f, 0.75f);
	g_Hito.moveFlag = wait;
	g_Hito.use = TRUE;			// TRUE:生きてる

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSeaFieldObj(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_SEA_WAVE; i++)
	{
		if (g_Nami[i].load)
		{
			UnloadModel(&g_Nami[i].model);
			g_Nami[i].load = FALSE;
		}
	}

	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Kuzira[i].load)
		{
			UnloadModel(&g_Kuzira[i].model);
			g_Kuzira[i].load = FALSE;
		}
	}

	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Shio[i].load)
		{
			UnloadModel(&g_Shio[i].model);
			g_Shio[i].load = FALSE;
		}
	}

	if (g_Horagai.load)
	{
		UnloadModel(&g_Horagai.model);
		g_Horagai.load = FALSE;
	}

	if (g_Shima.load)
	{
		UnloadModel(&g_Shima.model);
		g_Shima.load = FALSE;
	}

	if (g_Hito.load)
	{
		UnloadModel(&g_Hito.model);
		g_Hito.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSeaFieldObj(void)
{

	popCount++;

	// クジラの処理
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{

		// 時間経過とエネミーの出現数次第でポップするか判断
		if (popCount == KUZIRA_POP_COUNT)
		{
			SetKuzira();
			popCount = 0;
		}

		if (g_Kuzira[i].use == TRUE)
		{
			switch (g_Kuzira[i].moveFlag)
			{
			case hunsya:
				// 浮上する
				hunsyaTime++;
				SetShio(g_Kuzira[i].pos);

				if (hunsyaTime > MAX_HUNSYA_TIME)
				{
					hunsyaTime = 0;
					g_Kuzira[i].moveFlag = up;
				}
				break;

			case up:
				// 浮上する
				g_Kuzira[i].pos.y += VALUE_POSY;
				if (g_Kuzira[i].pos.y > MAX_LOWPOS_KUZIRA_Y)
				{
					g_Kuzira[i].moveFlag = move;
				}
				break;

			case move:
				// x軸とy軸で回転
				g_Kuzira[i].rot.x += VALUE_ROTATE;
				g_Kuzira[i].rot.z += VALUE_ROTATE;

				if (g_Kuzira[i].rot.x > MAX_FRONT_ROT)
				{
					g_Kuzira[i].moveFlag = down;
				}
				break;

			case down:
				// x軸とy軸で回転しつつ落ちる
				g_Kuzira[i].pos.y -= VALUE_POSY;
				g_Kuzira[i].rot.x += VALUE_ROTATE * 2.0f;
				g_Kuzira[i].rot.z += VALUE_ROTATE * 2.0f;
				g_Kuzira[i].pos.z -= VALUE_POSY;

				if (g_Kuzira[i].pos.y < -MAX_LOWPOS_KUZIRA_Y)
				{
					g_Kuzira[i].use = FALSE;
				}
				break;

				//case wait:
				//	g_Kuzira[i].rot.x += VALUE_ROTATE;
				//	if (g_Kuzira[i].rot.x > MAX_FRONT_ROT)
				//	{
				//		g_Kuzira[i].moveFlag = move;
				//	}
				//	break;

				//case move:
				//	g_Kuzira[i].rot.x -= VALUE_ROTATE;
				//	if (g_Kuzira[i].rot.x < MAX_BACK_ROT)
				//	{
				//		g_Kuzira[i].moveFlag = wait;
				//	}
				//	break;
			}
		}
	}

	// 波の処理
	for (int i = 0; i < MAX_SEA_WAVE; i++)
	{
		SAME *same = GetSame();
		int dir;

		if (g_Nami[i].use == FALSE)
		{
			dir = i % MAX_SEA_WAVE;
			switch (dir)
			{
			case 0:
				SetNami(dir, same->mode);
				break;

			case 1:
				SetNami(dir, same->mode);
				break;
			}
		}
		else if (g_Nami[i].use == TRUE)
		{
			dir = i % MAX_SEA_WAVE;
			switch (dir)
			{
			case 0:
				g_Nami[i].pos.x += -VALUE_FRONT;
				g_Nami[i].pos.z -= VALUE_FRONT;
				g_Nami[i].pos.y += VALUE_DOWN_POSY;
				if (g_Nami[i].pos.y > MAX_LOW_NAMI)
				{
					g_Nami[i].use = FALSE;
				}
				break;

			case 1:
				g_Nami[i].pos.x += VALUE_FRONT;
				g_Nami[i].pos.z -= VALUE_FRONT;
				g_Nami[i].pos.y += VALUE_DOWN_POSY;
				if (g_Nami[i].pos.y > MAX_LOW_NAMI)
				{
					g_Nami[i].use = FALSE;
				}
				break;
			}

		}
	}

	// 潮の処理
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Shio[i].use == TRUE)
		{
			switch (g_Shio[i].moveFlag)
			{
			case up:
				g_Shio[i].pos.y += VALUE_POSY;
				if (g_Shio[i].pos.y > 0.0f)
				{
					g_Shio[i].moveFlag = down;
				}
				break;

			case down:
				g_Shio[i].pos.y -= VALUE_POSY;
				if (g_Shio[i].pos.y < 0.0f)
				{
					g_Shio[i].moveFlag = up;
					g_Shio[i].use = FALSE;
				}
				break;
			}
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		PrintDebugProc("g_Nami[%d].pos.y:%f\n", i, g_Nami[i].pos.y);
		PrintDebugProc("g_Kuzira[%d].rot.x:%f\n", i, g_Kuzira[i].rot.x);
		PrintDebugProc("g_Shio[%d].pos.y:%f\n", i , g_Shio[i].pos.y);
	}
	PrintDebugProc("Popcount: %d\n", popCount);


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSeaFieldObj(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// 波の描画
	for (int i = 0; i < MAX_SEA_WAVE; i++)
	{
		if (g_Nami[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Nami[i].scl.x, g_Nami[i].scl.y, g_Nami[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Nami[i].rot.x, g_Nami[i].rot.y + XM_PI, g_Nami[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Nami[i].pos.x, g_Nami[i].pos.y, g_Nami[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Nami[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Nami[0].model);

	}

	// クジラの描画
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Kuzira[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Kuzira[i].scl.x, g_Kuzira[i].scl.y, g_Kuzira[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Kuzira[i].rot.x, g_Kuzira[i].rot.y + XM_PI, g_Kuzira[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Kuzira[i].pos.x, g_Kuzira[i].pos.y, g_Kuzira[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Kuzira[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Kuzira[0].model);

	}

	// 潮の描画
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Shio[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Shio[i].scl.x, g_Shio[i].scl.y, g_Shio[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Shio[i].rot.x, g_Shio[i].rot.y + XM_PI, g_Shio[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Shio[i].pos.x, g_Shio[i].pos.y, g_Shio[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Shio[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Shio[0].model);
	}

	// ほら貝の描画

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Horagai.scl.x, g_Horagai.scl.y, g_Horagai.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Horagai.rot.x, g_Horagai.rot.y + XM_PI, g_Horagai.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Horagai.pos.x, g_Horagai.pos.y, g_Horagai.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Horagai.mtxWorld, mtxWorld);

	// モデル描画
	DrawModel(&g_Horagai.model);

	// 島の描画

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Shima.scl.x, g_Shima.scl.y, g_Shima.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Shima.rot.x, g_Shima.rot.y + XM_PI, g_Shima.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Shima.pos.x, g_Shima.pos.y, g_Shima.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Shima.mtxWorld, mtxWorld);

	// モデル描画
	DrawModel(&g_Shima.model);

	// 人の描画

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Hito.scl.x, g_Hito.scl.y, g_Hito.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Hito.rot.x, g_Hito.rot.y + XM_PI, g_Hito.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Hito.pos.x, g_Hito.pos.y, g_Hito.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Hito.mtxWorld, mtxWorld);

	// モデル描画
	DrawModel(&g_Hito.model);

}

//=============================================================================
// 波の発生
//=============================================================================
void SetNami(int i, int mode)
{
	for (int i = 0; i < MAX_SEA_WAVE; i++)
	{
		SAME *same = GetSame();

		if (g_Nami[i].use == FALSE && same->mode == PACKNCYO)
		{
			g_Nami[i].use = TRUE;

			if (i == 1)
			{	// 右に設置
				g_Nami[i].pos = same->pos;
				g_Nami[i].pos.x += VALUE_NAMI;
				g_Nami[i].pos.y -= VALUE_NAMI;
				g_Nami[i].rot = XMFLOAT3(0.0f, 2.0f, 0.0f);
			}
			else
			{	// 左に設置
				g_Nami[i].pos = same->pos;
				g_Nami[i].pos.x += -VALUE_NAMI;
				g_Nami[i].pos.y -=  VALUE_NAMI;
				g_Nami[i].rot = XMFLOAT3(0.0f, -2.0f, 0.0f);
			}
			return;
		}
	}
}

//=============================================================================
// クジラの発生
//=============================================================================
void SetKuzira(void)
{
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Kuzira[i].use == FALSE)
		{
			//SetSourceVolume(SOUND_LABEL_SE_carHorn01, 1.0f);
			// SEのセット

			g_Kuzira[i].use = TRUE;
			g_Kuzira[i].moveFlag = hunsya;
			g_Kuzira[i].pos.z = KUZIRA_POP_Z;
			g_Kuzira[i].pos.y = -MAX_LOWPOS_KUZIRA_Y;
			g_Kuzira[i].rot = XMFLOAT3(-1.57f,0.0f, 0.0f);

			// x座標はランダム
			g_Kuzira[i].pos.x = (float)(rand() % KUZIRA_POP_X) - ((float)KUZIRA_POP_X / 2.0f);
			// z地点もランダム
			g_Kuzira[i].pos.z = (float)(rand() % KUZIRA_POP_Z) + ((float)KUZIRA_POP_Z / 2.0f);

			return;
		}
	}
}

//=============================================================================
// クジラの発生
//=============================================================================
void SetShio(XMFLOAT3 pos)
{
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Shio[i].use == FALSE)
		{
			//SetSourceVolume(SOUND_LABEL_SE_carHorn01, 1.0f);
			// SEのセット

			g_Shio[i].use = TRUE;
			g_Shio[i].pos = pos;
			g_Shio[i].pos.y = -20.0f;
			g_Shio[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

			return;
		}
	}
}