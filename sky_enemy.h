#pragma once
//=============================================================================
//
// モデル処理 [sky_enemy.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_SKY_ENEMY		(10)					// エネミーの数

#define	SKY_ENEMY_SIZE		(30.0f)				// 当たり判定の大きさ

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct SKY_ENEMY
{
	XMFLOAT3			pos;		// ポリゴンの位置
	XMFLOAT3			rot;		// ポリゴンの向き(回転)
	XMFLOAT3			scl;		// ポリゴンの大きさ(スケール)

	XMFLOAT4X4			mtxWorld;	// ワールドマトリックス

	BOOL				load;
	DX11_MODEL			model;		// モデル情報

	float				circle1_spd;// 回転スピード
	float				circle2_spd;
	float				dir;		// 向き
	float				size;		// 当たり判定の大きさ
	int					shadowIdx;	// 影のIndex

	BOOL				use;

	// 階層アニメーション用のメンバー変数
	INTERPOLATION_DATA	*tbl_adr;	// アニメデータのテーブル先頭アドレス
	int					tbl_size;	// 登録したテーブルのレコード総数
	float				move_time;	// 実行時間

	// 親は、NULL、子供は親のアドレスを入れる
	SKY_ENEMY				*parent;	// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// クォータニオン
	XMFLOAT4			quaternion;	// クォータニオン
	XMFLOAT3			upVector;	// 自分が立っている所

	float angle1;
	float angle2;

	float radius1;
	float radius2;

	int stay_count;
	int move_count;

	BOOL target;

	int spawn;

	int EnemyType;

	BOOL				isHit;				// 当たってるフラグ TRUE:当たっている
	int					hitTime;			// 移動時間
	int					liveCount;			// ポップしてからの時間を管理

	int					type;				// エネミーのタイプ（0:パトカー、1:白バイ
	BOOL				fuchi;				// リムライトのオンオフ

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSkyEnemy(void);
void UninitSkyEnemy(void);
void UpdateSkyEnemy(void);
void DrawSkyEnemy(void);

SKY_ENEMY *GetSkyEnemy(void);
void SetSkyEnemy(void);
BOOL RayHitEnemySky(XMFLOAT3 Pos, XMFLOAT3 CameraPos, XMFLOAT3 *HitPosition, int num);