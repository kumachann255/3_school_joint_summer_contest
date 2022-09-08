//=============================================================================
//
// ターゲットUI処理 [target.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "target.h"
#include "model.h"
#include "enemy.h"
#include "player.h"
#include "camera.h"
#include "targetObj.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(120)			// 照準のサイズ横
#define TEXTURE_HEIGHT				(TEXTURE_WIDTH)	// 照準のサイズ縦
#define TEXTURE_MAX					(2)				// テクスチャの数
#define	VALUE_MOVE					(4.0f)			// 移動量

#define OFFSET_Y					(6.0f)			// 調整
#define OFFSET_OBJ					(10.0f)



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
XMVECTOR Screenpos(XMVECTOR World_Pos);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/target_outsite.png",
	"data/TEXTURE/target_insite.png",

};

static TARGET g_Target[MAX_TARGET];

BOOL		  g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTarget(void)
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

	for (int i = 0; i < MAX_TARGET; i++)
	{
		// 変数の初期化
		g_Target[i].w = TEXTURE_WIDTH;
		g_Target[i].h = TEXTURE_HEIGHT;
		g_Target[i].pos = XMFLOAT3(60.0f, 60.0f, 0.0f);
		g_Target[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Target[i].spd = 4.0f;
		g_Target[i].TexNo = i;
		g_Target[i].use = TRUE;
		g_Target[i].count = 0;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTarget(void)
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
void UpdateTarget(void)
{
	// 攻撃方法がサークルの場合はスキップ
	if (!GetPlayer()->rockOn) return;

	for (int i = 0; i < MAX_TARGET; i++)
	{
		if (!g_Target[i].use) return;

		// ワールド座標をスクリーン座標へ変換
		TARGETOBJ *targetObj = GetTargetObj();
		XMVECTOR pPos = XMLoadFloat3(&targetObj->pos);
		XMVECTOR ans = Screenpos(pPos);

		XMStoreFloat3(&g_Target[i].pos, ans);

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTarget(void)
{
	// 攻撃方法がサークルの場合はスキップ
	if (!GetPlayer()->rockOn) return;

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

	// ターゲットの描画
	for (int i = 0; i < MAX_TARGET; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

		SetSpriteColorRotation(g_VertexBuffer, g_Target[i].pos.x, g_Target[i].pos.y, g_Target[i].w, g_Target[i].h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), g_Target[i].rot.z);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}

//=============================================================================
// 照準情報を取得
//=============================================================================
TARGET *GetTarget(void)
{
	return &g_Target[0];
}


//=============================================================================
// ワールド座標をスクリーン座標へ変換
//=============================================================================
XMVECTOR Screenpos(XMVECTOR World_Pos)
{
	CAMERA *camera = GetCamera();

	XMVECTOR Eye = XMVectorSet(camera->pos.x, camera->pos.y, camera->pos.z, 0.0f);
	XMVECTOR At = XMVectorSet(camera->at.x, camera->at.y, camera->at.z, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX g_View = XMMatrixLookAtLH(Eye, At, Up);

	//XMMATRIX g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, 0.1f, 1000.0f);
	XMMATRIX g_Projection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	float w = (FLOAT)SCREEN_WIDTH / 2.0f;
	float h = (FLOAT)SCREEN_HEIGHT / 2.0f;
	XMMATRIX viewport = {
	w, 0, 0, 0,
	0, -h, 0, 0,
	0, 0, 1, 0,
	w, h, 0, 1
	};

	// ビュー変換とプロジェクション変換
	World_Pos = XMVector3Transform(World_Pos, g_View);
	World_Pos = XMVector3Transform(World_Pos, g_Projection);

	XMFLOAT3 temp;
	XMStoreFloat3(&temp, World_Pos);
	// zで割って-1~1の範囲に収める
	// スクリーン変換
	const XMVECTOR view_vec = XMVectorSet(temp.x / temp.z, temp.y / temp.z, 1.0f, 1.0f);
	return XMVector3Transform(view_vec, viewport);
}


//=============================================================================
// ターゲットアイコンが画面外に出ていないかを確認
//=============================================================================
BOOL GetTargetArea(int type)
{
	BOOL ans = TRUE;

	for (int i = 0; i < MAX_TARGET; i++)
	{
		switch (type)
		{
		case left:
			if (g_Target[i].pos.x - TEXTURE_WIDTH * 0.5f < 0.0f)
			{
				ans = FALSE;
			}

			break;

		case right:
			if (g_Target[0].pos.x + TEXTURE_WIDTH * 0.5f > SCREEN_WIDTH)
			{
				ans = FALSE;
			}

			break;

		case up:
			if (g_Target[i].pos.y - TEXTURE_HEIGHT * 0.5f < 0.0f)
			{
				ans = FALSE;
			}

			break;

		case down:
			if (g_Target[i].pos.y + TEXTURE_HEIGHT * 0.5f > SCREEN_HEIGHT)
			{
				ans = FALSE;
			}

			break;
		}
	}

	return ans;
}