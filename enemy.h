//=============================================================================
//
// エネミーモデル処理 [enemy.h]
// Author : 熊澤義弘＋エナ
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ENEMY		(50)					// エネミーの数

#define	ENEMY_SIZE		(30.0f)				// 当たり判定の大きさ

#define ENEMY_OFFSET_Y		(15.0f)			// エネミーの足元をあわせる


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号
	
	float				zGoal;				// z座標のプレイヤーに近づく最大距離

	XMFLOAT3			hitPos;				// 爆発の座標
	XMFLOAT3			hitRot;				// 当たり判定後アニメーション用、毎回の移動量
	BOOL				isHit;				// 当たってるフラグ TRUE:当たっている
	
	BOOL				cupHit;				// 当たってるフラグ TRUE:当たっている
	BOOL				cupRot;				// パラメーターセット用（カップ）
	BOOL				sameHit;				// 当たってるフラグ TRUE:当たっている
	BOOL				sameRot;				// パラメーターセット用（カップ）
	BOOL				takoHit;				// 当たってるフラグ TRUE:当たっている
	BOOL				takoRot;				// パラメーターセット用（カップ）

	float				radian;				// 回転量

	int					hitTime;			// 移動時間
	int					liveCount;			// ポップしてからの時間を管理

	int					type;				// エネミーのタイプ（0:パトカー、1:白バイ
	BOOL				fuchi;				// リムライトのオンオフ

	// クォータニオン
	XMFLOAT4			quaternion;	// クォータニオン
	XMFLOAT3			upVector;	// 自分が立っている所

	BOOL target;

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
BOOL RayHitEnemy(XMFLOAT3 Pos, XMFLOAT3 CameraPos, XMFLOAT3 *HitPosition, int num);
void ResetEnemyTarget(void);


