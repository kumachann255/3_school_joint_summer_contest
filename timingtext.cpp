//=============================================================================
//
// �^�C�~���O�e�L�X�g�̏��� [timingtext.cpp]
// Author : �F�V�`�O
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "timingtext.h"
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
#define TEXTURE_WIDTH				(100)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(35)	// 
#define TEXTURE_MAX					(3)		// �e�N�X�`���̐�

#define START_X						(TARGET_X)	// �e�L�X�g�̍ŏ��̕\���ʒu
#define START_Y						(TARGET_Y + 10.0f)	// �e�L�X�g�̍ŏ��̕\���ʒu
#define END_Y						(START_Y - 50.0f)	// �e�L�X�g�̍ŏI�I�ȕ\���ʒu
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
	"data/TEXTURE/hyouka_miss.png",
	"data/TEXTURE/hyouka_good.png",
	"data/TEXTURE/hyouka_perfect.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�
static TIMINGTEXT				g_Text[TTEXT_MAX];		// ����

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTimingText(void)
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
	for (int i = 0; i < TTEXT_MAX; i++)
	{
		g_Text[i].pos.x = START_X;
		g_Text[i].pos.y = START_Y;
		g_Text[i].use = FALSE;
		g_Text[i].texNum = 0;
		g_Text[i].life = LIFE_MAX;
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
void UninitTimingText(void)
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
void UpdateTimingText(void)
{
	for (int i = 0; i < NOTE_MAX; i++)
	{
		if (g_Text[i].use)
		{
			// �ŏI�ʒu�ֈړ�������
			g_Text[i].pos.y -= (g_Text[i].pos.y - END_Y) * 0.9f;

			// ���������炷
			g_Text[i].life--;
			if (g_Text[i].life < 0) g_Text[i].use = FALSE;
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTimingText(void)
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


	// �e�L�X�g�̕`��
	for (int i = 0; i < TTEXT_MAX; i++)
	{
		if (g_Text[i].use)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Text[i].texNum]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, g_Text[i].pos.x, g_Text[i].pos.y, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


// �����̔���
void SetTimingText(int text)
{
	for (int i = 0; i < TTEXT_MAX; i++)
	{
		if (!g_Text[i].use)
		{
			// ������
			g_Text[i].use = TRUE;
			g_Text[i].pos = XMFLOAT2(START_X, START_Y);
			g_Text[i].texNum = text;
			g_Text[i].life = LIFE_MAX;

			return;
		}
	}
}