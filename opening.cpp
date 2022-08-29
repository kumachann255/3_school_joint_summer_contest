//=============================================================================
//
// �I�[�v�j���O��ʏ��� [opening.cpp]
// Author : �G�i
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "opening.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(5)				// �e�N�X�`���̐�

#define PAGE_FADE_RATE				(0.02f)			// �t�F�[�h�W��
#define NEXT_PAGE					(150)			// �����Ŏ��̃y�[�W�ɕς�鎞��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/fade_black.png",
	"data/TEXTURE/opening_001.png",
	"data/TEXTURE/opening_002.png",
	"data/TEXTURE/opening_003.png",
	"data/TEXTURE/opening_004.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_PageFade;					// �t�F�[�h�̏��
static XMFLOAT4					g_PageColor;				// �t�F�[�h�̃J���[�i���l�j

static int						g_Count;						// �o�ߎ���

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitOpening(void)
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = OPENING_PAGE_1;

	g_PageFade = FADE_IN;
	g_PageColor = { 1.0, 0.0, 0.0, 1.0 };
	
	g_Count = 0;

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_bgm_title);

	// ������


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitOpening(void)
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
void UpdateOpening(void)
{
	// �t�F�[�h����
	{
		if (g_PageFade != FADE_NONE)
		{// �t�F�[�h������
			if (g_PageFade == FADE_OUT)
			{// �t�F�[�h�A�E�g����
				g_PageColor.w += PAGE_FADE_RATE;		// ���l�����Z���ĉ�ʂ������Ă���
				
				if (g_PageColor.w >= 1.0f)
				{
					g_PageColor.w = 1.0f;

					// �t�F�[�h�C�������ɐ؂�ւ�
					g_PageFade = FADE_IN;
					
					// �e�N�X�`���ԍ���ݒ�
					g_TexNo++;
				}

			}

			else if (g_PageFade == FADE_IN)
			{// �t�F�[�h�C������
				g_PageColor.w -= PAGE_FADE_RATE;		// ���l�����Z���ĉ�ʂ𕂂��オ�点��
				
				if (g_PageColor.w <= 0.0f)
				{
					g_PageColor.w = 0.0f;

					// �t�F�[�h�����I��
					g_PageFade = FADE_NONE;
					g_Count = 0;
				}
			}
		}
	}


	// �L�[���͏���
	{
		if (GetKeyboardTrigger(DIK_RETURN))
		{// Enter��������A�X�e�[�W��؂�ւ���

			// SE�̑}���i���[�h�I�����j
			PlaySound(SOUND_LABEL_SE_selectBomb01);

			g_PageFade = FADE_OUT;

			if (g_TexNo == (OPENING_PAGE_MAX - 1))
			{// ���X�g�V�[��
				g_PageFade = FADE_NONE;

				SetStage(tutorial);
				SetFade(FADE_OUT, MODE_GAME_CITY);
			
				// main�X�R�A��������
				ResetMainScore();
			}
		}

		else if (IsButtonTriggered(0, BUTTON_B))
		{
			// SE�̑}���i���[�h�I�����j
			PlaySound(SOUND_LABEL_SE_selectBomb01);

			g_PageFade = FADE_OUT;

			if (g_TexNo == (OPENING_PAGE_MAX - 1))
			{// ���X�g�V�[��
				g_PageFade = FADE_NONE;

				SetStage(tutorial);
				SetFade(FADE_OUT, MODE_GAME_CITY);

				// main�X�R�A��������
				ResetMainScore();
			}
		}
	}

	g_Count++;

	if (g_Count == NEXT_PAGE)
	{
		// SE�̑}���i���[�h�I�����j
		PlaySound(SOUND_LABEL_SE_selectBomb01);

		g_PageFade = FADE_OUT;

		if (g_TexNo == (OPENING_PAGE_MAX - 1))
		{// ���X�g�V�[��
			g_PageFade = FADE_NONE;

			SetStage(tutorial);
			SetFade(FADE_OUT, MODE_GAME_CITY);

			// main�X�R�A��������
			ResetMainScore();
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawOpening(void)
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

	// �I�[�v�j���O�̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �t�F�[�h��ʂ�`��
	{
		if (g_PageFade == FADE_NONE) return;	// �t�F�[�h���Ȃ��̂Ȃ�`�悵�Ȃ�

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[OPENING_PAGE_FADE]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH / 2, TEXTURE_WIDTH / 2, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f,
			g_PageColor);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


}





