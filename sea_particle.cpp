//=============================================================================
//
// パーティクル処理 [sea_particle.cpp]
// Author : 熊澤義弘＋大塚勝亮
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "sea_particle.h"
#include "cup.h"
#include "tako.h"
#include "same.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(21)		// テクスチャの数

#define	SEA_PARTICLE_SIZE_X	(30.0f)		// 頂点サイズ・パーティクルサイズ
#define	SEA_PARTICLE_SIZE_Y	(30.0f)		// 頂点サイズ・パーティクルサイズ

#define	VALUE_SUB			(0.1f)		// パーティクルの減算値
#define	VALUE_START_SUB		(10)		// パーティクルの減算開始値

#define	MAX_SEA_PARTICLE	(512)		// 海パーティクル最大数
#define	MAX_LIFE			(75)		// パーティクルの寿命
#define PARTICLE_SPEED		(0.01f)		// パーティクルの動く速さ

#define BEZIER_P_X			(50.0f)		// ベジェ曲線アンカーX
#define BEZIER_P_Y			(75.0f)		// ベジェ曲線アンカーY
#define BEZIER_P_Z			(50.0f)		// ベジェ曲線アンカーZ

#define BEZIER_L_X			(100.0f)	// ベジェ曲線終点X
#define BEZIER_L_Y			(0.0f)		// ベジェ曲線終点Y
#define BEZIER_L_Z			(100.0f)	// ベジェ曲線終点Z

#define BEZIER_RANDOM		(20)		// ベジェ曲線用乱数
#define BEZIER_RANDOM_Z		(200)		// ベジェ曲線用乱数(Z値用)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexSeaParticle(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static SEA_PARTICLE					g_SeaParticle[MAX_SEA_PARTICLE];	// パーティクルワーク

static XMFLOAT3		control0[MAX_SEA_PARTICLE], control1[MAX_SEA_PARTICLE], control2[MAX_SEA_PARTICLE];	// パーティクルの挙動制御


static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/onpu01_01.png",
	"data/TEXTURE/onpu01_02.png",
	"data/TEXTURE/onpu01_03.png",
	"data/TEXTURE/onpu02_01.png",
	"data/TEXTURE/onpu02_02.png",
	"data/TEXTURE/onpu02_03.png",
	"data/TEXTURE/onpu03_01.png",
	"data/TEXTURE/onpu03_02.png",
	"data/TEXTURE/onpu03_03.png",
	"data/TEXTURE/onpu04_01.png",
	"data/TEXTURE/onpu04_02.png",
	"data/TEXTURE/onpu04_03.png",
	"data/TEXTURE/onpu05_01.png",
	"data/TEXTURE/onpu05_02.png",
	"data/TEXTURE/onpu05_03.png",
	"data/TEXTURE/onpu06_01.png",
	"data/TEXTURE/onpu06_02.png",
	"data/TEXTURE/onpu06_03.png",
	"data/TEXTURE/onpu07_01.png",
	"data/TEXTURE/onpu07_02.png",
	"data/TEXTURE/onpu07_03.png",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSeaParticle(void)
{
	// 頂点情報の作成
	MakeVertexSeaParticle();

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

	// パーティクルワークの初期化
	for(int i = 0; i < MAX_SEA_PARTICLE; i++)
	{
		g_SeaParticle[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_SeaParticle[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_SeaParticle[i].scl = XMFLOAT3(0.75f, 0.75f, 0.75f);
		g_SeaParticle[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_SeaParticle[i].time = 0.0f;
		g_SeaParticle[i].speed = PARTICLE_SPEED;

		ZeroMemory(&g_SeaParticle[i].material, sizeof(g_SeaParticle[i].material));
		g_SeaParticle[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_SeaParticle[i].tex_No = 0;
		g_SeaParticle[i].life = MAX_LIFE;
		//g_SeaParticle[i].pop = 0;
		g_SeaParticle[i].use = FALSE;
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSeaParticle(void)
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
void UpdateSeaParticle(void)
{
	//PLAYER *pPlayer = GetPlayer();
	//g_posBase = pPlayer->pos;
	
	for(int i = 0; i < MAX_SEA_PARTICLE; i++)
	{
		//if (g_SeaParticle[i].use == FALSE)
		//{
		//	SetSeaParticle();
		//}

		// パーティクルワーク処理
		if (g_SeaParticle[i].use == TRUE)		// 使用中
		{
			// パーティクルの処理
			g_SeaParticle[i].life--;

			if (g_SeaParticle[i].life <= VALUE_START_SUB)
			{	// 寿命が１０フレーム切ったら段々透明になっていく
				g_SeaParticle[i].material.Diffuse.w -= VALUE_SUB;
				if (g_SeaParticle[i].material.Diffuse.w < 0.0f)
				{
					g_SeaParticle[i].material.Diffuse.w = 0.0f;
				}
			}

			if (g_SeaParticle[i].life <= 0)
			{
				g_SeaParticle[i].use = FALSE;
			}

			// パーティクルの挙動
			{
				g_SeaParticle[i].time += g_SeaParticle[i].speed;

				if (1.0f <= g_SeaParticle[i].time)
				{
					g_SeaParticle[i].time = 0.0f;
				}

				g_SeaParticle[i].pos.x = ((1.0f - g_SeaParticle[i].time) * (1.0f - g_SeaParticle[i].time) * control0[i].x) +
					(2 * g_SeaParticle[i].time * (1.0f - g_SeaParticle[i].time) * control1[i].x) +
					(g_SeaParticle[i].time * g_SeaParticle[i].time * control2[i].x);

				g_SeaParticle[i].pos.y = ((1.0f - g_SeaParticle[i].time) * (1.0f - g_SeaParticle[i].time) * control0[i].y) +
					(2 * g_SeaParticle[i].time * (1.0f - g_SeaParticle[i].time) * control1[i].y) +
					(g_SeaParticle[i].time * g_SeaParticle[i].time * control2[i].y);

				g_SeaParticle[i].pos.z = ((1.0f - g_SeaParticle[i].time) * (1.0f - g_SeaParticle[i].time) * control0[i].z) +
					(2 * g_SeaParticle[i].time * (1.0f - g_SeaParticle[i].time) * control1[i].z) +
					(g_SeaParticle[i].time * g_SeaParticle[i].time * control2[i].z);
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSeaParticle(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ライティングを無効に
	SetLightEnable(FALSE);

	// 加算合成に設定
	//SetBlendState(BLEND_MODE_ADD);

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


	for(int i = 0; i < MAX_SEA_PARTICLE; i++)
	{

		if(g_SeaParticle[i].use == TRUE)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_SeaParticle[i].tex_No]);

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
			mtxScl = XMMatrixScaling(g_SeaParticle[i].scl.x, g_SeaParticle[i].scl.y, g_SeaParticle[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_SeaParticle[i].pos.x, g_SeaParticle[i].pos.y, g_SeaParticle[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_SeaParticle[i].material);

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
HRESULT MakeVertexSeaParticle(void)
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
		vertex[0].Position = XMFLOAT3(-SEA_PARTICLE_SIZE_X / 2, SEA_PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(SEA_PARTICLE_SIZE_X / 2, SEA_PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-SEA_PARTICLE_SIZE_X / 2, -SEA_PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(SEA_PARTICLE_SIZE_X / 2, -SEA_PARTICLE_SIZE_Y / 2, 0.0f);

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
void SetColorSeaParticle(int nIdxSeaParticle, XMFLOAT4 col)
{
	g_SeaParticle[nIdxSeaParticle].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理
//=============================================================================
//void SetSeaParticle(int mode)
void SetSeaParticleTako(void)
{
	//for(int i = 0; i < MAX_SEA_PARTICLE; i++)
	//{
	//	TAKO *tako = GetTako();

	//	for (int j = 0; j < MAX_TAKO; j++)
	//	{
	//		if (g_SeaParticle[i].use == FALSE && tako[j].mode == CATCH)
	//		{
	//			g_SeaParticle[i].use = TRUE;
	//			g_SeaParticle[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//			g_SeaParticle[i].life = MAX_LIFE;
	//			g_SeaParticle[i].pos = tako[j].pos;
	//			g_SeaParticle[i].pos.y += 10.0f;
	//			g_SeaParticle[i].tex_No = rand() % TEXTURE_MAX;
	//		}
	//	}
	//}

	//return;
	//TAKO *tako = GetTako();

	//for (int i = 0; i < MAX_TAKO; i++)
	//{
	//	if (tako[i].mode == WAIT)
	//	{
	//		for (int j = 0; j < MAX_SEA_PARTICLE; j++)
	//		{
	//			if (g_SeaParticle[j].use == FALSE)
	//			{
	//				g_SeaParticle[j].use = TRUE;
	//				g_SeaParticle[j].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//				g_SeaParticle[j].life = MAX_LIFE;
	//				g_SeaParticle[j].pos = tako[i].pos;
	//				g_SeaParticle[j].pos.y += 10.0f;
	//				g_SeaParticle[j].tex_No = rand() % TEXTURE_MAX;
	//				break;
	//			}
	//		}
	//	}
	//}

	TAKO *tako = GetTako();

	for (int i = 0; i < MAX_TAKO; i++)
	{
		if (tako[i].mode == WAIT)
		{
			for (int j = 0; j < MAX_SEA_PARTICLE; j++)
			{
				if (g_SeaParticle[j].use == FALSE)
				{
					g_SeaParticle[j].use = TRUE;
					g_SeaParticle[j].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					g_SeaParticle[j].life = MAX_LIFE;
					g_SeaParticle[j].pos = tako[i].pos;

					// 噴水みたいにパーティクルを発生させる処理
					// 右の放物線
					if (j % 2 == 0)
					{
						// ベジェ開始位置
						control0[j] = tako[i].pos;

						// ポイント
						control1[j].x = tako[i].pos.x + BEZIER_P_X;
						control1[j].y = tako[i].pos.y + BEZIER_P_Y + (float)(rand() % BEZIER_RANDOM);
						control1[j].z = tako[i].pos.z;

						// ベジェゴール位置
						control2[j].x = tako[i].pos.x + BEZIER_L_X + (float)(rand() % BEZIER_RANDOM);
						control2[j].y = tako[i].pos.y - BEZIER_L_Y;
						control2[j].z = tako[i].pos.z + BEZIER_L_Z - (float)(rand() % BEZIER_RANDOM_Z);
					}
					// 左の放物線
					else if (j % 2 == 1)
					{
						// ベジェ開始位置
						control0[j] = tako[i].pos;

						// ポイント
						control1[j].x = tako[i].pos.x - BEZIER_P_X;
						control1[j].y = tako[i].pos.y + BEZIER_P_Y + (float)(rand() % BEZIER_RANDOM);
						control1[j].z = tako[i].pos.z;

						// ベジェゴール位置
						control2[j].x = tako[i].pos.x - BEZIER_L_X - (float)(rand() % BEZIER_RANDOM);
						control2[j].y = tako[i].pos.y - BEZIER_L_Y;
						control2[j].z = tako[i].pos.z + BEZIER_L_Z - (float)(rand() % BEZIER_RANDOM_Z);
					}
					//g_SeaParticle[j].pos.y += 10.0f;
					g_SeaParticle[j].tex_No = rand() % TEXTURE_MAX;
					break;
				}
			}
		}
	}

}
