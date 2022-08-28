//=============================================================================
//
// タイミングバーの処理 [timingBar.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "timingBar.h"
#include "sprite.h"
#include "model.h"
#include "player.h"
#include "score.h"
#include "gameUI.h"
#include "time.h"
#include "sound.h"
#include "combo.h"
#include "timingEffect.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SENHU_TEXTURE_WIDTH			(860)	// キャラサイズ
#define SENHU_TEXTURE_HEIGHT		(150)	// 
#define TEXTURE_MAX					(10)	// テクスチャの数

#define SENHU_X						(960.0f / 2.0f)	// 五線譜の表示位置
#define SENHU_Y						(70.0f)	// 五線譜の表示位置

#define ONPU_TEXTURE_X				(40.0f)	//音符の幅
#define ONPU_TEXTURE_Y				(60.0f)//音符の高さ
#define ONPU_X						(1000.0f)	//音符の表示位置
#define ONPU_Y						(SENHU_Y)//音符の表示位置

#define TARGET_TEXTURE_X			(100.0f)	// ターゲットのサイズ
#define TARGET_TEXTURE_Y			(100.0f)

#define ONPU_SPEED					(5.0f)	// 音符の速度

#define DISTANCE_ONPU_CITY			(40)	// 街ステージの音符の間隔(フレーム)
#define DISTANCE_ONPU_SEA			(40)	// 海ステージの音符の間隔(フレーム)
#define DISTANCE_ONPU_SKY			(40)	// 空ステージの音符の間隔(フレーム)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/flat.png",
	"data/TEXTURE/sharp.png",
	"data/TEXTURE/8bu_onpu.png",
	"data/TEXTURE/16bu_onpu.png",
	"data/TEXTURE/6bu_onpu_renkou.png",
	"data/TEXTURE/8bu_onpu_renkou.png",
	"data/TEXTURE/8bu_kyuufu.png",
	"data/TEXTURE/16bu_kyuufu.png",
	"data/TEXTURE/gosenhu.png",
	"data/TEXTURE/onpuTarget.png",
	
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static TIMINGNOTE				g_Note[NOTE_MAX];			// 音符
static TIMINGNOTE				g_Target;					// 音符を押すタイミングのテクスチャ
static int						g_Time;						// 時間
static int						g_Distance;

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTImingBar(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	for (int i = 0; i < NOTE_MAX; i++)
	{
		g_Note[i].pos.x = ONPU_X;
		g_Note[i].pos.y = ONPU_Y;
		g_Note[i].use = FALSE;
		g_Note[i].texNum = 0;
	}

	g_Target.pos.x = TARGET_X;
	g_Target.pos.y = TARGET_Y;
	g_Target.use = TRUE;
	g_Target.texNum = 9;


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// プレイヤーの初期化
	g_Use   = TRUE;
	g_w     = SENHU_TEXTURE_WIDTH;
	g_h     = SENHU_TEXTURE_HEIGHT;
	g_Pos   = { SENHU_X, SENHU_Y, 0.0f };
	g_TexNo = 0;

	// 時間の初期化
	g_Time = 0;

	switch (GetMode())
	{
	case MODE_GAME_CITY:
		g_Distance = DISTANCE_ONPU_CITY;

		break;

	case MODE_GAME_SEA:
		g_Distance = DISTANCE_ONPU_SEA;

		break;

	case MODE_GAME_SKY:
		g_Distance = DISTANCE_ONPU_SKY;

		break;

	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTImingBar(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTImingBar(void)
{
	for (int i = 0; i < NOTE_MAX; i++)
	{
		if (g_Note[i].use)
		{
			// 左へ移動させる
			g_Note[i].pos.x -= ONPU_SPEED;
			if (g_Note[i].pos.x < -50.0f) g_Note[i].use = FALSE;

		}
	}


	// 音符の出現
	g_Time++;

	// 一定時間で音符を出現させる
	if (g_Time % g_Distance == 0)
	{
		SetNote();
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTImingBar(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	// 五線譜の描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);


	// ターゲットの描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Target.texNum]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColor(g_VertexBuffer, g_Target.pos.x, g_Target.pos.y, TARGET_TEXTURE_X, TARGET_TEXTURE_Y, 0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);


	// 音符の描画
	for (int i = 0; i < NOTE_MAX; i++)
	{
		if (g_Note[i].use)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Note[i].texNum]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, g_Note[i].pos.x, g_Note[i].pos.y, ONPU_TEXTURE_X, ONPU_TEXTURE_Y, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


// 音符の発生
void SetNote(void)
{
	for (int i = 0; i < NOTE_MAX; i++)
	{
		if (!g_Note[i].use)
		{
			// 初期化
			g_Note[i].use = TRUE;
			g_Note[i].pos = XMFLOAT2(ONPU_X, ONPU_Y);
			g_Note[i].texNum = rand() % NOTE_TEX_MAX;
			
			return;
		}
	}
}


// ジャストタイミングかどうかを判断
// 戻り値が0ならミス(範囲内でミスしたら音符が消える)、1ならGOOD、2ならJUST
int GetNoteTiming(void)
{
	int ans = 0;

	for (int i = 0; i < NOTE_MAX; i++)
	{
		if (g_Note[i].use)
		{
			// ミスかどうか
			if ((g_Note[i].pos.x > g_Target.pos.x - NOTE_TIMING_MISS) && (g_Note[i].pos.x < g_Target.pos.x + NOTE_TIMING_MISS))
			{
				
				// ジャストタイミングかどうか
				if ((g_Note[i].pos.x > g_Target.pos.x - NOTE_TIMING_GOOD) && (g_Note[i].pos.x < g_Target.pos.x + NOTE_TIMING_GOOD))
				{
					// 良い
					ans++;

					if ((g_Note[i].pos.x > g_Target.pos.x - NOTE_TIMING_JUST) && (g_Note[i].pos.x < g_Target.pos.x + NOTE_TIMING_JUST))
					{
						// 最高
						ans++;
					}
				}

				g_Note[i].use = FALSE;
				break;
			}
		}
	}

	// 最高のタイミングだったら歓声を流す
	if (ans >= 2) PlaySound(SOUND_LABEL_SE_cheers02);

	// 良い以上の時にエフェクトを発生(エフェクトの種類はランダム)
	if (ans >= 1) SetTimingEffect(rand() % TEFFECT_TYPE_MAX);

	// 良い以上であればコンボを設定
	if (ans >= 1) 
	{
		SetTimingHas(TRUE);
		ResetComboTime();	// コンボ継続時間もリセット
	}
	else SetTimingHas(FALSE);

	return ans;
}