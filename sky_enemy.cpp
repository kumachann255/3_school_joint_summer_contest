//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include <time.h>
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "sky_enemy.h"
#include "shadow.h"
#include "damageEF.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"
#include "collision.h"
#include "target.h"
#include "targetObj.h"
#include "rockOn.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_SKY_ENEMY_W	"data/MODEL/enemy04.obj"			// エネミー白
#define	MODEL_SKY_ENEMY_B	"data/MODEL/enemy05.obj"			// エネミー黒
#define MODEL_SKY_ENEMY_D	"data/MODEL/enemy09.obj"			// なんかちょっと怖いやつ 

#define	MODEL_ENEMY_COLLISION	"data/MODEL/collisionBox.obj"	// 読み込むモデル名

#define	VALUE_MOVE				(2.0f)							// 移動量
#define	VALUE_ROTATE			(XM_PI * 0.02f)					// 回転量

#define SKY_ENEMY_SHADOW_SIZE	(1.0f)							// 影の大きさ
#define SKY_ENEMY_OFFSET_Y		(30.0f)							// エネミーの足元をあわせる

#define SKY_ENEMY_PARTS_MAX		(2)								// エネミーのパーツの数

#define STAGE0_POP_COUNT	(80)						// エネミーのポップ間隔
#define STAGE0_MAX_POP		(40)						// 最大、場に何体エネミーを出すか

#define ENEMY_HIT_MOVE		(5.0f)						// 当たり判定後アニメーション用移動量

#define ENEMY_ATTACK_0		(300)						// エネミーが点滅するまでの時間
#define ENEMY_ATTACK_1		(120 + ENEMY_ATTACK_0)		// 点滅が早くなるまでの時間
#define ENEMY_ATTACK_2		(120 + ENEMY_ATTACK_1)		// 攻撃するまでの時間

#define ENEMY_BLINKING0		(50)						// 点滅の間隔
#define ENEMY_BLINKING1		(14)						// 点滅の間隔

#define ENEMY_ANGLE_SHRINK		(1.5f)					// 相手が近づいてくる速度
#define ENEMY_DISTANCE_CIRCLE	(400.0f)				// 近づいてくる限界
#define ENEMY_SHINK_SPEED_Y		(0.01f)					// プレイヤーの周りを回転する際の回転速度(Y軸)
#define ENEMY_SHINK_SPEED_X		(0.01f)					// プレイヤーの周りを回転する際の回転速度(X軸)

#define ENEMY_POP_DEISTANCE			(1000)				// エネミーのポップする距離(ベース)
#define ENEMY_POP_DEISTANCE_RAND	(300)				// エネミーのポップする距離(乱数)

#define CIRCLE_INSIDE_MIN				(200.0f)	// エネミーがサークル１の内側入ってプレイヤーに近づく限界値

#define ENEMY_POP_Y_RAND			(100.0f)			// エネミーがポップする位置の乱数



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static SKY_ENEMY		g_SkyEnemy[MAX_SKY_ENEMY];						// プレイヤー
static ENEMY			g_Collision[MAX_SKY_ENEMY];			// 当たり判定用のボックス

static SKY_ENEMY		g_Parts[MAX_SKY_ENEMY][SKY_ENEMY_PARTS_MAX];		// プレイヤーのパーツ用

static VERTEX_3D		*g_Vertex = NULL;				// 
static MODEL			g_Collision_Vertex;	// 

static BOOL		g_Load = FALSE;

static int g_count;

static int g_Stage;



// プレイヤーの階層アニメーションデータ
// プレイヤーの頭を左右に動かしているアニメデータ
static INTERPOLATION_DATA move_tbl_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSkyEnemy(void)
{
	LoadModel(MODEL_ENEMY_COLLISION, &g_Collision[0].model);

	LoadModel(MODEL_SKY_ENEMY_W, &g_SkyEnemy[0].model);
	LoadModel(MODEL_SKY_ENEMY_B, &g_SkyEnemy[1].model);
	LoadModel(MODEL_SKY_ENEMY_D, &g_SkyEnemy[2].model);

	for (int i = 0; i < MAX_SKY_ENEMY; i++)
	{

		g_SkyEnemy[i].load = TRUE;

		g_SkyEnemy[i].radius1 = (float)(150 + rand() % 250);					// サークル１の半径	rand関数はint型なのでfloat型にキャストしている

		g_SkyEnemy[i].angle1 = (float)(rand() % 360) * (XM_2PI / 360);		// サークル１の角度	rand関数はint型なのでfloat型にキャストしている
		g_SkyEnemy[i].angle2 = (float)(rand() % 360) * (XM_2PI / 360);		// サークル２の角度 rand関数はint型なのでfloat型にキャストしている


		// エネミーが出現する初期位置
		g_SkyEnemy[i].pos = { 0.0f, 0.0f, 0.0f };
		g_SkyEnemy[i].rot = { 0.0f, 0.0f, 0.0f };
		g_SkyEnemy[i].scl = { 1.0f, 1.0f, 1.0f };

		// 一定範囲内に入った際の回転速度の初期化
		g_SkyEnemy[i].circle1_spd = RamdomFloat(2, ENEMY_SHINK_SPEED_Y, -ENEMY_SHINK_SPEED_Y);
		g_SkyEnemy[i].circle2_spd = RamdomFloat(2, ENEMY_SHINK_SPEED_X, -ENEMY_SHINK_SPEED_X);


		g_SkyEnemy[i].size = SKY_ENEMY_SIZE;	// 当たり判定の大きさ
		g_SkyEnemy[i].use = FALSE;
		g_SkyEnemy[i].spawn = 0;
		g_SkyEnemy[i].move_count = 0;
		g_SkyEnemy[i].EnemyType = 0;
		g_SkyEnemy[i].target = FALSE;

		g_SkyEnemy[i].particleOn = FALSE;
		g_SkyEnemy[i].enemyPop = FALSE;

		g_Collision[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Collision[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		GetModelDiffuse(&g_Collision[i].model, &g_Collision[i].diffuse[0]);
		g_Collision[i].use = FALSE;			// TRUE:生きてる


		// 階層アニメーション用の初期化処理
		g_SkyEnemy[i].parent = NULL;			// 本体（親）なのでNULLを入れる
		g_Parts[i][0].parent = &g_SkyEnemy[i];		// 親をセット
		g_Parts[i][1].parent = &g_SkyEnemy[i];		// 親をセット

		g_Load = TRUE;
	}

	g_Stage = GetStage();

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

	return S_OK;

}



//=============================================================================
// 終了処理
//=============================================================================
void UninitSkyEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_SKY_ENEMY; i++)
	{
		// モデルの解放処理
		if (g_SkyEnemy[i].load)
		{
			UnloadModel(&g_SkyEnemy[i].model);
			g_SkyEnemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSkyEnemy(void)
{

	CAMERA *cam = GetCamera();

	g_count++;
	int useCount = 0;

	// 今何体出現しているかを確認
	for (int i = 0; i < MAX_SKY_ENEMY; i++)
	{
		if (g_SkyEnemy[i].use == TRUE) useCount++;

	}

	// 時間経過とエネミーの出現数次第でポップするか判断
	if ((g_count % STAGE0_POP_COUNT == 0) && (useCount < STAGE0_MAX_POP))
	{

		SetSkyEnemy();
	}


	for (int i = 0; i < MAX_SKY_ENEMY; i++)
	{
		g_SkyEnemy[i].move_count++;


		// 1つ目のサークルの円運動
		XMFLOAT3 first_circle;

		first_circle.x = sinf(g_SkyEnemy[i].angle1) * g_SkyEnemy[i].radius2;		// 1つ目の円に2つ目の円を合成　円運動をする基点（中心点）＊2つ目の円運動の半径
		first_circle.z = cosf(g_SkyEnemy[i].angle1) * g_SkyEnemy[i].radius2;		// 1つ目の円に2つ目の円を合成　円運動をする基点（中心点）＊2つ目の円運動の半径



		// 2つ目のサークルの円運動
		XMFLOAT3 second_circle;
		g_SkyEnemy[i].radius2 = g_SkyEnemy[i].radius1 + cosf(g_SkyEnemy[i].angle2) * 50;		// 現在の位置からのサークル２の半径を求める

		second_circle.y = sinf(g_SkyEnemy[i].angle2) * 50;								// サインカーブ　＊　この半径を元に


		// サークル1の範囲外にいる場合
		if (g_SkyEnemy[i].radius1 >= ENEMY_DISTANCE_CIRCLE)					// エネミーがサークル１の外周よりも遠くにいたとき
		{
			g_SkyEnemy[i].radius1 -= ENEMY_ANGLE_SHRINK;					// サークル1に向かって一直線に来る　エネミーのポップする位置はワールド座標の原点を中心として三角関数で管理しているので、半径を縮めていけばサークル１に向かってくる
		}
		else if (g_SkyEnemy[i].radius1 > CIRCLE_INSIDE_MIN)				// エネミーがサークル1の内側（ここから（CIRCLE_INSIDE_MIN）～ここまで（ENEMY_DISTANCE_CIRCLE））に入ってきた場合
		{
			g_SkyEnemy[i].angle1 += g_SkyEnemy[i].circle1_spd*0.1f;	// 回転割合

			g_SkyEnemy[i].angle2 += g_SkyEnemy[i].circle2_spd;	// 回転割合
			g_SkyEnemy[i].pos.y = second_circle.y;

			g_SkyEnemy[i].radius1 += 0.0f;				

		}
		//else											// 半径が０以下になった場合
		//{
		//	g_SkyEnemy[i].radius1 = 0.0f;					// 0より小さくなってしまうと止まってうので半径を0にしている
		//}

		if (g_SkyEnemy[i].move_count <= 60)
		{
			g_SkyEnemy[i].pos.x = first_circle.x;			// カウント中だけY軸中心に円運動
			g_SkyEnemy[i].pos.z = first_circle.z;			// カウント中だけY軸中心に円運動
		}
		else if (g_SkyEnemy[i].move_count >= 60)
		{

			g_SkyEnemy[i].move_count = 0;
		}

		g_SkyEnemy[i].rot.y = g_SkyEnemy[i].angle1;				// 常にプレイヤーの向きを向くようにする


		// 当たり判定ようのボックスと位置を共有する
		g_Collision[i].pos = g_SkyEnemy[i].pos;

		// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！{
		if (g_SkyEnemy[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			// 生存時間をカウント
			if (g_Stage != tutorial) g_SkyEnemy[i].liveCount++;

			// 攻撃を食らっていなけらば攻撃処理
			if (g_SkyEnemy[i].isHit == FALSE)
			{
				// 攻撃処理
				if (g_SkyEnemy[i].liveCount > ENEMY_ATTACK_2)
				{	// 攻撃を行う
					// 生存時間をリセット
					g_SkyEnemy[i].liveCount = 0;

					// リムライトオフ
					g_SkyEnemy[i].fuchi = FALSE;


					// 攻撃
					SetDamageEF(TRUE);
					// SetCameraShake(20);
					SetPlayerDamage(1);

				}
				else if (g_SkyEnemy[i].liveCount > ENEMY_ATTACK_1)
				{	// 赤い点滅が早くなる

					if (g_SkyEnemy[i].liveCount % ENEMY_BLINKING1 < ENEMY_BLINKING1 / 2)
					{	// オブジェクトを赤くする

						// リムライトオン
						g_SkyEnemy[i].fuchi = TRUE;
					}
					else
					{	// オブジェクトの色を戻す

						// リムライトオフ
						g_SkyEnemy[i].fuchi = FALSE;
					}


				}
				else if (g_SkyEnemy[i].liveCount > ENEMY_ATTACK_0)
				{	// 赤く点滅する

					if (g_SkyEnemy[i].liveCount % ENEMY_BLINKING0 < ENEMY_BLINKING0 / 2)
					{	// オブジェクトを赤くする

						// リムライトオン
						g_SkyEnemy[i].fuchi = TRUE;
					}
					else
					{	// オブジェクトの色を戻す

						// リムライトオフ
						g_SkyEnemy[i].fuchi = FALSE;
					}

				}
			}
		}

#ifdef _DEBUG
		if (GetKeyboardPress(DIK_R))
		{
			g_SkyEnemy[i].pos.z = g_SkyEnemy[i].pos.x = 0.0f;
			g_SkyEnemy[i].rot.y = g_SkyEnemy[i].dir = 0.0f;
		}
#endif


		//g_Player.angle += 0.01f;
		//g_Player.rot.y = GetCamera()->rot.y = g_Player.angle;

		CAMERA *camera = GetCamera();
		TARGETOBJ *targetObj = GetTargetObj();
		TARGET *target = GetTarget();

		// レイキャストして足元の高さを求める
		XMFLOAT3 hitPosition;				// 交点
		hitPosition = g_SkyEnemy[i].pos;	// 外れた時用に初期化しておく
		bool ans = RayHitEnemySky(targetObj[0].pos, camera->pos, &hitPosition, i);

		if ((ans) && (!g_SkyEnemy[i].target) && (g_SkyEnemy[i].use))
		{
			//g_SkyEnemy[i].use = FALSE;
			g_SkyEnemy[i].target = TRUE;
			g_Collision[i].use = FALSE;
			target[0].enemyNum[target[0].count] = i;
			target[0].count++;

			SetRockOn();
		}

#ifdef _DEBUG	// デバッグ情報を表示する
		//PrintDebugProc("g_SkyEnemy[%d]:X:%f Y:%f Z:%f\n", i, g_SkyEnemy[i].pos.x, g_SkyEnemy[i].pos.y, g_SkyEnemy[i].pos.z);
#endif
		}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSkyEnemy(void)
{
	for (int i = 0; i < MAX_SKY_ENEMY; i++)
	{
		if (g_SkyEnemy[i].use == TRUE)
		{
			// リムライトの設定
			SetFuchi(g_SkyEnemy[i].fuchi);


			// カリング無効
			SetCullingMode(CULL_MODE_NONE);

			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_SkyEnemy[i].scl.x, g_SkyEnemy[i].scl.y, g_SkyEnemy[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_SkyEnemy[i].rot.x, g_SkyEnemy[i].rot.y + XM_PI, g_SkyEnemy[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_SkyEnemy[i].pos.x, g_SkyEnemy[i].pos.y, g_SkyEnemy[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_SkyEnemy[i].mtxWorld, mtxWorld);


			// モデル描画
			DrawModel(&g_SkyEnemy[g_SkyEnemy[i].EnemyType].model);
      

			//// コリジョン用のボックスの描画
			//// ワールドマトリックスの初期化
			//mtxWorld = XMMatrixIdentity();

			//// スケールを反映
			//mtxScl = XMMatrixScaling(g_Collision[i].scl.x, g_Collision[i].scl.y, g_Collision[i].scl.z);
			//mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			//// 回転を反映
			//mtxRot = XMMatrixRotationRollPitchYaw(g_Collision[i].rot.x, g_Collision[i].rot.y + XM_PI, g_Collision[i].rot.z);
			//mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			//// 移動を反映
			//mtxTranslate = XMMatrixTranslation(g_Collision[i].pos.x, g_Collision[i].pos.y, g_Collision[i].pos.z);
			//mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			//// ワールドマトリックスの設定
			//SetWorldMatrix(&mtxWorld);
			//XMStoreFloat4x4(&g_Collision[i].mtxWorld, mtxWorld);

			//// モデル描画
			//DrawModel(&g_Collision[0].model);
		}
	}

	// リムライトの設定
	SetFuchi(FALSE);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// エネミー情報を取得
//=============================================================================
SKY_ENEMY *GetSkyEnemy(void)
{
	return &g_SkyEnemy[0];
}

// 空ステージのエネミーの発生
void SetSkyEnemy(void)
{
	for (int i = 0; i < MAX_SKY_ENEMY; i++)
	{
		if (!g_SkyEnemy[i].use)
		{
			g_SkyEnemy[i].use = TRUE;
			PlaySound(SOUND_LABEL_SE_enemy_pop);

			g_SkyEnemy[i].enemyPop = TRUE;
			g_SkyEnemy[i].EnemyType = rand() % 3;
			g_SkyEnemy[i].radius1 = (float)(ENEMY_POP_DEISTANCE + rand() % ENEMY_POP_DEISTANCE_RAND);				// サークル１の半径	rand関数はint型なのでfloat型にキャストしている
			g_SkyEnemy[i].target = FALSE;

			g_SkyEnemy[i].angle1 = (float)(rand() % 360) * (XM_2PI / 360);		// サークル１の角度	rand関数はint型なのでfloat型にキャストしている
			g_SkyEnemy[i].angle2 = (float)(rand() % 360) * (XM_2PI / 360);		// サークル２の角度 rand関数はint型なのでfloat型にキャストしている

			// エネミーが出現する初期位置
			g_SkyEnemy[i].pos = { g_SkyEnemy[i].radius1 * cosf(g_SkyEnemy[i].angle1),	// X
								  RamdomFloat(2, ENEMY_POP_Y_RAND, -ENEMY_POP_Y_RAND),	// Y
								  g_SkyEnemy[i].radius1 * sinf(g_SkyEnemy[i].angle1) };	// Z

			g_SkyEnemy[i].rot = { 0.0f, 0.0f, 0.0f };
			g_SkyEnemy[i].scl = { 1.0f, 1.0f, 1.0f };

			// 一定範囲内に入った際の回転速度の初期化
			g_SkyEnemy[i].circle1_spd = RamdomFloat(5, ENEMY_SHINK_SPEED_Y, -ENEMY_SHINK_SPEED_Y);
			g_SkyEnemy[i].circle2_spd = RamdomFloat(3, ENEMY_SHINK_SPEED_X, -ENEMY_SHINK_SPEED_X);

			g_SkyEnemy[i].particleOn = FALSE;


			return;
		}
	}
}


BOOL RayHitEnemySky(XMFLOAT3 Pos, XMFLOAT3 CameraPos, XMFLOAT3 *HitPosition, int num)
{
	CameraPos.y = 5.0f;

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
void ResetSkyEnemyTarget(void)
{
	for (int i = 0; i < MAX_SKY_ENEMY; i++)
	{
		g_SkyEnemy[i].target = FALSE;
	}
}