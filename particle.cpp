//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : 熊澤義弘＋エナ
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "particle.h"
#include "cup.h"
#include "blast.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX				(22)				// テクスチャの数

#define	PARTICLE_SIZE_X			(30.0f)				// 頂点サイズ・パーティクルサイズ
#define	PARTICLE_SIZE_Y			(30.0f)				// 頂点サイズ・パーティクルサイズ

#define	MAX_PARTICLE			(512)				// パーティクル最大数

#define	MAX_PARTICLE_DEVIDE		(100)				// パーティクル最大数


#define	ADRESS_BLAST			(0)					// 使える分の先頭アドレス(ネバネバ)
#define	MAX_PARTICLE_BLAST		(5)					// パーティクルワーク(ネバネバ)


#define	ADRESS_CUP				(100)				// 使える分の先頭アドレス(カップ爆発)
#define	MAX_PARTICLE_CUP		(5)					// パーティクルワーク(カップ爆発)


#define	ADRESS_CUP_NOTE			(200)				// 使える分の先頭アドレス(カップ音符)
#define	MAX_PARTICLE_CUP_NOTE	(1)					// パーティクルワーク(カップ音符)

#define	CUP_NOTE_RADIAN			(XM_2PI * 0.02f)	// カップ音符のラジアン
#define	CUP_NOTE_RADIUS			(2.0f)				// カップ音符移動量
#define	CUP_NOTE_POP			(2)				// カップ音符の数

#define	DISP_SHADOW									// 影の表示
#undef DISP_SHADOW


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticle(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;				// 頂点バッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static PARTICLE						g_Particle[MAX_PARTICLE];	// パーティクルワーク

static int							g_CupNotePop;						// カップ音符の数を制限用

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.png",
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
HRESULT InitParticle(void)
{
	// 頂点情報の作成
	MakeVertexParticle();

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
	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		g_Particle[i].texNo = 0;
		g_Particle[i].type = PARTICLE_TYPE_CUP;
		g_Particle[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Particle[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Particle[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Particle[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);

		ZeroMemory(&g_Particle[i].material, sizeof(g_Particle[i].material));
		g_Particle[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Particle[i].nIdxShadow = -1;
		g_Particle[i].life = 0;
		g_Particle[i].pop = 0.0f;
		g_Particle[i].use = FALSE;
		g_Particle[i].radian = 0.0f;
	}

	g_CupNotePop = CUP_NOTE_POP;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticle(void)
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
void UpdateParticle(void)
{
	//PLAYER *pPlayer = GetPlayer();
	//g_posBase = pPlayer->pos;

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		// パーティクルワーク処理
		if(g_Particle[i].use == TRUE)		// 使用中
		{
			g_Particle[i].life--;
			
			if (g_Particle[i].life <= 20)
			{	// 寿命が１０フレーム切ったら段々透明になっていく
				g_Particle[i].material.Diffuse.w -= 0.1f;
				if (g_Particle[i].material.Diffuse.w < 0.0f)
				{
					g_Particle[i].material.Diffuse.w = 0.0f;
				}
			}

			if (g_Particle[i].life <= 0)
			{
				g_Particle[i].use = FALSE;
			}


			// 移動処理
			g_Particle[i].pos.x += g_Particle[i].move.x;
			g_Particle[i].pos.y += g_Particle[i].move.y;
			g_Particle[i].pos.z += g_Particle[i].move.z;

			// 表示タイミングの処理
			if (g_Particle[i].pop > 0.0f)
			{
				g_Particle[i].pop--;
			}


			// カップ爆発の更新処理
			if (g_Particle[i].type == PARTICLE_TYPE_CUP)
			{
				// 拡大処理
				g_Particle[i].scl.x += RamdomFloat(2, 0.2f, 0.1f);
				g_Particle[i].scl.y += RamdomFloat(2, 0.2f, 0.1f);
			}


			// カップ音符の更新処理
			if (g_Particle[i].type == PARTICLE_TYPE_CUP_NOTE)
			{
				float radius = CUP_NOTE_RADIUS;

				g_Particle[i].move.x = sinf(g_Particle[i].radian) * radius;
				g_Particle[i].radian += CUP_NOTE_RADIAN;
				
				//if (g_Particle[i].radian >= XM_2PI)
				//{
				//	g_Particle[i].radian = 0.0f;
				//}
			}
		}
	}

	// カップ音符用POPをリセット
	g_CupNotePop--;
	if (g_CupNotePop < 0)
	{
		g_CupNotePop = CUP_NOTE_POP;
	}

	//エフェクトの発生処理（カップ爆発）
	{
		BOOL particleOn = GetCupParticleOn();

		if (particleOn == TRUE)
		{
			CUP *cup = GetCup();

			for (int j = 0; j < MAX_PARTICLE_CUP; j++)
			{
				int texNo = 0;
				XMFLOAT3 pos;
				XMFLOAT3 scl;
				XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };
				XMFLOAT4 col;

				// 発生位置を設定
				pos.x = cup[0].pos.x + RamdomFloat(2, 40.0f, -40.0f);
				pos.y = cup[0].pos.y + RamdomFloat(2, 60.0f,  10.0f);
				pos.z = cup[0].pos.z + RamdomFloat(2, 40.0f, -40.0f);

				// サイズ設定
				scl.x = RamdomFloat(2, 0.5f, 0.1f);
				scl.y = RamdomFloat(2, 0.5f, 0.1f);
				scl.z = RamdomFloat(2, 0.5f, 0.1f);

				// カラー設定
				col.x = 1.0f;
				col.y = RamdomFloat(2, 1.0f, 0.5f);
				col.z = RamdomFloat(2, 1.0f, 0.5f);
				col.w = RamdomFloat(2, 1.0f, 0.5f);

				SetParticle(ADRESS_CUP, PARTICLE_TYPE_CUP, 0, pos, scl, move, col, 5.0f, 60);
			}
		}
	}

	//エフェクトの発生処理（カップ音符）
	{
		CUP *cup = GetCup();

		if (cup[0].use == TRUE && g_CupNotePop == 0)
		{
			for (int j = 0; j < MAX_PARTICLE_CUP_NOTE; j++)
			{
				int texNo;
				XMFLOAT3 pos;
				XMFLOAT3 scl = XMFLOAT3(0.4f, 0.4f, 0.4f);
				XMFLOAT3 move;
				XMFLOAT4 col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				// テクスチャ番号を設定
				texNo = rand() % 21 + 1;

				// 発生位置を設定
				pos.x = cup[0].pos.x + RamdomFloat(2, 10.0f, -10.0f);
				pos.y = cup[0].pos.y + RamdomFloat(2, 30.0f, 20.0f);
				pos.z = cup[0].pos.z + RamdomFloat(2, 10.0f, -10.0f);


				// 移動量設定
				move.x = 0.0f;
				move.y = 1.3f;
				move.z = 0.0f;

				g_Particle[j].radian = 0.0f;

				SetParticle(ADRESS_CUP_NOTE, PARTICLE_TYPE_CUP_NOTE, texNo, pos, scl, move, col, 0.0f, 80);
			}
		}
	}

	//エフェクトの発生処理（ネバネバ音符）
	{
		BOOL particleOn = GetBlastParticleOn();

		if (particleOn == TRUE)
		{
			BLAST *blast = GetBlast();

			for (int j = 0; j < MAX_PARTICLE_BLAST; j++)
			{
				int texNo;
				XMFLOAT3 pos;
				XMFLOAT3 scl = XMFLOAT3(0.5f, 0.5f, 0.5f);
				XMFLOAT3 move;
				XMFLOAT4 col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				// テクスチャ番号を設定
				texNo = rand() % 21 + 1;

				// 発生位置を設定
				pos.x = blast[0].pos.x;
				pos.y = blast[0].pos.y;
				pos.z = blast[0].pos.z;

				// 移動量設定
				move.x = RamdomFloat(2, 0.7f, -0.7f);
				move.y = RamdomFloat(2, 0.7f, -0.7f);
				move.z = RamdomFloat(2, 0.7f, -0.7f);

				SetParticle(ADRESS_BLAST, PARTICLE_TYPE_BLAST, texNo, pos, scl, move, col, 1.0f, rand()%21 + 60);
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticle(void)
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

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Particle[i].texNo]);

		if(g_Particle[i].use == TRUE && g_Particle[i].pop <= 0.0f)
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
			mtxScl = XMMatrixScaling(g_Particle[i].scl.x, g_Particle[i].scl.y, g_Particle[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Particle[i].pos.x, g_Particle[i].pos.y, g_Particle[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_Particle[i].material);

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
HRESULT MakeVertexParticle(void)
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
void SetColorParticle(int nIdxParticle, XMFLOAT4 col)
{
	g_Particle[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理(int 先頭アドレス, int 種類, int テクスチャ番号,
//						  XMFLOAT3 座標, XMFLOAT3 サイズ, XMFLOAT3 移動量
//						  XMFLOAT4 色, float pop, int life)
//=============================================================================
int SetParticle(int adr, int type, int texNo,XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 move, XMFLOAT4 col, float pop, int life)
{
	int nIdxParticle = -1;

	for(int i = 0; i < MAX_PARTICLE_DEVIDE; i++)
	{
		if(!g_Particle[adr + i].use)
		{
			g_Particle[adr + i].texNo = texNo;
			g_Particle[adr + i].type = type;
			g_Particle[adr + i].pos  = pos;
			g_Particle[adr + i].rot  = { 0.0f, 0.0f, 0.0f };
			g_Particle[adr + i].scl  = scl;
			g_Particle[adr + i].move = move;
			g_Particle[adr + i].material.Diffuse = col;
			g_Particle[adr + i].life = life;
			g_Particle[adr + i].pop = pop * i;
			g_Particle[adr + i].use  = TRUE;

			nIdxParticle = i;

			break;
		}
	}

	return nIdxParticle;
}
