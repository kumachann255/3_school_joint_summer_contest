//=============================================================================
//
// �p�[�e�B�N������ [particle.cpp]
// Author : �F�V�`�O�{�G�i
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

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)			// �e�N�X�`���̐�

#define	PARTICLE_SIZE_X		(30.0f)		// ���_�T�C�Y�E�p�[�e�B�N���T�C�Y
#define	PARTICLE_SIZE_Y		(30.0f)		// ���_�T�C�Y�E�p�[�e�B�N���T�C�Y

#define	MAX_PARTICLE		(512)		// �p�[�e�B�N���ő吔

#define	MAX_PARTICLE_CUP	(10)			// �p�[�e�B�N���ő吔(�J�b�v)

#define	DISP_SHADOW						// �e�̕\��
#undef DISP_SHADOW


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticle(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;					// �e�N�X�`���ԍ�

static PARTICLE					g_Particle[MAX_PARTICLE];		// �p�[�e�B�N�����[�N

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.png",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticle(void)
{
	// ���_���̍쐬
	MakeVertexParticle();

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
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitParticle(void)
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
void UpdateParticle(void)
{
	//PLAYER *pPlayer = GetPlayer();
	//g_posBase = pPlayer->pos;
	
	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		// �p�[�e�B�N�����[�N����
		if(g_Particle[i].use == TRUE)		// �g�p��
		{
			g_Particle[i].life--;
			
			if (g_Particle[i].life <= 10)
			{	// �������P�O�t���[���؂�����i�X�����ɂȂ��Ă���
				g_Particle[i].material.Diffuse.w -= 0.1f;
				if (g_Particle[i].material.Diffuse.w < 0.0f)
				{
					g_Particle[i].material.Diffuse.w = 0.0f;
				}
			}

			if (g_Particle[i].life <= 0)
			{
				g_Particle[i].use = FALSE;
				ReleaseShadow(g_Particle[i].nIdxShadow);
				g_Particle[i].nIdxShadow = -1;
			}
			//else
			//{
			//	if(g_Particle[i].life <= 80)
			//	{
			//		g_Particle[i].material.Diffuse.x = 0.8f - (float)(80 - g_Particle[i].life) / 80 * 0.8f;
			//		g_Particle[i].material.Diffuse.y = 0.7f - (float)(80 - g_Particle[i].life) / 80 * 0.7f;
			//		g_Particle[i].material.Diffuse.z = 0.2f - (float)(80 - g_Particle[i].life) / 80 * 0.2f;
			//	}

			//	if(g_Particle[i].life <= 20)
			//	{
			//		// ���l�ݒ�
			//		g_Particle[i].material.Diffuse.w -= 0.05f;
			//		if(g_Particle[i].material.Diffuse.w < 0.0f)
			//		{
			//			g_Particle[i].material.Diffuse.w = 0.0f;
			//		}
			//	}
			//}

			// �\���^�C�~���O�̏���
			if (g_Particle[i].pop > 0.0f)
			{
				g_Particle[i].pop--;
			}


			// �J�b�v�̏ꍇ�̏���
			if (g_Particle[i].type == PARTICLE_TYPE_CUP)
			{
				// �ړ�������
				//g_Particle[i].pos.x += RamdomFloat(2, 3.0f, -3.0f);
				//g_Particle[i].pos.y += RamdomFloat(2, 3.0f, -3.0f);
				//g_Particle[i].pos.z += RamdomFloat(2, 3.0f, -3.0f);

				// �g�又��
				g_Particle[i].scl.x += RamdomFloat(2, 0.2f, 0.1f);
				g_Particle[i].scl.y += RamdomFloat(2, 0.2f, 0.1f);
			}




#ifdef DISP_SHADOW
			if(g_Particle[i].nIdxShadow != -1)
			{// �e�g�p��
				float colA;

				// �e�̈ʒu�ݒ�
				SetPositionShadow(g_Particle[i].nIdxShadow, XMFLOAT3(g_Particle[i].pos.x, 0.1f, g_Particle[i].pos.z));

				// �e�̐F�̐ݒ�
				colA = g_Particle[i].material.Diffuse.w;
				SetColorShadow(g_Particle[i].nIdxShadow, XMFLOAT4(0.5f, 0.5f, 0.5f, colA));
			}
#endif
		}
	}


	//�G�t�F�N�g�̔��������i�J�b�v�j
	{
		BOOL particleOn = GetParticleOn();

		if (particleOn == TRUE)
		{
			CUP *cup = GetCup();

			for (int j = 0; j < MAX_PARTICLE_CUP; j++)
			{
				XMFLOAT3 pos;
				XMFLOAT3 scl;
				XMFLOAT4 col;

				// �����ʒu��ݒ�
				pos.x = cup[0].pos.x + RamdomFloat(2, 40.0f, -40.0f);
				pos.y = cup[0].pos.y + RamdomFloat(2, 60.0f,  10.0f);
				pos.z = cup[0].pos.z + RamdomFloat(2, 40.0f, -40.0f);

				// �T�C�Y�ݒ�
				scl.x = RamdomFloat(2, 0.5f, 0.1f);
				scl.y = RamdomFloat(2, 0.5f, 0.1f);
				scl.z = RamdomFloat(2, 0.5f, 0.1f);

				// �J���[�ݒ�
				col.x = 1.0f;
				col.y = RamdomFloat(2, 0.8f, 0.6f);
				col.z = RamdomFloat(2, 0.7f, 0.3f);
				col.w = RamdomFloat(2, 1.0f, 0.8f);

				SetParticle(PARTICLE_TYPE_CUP, pos, scl, col, 5.0f,50);
			}
		}
	}

	//// �p�[�e�B�N������
	//{
	//	XMFLOAT3 pos;
	//	XMFLOAT3 move;
	//	float fAngle, fLength;
	//	int nLife;
	//	float fSize;

	//	pos = g_posBase;

	//	fAngle = (float)(rand() % 628 - 314) / 100.0f;
	//	fLength = rand() % (int)(g_fWidthBase * 200 ) / 100.0f - g_fWidthBase;
	//	move.x = sinf(fAngle) * fLength;
	//	move.y = rand() % 300 / 100.0f + g_fHeightBase;
	//	move.z = cosf(fAngle) * fLength;

	//	nLife = rand() % 100 + 150;  

	//	fSize = (float)(rand() % 30 + 20);

	//	pos.y = fSize / 2;

	//	// �r���{�[�h�̐ݒ�
	//	SetParticle(pos, move, XMFLOAT4(0.8f, 0.7f, 0.2f, 0.85f), fSize, fSize, nLife);
	//}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawParticle(void)
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

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		if(g_Particle[i].use == TRUE && g_Particle[i].pop <= 0.0f)
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
			mtxScl = XMMatrixScaling(g_Particle[i].scl.x, g_Particle[i].scl.y, g_Particle[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Particle[i].pos.x, g_Particle[i].pos.y, g_Particle[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_Particle[i].material);

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
HRESULT MakeVertexParticle(void)
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
void SetColorParticle(int nIdxParticle, XMFLOAT4 col)
{
	g_Particle[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int SetParticle(int type, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT4 col, float pop, int life)
{
	int nIdxParticle = -1;

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		if(!g_Particle[i].use)
		{
			g_Particle[i].type = type;
			g_Particle[i].pos  = pos;
			g_Particle[i].rot  = { 0.0f, 0.0f, 0.0f };
			g_Particle[i].scl  = scl;
			g_Particle[i].move = { 0.0f, 0.0f, 0.0f };
			g_Particle[i].material.Diffuse = col;
			g_Particle[i].life = life;
			g_Particle[i].pop = pop * i;
			g_Particle[i].use  = TRUE;

			nIdxParticle = i;

#ifdef DISP_SHADOW
			// �e�̐ݒ�
			g_Particle[i].nIdxShadow = CreateShadow(XMFLOAT3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// �e�̐ݒ�
			if(g_Particle[i].nIdxShadow != -1)
			{
				SetColorShadow(g_Particle[i].nIdxShadow, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			break;
		}
	}

	return nIdxParticle;
}
