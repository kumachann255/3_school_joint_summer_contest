//=============================================================================
//
// オープニング画面処理 [opening.cpp]
// Author : エナ
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "opening.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(5)				// テクスチャの数

#define PAGE_FADE_RATE				(0.02f)			// フェード係数
#define NEXT_PAGE					(150)			// 自動で次のページに変わる時間

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/fade_black.png",
	"data/TEXTURE/opening_001.png",
	"data/TEXTURE/opening_002.png",
	"data/TEXTURE/opening_003.png",
	"data/TEXTURE/opening_004.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static int						g_PageFade;					// フェードの状態
static XMFLOAT4					g_PageColor;				// フェードのカラー（α値）

static int						g_Count;						// 経過時間

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitOpening(void)
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = OPENING_PAGE_1;

	g_PageFade = FADE_IN;
	g_PageColor = { 1.0, 0.0, 0.0, 1.0 };
	
	g_Count = 0;

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_bgm_title);

	// 初期化


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitOpening(void)
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
void UpdateOpening(void)
{
	// フェード処理
	{
		if (g_PageFade != FADE_NONE)
		{// フェード処理中
			if (g_PageFade == FADE_OUT)
			{// フェードアウト処理
				g_PageColor.w += PAGE_FADE_RATE;		// α値を加算して画面を消していく
				
				if (g_PageColor.w >= 1.0f)
				{
					g_PageColor.w = 1.0f;

					// フェードイン処理に切り替え
					g_PageFade = FADE_IN;
					
					// テクスチャ番号を設定
					g_TexNo++;
				}

			}

			else if (g_PageFade == FADE_IN)
			{// フェードイン処理
				g_PageColor.w -= PAGE_FADE_RATE;		// α値を減算して画面を浮き上がらせる
				
				if (g_PageColor.w <= 0.0f)
				{
					g_PageColor.w = 0.0f;

					// フェード処理終了
					g_PageFade = FADE_NONE;
					g_Count = 0;
				}
			}
		}
	}


	// キー入力処理
	{
		if (GetKeyboardTrigger(DIK_RETURN))
		{// Enter押したら、ステージを切り替える

			// SEの挿入（モード選択音）
			PlaySound(SOUND_LABEL_SE_selectBomb01);

			g_PageFade = FADE_OUT;

			if (g_TexNo == (OPENING_PAGE_MAX - 1))
			{// ラストシーン
				g_PageFade = FADE_NONE;

				SetStage(tutorial);
				SetFade(FADE_OUT, MODE_GAME_CITY);
			
				// mainスコアを初期化
				ResetMainScore();
			}
		}

		else if (IsButtonTriggered(0, BUTTON_B))
		{
			// SEの挿入（モード選択音）
			PlaySound(SOUND_LABEL_SE_selectBomb01);

			g_PageFade = FADE_OUT;

			if (g_TexNo == (OPENING_PAGE_MAX - 1))
			{// ラストシーン
				g_PageFade = FADE_NONE;

				SetStage(tutorial);
				SetFade(FADE_OUT, MODE_GAME_CITY);

				// mainスコアを初期化
				ResetMainScore();
			}
		}
	}

	g_Count++;

	if (g_Count == NEXT_PAGE)
	{
		// SEの挿入（モード選択音）
		PlaySound(SOUND_LABEL_SE_selectBomb01);

		g_PageFade = FADE_OUT;

		if (g_TexNo == (OPENING_PAGE_MAX - 1))
		{// ラストシーン
			g_PageFade = FADE_NONE;

			SetStage(tutorial);
			SetFade(FADE_OUT, MODE_GAME_CITY);

			// mainスコアを初期化
			ResetMainScore();
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawOpening(void)
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

	// オープニングの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// フェード画面を描画
	{
		if (g_PageFade == FADE_NONE) return;	// フェードしないのなら描画しない

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[OPENING_PAGE_FADE]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH / 2, TEXTURE_WIDTH / 2, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f,
			g_PageColor);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


}





