//=============================================================================
//
// �p�[�e�B�N������ [sea_particle.cpp]
// Author : �F�V�`�O�{��ˏ���
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

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(21)			// �e�N�X�`���̐�

#define	SEA_PARTICLE_SIZE_X	(30.0f)		// ���_�T�C�Y�E�p�[�e�B�N���T�C�Y
#define	SEA_PARTICLE_SIZE_Y	(30.0f)		// ���_�T�C�Y�E�p�[�e�B�N���T�C�Y

#define	MAX_PARTICLE		(512)		// �p�[�e�B�N���ő吔

#define	MAX_SEA_PARTICLE	(10)		// �p�[�e�B�N���ő吔

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexSeaParticle(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;					// �e�N�X�`���ԍ�

static SEA_PARTICLE					g_SeaParticle[MAX_SEA_PARTICLE];	// �p�[�e�B�N�����[�N

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
// ����������
//=============================================================================
HRESULT InitSeaParticle(void)
{
	// ���_���̍쐬
	MakeVertexSeaParticle();

	// �e�N�X�`������
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

	// �p�[�e�B�N�����[�N�̏�����
	for(int i = 0; i < MAX_SEA_PARTICLE; i++)
	{
		g_SeaParticle[i].type = PARTICLE_TYPE_MAX;
		g_SeaParticle[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_SeaParticle[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_SeaParticle[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_SeaParticle[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);

		ZeroMemory(&g_SeaParticle[i].material, sizeof(g_SeaParticle[i].material));
		g_SeaParticle[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_SeaParticle[i].life = 0;
		g_SeaParticle[i].pop = 0;
		g_SeaParticle[i].use = FALSE;
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSeaParticle(void)
{
	if (g_Load == FALSE) return;

	//�e�N�X�`���̉��
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSeaParticle(void)
{
	//PLAYER *pPlayer = GetPlayer();
	//g_posBase = pPlayer->pos;
	
	for(int i = 0; i < MAX_SEA_PARTICLE; i++)
	{
		// �p�[�e�B�N�����[�N����
		if(g_SeaParticle[i].use == TRUE)		// �g�p��
		{
			switch (g_SeaParticle[i].type)
			{
			case PARTICLE_TYPE_TAKO:

				// �^�R�̏ꍇ�̏���
				g_SeaParticle[i].life--;

				if (g_SeaParticle[i].life <= 10)
				{	// �������P�O�t���[���؂�����i�X�����ɂȂ��Ă���
					g_SeaParticle[i].material.Diffuse.w -= 0.1f;
					if (g_SeaParticle[i].material.Diffuse.w < 0.0f)
					{
						g_SeaParticle[i].material.Diffuse.w = 0.0f;
					}
				}

				if (g_SeaParticle[i].life <= 0)
				{
					g_SeaParticle[i].use = FALSE;
				}

				// �^�R�̏ꍇ�̋���
				{

				}

				break;

			case PARTICLE_TYPE_SAME:

				// �^�R�̏ꍇ�̏���
				g_SeaParticle[i].life--;

				if (g_SeaParticle[i].life <= 10)
				{	// �������P�O�t���[���؂�����i�X�����ɂȂ��Ă���
					g_SeaParticle[i].material.Diffuse.w -= 0.1f;
					if (g_SeaParticle[i].material.Diffuse.w < 0.0f)
					{
						g_SeaParticle[i].material.Diffuse.w = 0.0f;
					}
				}

				if (g_SeaParticle[i].life <= 0)
				{
					g_SeaParticle[i].use = FALSE;
				}
				break;
			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSeaParticle(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���C�e�B���O�𖳌���
	SetLightEnable(FALSE);

	// ���Z�����ɐݒ�
	//SetBlendState(BLEND_MODE_ADD);

	// Z��r����
	SetDepthEnable(FALSE);

	// �t�H�O����
	//SetFogEnable(FALSE);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for(int i = 0; i < MAX_SEA_PARTICLE; i++)
	{
		if(g_SeaParticle[i].use == TRUE && g_SeaParticle[i].pop <= 0.0f)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			// ������������������
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_SeaParticle[i].scl.x, g_SeaParticle[i].scl.y, g_SeaParticle[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_SeaParticle[i].pos.x, g_SeaParticle[i].pos.y, g_SeaParticle[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_SeaParticle[i].material);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r�L��
	SetDepthEnable(TRUE);

	// �t�H�O�L��
	//SetFogEnable(TRUE);

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexSeaParticle(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(-SEA_PARTICLE_SIZE_X / 2, SEA_PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(SEA_PARTICLE_SIZE_X / 2, SEA_PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-SEA_PARTICLE_SIZE_X / 2, -SEA_PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(SEA_PARTICLE_SIZE_X / 2, -SEA_PARTICLE_SIZE_Y / 2, 0.0f);

		// �@���̐ݒ�
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// ���ˌ��̐ݒ�
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// �}�e���A���J���[�̐ݒ�
//=============================================================================
void SetColorSeaParticle(int nIdxSeaParticle, XMFLOAT4 col)
{
	g_SeaParticle[nIdxSeaParticle].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
//int SetSeaParticle(int type, XMFLOAT3 pos, int life)
//{
//	for(int i = 0; i < MAX_SEA_PARTICLE; i++)
//	{
//		if(!g_SeaParticle[i].use)
//		{
//			g_SeaParticle[i].type = type;
//			g_SeaParticle[i].pos  = pos;
//			g_SeaParticle[i].life = life;
//			g_SeaParticle[i].use  = TRUE;
//
//			return;
//		}
//	}
//}
