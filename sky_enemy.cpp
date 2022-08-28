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

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_SKY_ENEMY_W	"data/MODEL/enemy04.obj"			// 読み込むモデル名
#define	MODEL_SKY_ENEMY_B	"data/MODEL/enemy05.obj"			// 読み込むモデル名
#define MODEL_SKY_ENEMY_D	"data/MODEL/enemy09.obj"

#define	MODEL_SKY_ENEMY		"data/MODEL/m.obj"			// 読み込むモデル名
#define	MODEL_SKY_ENEMY_PARTS	"data/MODEL/torus.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_COLLISION	"data/MODEL/collisionBox.obj"	// 読み込むモデル名

#define	VALUE_MOVE				(2.0f)							// 移動量
#define	VALUE_ROTATE			(XM_PI * 0.02f)					// 回転量

#define SKY_ENEMY_SHADOW_SIZE	(1.0f)							// 影の大きさ
#define SKY_ENEMY_OFFSET_Y		(30.0f)							// エネミーの足元をあわせる

#define SKY_ENEMY_PARTS_MAX		(2)								// エネミーのパーツの数

#define STAGE0_POP_COUNT	(300)						// エネミーのポップ間隔
#define STAGE0_MAX_POP		(40)						// 最大、場に何体エネミーを出すか

#define ENEMY_HIT_MOVE		(5.0f)						// 当たり判定後アニメーション用移動量

#define ENEMY_ATTACK_0		(300)						// エネミーが点滅するまでの時間
#define ENEMY_ATTACK_1		(120 + ENEMY_ATTACK_0)		// 点滅が早くなるまでの時間
#define ENEMY_ATTACK_2		(120 + ENEMY_ATTACK_1)		// 攻撃するまでの時間

#define ENEMY_BLINKING0		(50)						// 点滅の間隔
#define ENEMY_BLINKING1		(14)						// 点滅の間隔



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
		g_SkyEnemy[i].pos = { g_SkyEnemy[i].radius1 * cosf(g_SkyEnemy[i].angle1),	// X
							  -150.0f,	// Y
							  g_SkyEnemy[i].radius1 * sinf(g_SkyEnemy[i].angle1) };	// Z

		g_SkyEnemy[i].rot = { 0.0f, 0.0f, 0.0f };
		g_SkyEnemy[i].scl = { 1.0f, 1.0f, 1.0f };

		g_SkyEnemy[i].circle1_spd = (float)((rand() % 2) + 5) / 1000.0f;			// サークル1を移動するスピード rand関数はint型なのでfloat型にキャストしている
		g_SkyEnemy[i].circle2_spd = (float)((rand() % 1) + 3) / 100.0f;			// サークル2を移動するスピード rand関数はint型なのでfloat型にキャストしている


		g_SkyEnemy[i].size = SKY_ENEMY_SIZE;	// 当たり判定の大きさ

		g_SkyEnemy[i].use = FALSE;

		g_SkyEnemy[i].spawn = 0;

		g_SkyEnemy[i].move_count = 0;

		g_SkyEnemy[i].EnemyType = 0;

		g_SkyEnemy[i].target = FALSE;


		g_Collision[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Collision[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		GetModelDiffuse(&g_Collision[i].model, &g_Collision[i].diffuse[0]);
		g_Collision[i].use = FALSE;			// TRUE:生きてる


		// ここでプレイヤー用の影を作成している
		XMFLOAT3 pos = g_SkyEnemy[i].pos;
		pos.y -= (SKY_ENEMY_OFFSET_Y - 0.1f);
		g_SkyEnemy[i].shadowIdx = CreateShadow(pos, SKY_ENEMY_SHADOW_SIZE, SKY_ENEMY_SHADOW_SIZE);
		//          ↑
		//        このメンバー変数が生成した影のIndex番号

		// 階層アニメーション用の初期化処理
		g_SkyEnemy[i].parent = NULL;			// 本体（親）なのでNULLを入れる
		g_Parts[i][0].parent = &g_SkyEnemy[i];		// 親をセット
		g_Parts[i][1].parent = &g_SkyEnemy[i];		// 親をセット



		//// パーツの初期化
		//for (int j = 0; j < SKY_ENEMY_PARTS_MAX; j++)
		//{
		//	g_Parts[i][j].use = FALSE;

		//	// 位置・回転・スケールの初期設定
		//	g_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		//	g_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		//	g_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		//	// 親子関係
		//	g_Parts[i][j].parent = &g_SkyEnemy[i];		// ← ここに親のアドレスを入れる
		////	g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
		////	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

		//	// 階層アニメーション用のメンバー変数の初期化
		//	g_Parts[i][j].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		//	g_Parts[i][j].move_time = 0.0f;	// 実行時間をクリア
		//	g_Parts[i][j].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		//	// パーツの読み込みはまだしていない
		//	g_Parts[i][j].load = 0;
		//}

		//g_Parts[i][0].use = TRUE;
		//g_Parts[i][0].tbl_adr = move_tbl_right;	// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[i][0].tbl_size = sizeof(move_tbl_right) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		//g_Parts[i][0].load = 1;
		//LoadModel(MODEL_SKY_ENEMY_PARTS, &g_Parts[i][0].model);

		//g_Parts[i][1].use = TRUE;
		//g_Parts[i][1].tbl_adr = move_tbl_left;	// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[i][1].tbl_size = sizeof(move_tbl_left) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		//g_Parts[i][1].load = 1;
		//LoadModel(MODEL_SKY_ENEMY_PARTS, &g_Parts[i][1].model);

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
		//if (g_SkyEnemy[i].use)
		//{

		//	g_SkyEnemy[i].spawn++;

		//	if (g_SkyEnemy[i].spawn == 100 * i)
		//	{
		//		g_SkyEnemy[i].use = TRUE;
		//	}

		g_SkyEnemy[i].move_count++;


		// 1つ目のサークル
		XMFLOAT3 first_circle;

		first_circle.x = sinf(g_SkyEnemy[i].angle1) * g_SkyEnemy[i].radius2;		// 1つ目の円に2つ目の円を合成　円運動をする基点（中心点）＊2つ目の円運動の半径
		first_circle.z = cosf(g_SkyEnemy[i].angle1) * g_SkyEnemy[i].radius2;		// 1つ目の円に2つ目の円を合成　円運動をする基点（中心点）＊2つ目の円運動の半径



		// 2つ目のサークル
		XMFLOAT3 second_circle;
		g_SkyEnemy[i].radius2 = g_SkyEnemy[i].radius1 + cosf(g_SkyEnemy[i].angle2) * 50;		// 現在の位置からのサークル２の半径を求める

		second_circle.y = sinf(g_SkyEnemy[i].angle2) * 50;								// サインカーブ　＊　この半径を元に



		if (g_SkyEnemy[i].radius1 >= 150)					// サークル1の範囲外にいる場合
		{
			g_SkyEnemy[i].radius1 -= 0.02f;					// サークル1に向かって一直線に来る　エネミーのポップする位置はワールド座標の原点を中心として三角関数で管理しているので、半径を縮めていけばサークル１に向かってくる
		}
		else if (g_SkyEnemy[i].radius1 > 0.0f)				// サークル1の内側（半径が０～１５０）の場合
		{
			g_SkyEnemy[i].angle1 += g_SkyEnemy[i].circle1_spd;	// 回転割合
			g_SkyEnemy[i].radius1 -= 0.02f;					// 半径を短くして行ってプレイヤーに近づいてくるようにしている

			g_SkyEnemy[i].angle2 += g_SkyEnemy[i].circle2_spd;	// 回転割合
			g_SkyEnemy[i].pos.y = second_circle.y;
		}
		else											// 半径が０以下になった場合
		{
			g_SkyEnemy[i].radius1 = 0.0f;					// 0より小さくなってしまうと止まってうので半径を0にしている
		}

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

		if ((ans) && (!g_SkyEnemy[i].target))
		{
			//g_SkyEnemy[i].use = FALSE;
			g_SkyEnemy[i].target = TRUE;
			g_Collision[i].use = FALSE;
			target[0].enemyNum[target[0].count] = i;
			target[0].count++;

			SetRockOn();
		}


		//g_Player.angle += 0.01f;
		//g_Player.rot.y = GetCamera()->rot.y = g_Player.angle;


			//// 入力のあった方向へプレイヤーを向かせて移動させる
			//g_Player.pos.z = 0.0f + cosf(g_Player.angle) * 100;
			//g_Player.pos.x = 0.0f + sinf(g_Player.angle) * 100;





			//// レイキャストして足元の高さを求める
			//XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// ぶつかったポリゴンの法線ベクトル（向き）
			//XMFLOAT3 hitPosition;								// 交点
			//hitPosition.y = g_Air.pos.y - AIR_OFFSET_Y;	// 外れた時用に初期化しておく
			//bool ans = RayHitField(g_Air.pos, &hitPosition, &normal);
			//g_Air.pos.y = hitPosition.y + AIR_OFFSET_Y;
			//g_Air.pos.y =AIR_OFFSET_Y;


			// 影もプレイヤーの位置に合わせる
			XMFLOAT3 pos = g_SkyEnemy[i].pos;
			pos.y -= (SKY_ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_SkyEnemy[i].shadowIdx, pos);



			{	// ポイントライトのテスト
				LIGHT *light = GetLightData(1);
				XMFLOAT3 pos = g_SkyEnemy[i].pos;
				pos.y += 20.0f;

				light->Position = pos;
				light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				light->Type = LIGHT_TYPE_POINT;
				light->Enable = TRUE;
				SetLightData(1, light);
			}

		//}
		////////////////////////////////////////////////////////////////////////
		//// 姿勢制御
		////////////////////////////////////////////////////////////////////////

		//XMVECTOR vx, nvx, up;
		//XMVECTOR quat;
		//float len, angle;

		//// ２つのベクトルの外積を取って任意の回転軸を求める
		//g_Air.upVector = normal;
		//up = { 0.0f, 1.0f, 0.0f, 0.0f };
		//vx = XMVector3Cross(up, XMLoadFloat3(&g_Air.upVector));

		//// 求めた回転軸からクォータニオンを作り出す
		//nvx = XMVector3Length(vx);
		//XMStoreFloat(&len, nvx);
		//nvx = XMVector3Normalize(vx);
		//angle = asinf(len);
		//quat = XMQuaternionRotationNormal(nvx, angle);

		//// 前回のクォータニオンから今回のクォータニオンまでの回転を滑らかにする
		//quat = XMQuaternionSlerp(XMLoadFloat4(&g_Air.quaternion), quat, 0.05f);

		//// 今回のクォータニオンの結果を保存する
		//XMStoreFloat4(&g_Air.quaternion, quat);

		// 階層アニメーション
		for (int j = 0; j < SKY_ENEMY_PARTS_MAX; j++)
		{
			// 使われているなら処理する
			if ((g_Parts[i][j].use == TRUE) && (g_Parts[i][j].tbl_adr != NULL))
			{
				// 移動処理
				int		index = (int)g_Parts[i][j].move_time;
				float	time = g_Parts[i][j].move_time - index;
				int		size = g_Parts[i][j].tbl_size;

				float dt = 1.0f / g_Parts[i][j].tbl_adr[index].frame;	// 1フレームで進める時間
				g_Parts[i][j].move_time += dt;					// アニメーションの合計時間に足す

				if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
				{
					g_Parts[i][j].move_time = 0.0f;
					index = 0;
				}

		//		float dt = 1.0f / g_Parts[i][j].tbl_adr[index].frame;	// 1フレームで進める時間
		//		g_Parts[i][j].move_time += dt;					// アニメーションの合計時間に足す

		//		if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
		//		{
		//			g_Parts[i][j].move_time = 0.0f;
		//			index = 0;
		//		}

		//		// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
		//		XMVECTOR p1 = XMLoadFloat3(&g_Parts[i][j].tbl_adr[index + 1].pos);	// 次の場所
		//		XMVECTOR p0 = XMLoadFloat3(&g_Parts[i][j].tbl_adr[index + 0].pos);	// 現在の場所
		//		XMVECTOR vec = p1 - p0;
		//		XMStoreFloat3(&g_Parts[i][j].pos, p0 + vec * time);
			}

#ifdef _DEBUG	// デバッグ情報を表示する
		PrintDebugProc("g_SkyEnemy[%d]:X:%f Y:%f Z:%f\n", i, g_SkyEnemy[i].pos.x, g_SkyEnemy[i].pos.y, g_SkyEnemy[i].pos.z);
#endif
		}
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

			//// クォータニオンを反映
			//XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.quaternion));
			//mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_SkyEnemy[i].pos.x, g_SkyEnemy[i].pos.y, g_SkyEnemy[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_SkyEnemy[i].mtxWorld, mtxWorld);


			// モデル描画
			DrawModel(&g_SkyEnemy[g_SkyEnemy[i].EnemyType].model);
      

			// コリジョン用のボックスの描画
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Collision[i].scl.x, g_Collision[i].scl.y, g_Collision[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Collision[i].rot.x, g_Collision[i].rot.y + XM_PI, g_Collision[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Collision[i].pos.x, g_Collision[i].pos.y, g_Collision[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);
			XMStoreFloat4x4(&g_Collision[i].mtxWorld, mtxWorld);

			// モデル描画
			DrawModel(&g_Collision[0].model);
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
			g_SkyEnemy[i].EnemyType = rand() % 3;
			g_SkyEnemy[i].radius1 = (float)(150 + rand() % 250);					// サークル１の半径	rand関数はint型なのでfloat型にキャストしている

			g_SkyEnemy[i].angle1 = (float)(rand() % 360) * (XM_2PI / 360);		// サークル１の角度	rand関数はint型なのでfloat型にキャストしている
			g_SkyEnemy[i].angle2 = (float)(rand() % 360) * (XM_2PI / 360);		// サークル２の角度 rand関数はint型なのでfloat型にキャストしている


			// エネミーが出現する初期位置
			g_SkyEnemy[i].pos = { g_SkyEnemy[i].radius1 * cosf(g_SkyEnemy[i].angle1),	// X
								 -150.0f,	// Y
								  g_SkyEnemy[i].radius1 * sinf(g_SkyEnemy[i].angle1) };	// Z

			g_SkyEnemy[i].rot = { 0.0f, 0.0f, 0.0f };
			g_SkyEnemy[i].scl = { 1.0f, 1.0f, 1.0f };

			g_SkyEnemy[i].circle1_spd = (float)((rand() % 2) + 5) / 1000.0f;			// サークル1を移動するスピード rand関数はint型なのでfloat型にキャストしている
			g_SkyEnemy[i].circle2_spd = (float)((rand() % 1) + 3) / 100.0f;			// サークル2を移動するスピード rand関数はint型なのでfloat型にキャストしている

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
