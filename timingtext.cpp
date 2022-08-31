//=============================================================================
//
// タイミングテキストの処理 [timingtext.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "timingtext.h"
#include "sprite.h"
#include "model.h"
#include "player.h"
#include "score.h"
#include "gameUI.h"
#include "time.h"
#include "sound.h"
#include "timingBar.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// キャラサイズ
#define TEXTURE_HEIGHT				(35)	// 
#define TEXTURE_MAX					(3)		// テクスチャの数

#define START_X						(TARGET_X)	// テキストの最初の表示位置
#define START_Y						(TARGET_Y + 10.0f)	// テキストの最初の表示位置
#define END_Y						(START_Y - 50.0f)	// テキストの最終的な表示位置
#define LIFE_MAX					(30)	// 表示するフレーム


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/hyouka_miss.png",
	"data/TEXTURE/hyouka_good.png",
	"data/TEXTURE/hyouka_perfect.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static TIMINGTEXT				g_Text[TTEXT_MAX];		// 音符

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTimingText(void)
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
	
	// タイミングテキストの初期化
	for (int i = 0; i < TTEXT_MAX; i++)
	{
		g_Text[i].pos.x = START_X;
		g_Text[i].pos.y = START_Y;
		g_Text[i].use = FALSE;
		g_Text[i].texNum = 0;
		g_Text[i].life = LIFE_MAX;
	}

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTimingText(void)
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
void UpdateTimingText(void)
{
	for (int i = 0; i < NOTE_MAX; i++)
	{
		if (g_Text[i].use)
		{
			// 最終位置へ移動させる
			g_Text[i].pos.y -= (g_Text[i].pos.y - END_Y) * 0.9f;

			// 寿命を減らす
			g_Text[i].life--;
			if (g_Text[i].life < 0) g_Text[i].use = FALSE;
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTimingText(void)
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


	// テキストの描画
	for (int i = 0; i < TTEXT_MAX; i++)
	{
		if (g_Text[i].use)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Text[i].texNum]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, g_Text[i].pos.x, g_Text[i].pos.y, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


// 音符の発生
void SetTimingText(int text)
{
	for (int i = 0; i < TTEXT_MAX; i++)
	{
		if (!g_Text[i].use)
		{
			// 初期化
			g_Text[i].use = TRUE;
			g_Text[i].pos = XMFLOAT2(START_X, START_Y);
			g_Text[i].texNum = text;
			g_Text[i].life = LIFE_MAX;

			return;
		}
	}
}