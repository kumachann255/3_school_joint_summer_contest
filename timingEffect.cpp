//=============================================================================
//
// �^�C�~���O�G�t�F�N�g�̏��� [timingEffect.cpp]
// Author : �F�V�`�O
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(30)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(30)	// 
#define TEXTURE_WIDTH_MAX			(100)	// �L�����T�C�Y
#define TEXTURE_HEIGHT_MAX			(100)	// 
#define TEXTURE_MAX					(TEFFECT_TYPE_MAX)		// �e�N�X�`���̐�

#define SIZE_BIG_SPEED				(0.5f)	// �T�C�Y�̑傫���Ȃ�X�s�[�h

#define LIFE_MAX					(30)	// �\������t���[��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/timingEffect_0.png",
	"data/TEXTURE/timingEffect_1.png",
	"data/TEXTURE/timingEffect_2.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�
static TIMINGEFFECT				g_Effect[TEFFECT_MAX];			// ����

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTimingEffect(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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
	
	// �^�C�~���O�e�L�X�g�̏�����
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

	// ���_�o�b�t�@����
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
// �I������
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
// �X�V����
//=============================================================================
void UpdateTimingEffect(void)
{
	for (int i = 0; i < TEFFECT_MAX; i++)
	{
		if (g_Effect[i].use)
		{
			// �ŏI�ʒu�ֈړ�������
			g_Effect[i].size.x += (TEXTURE_WIDTH_MAX - g_Effect[i].size.x) * SIZE_BIG_SPEED;
			g_Effect[i].size.y += (TEXTURE_HEIGHT_MAX - g_Effect[i].size.y) * SIZE_BIG_SPEED;

			// ���������炷
			g_Effect[i].life--;
			if (g_Effect[i].life < 0) g_Effect[i].use = FALSE;
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTimingEffect(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	// �G�t�F�N�g�̕`��
	for (int i = 0; i < TEFFECT_MAX; i++)
	{
		if (g_Effect[i].use)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Effect[i].texNum]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, g_Effect[i].pos.x, g_Effect[i].pos.y, g_Effect[i].size.x, g_Effect[i].size.y, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


// �G�t�F�N�g�̔���
void SetTimingEffect(int tex)
{
	for (int i = 0; i < TEFFECT_MAX; i++)
	{
		if (!g_Effect[i].use)
		{
			// ������
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