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
#include "blast.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX				(22)				// �e�N�X�`���̐�

#define	PARTICLE_SIZE_X			(30.0f)				// ���_�T�C�Y�E�p�[�e�B�N���T�C�Y
#define	PARTICLE_SIZE_Y			(30.0f)				// ���_�T�C�Y�E�p�[�e�B�N���T�C�Y

#define	MAX_PARTICLE			(512)				// �p�[�e�B�N���ő吔

#define	MAX_PARTICLE_DEVIDE		(100)				// �p�[�e�B�N���ő吔


#define	ADRESS_BLAST			(0)					// �g���镪�̐擪�A�h���X(�l�o�l�o)
#define	MAX_PARTICLE_BLAST		(5)					// �p�[�e�B�N�����[�N(�l�o�l�o)


#define	ADRESS_CUP				(100)				// �g���镪�̐擪�A�h���X(�J�b�v����)
#define	MAX_PARTICLE_CUP		(5)					// �p�[�e�B�N�����[�N(�J�b�v����)


#define	ADRESS_CUP_NOTE			(200)				// �g���镪�̐擪�A�h���X(�J�b�v����)
#define	MAX_PARTICLE_CUP_NOTE	(1)					// �p�[�e�B�N�����[�N(�J�b�v����)

#define	CUP_NOTE_RADIAN			(XM_2PI * 0.02f)	// �J�b�v�����̃��W�A��
#define	CUP_NOTE_RADIUS			(2.0f)				// �J�b�v�����ړ���
#define	CUP_NOTE_POP			(2)				// �J�b�v�����̐�

#define	DISP_SHADOW									// �e�̕\��
#undef DISP_SHADOW


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticle(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;				// ���_�o�b�t�@

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static PARTICLE						g_Particle[MAX_PARTICLE];	// �p�[�e�B�N�����[�N

static int							g_CupNotePop;						// �J�b�v�����̐��𐧌��p

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


	// �p�[�e�B�N�����[�N�̏�����
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
			
			if (g_Particle[i].life <= 20)
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
			}


			// �ړ�����
			g_Particle[i].pos.x += g_Particle[i].move.x;
			g_Particle[i].pos.y += g_Particle[i].move.y;
			g_Particle[i].pos.z += g_Particle[i].move.z;

			// �\���^�C�~���O�̏���
			if (g_Particle[i].pop > 0.0f)
			{
				g_Particle[i].pop--;
			}


			// �J�b�v�����̍X�V����
			if (g_Particle[i].type == PARTICLE_TYPE_CUP)
			{
				// �g�又��
				g_Particle[i].scl.x += RamdomFloat(2, 0.2f, 0.1f);
				g_Particle[i].scl.y += RamdomFloat(2, 0.2f, 0.1f);
			}


			// �J�b�v�����̍X�V����
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

	// �J�b�v�����pPOP�����Z�b�g
	g_CupNotePop--;
	if (g_CupNotePop < 0)
	{
		g_CupNotePop = CUP_NOTE_POP;
	}

	//�G�t�F�N�g�̔��������i�J�b�v�����j
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
				col.y = RamdomFloat(2, 1.0f, 0.5f);
				col.z = RamdomFloat(2, 1.0f, 0.5f);
				col.w = RamdomFloat(2, 1.0f, 0.5f);

				SetParticle(ADRESS_CUP, PARTICLE_TYPE_CUP, 0, pos, scl, move, col, 5.0f, 60);
			}
		}
	}

	//�G�t�F�N�g�̔��������i�J�b�v�����j
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

				// �e�N�X�`���ԍ���ݒ�
				texNo = rand() % 21 + 1;

				// �����ʒu��ݒ�
				pos.x = cup[0].pos.x + RamdomFloat(2, 10.0f, -10.0f);
				pos.y = cup[0].pos.y + RamdomFloat(2, 30.0f, 20.0f);
				pos.z = cup[0].pos.z + RamdomFloat(2, 10.0f, -10.0f);


				// �ړ��ʐݒ�
				move.x = 0.0f;
				move.y = 1.3f;
				move.z = 0.0f;

				g_Particle[j].radian = 0.0f;

				SetParticle(ADRESS_CUP_NOTE, PARTICLE_TYPE_CUP_NOTE, texNo, pos, scl, move, col, 0.0f, 80);
			}
		}
	}

	//�G�t�F�N�g�̔��������i�l�o�l�o�����j
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

				// �e�N�X�`���ԍ���ݒ�
				texNo = rand() % 21 + 1;

				// �����ʒu��ݒ�
				pos.x = blast[0].pos.x;
				pos.y = blast[0].pos.y;
				pos.z = blast[0].pos.z;

				// �ړ��ʐݒ�
				move.x = RamdomFloat(2, 0.7f, -0.7f);
				move.y = RamdomFloat(2, 0.7f, -0.7f);
				move.z = RamdomFloat(2, 0.7f, -0.7f);

				SetParticle(ADRESS_BLAST, PARTICLE_TYPE_BLAST, texNo, pos, scl, move, col, 1.0f, rand()%21 + 60);
			}
		}
	}
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

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Particle[i].texNo]);

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
// �p�[�e�B�N���̔�������(int �擪�A�h���X, int ���, int �e�N�X�`���ԍ�,
//						  XMFLOAT3 ���W, XMFLOAT3 �T�C�Y, XMFLOAT3 �ړ���
//						  XMFLOAT4 �F, float pop, int life)
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
