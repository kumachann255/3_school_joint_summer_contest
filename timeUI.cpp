//=============================================================================
//
// タイム処琁E[timeUI.cpp]
// Author : 熊澤義弁E
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "time.h"
#include "sprite.h"
#include "timeUI.h"
#include "fade.h"
#include "score.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(45)	// 時間サイズ
#define TEXTURE_HEIGHT				(90)	// 
#define TEXTURE_MAX					(2)		// チE��スチャの数


#define TIME_MAX					(300)	// ���Ԑ���

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetEndTime(void);
void GetStageClear(int score, int mode, int stage);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点惁E��
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// チE��スチャ惁E��

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/time0.png",
	"data/TEXTURE/time1.png",
};


static BOOL						g_Use;						// TRUE:使ってぁE��  FALSE:未使用
static float					g_w, g_h;					// 幁E��高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座樁E
static int						g_TexNo;					// チE��スチャ番号

static int						g_Time;						// 残り時間

static BOOL						g_Load = FALSE;

static int						g_stage;					// 現在のスチE�Eジ数
static int						g_Mode_old;					// 直前�Eモードを記録
static BOOL						g_Fade;						// フェード中かどぁE��

static time_t naw_time = 0;		// 現在の時間
static time_t end_time = 0;		// 終亁E��閁E

//=============================================================================
// 初期化�E琁E
//=============================================================================
HRESULT InitTime(void)
{
	ID3D11Device *pDevice = GetDevice();

	//チE��スチャ生�E
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


	// 頂点バッファ生�E
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// プレイヤーの初期匁E
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 500.0f, 60.0f, 0.0f };
	g_TexNo = 0;

	g_Time = 0;	// 時間の初期匁E

	g_stage = GetStage();
	g_Mode_old = GetMode();
	g_Fade = FALSE;

	// 終亁E��間�E設宁E
	SetEndTime();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終亁E�E琁E
//=============================================================================
void UninitTime(void)
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
// 更新処琁E
//=============================================================================
void UpdateTime(void)
{
	// 終亁E��間から現在の時間を引いて残り時間を算�Eする
	g_Time = (int)(end_time - time(NULL));

	// 時間ぁE以下にならなぁE��ぁE��
	if (g_Time < 0) g_Time = 0;

	// シーン遷移
	// スチE�EジクリアしてぁE��ときに処琁E
	if ((g_Time == 0) && (!g_Fade))
	{
		g_Fade = TRUE;
		GetStageClear(GetScore(), GetMode(), GetStage());
	}

	if (g_Time > 10)
	{
		g_TexNo = 0;
	}
	else
	{
		g_TexNo = 1;
	}
}

//=============================================================================
// 描画処琁E
//=============================================================================
void DrawTime(void)
{
	// 頂点バッファ設宁E
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設宁E
	SetWorldViewProjection2D();

	// プリミティブトポロジ設宁E
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設宁E
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// チE��スチャ設宁E
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// 桁数刁E�E琁E��めE
	int number = g_Time;
	for (int i = 0; i < TIME_DIGIT; i++)
	{
		// 今回表示する桁�E数孁E
		float x = (float)(number % 10);

		// スコアの位置めE��クスチャー座標を反映
		float px = g_Pos.x - g_w*i;	// スコアの表示位置X
		float py = g_Pos.y;			// スコアの表示位置Y
		float pw = g_w;				// スコアの表示幁E
		float ph = g_h;				// スコアの表示高さ

		float tw = 1.0f / 10;		// チE��スチャの幁E
		float th = 1.0f / 1;		// チE��スチャの高さ
		float tx = x * tw;			// チE��スチャの左上X座樁E
		float ty = 0.0f;			// チE��スチャの左上Y座樁E

		// �E�枚のポリゴンの頂点とチE��スチャ座標を設宁E
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// 次の桁へ
		number /= 10;
	}
}


//=============================================================================
// タイムを加算すめE
// 引数:add :追加する点数。�Eイナスも可能
//=============================================================================
void AddTime(int add)
{
	g_Time += add;
	if (g_Time > TIME_MAX)
	{
		g_Time = TIME_MAX;
	}

}

//=============================================================================
// 残り時間を取征E
//=============================================================================
int GetTime(void)
{
	return g_Time;
}


//=============================================================================
// 終亁E��間をセチE��する関数
//=============================================================================
void SetEndTime(void)
{
	end_time = time(NULL) + TIME_MAX;
}


//=============================================================================
// ゲームクリアしてぁE��かを判断とシーン遷移
//=============================================================================
void GetStageClear(int score, int mode, int stage)
{
	switch (mode)
	{
	case MODE_GAME_CITY:

		// スチE�Eジごとに判宁E
		switch (stage)
		{
		case stage0:
			if (CITY_STAGE0_BORDER < score)
			{
				SetStage(stage1);
				SetFade(FADE_OUT, MODE_GAME_COUNT);
			}
			else SetFade(FADE_OUT, MODE_RESULT);
			return;

		case stage1:
			if (CITY_STAGE1_BORDER < score)
			{
				SetFade(FADE_OUT, MODE_GAME_COUNT);
			}
			else SetFade(FADE_OUT, MODE_RESULT);

			return;
		}

		break;

	case MODE_GAME_SEA:

		// スチE�Eジごとに判宁E
		switch (stage)
		{
		case stage0:
			if (SEA_STAGE0_BORDER < score)
			{
				SetStage(stage1);
				SetFade(FADE_OUT, MODE_GAME_COUNT);
			}
			else SetFade(FADE_OUT, MODE_RESULT);

			break;

		case stage1:
			if (SEA_STAGE1_BORDER < score)
			{
				SetFade(FADE_OUT, MODE_GAME_COUNT);
			}
			else SetFade(FADE_OUT, MODE_RESULT);

			break;
		}

		break;

	case MODE_GAME_SKY:

		// スチE�Eジごとに判宁E
		switch (stage)
		{
		case stage0:
			if (SKY_STAGE0_BORDER < score)
			{
				SetStage(stage1);
				SetFade(FADE_OUT, MODE_GAME_COUNT);
			}
			else SetFade(FADE_OUT, MODE_RESULT);

			break;

		case stage1:
			SetFade(FADE_OUT, MODE_RESULT);

			break;
		}

		break;
	}
}


// 直前�Eゲームモードを返す
int GetModeOld(void)
{
	return g_Mode_old;
}