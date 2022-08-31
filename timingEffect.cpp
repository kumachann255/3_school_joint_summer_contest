//=============================================================================
//
// タイミングエフェクトの処理 [timingEffect.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "timingEffect.h"
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
#define TEXTURE_WIDTH				(30)	// キャラサイズ
#define TEXTURE_HEIGHT				(30)	// 
#define TEXTURE_WIDTH_MAX			(100)	// キャラサイズ
#define TEXTURE_HEIGHT_MAX			(100)	// 
#define TEXTURE_MAX					(TEFFECT_TYPE_MAX)		// テクスチャの数

#define SIZE_BIG_SPEED				(0.5f)	// サイズの大きくなるスピード

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
	"data/TEXTURE/timingEffect_0.png",
	"data/TEXTURE/timingEffect_1.png",
	"data/TEXTURE/timingEffect_2.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static TIMINGEFFECT				g_Effect[TEFFECT_MAX];			// 音符

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTimingEffect(void)
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
	for (int i = 0; i < TEFFECT_MAX; i++)
	{
		g_Effect[i].pos.x = TARGET_X;
		g_Effect[i].pos.y = TARGET_Y;
		g_Effect[i].use = FALSE;		
		g_Effect[i].size.x = TEXTURE_WIDTH;
		g_Effect[i].size.y = TEXTURE_HEIGHT;

		g_Effect[i].texNum = 0;
		g_Effect[i].life = LIFE_MAX;
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
void UninitTimingEffect(void)
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
void UpdateTimingEffect(void)
{
	for (int i = 0; i < TEFFECT_MAX; i++)
	{
		if (g_Effect[i].use)
		{
			// 最終位置へ移動させる
			g_Effect[i].size.x += (TEXTURE_WIDTH_MAX - g_Effect[i].size.x) * SIZE_BIG_SPEED;
			g_Effect[i].size.y += (TEXTURE_HEIGHT_MAX - g_Effect[i].size.y) * SIZE_BIG_SPEED;

			// 寿命を減らす
			g_Effect[i].life--;
			if (g_Effect[i].life < 0) g_Effect[i].use = FALSE;
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTimingEffect(void)
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


	// エフェクトの描画
	for (int i = 0; i < TEFFECT_MAX; i++)
	{
		if (g_Effect[i].use)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Effect[i].texNum]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, g_Effect[i].pos.x, g_Effect[i].pos.y, g_Effect[i].size.x, g_Effect[i].size.y, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


// エフェクトの発生
void SetTimingEffect(int tex)
{
	for (int i = 0; i < TEFFECT_MAX; i++)
	{
		if (!g_Effect[i].use)
		{
			// 初期化
			g_Effect[i].use = TRUE;
			g_Effect[i].pos.x = TARGET_X;
			g_Effect[i].pos.y = TARGET_Y;
			g_Effect[i].size.x = TEXTURE_WIDTH;
			g_Effect[i].size.y = TEXTURE_HEIGHT;
			g_Effect[i].texNum = tex;
			g_Effect[i].life = LIFE_MAX;

			return;
		}
	}
}