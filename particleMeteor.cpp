//=============================================================================
//
// メテオのパーティクル処理 [particleMeteor.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "particle.h"
#include "particleMeteor.h"
#include "sky_smallmeteor.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)			// テクスチャの数

#define	PARTICLE_SIZE_X		(15.0f)		// 頂点サイズ・パーティクルサイズ
#define	PARTICLE_SIZE_Y		(15.0f)		// 頂点サイズ・パーティクルサイズ

#define	MAX_PARTICLE		(1024)		// パーティクル最大数

#define METEOR_POP_RAND		(10.0f)		// 発生位置の乱数

#define METEOR_MOVE_Y		(0.4f)		// y軸の落ちる速度の減速倍率
#define METEOR_MOVE_XY		(5.0f)		// xy軸の移動量の乱数

#define MAX_METEOR_COLOR	(0.5f)		// カラーの乱数
#define MIN_METEOR_COLOR	(0.2f)		// カラーの乱数

#define MAX_METEOR_LIFE		(50)		// カラーの乱数
#define MIN_METEOR_LIFE		(50)		// カラーの乱数

#define METEOR_LIFE_ALFA	(20)		// カラーの乱数

#define METEOR_FLAM_NUM		(3)			// 1フレームに何個出すか

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticleMeteor(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;					// テクスチャ番号

static PARTICLE						g_ParticleMeteor[MAX_PARTICLE];		// パーティクルワーク

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.jpg",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticleMeteor(void)
{
	// 頂点情報の作成
	MakeVertexParticleMeteor();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// パーティクルワークの初期化
	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		g_ParticleMeteor[i].type = 0;
		g_ParticleMeteor[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ParticleMeteor[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ParticleMeteor[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_ParticleMeteor[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);

		ZeroMemory(&g_ParticleMeteor[i].material, sizeof(g_ParticleMeteor[i].material));
		g_ParticleMeteor[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_ParticleMeteor[i].life = 0;
		g_ParticleMeteor[i].pop = 0.0f;
		g_ParticleMeteor[i].use = FALSE;
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticleMeteor(void)
{
	if (g_Load == FALSE) return;

	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticleMeteor(void)
{
	S_METEOR *meteorS = GetS_Meteor();

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		// パーティクルワーク処理
		if(g_ParticleMeteor[i].use)		// 使用中
		{
			g_ParticleMeteor[i].life--;
			
			if (g_ParticleMeteor[i].life <= METEOR_LIFE_ALFA)
			{	// 寿命が１０フレーム切ったら段々透明になっていく
				g_ParticleMeteor[i].material.Diffuse.w -= 1.0f / METEOR_LIFE_ALFA;
				if (g_ParticleMeteor[i].material.Diffuse.w < 0.0f)
				{
					g_ParticleMeteor[i].material.Diffuse.w = 0.0f;
				}
			}

			if (g_ParticleMeteor[i].life <= 0)
			{
				g_ParticleMeteor[i].use = FALSE;
			}

			// 移動処理
			g_ParticleMeteor[i].pos.x += g_ParticleMeteor[i].move.x;
			g_ParticleMeteor[i].pos.y += g_ParticleMeteor[i].move.y;
			g_ParticleMeteor[i].pos.z += g_ParticleMeteor[i].move.z;
		}
	}


	//エフェクトの発生処理
	{
		for (int i = 0; i < MAX_S_METEOR; i++)
		{
			if (meteorS[i].use)
			{
				for (int p = 0; p < METEOR_FLAM_NUM; p++)
				{
					XMFLOAT3 pos, move, scl;
					XMFLOAT4 col;
					int life;

					// 発生位置を設定
					pos.x = meteorS[i].pos.x + RamdomFloat(2, METEOR_POP_RAND, -METEOR_POP_RAND);
					pos.y = meteorS[i].pos.y + RamdomFloat(2, METEOR_POP_RAND, -METEOR_POP_RAND);
					pos.z = meteorS[i].pos.z + RamdomFloat(2, METEOR_POP_RAND, -METEOR_POP_RAND);

					// 移動量を設定
					move.x = RamdomFloat(2, METEOR_MOVE_XY, -METEOR_MOVE_XY);
					move.z = RamdomFloat(2, METEOR_MOVE_XY, -METEOR_MOVE_XY);
					move.y = meteorS[i].randMove.y * METEOR_MOVE_Y;

					// カラー設定
					col.x = 0.8f;
					col.y = RamdomFloat(2, MAX_METEOR_COLOR, MIN_METEOR_COLOR);
					col.z = RamdomFloat(2, MAX_METEOR_COLOR, MIN_METEOR_COLOR);
					col.w = 1.0f;

					// 寿命の設定
					life = (rand() % MAX_METEOR_LIFE) + MIN_METEOR_LIFE;

					scl = { 1.0f,1.0f,1.0f };

					SetParticleMeteor(pos, scl, move, col, life);
				}
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticleMeteor(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ライティングを無効に
	SetLightEnable(FALSE);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(FALSE);

	// フォグ無効
	//SetFogEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		if(g_ParticleMeteor[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			// 処理が速いしお勧め
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// スケールを反映
			mtxScl = XMMatrixScaling(g_ParticleMeteor[i].scl.x, g_ParticleMeteor[i].scl.y, g_ParticleMeteor[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_ParticleMeteor[i].pos.x, g_ParticleMeteor[i].pos.y, g_ParticleMeteor[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_ParticleMeteor[i].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(TRUE);

	// フォグ有効
	//SetFogEnable(TRUE);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticleMeteor(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorParticleMeteor(int nIdxParticle, XMFLOAT4 col)
{
	g_ParticleMeteor[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理
//=============================================================================
int SetParticleMeteor(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 scl, XMFLOAT4 col, int life)
{
	int nIdxParticle = -1;

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		if(!g_ParticleMeteor[i].use)
		{
			g_ParticleMeteor[i].pos  = pos;
			g_ParticleMeteor[i].rot  = { 0.0f, 0.0f, 0.0f };
			g_ParticleMeteor[i].scl  = { 1.0f, 1.0f, 1.0f };
			g_ParticleMeteor[i].move = move;
			g_ParticleMeteor[i].material.Diffuse = col;
			g_ParticleMeteor[i].life = life;
			g_ParticleMeteor[i].use  = TRUE;

			nIdxParticle = i;

			break;
		}
	}

	return nIdxParticle;
}
