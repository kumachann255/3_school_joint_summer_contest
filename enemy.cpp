//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 熊澤義弘＋エナ 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "blast.h"
#include "sea_meshfield.h"
#include "shadow.h"
#include "collision.h"
#include "damageEF.h"
#include "debugproc.h"
#include "player.h"
#include "tako.h"
#include "target.h"
#include "fade.h"
#include "tutorial.h"
#include "timeUI.h"
#include "target.h"
#include "targetObj.h"
#include "rockOn.h"

#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY_00			"data/MODEL/enemy00.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_01			"data/MODEL/enemy01.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_02			"data/MODEL/enemy02.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_03			"data/MODEL/enemy03.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_04			"data/MODEL/enemy04.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_05			"data/MODEL/enemy05.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_06			"data/MODEL/enemy06.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_07			"data/MODEL/enemy07.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_08			"data/MODEL/enemy08.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_COLLISION	"data/MODEL/collisionBox.obj"	// 読み込むモデル名


#define ENEMY_TYPE_MAX		(9)							// エネミータイプの最大数

#define	VALUE_MOVE			(3.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ

#define ENEMY_POP_Z			(500.0f)					// エネミーの初期ポップ位置(z座標)
#define ENEMY_POP_X			(250)						// エネミーの初期ポップの範囲(x座標)

#define ENEMY_GOAL_Z		(70.0f)						// エネミーのゴール基準位置(z座標)
#define ENEMY_GOAL_Z_OFFSET	(60)						// エネミーのゴール位置の乱数

#define STAGE_TUTORIAL_MAX_POP	(1)						// 最大、場に何体エネミーを出すか

#define STAGE0_POP_COUNT	(100)						// エネミーのポップ間隔
#define STAGE0_MAX_POP		(20)						// 最大、場に何体エネミーを出すか

#define STAGE1_POP_COUNT	(70)						// エネミーのポップ間隔
#define STAGE1_MAX_POP		(25)						// 最大、場に何体エネミーを出すか

#define STAGE2_POP_COUNT	(50)						// エネミーのポップ間隔
#define STAGE2_MAX_POP		(30)						// 最大、場に何体エネミーを出すか

#define STAGE3_POP_COUNT	(30)						// エネミーのポップ間隔
#define STAGE3_MAX_POP		(45)						// 最大、場に何体エネミーを出すか


#define ENEMY_HIT_MOVE		(5.0f)						// 当たり判定後アニメーション用移動量

#define ENEMY_ATTACK_0		(300)						// エネミーが点滅するまでの時間
#define ENEMY_ATTACK_1		(120 + ENEMY_ATTACK_0)		// 点滅が早くなるまでの時間
#define ENEMY_ATTACK_2		(120 + ENEMY_ATTACK_1)		// 攻撃するまでの時間

#define ENEMY_BLINKING0		(50)						// 点滅の間隔
#define ENEMY_BLINKING1		(14)						// 点滅の間隔

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetEnemy(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー
static ENEMY			g_Collision[MAX_ENEMY];			// 当たり判定用のボックス

static VERTEX_3D		*g_Vertex = NULL;				// 
static MODEL			g_Collision_Vertex;	// 

static int				g_Stage;

static BOOL				g_Load = FALSE;

static int				count = 0;		// ポップカウント



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	LoadModel(MODEL_ENEMY_00, &g_Enemy[0].model);
	LoadModel(MODEL_ENEMY_01, &g_Enemy[1].model);
	LoadModel(MODEL_ENEMY_02, &g_Enemy[2].model);
	LoadModel(MODEL_ENEMY_03, &g_Enemy[3].model);
	LoadModel(MODEL_ENEMY_04, &g_Enemy[4].model);
	LoadModel(MODEL_ENEMY_05, &g_Enemy[5].model);
	LoadModel(MODEL_ENEMY_06, &g_Enemy[6].model);
	LoadModel(MODEL_ENEMY_07, &g_Enemy[7].model);
	LoadModel(MODEL_ENEMY_08, &g_Enemy[8].model);

	LoadModel(MODEL_ENEMY_COLLISION, &g_Collision[0].model);

	for (int i = 0; i < MAX_ENEMY; i++)
	{	
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Collision[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);

		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Collision[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ
		g_Enemy[i].target = FALSE;

			// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);
		GetModelDiffuse(&g_Collision[i].model, &g_Collision[i].diffuse[0]);

		//XMFLOAT3 pos = g_Enemy[i].pos;
		//pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		//g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].use = FALSE;			// TRUE:生きてる
		g_Collision[i].use = FALSE;			// TRUE:生きてる

		g_Enemy[i].hitPos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 0.0f);	// 爆発の中心
		g_Enemy[i].hitRot = XMFLOAT3(0.0f, 0.0f, 0.0f);				// 当たり判定後アニメーション用スピード
		g_Enemy[i].isHit = FALSE;									// TRUE:当たってる
		g_Enemy[i].hitTime = 0;										// タイミング管理用
		g_Enemy[i].liveCount = 0;		// 生存時間をリセット
		g_Enemy[i].type = 0;			// エネミータイプ

		g_Enemy[i].cupHit = FALSE;									// TRUE:当たってる
		g_Enemy[i].cupRot = FALSE;									// TRUE:当たってる
		g_Enemy[i].sameHit = FALSE;									// TRUE:当たってる
		g_Enemy[i].sameRot = FALSE;									// TRUE:当たってる
		g_Enemy[i].takoHit = FALSE;									// TRUE:当たってる
		g_Enemy[i].takoRot = FALSE;									// TRUE:当たってる

		g_Enemy[i].radian = 0.0f;									// 回転量

		g_Enemy[i].fuchi = FALSE;


	}

	LoadObj(MODEL_ENEMY_COLLISION, &g_Collision_Vertex);

	g_Vertex = new VERTEX_3D[g_Collision_Vertex.VertexNum];

	// 初期化
	for (int i = 0; i < g_Collision_Vertex.VertexNum; i++)
	{
		g_Vertex[i].Position = g_Collision_Vertex.VertexArray[i].Position;
		g_Vertex[i].Diffuse = g_Collision_Vertex.VertexArray[i].Diffuse;
		g_Vertex[i].Normal = g_Collision_Vertex.VertexArray[i].Normal;
		g_Vertex[i].TexCoord = g_Collision_Vertex.VertexArray[i].TexCoord;
	}


	g_Stage = GetStage();

	count = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	//return;

	
	{	// pop処理
		count++;
		int useCount = 0;

		switch (g_Stage)
		{
		case stage0:
			// 今何体出現しているかを確認
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].use == TRUE) useCount++;
			}

			// 時間経過とエネミーの出現数次第でポップするか判断
			if ((count % STAGE0_POP_COUNT == 0) && (useCount < STAGE0_MAX_POP))
			{
				SetEnemy();
			}

			break;

		case stage1:
			// 今何体出現しているかを確認
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].use == TRUE) useCount++;
			}

			// 時間経過とエネミーの出現数次第でポップするか判断
			if ((count % STAGE1_POP_COUNT == 0) && (useCount < STAGE1_MAX_POP))
			{
				SetEnemy();
			}

			break;

		case tutorial:
			// 今何体出現しているかを確認
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].use == TRUE) useCount++;
			}

			// 時間経過とエネミーの出現数次第でポップするか判断
			if ((count % STAGE2_POP_COUNT == 0) && (useCount < STAGE_TUTORIAL_MAX_POP))
			{
				SetEnemy();
			}

			break;
		}
	}


	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			// 生存時間をカウント
			if(g_Stage != tutorial) g_Enemy[i].liveCount++;

			// 攻撃を食らっていなけらば攻撃処理
			if (g_Enemy[i].isHit == FALSE)
			{
				// 攻撃処理
				if (g_Enemy[i].liveCount > ENEMY_ATTACK_2)
				{	// 攻撃を行う
					// 生存時間をリセット
					g_Enemy[i].liveCount = 0;

					// リムライトオフ
					g_Enemy[i].fuchi = FALSE;


					// 攻撃
					SetDamageEF(TRUE);
					// SetCameraShake(20);
					SetPlayerDamage(1);

				}
				else if(g_Enemy[i].liveCount > ENEMY_ATTACK_1)
				{	// 赤い点滅が早くなる
				
					if (g_Enemy[i].liveCount % ENEMY_BLINKING1 < ENEMY_BLINKING1 / 2)
					{	// オブジェクトを赤くする

						// リムライトオン
						g_Enemy[i].fuchi = TRUE;
					}
					else
					{	// オブジェクトの色を戻す

						// リムライトオフ
						g_Enemy[i].fuchi = FALSE;
					}


				}
				else if (g_Enemy[i].liveCount > ENEMY_ATTACK_0)
				{	// 赤く点滅する

					if (g_Enemy[i].liveCount % ENEMY_BLINKING0 < ENEMY_BLINKING0 / 2)
					{	// オブジェクトを赤くする

						// リムライトオン
						g_Enemy[i].fuchi = TRUE;
					}
					else
					{	// オブジェクトの色を戻す

						// リムライトオフ
						g_Enemy[i].fuchi = FALSE;
					}
				}
			}


			// 目標地点まで到達していない場合に移動処理
			if (g_Enemy[i].pos.z > g_Enemy[i].zGoal)
			{
				BOOL ans = TRUE;
				// 他のパトカーと当たっていないかを確認
				for (int p = 0; p < MAX_ENEMY; p++)
				{
					//敵の有効フラグをチェックする
					if ((g_Enemy[p].use == FALSE) || (i == p) || (g_Enemy[p].isHit == TRUE)) continue;

					//BCの当たり判定
					if (CollisionBC(g_Enemy[i].pos, g_Enemy[p].pos, g_Enemy[p].size, g_Enemy[p].size) &&
						(g_Enemy[i].pos.z > g_Enemy[p].pos.z))
					{	// 当たっていない場合に移動

						ans = FALSE;
						break;
					}
				}

				if ((ans) && (GetTime() > 1))
				{
					g_Enemy[i].pos.z -= VALUE_MOVE;
				}
				else if ((ans) && (GetTime() <= 1))
				{
					g_Enemy[i].pos.z += VALUE_MOVE;
				}
			}


			// 当たり判定ようのボックスと位置を共有する
			g_Collision[i].pos = g_Enemy[i].pos;

			//=======================================================================
			// ↓今回は当たった後のエネミーの挙動は攻撃オブジェクト側で行いましょう！
			//=======================================================================

			// エネミーの消去アニメーション
			if (g_Enemy[i].isHit == TRUE)				// 攻撃が当たってるか？
			{											// Yes
				//BOOL ans = TRUE;

				// SEの停止
				StopSound(SOUND_LABEL_SE_siren01);

				// 色を元に戻す
				g_Enemy[i].fuchi = FALSE;

				BLAST *blast = GetBlast();		// 爆破オブジェクトの初期化

				// 縮まる処理
				if ((blast[0].shrink) && (g_Enemy[i].hitTime > 0))
				{
					g_Enemy[i].pos.x += (g_Enemy[i].hitPos.x - g_Enemy[i].pos.x) / ENEMY_HIT_MOVE;
					g_Enemy[i].pos.y += (g_Enemy[i].hitPos.y - g_Enemy[i].pos.y) / ENEMY_HIT_MOVE;
					g_Enemy[i].pos.z += (g_Enemy[i].hitPos.z - g_Enemy[i].pos.z) / ENEMY_HIT_MOVE;

					// ランダムに回転させる
					g_Enemy[i].hitRot.x = RamdomFloat(2, 6.28f, -6.28f);
					g_Enemy[i].hitRot.y = RamdomFloat(2, 6.28f, -6.28f);
					g_Enemy[i].hitRot.z = RamdomFloat(2, 6.28f, -6.28f);

					if (g_Enemy[i].hitTime == 15)
					{
						g_Enemy[i].rot.x += g_Enemy[i].hitRot.x;
						g_Enemy[i].rot.y += g_Enemy[i].hitRot.y;
						g_Enemy[i].rot.z += g_Enemy[i].hitRot.z;
					}

					g_Enemy[i].hitTime--;
				}
				

				// 爆弾と一緒に落下する
				BOOL camera = GetCameraSwitch();

				if (camera == FALSE && blast[0].move == FALSE)
				{
					g_Enemy[i].pos.y -= BLAST_DOWN / BLASE_DOWN_SPEED;

					if (g_Enemy[i].pos.y < ENEMY_OFFSET_Y)
					{
						g_Enemy[i].pos.y = ENEMY_OFFSET_Y;
					}
				}
				
				//爆弾と一緒に奥へ移動する
				if (blast[0].move == TRUE) /*&& (g_Enemy[i].move == TRUE)*/ //&& (g_Enemy[i].hitTime == 0))
				{
					g_Enemy[i].pos.z += FIELD_SPEED+ 2.0f;
				}


				if (blast[0].use == FALSE)
				{
					g_Enemy[i].use = FALSE;
					g_Collision[i].use = FALSE;
				}
			}

			CAMERA *camera = GetCamera();
			TARGETOBJ *targetObj = GetTargetObj();
			TARGET *target = GetTarget();

			// レイキャストして足元の高さを求める
			XMFLOAT3 hitPosition;								// 交点
			hitPosition = g_Enemy[i].pos;	// 外れた時用に初期化しておく
			bool ans = RayHitEnemy(targetObj[0].pos, camera->pos, &hitPosition, i);

			if ((ans) && (!g_Enemy[i].target) && (g_Enemy[i].use))
			{
				g_Enemy[i].target = TRUE;
				g_Collision[i].use = FALSE;
				target[0].enemyNum[target[0].count] = i;
				target[0].count++;

				SetRockOn();
			}
			////////////////////////////////////////////////////////////////////////
			//// 姿勢制御
			////////////////////////////////////////////////////////////////////////

			//XMVECTOR vx, nvx, up;
			//XMVECTOR quat;
			//float len, angle;

			//// ２つのベクトルの外積を取って任意の回転軸を求める
			//g_Enemy[i].upVector = normal;
			//up = { 0.0f, 1.0f, 0.0f, 0.0f };
			//vx = XMVector3Cross(up, XMLoadFloat3(&g_Enemy[i].upVector));

			//// 求めた回転軸からクォータニオンを作り出す
			//nvx = XMVector3Length(vx);
			//XMStoreFloat(&len, nvx);
			//nvx = XMVector3Normalize(vx);
			//angle = asinf(len);
			//quat = XMQuaternionRotationNormal(nvx, angle);

			//// 前回のクォータニオンから今回のクォータニオンまでの回転を滑らかにする
			//quat = XMQuaternionSlerp(XMLoadFloat4(&g_Enemy[i].quaternion), quat, 0.05f);

			//// 今回のクォータニオンの結果を保存する
			//XMStoreFloat4(&g_Enemy[i].quaternion, quat);

			//// 影もプレイヤーの位置に合わせる
			//XMFLOAT3 pos = g_Enemy[i].pos;
			//pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			//SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// フォグ有効
	SetFogEnable(TRUE);

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// リムライトの設定
		SetFuchi(g_Enemy[i].fuchi);

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// クォータニオンを反映
		XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Enemy[i].quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// モデル描画
		switch (g_Enemy[i].type)
		{
		case 0:
			DrawModel(&g_Enemy[0].model);
			break;

		case 1:
			DrawModel(&g_Enemy[1].model);
			break;
		
		case 2:
			DrawModel(&g_Enemy[2].model);
			break;
		
		case 3:
			DrawModel(&g_Enemy[3].model);
			break;
		
		case 4:
			DrawModel(&g_Enemy[4].model);
			break;
		
		case 5:
			DrawModel(&g_Enemy[5].model);
			break;
		
		case 6:
			DrawModel(&g_Enemy[6].model);
			break;
		
		case 7:
			DrawModel(&g_Enemy[7].model);
			break;
		
		case 8:
			DrawModel(&g_Enemy[8].model);
			break;
		}

#ifdef _DEBUG	// デバッグ時のみ表示

		// コリジョン用のボックスの描画
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Collision[i].scl.x, g_Collision[i].scl.y, g_Collision[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Collision[i].rot.x, g_Collision[i].rot.y + XM_PI, g_Collision[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// クォータニオンを反映
		quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Collision[i].quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Collision[i].pos.x, g_Collision[i].pos.y, g_Collision[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);
		XMStoreFloat4x4(&g_Collision[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Collision[0].model);


		// リムライトの設定
		SetFuchi(FALSE);
#endif

	}
	
	// フォグ有効
	SetFogEnable(FALSE);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}


//=============================================================================
// エネミーの発生
//=============================================================================
void SetEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE)
		{
			//SetSourceVolume(SOUND_LABEL_SE_carHorn01, 1.0f);
			// SEのセット
			PlaySound(SOUND_LABEL_SE_siren01);

			g_Enemy[i].use = TRUE;
			g_Enemy[i].pos.z = ENEMY_POP_Z;
			g_Enemy[i].pos.y = ENEMY_OFFSET_Y;
			g_Enemy[i].isHit = FALSE;
			g_Enemy[i].cupHit = FALSE;
			g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

			// x座標はランダム
			g_Enemy[i].pos.x = (float)(rand() % ENEMY_POP_X) - ((float)ENEMY_POP_X / 2.0f);
			if (g_Stage == tutorial) g_Enemy[i].pos.x = 0.0f;

			// 到達地点もランダム
			g_Enemy[i].zGoal = (float)(rand() % ENEMY_GOAL_Z_OFFSET) + ENEMY_GOAL_Z;
			if (g_Stage == tutorial) g_Enemy[i].zGoal = 150.0f;

			g_Enemy[i].liveCount = 0;

			// エネミーのタイプをランダムに
			g_Enemy[i].type = rand() % ENEMY_TYPE_MAX;

			// リムライトオフ
			g_Enemy[i].fuchi = FALSE;

			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE * 2.0f, ENEMY_SHADOW_SIZE * 2.0f);


			g_Collision[i].use = TRUE;
			g_Collision[i].pos = g_Enemy[i].pos;



			return;
		}
	}
}


BOOL RayHitEnemy(XMFLOAT3 Pos, XMFLOAT3 CameraPos, XMFLOAT3 *HitPosition, int num)
{
	XMFLOAT3 start = CameraPos;
	XMFLOAT3 end = Pos;
	XMFLOAT3 org = *HitPosition;

	// 必要数分検索を繰り返す
	for (int i = 0; i < g_Collision_Vertex.VertexNum - 2; i++)
	{
		XMFLOAT3 p0 = g_Vertex[i].Position;
		p0.x += g_Collision[num].pos.x;
		p0.y += g_Collision[num].pos.y;
		p0.z += g_Collision[num].pos.z;

		XMFLOAT3 p1 = g_Vertex[i + 1].Position;
		p1.x += g_Collision[num].pos.x;
		p1.y += g_Collision[num].pos.y;
		p1.z += g_Collision[num].pos.z;

		XMFLOAT3 p2 = g_Vertex[i + 2].Position;
		p2.x += g_Collision[num].pos.x;
		p2.y += g_Collision[num].pos.y;
		p2.z += g_Collision[num].pos.z;

		// 三角ポリゴンだから２枚分の当たり判定
		BOOL ans = RayCast(p0, p2, p1, start, end, HitPosition, &g_Vertex[i].Normal);
		if (ans)
		{
			return TRUE;
		}
	}

	// 何処にも当たっていなかった時
	*HitPosition = org;
	return FALSE;
}


// ロックオンされているかの初期化
// 攻撃したときに呼び出される
void ResetEnemyTarget(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		g_Enemy[i].target = FALSE;
	}
}

// スクリーン座標をワールド座標へ変換
//void SetScreenToWorld(void)
//{
//	TARGET *target = GetTarget();
//	float sx, sy;
//	float z = 1.0f;
//	XMFLOAT3 ans;
//	XMMATRIX *view;
//	XMMATRIX *proj;
//
//	D3D11_VIEWPORT vp;
//	vp.Width = (FLOAT)SCREEN_WIDTH;
//	vp.Height = (FLOAT)SCREEN_HEIGHT;
//	vp.MinDepth = 0.0f;
//	vp.MaxDepth = 1.0f;
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//
//	ans.x = sx = target->pos.x;
//	ans.y = sy = target->pos.y;
//	ans.z = z;
//
//	XMMATRIX invMat, inv_proj, inv_view;
//	XMStoreFloat4x4(&g_Camera.mtxProjection, mtxProjection);
//
//	XMMatrixInverse(vp, invMat);
//}

