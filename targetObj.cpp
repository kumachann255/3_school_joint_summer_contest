//=============================================================================
//
// ターゲット用の空オブジェクト処理 [targetObj.cpp]
// Author : 熊澤
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "targetObj.h"
#include "debugproc.h"
#include "target.h"
#include "player.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_DOME_CITY			"data/MODEL/collisionBox.obj"	// 読み込むモデル名

#define MOVE_VALUE_Y			(5.0f)		// ターゲットが縦に動く速度
#define MOVE_VALUE_X			(0.02f)		// ターゲットが横に動く速度

#define OBJ_DISTANCE			(700.0f)	// カメラからターゲットの距離

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static TARGETOBJ		g_TargetObj;
static float			g_rot_y;

static BOOL				g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTargetObj(void)
{
	LoadModel(MODEL_DOME_CITY, &g_TargetObj.model);

	g_TargetObj.load = TRUE;

	g_TargetObj.use = TRUE;

	g_TargetObj.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_TargetObj.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_TargetObj.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	// モデルのディフューズを保存しておく。色変え対応の為。
	GetModelDiffuse(&g_TargetObj.model, &g_TargetObj.diffuse[0]);


	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTargetObj(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_TARGETOBJ; i++)
	{
		if (g_TargetObj.load)
		{
			UnloadModel(&g_TargetObj.model);
			g_TargetObj.load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTargetObj(void)
{
	// 攻撃方法がサークルの場合はスキップ
	if (!GetPlayer()->rockOn) return;

	if (!GetKeyboardPress(DIK_LSHIFT))
	{
		// 移動させちゃう
		if (GetKeyboardPress(DIK_A))
		{	// 左へ移動

			// ターゲットアイコンが画面外に出ていないかを確認
			if (GetTargetArea(left)) g_TargetObj.rot.y -= MOVE_VALUE_X;
		}
		if (GetKeyboardPress(DIK_D))
		{	// 右へ移動

			// ターゲットアイコンが画面外に出ていないかを確認
			if (GetTargetArea(right)) g_TargetObj.rot.y += MOVE_VALUE_X;
		}
		if (GetKeyboardPress(DIK_W))
		{	// 上へ移動

			// ターゲットアイコンが画面外に出ていないかを確認
			//if (GetTargetArea(up)) 
				//g_TargetObj.rot.x -= MOVE_VALUE_X;
			if (GetTargetArea(up)) g_TargetObj.pos.y += MOVE_VALUE_Y;
		}
		if (GetKeyboardPress(DIK_S))
		{	// 下へ移動

			// ターゲットアイコンが画面外に出ていないかを確認
			//if (GetTargetArea(down)) 
				//g_TargetObj.rot.x += MOVE_VALUE_X;
			if (GetTargetArea(down)) g_TargetObj.pos.y -= MOVE_VALUE_Y;
		}
	}

	CAMERA *camera = GetCamera();

	//g_TargetObj.rot.y += camera->rot.y;
	//g_TargetObj.rot.x += camera->rot.x;
	//g_TargetObj.rot.z += camera->rot.z;

	// 入力のあった方向へOBJを移動させる
	g_TargetObj.pos.x = sinf(g_TargetObj.rot.y) * OBJ_DISTANCE;
	g_TargetObj.pos.z = cosf(g_TargetObj.rot.y) * OBJ_DISTANCE;
	//g_TargetObj.pos.y = cosf(g_TargetObj.rot.x) * OBJ_DISTANCE;
	//g_TargetObj.pos.z += sinf(g_TargetObj.rot.x) * OBJ_DISTANCE;

#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("g_TargetObj:↑ → ↓ ←　Space\n");
	PrintDebugProc("g_TargetObj:X:%f Y:%f Z:%f\n", g_TargetObj.pos.x, g_TargetObj.pos.y, g_TargetObj.pos.z);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTargetObj(void)
{
#ifdef _DEBUG	// デバッグ時のみ表示

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	if (g_TargetObj.use == FALSE) return;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_TargetObj.scl.x, g_TargetObj.scl.y, g_TargetObj.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_TargetObj.rot.x, g_TargetObj.rot.y, g_TargetObj.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_TargetObj.pos.x, g_TargetObj.pos.y, g_TargetObj.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// プレイヤーを親とする
	//PLAYER *player = GetPlayer();
	//mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&player->mtxWorld));


	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_TargetObj.mtxWorld, mtxWorld);

	// モデル描画
	DrawModel(&g_TargetObj.model);

#endif
}

//=============================================================================
// エネミーの取得
//=============================================================================
TARGETOBJ *GetTargetObj()
{
	return &g_TargetObj;
}
