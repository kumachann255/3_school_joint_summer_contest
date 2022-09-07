//=============================================================================
//
// ���e�I�̃p�[�e�B�N������ [particleMeteor.cpp]
// Author : �F�V�`�O
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "particle.h"
#include "particleSky.h"
#include "sky_enemy.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)			// �e�N�X�`���̐�

#define	PARTICLE_SIZE_X		(15.0f)		// ���_�T�C�Y�E�p�[�e�B�N���T�C�Y
#define	PARTICLE_SIZE_Y		(15.0f)		// ���_�T�C�Y�E�p�[�e�B�N���T�C�Y

#define	MAX_PARTICLE		(1024)		// �p�[�e�B�N���ő吔

#define SKY_ENEMY_POP_RAND		(10.0f)		// �����ʒu�̗���

#define METEOR_MOVE_Y		(0.4f)		// y���̗����鑬�x�̌����{��
#define SKY_MOVE_XY			(3.0f)		// xy���̈ړ��ʂ̗���

#define MAX_SKY_ENEMY_COLOR	(0.5f)		// �J���[�̗���
#define MIN_SKY_ENEMY_COLOR	(0.2f)		// �J���[�̗���

#define MAX_SKY_ENEMY_LIFE		(50)		// �J���[�̗���
#define MIN_SKY_ENEMY_LIFE		(50)		// �J���[�̗���

#define SKY_ENEMY_LIFE_ALFA	(20)		// �J���[�̗���

#define SKY_ENEMY_FLAM_NUM		(10)			// 1�t���[���ɉ��o����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticleSky(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;					// �e�N�X�`���ԍ�

static PARTICLE						g_ParticleSky[MAX_PARTICLE];		// �p�[�e�B�N�����[�N

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/onpu01.png",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticleSky(void)
{
	// ���_���̍쐬
	MakeVertexParticleSky();

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
	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		g_ParticleSky[i].type = 0;
		g_ParticleSky[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ParticleSky[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ParticleSky[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_ParticleSky[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);

		ZeroMemory(&g_ParticleSky[i].material, sizeof(g_ParticleSky[i].material));
		g_ParticleSky[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_ParticleSky[i].life = 0;
		g_ParticleSky[i].pop = 0.0f;
		g_ParticleSky[i].use = FALSE;
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitParticleSky(void)
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
void UpdateParticleSky(void)
{
	SKY_ENEMY *sky_enemy = GetSkyEnemy();

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		// �p�[�e�B�N�����[�N����
		if(g_ParticleSky[i].use)		// �g�p��
		{
			g_ParticleSky[i].life--;
			
			if (g_ParticleSky[i].life <= SKY_ENEMY_LIFE_ALFA)
			{	// �������P�O�t���[���؂�����i�X�����ɂȂ��Ă���
				g_ParticleSky[i].material.Diffuse.w -= 1.0f / SKY_ENEMY_LIFE_ALFA;
				if (g_ParticleSky[i].material.Diffuse.w < 0.0f)
				{
					g_ParticleSky[i].material.Diffuse.w = 0.0f;
				}
			}

			if (g_ParticleSky[i].life <= 0)
			{
				g_ParticleSky[i].use = FALSE;
			}

			// �ړ�����
			g_ParticleSky[i].pos.x += g_ParticleSky[i].move.x;
			g_ParticleSky[i].pos.y += g_ParticleSky[i].move.y;
			g_ParticleSky[i].pos.z += g_ParticleSky[i].move.z;
		}
	}


	//�G�t�F�N�g�̔�������
	{
		for (int i = 0; i < MAX_SKY_ENEMY; i++)
		{
			if (sky_enemy[i].particleOn)
			{
				sky_enemy[i].particleOn = FALSE;

				for (int p = 0; p < SKY_ENEMY_FLAM_NUM; p++)
				{
					XMFLOAT3 pos, move, scl;
					XMFLOAT4 col;
					int life;

					// �����ʒu��ݒ�
					pos.x = sky_enemy[i].pos.x + RamdomFloat(2, SKY_ENEMY_POP_RAND, -SKY_ENEMY_POP_RAND);
					pos.y = sky_enemy[i].pos.y + RamdomFloat(2, SKY_ENEMY_POP_RAND, -SKY_ENEMY_POP_RAND);
					pos.z = sky_enemy[i].pos.z + RamdomFloat(2, SKY_ENEMY_POP_RAND, -SKY_ENEMY_POP_RAND);

					// �ړ��ʂ�ݒ�
					move.x = RamdomFloat(2, SKY_MOVE_XY, -SKY_MOVE_XY);
					move.z = RamdomFloat(2, SKY_MOVE_XY, -SKY_MOVE_XY);
					move.y = RamdomFloat(2, SKY_MOVE_XY, -SKY_MOVE_XY);

					// �J���[�ݒ�
					col.x = 0.8f;
					col.y = RamdomFloat(2, MAX_SKY_ENEMY_COLOR, MIN_SKY_ENEMY_COLOR);
					col.z = RamdomFloat(2, MAX_SKY_ENEMY_COLOR, MIN_SKY_ENEMY_COLOR);
					col.w = 1.0f;

					// �����̐ݒ�
					life = (rand() % MAX_SKY_ENEMY_LIFE) + MIN_SKY_ENEMY_LIFE;

					scl = { 1.0f,1.0f,1.0f };

					SetParticleSky(pos, move, scl, col, life);
				}
			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawParticleSky(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���C�e�B���O�𖳌���
	SetLightEnable(FALSE);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ADD);

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

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		if(g_ParticleSky[i].use)
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
			mtxScl = XMMatrixScaling(g_ParticleSky[i].scl.x, g_ParticleSky[i].scl.y, g_ParticleSky[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_ParticleSky[i].pos.x, g_ParticleSky[i].pos.y, g_ParticleSky[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_ParticleSky[i].material);

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
HRESULT MakeVertexParticleSky(void)
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
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

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
void SetColorParticleSky(int nIdxParticle, XMFLOAT4 col)
{
	g_ParticleSky[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int SetParticleSky(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 scl, XMFLOAT4 col, int life)
{
	int nIdxParticle = -1;

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		if(!g_ParticleSky[i].use)
		{
			g_ParticleSky[i].pos  = pos;
			g_ParticleSky[i].rot  = { 0.0f, 0.0f, 0.0f };
			g_ParticleSky[i].scl  = { 1.0f, 1.0f, 1.0f };
			g_ParticleSky[i].move = move;
			g_ParticleSky[i].material.Diffuse = col;
			g_ParticleSky[i].life = life;
			g_ParticleSky[i].use  = TRUE;
			

			nIdxParticle = i;

			break;
		}
	}

	return nIdxParticle;
}
