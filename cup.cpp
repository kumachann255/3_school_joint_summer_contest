//=============================================================================
//
// カップと当たり判定処理 [cup.cpp]
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
#include "cup.h"
#include "meshfield.h"
#include "speech.h"
#include "enemy.h"
#include "enemyHeli.h"
#include "tutorial.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_CUP			"data/MODEL/newneba00.obj"		// 読み込むモデル名
#define	MODEL_CUP_1			"data/MODEL/newneba01.obj"		// 読み込むモデル名
#define	MODEL_CUP_2			"data/MODEL/newneba02.obj"		// 読み込むモデル名
#define	MODEL_CUP_3			"data/MODEL/newneba03.obj"		// 読み込むモデル名
#define	MODEL_CUP_4			"data/MODEL/newneba04.obj"		// 読み込むモデル名


#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define CUP_OFFSET_Y		(12.0f)						// オブジェクトの足元をあわせる

#define CUP_LIFE			(150)						// 爆破オブジェクトの表示時間
#define CUP_MOVE_TIME_0	(3)								// ねばねばが広がる時間
#define CUP_MOVE_TIME_1	(10)							// 広がった状態で止まる時間
#define CUP_MOVE_TIME_2	(10)							// ねばねばが縮まる時間
#define CUP_MOVE_TIME_3	(20)							// ねばねばが空中で止まる時間

#define CUP_ROT_Y			(XM_PI * 0.04f)				// 回転量

#define CUP_SPEED_Z			(2.0f)						// 移動量(Z軸)
#define CUP_SPEED_X			(8.0f)						// 移動量(X軸)

#define CUP_RANGE_ELEA_X	(161.0f)					// カップの移動範囲

#define MAX_CUP_MOVE		(5)							// モーフィングの数



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CUP			g_Cup[MAX_CUP];				// 爆破オブジェクト

static BOOL				g_Load = FALSE;

static MODEL			g_Cup_Vertex[MAX_CUP_MOVE];		// モーフィング用モデルの頂点情報が入ったデータ配列
static VERTEX_3D		*g_CupVertex = NULL;			// 途中経過を記録する場所

static float			g_time;							// モーフィングの経過時間
static int				g_downCount;					// 落ち始めてどのくらい時間が経過したか

static int				g_morphingNum;					// モーフィングの番号

static BOOL				g_cameraOn;						// カメラのスイッチ

static int				g_stopTime;						// 爆弾が空中で止まる時間

static float			g_cupSpeed_Z;					// カップの移動スピード(Z軸)
static float			g_cupSpeed_X;					// カップの移動スピード(X軸)

static float			g_radius;						// エネミー回転用
static float			g_rot;							// エネミー回転用

static BOOL				g_particleOn;					// TRUE:爆発エフェクト発生

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCup(void)
{
	for (int i = 0; i < MAX_CUP; i++)
	{
		LoadModelMorphing(MODEL_CUP, &g_Cup[i].model);
		g_Cup[i].load = TRUE;

		g_Cup[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Cup[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Cup[i].scl = XMFLOAT3(1.3f, 1.3f, 1.3f);
		g_Cup[i].size = CUP_SIZE;
		g_Cup[i].life = 0;

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Cup[i].model, &g_Cup[i].diffuse[0]);

		g_Cup[i].use = FALSE;			// TRUE:生きてる
		//g_Cup[i].move = FALSE;		// TRUE:奥へ移動する
	
	}

	// モーフィングするオブジェクトの読み込み
	LoadObj(MODEL_CUP, &g_Cup_Vertex[0]);
	LoadObj(MODEL_CUP_1, &g_Cup_Vertex[1]);
	LoadObj(MODEL_CUP_2, &g_Cup_Vertex[2]);
	LoadObj(MODEL_CUP_3, &g_Cup_Vertex[3]);
	LoadObj(MODEL_CUP_4, &g_Cup_Vertex[4]);

	// 中身を配列として使用できるように仕様変更
	g_CupVertex = new VERTEX_3D[g_Cup_Vertex[0].VertexNum];

	// 差分(途中経過)の初期化
	for (int i = 0; i < g_Cup_Vertex[0].VertexNum; i++)
	{
		g_CupVertex[i].Position = g_Cup_Vertex[0].VertexArray[i].Position;
		g_CupVertex[i].Diffuse  = g_Cup_Vertex[0].VertexArray[i].Diffuse;
		g_CupVertex[i].Normal   = g_Cup_Vertex[0].VertexArray[i].Normal;
		g_CupVertex[i].TexCoord = g_Cup_Vertex[0].VertexArray[i].TexCoord;
	}

	g_morphingNum = 0;
	g_time = 0.0f;
	g_downCount = 0;
	g_cameraOn = FALSE;
	g_stopTime = 0;

	g_cupSpeed_Z = CUP_SPEED_Z;
	g_cupSpeed_X = CUP_SPEED_X;

	g_radius = 0.0f;
	g_rot	 = 0.0f;
	
	g_particleOn = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCup(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_CUP; i++)
	{
		if (g_Cup[i].load)
		{
			UnloadModel(&g_Cup[i].model);
			g_Cup[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateCup(void)
{
	for (int i = 0; i < MAX_CUP; i++)
	{
		g_particleOn = FALSE;		//エフェクトフラグをオフにする

		if (g_Cup[i].use == TRUE)			// この攻撃範囲が使われている？
		{									// Yes
			// 寿命を減らす
			g_Cup[i].life--;

			// 寿命が尽きたら未使用に
			if (g_Cup[i].life < 0)
			{
				g_Cup[i].use = FALSE;
			}

			// カップの移動処理
			{
				g_Cup[i].pos.z += g_cupSpeed_Z;
				g_Cup[i].pos.x += g_cupSpeed_X;
				g_Cup[i].rot.y += CUP_ROT_Y;


				if (g_Cup[i].pos.x <= -CUP_RANGE_ELEA_X || g_Cup[i].pos.x >= CUP_RANGE_ELEA_X)
				{
					g_cupSpeed_X *= -1;
				}
			}

			// 当たった後エネミーの挙動処理
			{
				ENEMY *enemy = GetEnemy();		// エネミーのポインターを初期化
				ENEMY_HELI *enemyheli = GetEnemyHeli();		// エネミーのポインターを初期化

				// 他エネミーの処理
				for (int j = 0; j < MAX_ENEMY; j++)
				{
					if (enemy[j].cupHit == TRUE)
					{
						// ランダム処理
						if (enemy[j].cupRot == TRUE)
						{
							enemy[j].rot.x = RamdomFloat(2, 6.28f, -6.28f);
							enemy[j].rot.z = RamdomFloat(2, 6.28f, -6.28f);
							enemy[j].pos.y += RamdomFloat(2, 50.0f, 20.0f);

							g_radius = RamdomFloat(2, 20.0f, 5.0f);
						
							enemy[j].cupRot = FALSE;
						}
						
						// 回転させる
						enemy[j].rot.y += CUP_ROT_Y;
						enemy[j].pos.x = g_Cup[i].pos.x + sinf(enemy[j].radian) * g_radius;
						enemy[j].pos.z = g_Cup[i].pos.z + cosf(enemy[j].radian) * g_radius;

						enemy[j].radian += CUP_ROT_Y;

						// エネミーを倒す
						if (g_Cup[0].use == FALSE)
						{
							enemy[j].use = FALSE;
							SetTutorialEnemy(TRUE);
							g_particleOn = TRUE;
						}

					}
				}
			
				// ヘリの処理
				for (int j = 0; j < MAX_ENEMY_HELI; j++)
				{
					if (enemyheli[j].cupHit == TRUE)
					{
						// ランダム処理
						if (enemyheli[j].cupRot == TRUE)
						{
							enemyheli[j].rot.x = RamdomFloat(2, 6.28f, -6.28f);
							enemyheli[j].rot.z = RamdomFloat(2, 6.28f, -6.28f);
							enemyheli[j].pos.y += RamdomFloat(2, 50.0f, 20.0f);

							g_radius = RamdomFloat(2, 20.0f, 5.0f);

							enemyheli[j].cupRot = FALSE;
						}

						// 回転させる
						enemyheli[j].rot.y += CUP_ROT_Y;
						enemyheli[j].pos.x = g_Cup[i].pos.x + sinf(enemyheli[j].radian) * g_radius;
						enemyheli[j].pos.z = g_Cup[i].pos.z + cosf(enemyheli[j].radian) * g_radius;

						enemyheli[j].radian += CUP_ROT_Y;

						// エネミーを倒す
						if (g_Cup[0].use == FALSE)
						{
							enemyheli[j].use = FALSE;
							//SetTutorialEnemy(TRUE);
							g_particleOn = TRUE;
						}

					}
				}

			}


			//// ねばねばのモーフィングの処理
			//{
			//	int after, brfore;

			//	// 広がり終えたら縮まる方向へモーフィング
			//	if (g_Cup[i].life < CUP_LIFE - CUP_MOVE_TIME_0)
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
			//		after = g_Cup[i].after;
			//		brfore = 0;

			//		break;

			//	case 1:
			//		after = 0;
			//		brfore = g_Cup[i].after;

			//		break;
			//	}


			//	// モーフィング処理
			//	for (int p = 0; p < g_Cup_Vertex[0].VertexNum; p++)
			//	{
			//		g_CupVertex[p].Position.x = g_Cup_Vertex[after].VertexArray[p].Position.x - g_Cup_Vertex[brfore].VertexArray[p].Position.x;
			//		g_CupVertex[p].Position.y = g_Cup_Vertex[after].VertexArray[p].Position.y - g_Cup_Vertex[brfore].VertexArray[p].Position.y;
			//		g_CupVertex[p].Position.z = g_Cup_Vertex[after].VertexArray[p].Position.z - g_Cup_Vertex[brfore].VertexArray[p].Position.z;

			//		g_CupVertex[p].Position.x *= g_time;
			//		g_CupVertex[p].Position.y *= g_time;
			//		g_CupVertex[p].Position.z *= g_time;

			//		g_CupVertex[p].Position.x += g_Cup_Vertex[brfore].VertexArray[p].Position.x;
			//		g_CupVertex[p].Position.y += g_Cup_Vertex[brfore].VertexArray[p].Position.y;
			//		g_CupVertex[p].Position.z += g_Cup_Vertex[brfore].VertexArray[p].Position.z;
			//	}

			//	// 時間を進める
			//	switch (g_morphingNum)
			//	{
			//	case 0:
			//		if (g_time < 1.0f) g_time += 1.0f / CUP_MOVE_TIME_0;

			//		break;

			//	case 1:
			//		if ((g_time < 1.0f) && (g_Cup[i].life < CUP_LIFE - CUP_MOVE_TIME_0 - CUP_MOVE_TIME_1))
			//		{
			//			g_time += 1.0f / CUP_MOVE_TIME_2;
			//			g_Cup[i].shrink = TRUE;
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

			//	if ((g_stopTime >= CUP_MOVE_TIME_3) && (g_downCount < BLASE_DOWN_SPEED))
			//	{
			//		g_Cup[i].pos.y -= CUP_DOWN / BLASE_DOWN_SPEED;
			//		g_downCount++;

			//		g_cameraOn = FALSE;
			//	}
			//	
			//	// 落ちた後は床と一緒に奥へ移動する
			//	if (g_downCount >= BLASE_DOWN_SPEED)
			//	{
			//		g_Cup[i].pos.z += FIELD_SPEED;
			//		g_Cup[i].move = TRUE;
			//	}
			//		

			//	D3D11_SUBRESOURCE_DATA sd;
			//	ZeroMemory(&sd, sizeof(sd));
			//	sd.pSysMem = g_CupVertex;

			//	// 頂点バッファに値をセットする
			//	D3D11_MAPPED_SUBRESOURCE msr;
			//	GetDeviceContext()->Map(g_Cup[0].model.VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
			//	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			//	// 全頂点情報を毎回上書きしているのはDX11ではこの方が早いからです
			//	memcpy(pVtx, g_CupVertex, sizeof(VERTEX_3D)*g_Cup_Vertex[0].VertexNum);

			//	GetDeviceContext()->Unmap(g_Cup[0].model.VertexBuffer, 0);
			//}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCup(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;


	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// αテストを有効に
	SetAlphaTestEnable(TRUE);

	for (int i = 0; i < MAX_CUP; i++)
	{
		if (g_Cup[i].use == TRUE)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Cup[i].scl.x, g_Cup[i].scl.y, g_Cup[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Cup[i].rot.x, g_Cup[i].rot.y + XM_PI, g_Cup[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Cup[i].pos.x, g_Cup[i].pos.y, g_Cup[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Cup[i].mtxWorld, mtxWorld);


			// モデル描画
			DrawModel(&g_Cup[i].model);
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
CUP *GetCup()
{
	return &g_Cup[0];
}


//=============================================================================
// カップの発生
//=============================================================================
void SetCup(void)
{
	for (int i = 0; i < MAX_CUP; i++)
	{
		if (g_Cup[i].use == FALSE)
		{
			// SEのセット
			PlaySound(SOUND_LABEL_SE_stickingSound01);

			g_Cup[i].use = TRUE;
			//g_Cup[i].move = FALSE;
			//g_Cup[i].shrink = FALSE;

			//g_Cup[i].pos = pos;
			ATTACKRANGE *AttackR = GetAttackR();
			
			g_Cup[i].pos = AttackR->pos;
			g_Cup[i].pos.y = CUP_OFFSET_Y;
			
			g_Cup[i].rot.y = 0.0f;

			g_Cup[i].life = CUP_LIFE;

			g_particleOn = FALSE;

			//g_Cup[i].after = (rand() % (MAX_CUP_MOVE - 1)) + 1;

			g_cameraOn = TRUE;

			//g_time = 0.0f;
			//g_morphingNum = 0;
			//g_downCount = 0;
			//g_stopTime = 0;

			//g_Cup[i].rot.y = RamdomFloat(2, CUP_ROT, -CUP_ROT);

			// 吹き出しを表示
			//SetBomSpeech();

			return;
		}
	}
}


//=============================================================================
// 爆発発生フラグを取得
//=============================================================================
BOOL GetParticleOn(void)
{
	return g_particleOn;
}

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