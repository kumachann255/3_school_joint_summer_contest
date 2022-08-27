//=============================================================================
//
// クラッカーと当たり判定処理 [cracker.cpp]
// Author : エナ
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "attackRange.h"
#include "shadow.h"
#include "cracker.h"
#include "meshfield.h"
#include "speech.h"
#include "enemy.h"
#include "enemyHeli.h"
#include "tutorial.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_CRACKER			"data/MODEL/newneba00.obj"		// 読み込むモデル名
#define	MODEL_CRACKER_1			"data/MODEL/newneba01.obj"		// 読み込むモデル名
#define	MODEL_CRACKER_2			"data/MODEL/newneba02.obj"		// 読み込むモデル名
#define	MODEL_CRACKER_3			"data/MODEL/newneba03.obj"		// 読み込むモデル名
#define	MODEL_CRACKER_4			"data/MODEL/newneba04.obj"		// 読み込むモデル名


#define	VALUE_MOVE				(5.0f)						// 移動量
#define	VALUE_ROTATE			(XM_PI * 0.02f)				// 回転量

#define CRACKER_OFFSET_Y		(12.0f)						// オブジェクトの足元をあわせる

#define CRACKER_LIFE			(150)						// 爆破オブジェクトの表示時間
#define CRACKER_MOVE_TIME_0	(3)								// ねばねばが広がる時間
#define CRACKER_MOVE_TIME_1	(10)							// 広がった状態で止まる時間
#define CRACKER_MOVE_TIME_2	(10)							// ねばねばが縮まる時間
#define CRACKER_MOVE_TIME_3	(20)							// ねばねばが空中で止まる時間

#define CRACKER_ROT				(XM_PI * 0.5f * 0.2f)		// 回転量(X軸)
#define CRACKER_ROT_MAX			(XM_PI * 0.5f)				// 回転最大値

#define CRACKER_MOVE			(-2.0f)						// 移動量(Z軸)
#define CRACKER_MOVE_TIME		(10)						// 移動最大値

#define MAX_CRACKER_MOVE		(5)							// モーフィングの数



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CRACKER			g_Cracker[MAX_CRACKER];				// 爆破オブジェクト

static BOOL				g_Load = FALSE;

static MODEL			g_Cracker_Vertex[MAX_CRACKER_MOVE];		// モーフィング用モデルの頂点情報が入ったデータ配列
static VERTEX_3D		*g_CrackerVertex = NULL;			// 途中経過を記録する場所

static float			g_time;							// モーフィングの経過時間
static int				g_downCount;					// 落ち始めてどのくらい時間が経過したか

static int				g_morphingNum;					// モーフィングの番号

static BOOL				g_cameraOn;						// カメラのスイッチ

static int				g_stopTime;						// 爆弾が空中で止まる時間

static float			g_CrackerMove;					//クラッカーの移動量(Z軸)
static int				g_CrackerTime;					//クラッカーの移動量(Z軸)
static float			g_CrackerRot;					// クラッカーの回転量(X軸)

static BOOL				g_particleOn;					// TRUE:爆発エフェクト発生

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCracker(void)
{
	for (int i = 0; i < MAX_CRACKER; i++)
	{
		LoadModelMorphing(MODEL_CRACKER, &g_Cracker[i].model);
		g_Cracker[i].load = TRUE;

		g_Cracker[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Cracker[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Cracker[i].scl = XMFLOAT3(1.3f, 1.3f, 1.3f);
		g_Cracker[i].size = CRACKER_SIZE;
		g_Cracker[i].life = 0;

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Cracker[i].model, &g_Cracker[i].diffuse[0]);

		g_Cracker[i].use = FALSE;			// TRUE:生きてる
		//g_Cracker[i].move = FALSE;		// TRUE:奥へ移動する
	
	}

	// モーフィングするオブジェクトの読み込み
	LoadObj(MODEL_CRACKER, &g_Cracker_Vertex[0]);
	LoadObj(MODEL_CRACKER_1, &g_Cracker_Vertex[1]);
	LoadObj(MODEL_CRACKER_2, &g_Cracker_Vertex[2]);
	LoadObj(MODEL_CRACKER_3, &g_Cracker_Vertex[3]);
	LoadObj(MODEL_CRACKER_4, &g_Cracker_Vertex[4]);

	// 中身を配列として使用できるように仕様変更
	g_CrackerVertex = new VERTEX_3D[g_Cracker_Vertex[0].VertexNum];

	// 差分(途中経過)の初期化
	for (int i = 0; i < g_Cracker_Vertex[0].VertexNum; i++)
	{
		g_CrackerVertex[i].Position = g_Cracker_Vertex[0].VertexArray[i].Position;
		g_CrackerVertex[i].Diffuse  = g_Cracker_Vertex[0].VertexArray[i].Diffuse;
		g_CrackerVertex[i].Normal   = g_Cracker_Vertex[0].VertexArray[i].Normal;
		g_CrackerVertex[i].TexCoord = g_Cracker_Vertex[0].VertexArray[i].TexCoord;
	}

	g_morphingNum = 0;
	g_time = 0.0f;
	g_downCount = 0;
	g_cameraOn = FALSE;
	g_stopTime = 0;

	g_CrackerMove = CRACKER_MOVE;
	g_CrackerTime = CRACKER_MOVE_TIME;
	g_CrackerRot = CRACKER_ROT;
	
	g_particleOn = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCracker(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_CRACKER; i++)
	{
		if (g_Cracker[i].load)
		{
			UnloadModel(&g_Cracker[i].model);
			g_Cracker[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateCracker(void)
{
	for (int i = 0; i < MAX_CRACKER; i++)
	{
		g_particleOn = FALSE;		//エフェクトフラグをオフにする

		if (g_Cracker[i].use == TRUE)			// この攻撃範囲が使われている？
		{									// Yes
			// 寿命を減らす
			g_Cracker[i].life--;

			// 寿命が尽きたら未使用に
			if (g_Cracker[i].life < 0)
			{
				g_Cracker[i].use = FALSE;
			}

			// クラッカーの移動処理
			{
				if (g_CrackerTime > 0)
				{
					g_Cracker[i].pos.z += g_CrackerMove;
				}

				g_CrackerTime--;

				// 回転処理
				g_Cracker[i].rot.x += g_CrackerRot;

				if (g_Cracker[i].rot.x >= CRACKER_ROT_MAX)
				{
					g_CrackerRot = g_CrackerRot * (-0.2f);
				}
		
				if (g_Cracker[i].rot.x <= 0.0f)
				{
					g_Cracker[i].rot.x = 0.0f;
				}
			
			}


			// 当たった後エネミーの挙動処理
			{
				//ENEMY *enemy = GetEnemy();		// エネミーのポインターを初期化
				//ENEMY_HELI *enemyheli = GetEnemyHeli();		// エネミーのポインターを初期化

				// 他エネミーの処理
				//for (int j = 0; j < MAX_ENEMY; j++)
				//{
				//	if (enemy[j].CrackerHit == TRUE)
				//	{
				//		// ランダム処理
				//		if (enemy[j].CrackerRot == TRUE)
				//		{
				//			enemy[j].rot.x = RamdomFloat(2, 6.28f, -6.28f);
				//			enemy[j].rot.z = RamdomFloat(2, 6.28f, -6.28f);
				//			enemy[j].pos.y += RamdomFloat(2, 50.0f, 20.0f);

				//			g_radius = RamdomFloat(2, 20.0f, 5.0f);
				//		
				//			enemy[j].CrackerRot = FALSE;
				//		}
				//		
				//		// 回転させる
				//		enemy[j].rot.y += CRACKER_ROT_Y;
				//		enemy[j].pos.x = g_Cracker[i].pos.x + sinf(enemy[j].radian) * g_radius;
				//		enemy[j].pos.z = g_Cracker[i].pos.z + cosf(enemy[j].radian) * g_radius;

				//		enemy[j].radian += CRACKER_ROT_Y;

				//		// エネミーを倒す
				//		if (g_Cracker[0].use == FALSE)
				//		{
				//			enemy[j].use = FALSE;
				//			SetTutorialEnemy(TRUE);
				//			g_particleOn = TRUE;
				//		}

				//	}
				//}
			
				// ヘリの処理
				//for (int j = 0; j < MAX_ENEMY_HELI; j++)
				//{
				//	if (enemyheli[j].CrackerHit == TRUE)
				//	{
				//		// ランダム処理
				//		if (enemyheli[j].CrackerRot == TRUE)
				//		{
				//			enemyheli[j].rot.x = RamdomFloat(2, 6.28f, -6.28f);
				//			enemyheli[j].rot.z = RamdomFloat(2, 6.28f, -6.28f);
				//			enemyheli[j].pos.y += RamdomFloat(2, 50.0f, 20.0f);

				//			g_radius = RamdomFloat(2, 20.0f, 5.0f);

				//			enemyheli[j].CrackerRot = FALSE;
				//		}

				//		// 回転させる
				//		enemyheli[j].rot.y += CRACKER_ROT_Y;
				//		enemyheli[j].pos.x = g_Cracker[i].pos.x + sinf(enemyheli[j].radian) * g_radius;
				//		enemyheli[j].pos.z = g_Cracker[i].pos.z + cosf(enemyheli[j].radian) * g_radius;

				//		enemyheli[j].radian += CRACKER_ROT_Y;

				//		// エネミーを倒す
				//		if (g_Cracker[0].use == FALSE)
				//		{
				//			enemyheli[j].use = FALSE;
				//			//SetTutorialEnemy(TRUE);
				//			g_particleOn = TRUE;
				//		}

				//	}
				//}

			}


			//// ねばねばのモーフィングの処理
			//{
			//	int after, brfore;

			//	// 広がり終えたら縮まる方向へモーフィング
			//	if (g_Cracker[i].life < CRACKER_LIFE - CRACKER_MOVE_TIME_0)
			//	{
			//		g_morphingNum = 1;
			//	}
			//	else
			//	{
			//		g_morphingNum = 0;
			//	}


			//	// モーフィングモデルの番号調整
			//	switch (g_morphingNum)
			//	{
			//	case 0:
			//		after = g_Cracker[i].after;
			//		brfore = 0;

			//		break;

			//	case 1:
			//		after = 0;
			//		brfore = g_Cracker[i].after;

			//		break;
			//	}


			//	// モーフィング処理
			//	for (int p = 0; p < g_Cracker_Vertex[0].VertexNum; p++)
			//	{
			//		g_CrackerVertex[p].Position.x = g_Cracker_Vertex[after].VertexArray[p].Position.x - g_Cracker_Vertex[brfore].VertexArray[p].Position.x;
			//		g_CrackerVertex[p].Position.y = g_Cracker_Vertex[after].VertexArray[p].Position.y - g_Cracker_Vertex[brfore].VertexArray[p].Position.y;
			//		g_CrackerVertex[p].Position.z = g_Cracker_Vertex[after].VertexArray[p].Position.z - g_Cracker_Vertex[brfore].VertexArray[p].Position.z;

			//		g_CrackerVertex[p].Position.x *= g_time;
			//		g_CrackerVertex[p].Position.y *= g_time;
			//		g_CrackerVertex[p].Position.z *= g_time;

			//		g_CrackerVertex[p].Position.x += g_Cracker_Vertex[brfore].VertexArray[p].Position.x;
			//		g_CrackerVertex[p].Position.y += g_Cracker_Vertex[brfore].VertexArray[p].Position.y;
			//		g_CrackerVertex[p].Position.z += g_Cracker_Vertex[brfore].VertexArray[p].Position.z;
			//	}

			//	// 時間を進める
			//	switch (g_morphingNum)
			//	{
			//	case 0:
			//		if (g_time < 1.0f) g_time += 1.0f / CRACKER_MOVE_TIME_0;

			//		break;

			//	case 1:
			//		if ((g_time < 1.0f) && (g_Cracker[i].life < CRACKER_LIFE - CRACKER_MOVE_TIME_0 - CRACKER_MOVE_TIME_1))
			//		{
			//			g_time += 1.0f / CRACKER_MOVE_TIME_2;
			//			g_Cracker[i].shrink = TRUE;
			//		}
			//		break;
			//	}

			//	// 第一モーフィングが完了したら次のモーフィングへ
			//	if ((g_time >= 1.0f) && (g_morphingNum == 0))
			//	{
			//		g_time = 0.0f;
			//		g_morphingNum++;
			//	}

			//	// 第二モーフィングが終わった後はちょっと止まる
			//	if ((g_time >= 1.0f) && (g_morphingNum == 1) && (g_downCount < BLASE_DOWN_SPEED))
			//	{
			//		g_stopTime++;
			//	}

			//	if ((g_stopTime >= CRACKER_MOVE_TIME_3) && (g_downCount < BLASE_DOWN_SPEED))
			//	{
			//		g_Cracker[i].pos.y -= CRACKER_DOWN / BLASE_DOWN_SPEED;
			//		g_downCount++;

			//		g_cameraOn = FALSE;
			//	}
			//	
			//	// 落ちた後は床と一緒に奥へ移動する
			//	if (g_downCount >= BLASE_DOWN_SPEED)
			//	{
			//		g_Cracker[i].pos.z += FIELD_SPEED;
			//		g_Cracker[i].move = TRUE;
			//	}
			//		

			//	D3D11_SUBRESOURCE_DATA sd;
			//	ZeroMemory(&sd, sizeof(sd));
			//	sd.pSysMem = g_CrackerVertex;

			//	// 頂点バッファに値をセットする
			//	D3D11_MAPPED_SUBRESOURCE msr;
			//	GetDeviceContext()->Map(g_Cracker[0].model.VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
			//	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			//	// 全頂点情報を毎回上書きしているのはDX11ではこの方が早いからです
			//	memcpy(pVtx, g_CrackerVertex, sizeof(VERTEX_3D)*g_Cracker_Vertex[0].VertexNum);

			//	GetDeviceContext()->Unmap(g_Cracker[0].model.VertexBuffer, 0);
			//}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCracker(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;


	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// αテストを有効に
	SetAlphaTestEnable(TRUE);

	for (int i = 0; i < MAX_CRACKER; i++)
	{
		if (g_Cracker[i].use == TRUE)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Cracker[i].scl.x, g_Cracker[i].scl.y, g_Cracker[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Cracker[i].rot.x, g_Cracker[i].rot.y + XM_PI, g_Cracker[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Cracker[i].pos.x, g_Cracker[i].pos.y, g_Cracker[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Cracker[i].mtxWorld, mtxWorld);


			// モデル描画
			DrawModel(&g_Cracker[i].model);
		}
	}

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

	// αテストを無効に
	SetAlphaTestEnable(FALSE);

}

//=============================================================================
// 爆破の取得
//=============================================================================
CRACKER *GetCracker()
{
	return &g_Cracker[0];
}


//=============================================================================
// クラッカーの発生
//=============================================================================
void SetCracker(void)
{
	for (int i = 0; i < MAX_CRACKER; i++)
	{
		if (g_Cracker[i].use == FALSE)
		{
			// SEのセット
			PlaySound(SOUND_LABEL_SE_stickingSound01);

			g_Cracker[i].use = TRUE;
			//g_Cracker[i].move = FALSE;
			//g_Cracker[i].shrink = FALSE;

			//g_Cracker[i].pos = pos;
			ATTACKRANGE *AttackR = GetAttackR();
			
			g_Cracker[i].pos = AttackR->pos;
			g_Cracker[i].pos.y = CRACKER_OFFSET_Y;

			g_Cracker[i].rot.x = 0.0f;

			g_Cracker[i].life = CRACKER_LIFE;

			g_CrackerTime = CRACKER_MOVE_TIME;

			g_particleOn = FALSE;

			//g_Cracker[i].after = (rand() % (MAX_CRACKER_MOVE - 1)) + 1;

			g_cameraOn = TRUE;

			//g_time = 0.0f;
			//g_morphingNum = 0;
			//g_downCount = 0;
			//g_stopTime = 0;

			//g_Cracker[i].rot.y = RamdomFloat(2, CRACKER_ROT, -CRACKER_ROT);

			// 吹き出しを表示
			//SetBomSpeech();

			return;
		}
	}
}


//=============================================================================
// 爆発発生フラグを取得
//=============================================================================
//BOOL GetParticleOn(void)
//{
//	return g_particleOn;
//}

//=============================================================================
// カメラを切り替えるかどうかのスイッチを知る関数
//=============================================================================
//BOOL GetCameraSwitch(void)
//{
//	return g_cameraOn;
//}


//=============================================================================
// カメラを切り替えるかどうかのスイッチ
//=============================================================================
//void SetCameraSwitch(BOOL data)
//{
//	g_cameraOn = data;
//}


//=============================================================================
// 現在のモーフィング番号を取得
//=============================================================================
//int GetMorphing(void)
//{
//	return g_morphingNum;
//}


//=============================================================================
// 爆弾が空中で止まる時間を返す
//=============================================================================
//int GetStopTime(void)
//{
//	return g_stopTime;
//}