//=============================================================================
//
// �^�C�~���O�o�[�̏��� [timingBar.cpp]
// Author : �F�V�`�O
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "timingBar.h"
#include "sprite.h"
#include "model.h"
#include "player.h"
#include "score.h"
#include "gameUI.h"
#include "time.h"
#include "sound.h"
#include "combo.h"
#include "timingEffect.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SENHU_TEXTURE_WIDTH			(860)	// �L�����T�C�Y
#define SENHU_TEXTURE_HEIGHT		(150)	// 
#define TEXTURE_MAX					(10)	// �e�N�X�`���̐�

#define SENHU_X						(960.0f / 2.0f)	// �ܐ����̕\���ʒu
#define SENHU_Y						(70.0f)	// �ܐ����̕\���ʒu

#define ONPU_TEXTURE_X				(40.0f)	//�����̕�
#define ONPU_TEXTURE_Y				(60.0f)//�����̍���
#define ONPU_X						(1000.0f)	//�����̕\���ʒu
#define ONPU_Y						(SENHU_Y)//�����̕\���ʒu

#define TARGET_TEXTURE_X			(100.0f)	// �^�[�Q�b�g�̃T�C�Y
#define TARGET_TEXTURE_Y			(100.0f)

#define ONPU_SPEED					(5.0f)	// �����̑��x

#define DISTANCE_ONPU_CITY			(40)	// �X�X�e�[�W�̉����̊Ԋu(�t���[��)
#define DISTANCE_ONPU_SEA			(40)	// �C�X�e�[�W�̉����̊Ԋu(�t���[��)
#define DISTANCE_ONPU_SKY			(40)	// ��X�e�[�W�̉����̊Ԋu(�t���[��)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/flat.png",
	"data/TEXTURE/sharp.png",
	"data/TEXTURE/8bu_onpu.png",
	"data/TEXTURE/16bu_onpu.png",
	"data/TEXTURE/6bu_onpu_renkou.png",
	"data/TEXTURE/8bu_onpu_renkou.png",
	"data/TEXTURE/8bu_kyuufu.png",
	"data/TEXTURE/16bu_kyuufu.png",
	"data/TEXTURE/gosenhu.png",
	"data/TEXTURE/onpuTarget.png",
	
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�
static TIMINGNOTE				g_Note[NOTE_MAX];			// ����
static TIMINGNOTE				g_Target;					// �����������^�C�~���O�̃e�N�X�`��
static int						g_Time;						// ����
static int						g_Distance;

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTImingBar(void)
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

	for (int i = 0; i < NOTE_MAX; i++)
	{
		g_Note[i].pos.x = ONPU_X;
		g_Note[i].pos.y = ONPU_Y;
		g_Note[i].use = FALSE;
		g_Note[i].texNum = 0;
	}

	g_Target.pos.x = TARGET_X;
	g_Target.pos.y = TARGET_Y;
	g_Target.use = TRUE;
	g_Target.texNum = 9;


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �v���C���[�̏�����
	g_Use   = TRUE;
	g_w     = SENHU_TEXTURE_WIDTH;
	g_h     = SENHU_TEXTURE_HEIGHT;
	g_Pos   = { SENHU_X, SENHU_Y, 0.0f };
	g_TexNo = 0;

	// ���Ԃ̏�����
	g_Time = 0;

	switch (GetMode())
	{
	case MODE_GAME_CITY:
		g_Distance = DISTANCE_ONPU_CITY;

		break;

	case MODE_GAME_SEA:
		g_Distance = DISTANCE_ONPU_SEA;

		break;

	case MODE_GAME_SKY:
		g_Distance = DISTANCE_ONPU_SKY;

		break;

	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTImingBar(void)
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
void UpdateTImingBar(void)
{
	for (int i = 0; i < NOTE_MAX; i++)
	{
		if (g_Note[i].use)
		{
			// ���ֈړ�������
			g_Note[i].pos.x -= ONPU_SPEED;
			if (g_Note[i].pos.x < -50.0f) g_Note[i].use = FALSE;

		}
	}


	// �����̏o��
	g_Time++;

	// ��莞�Ԃŉ������o��������
	if (g_Time % g_Distance == 0)
	{
		SetNote();
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTImingBar(void)
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


	// �ܐ����̕`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	// �^�[�Q�b�g�̕`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Target.texNum]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, g_Target.pos.x, g_Target.pos.y, TARGET_TEXTURE_X, TARGET_TEXTURE_Y, 0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	// �����̕`��
	for (int i = 0; i < NOTE_MAX; i++)
	{
		if (g_Note[i].use)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Note[i].texNum]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, g_Note[i].pos.x, g_Note[i].pos.y, ONPU_TEXTURE_X, ONPU_TEXTURE_Y, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


// �����̔���
void SetNote(void)
{
	for (int i = 0; i < NOTE_MAX; i++)
	{
		if (!g_Note[i].use)
		{
			// ������
			g_Note[i].use = TRUE;
			g_Note[i].pos = XMFLOAT2(ONPU_X, ONPU_Y);
			g_Note[i].texNum = rand() % NOTE_TEX_MAX;
			
			return;
		}
	}
}


// �W���X�g�^�C�~���O���ǂ����𔻒f
// �߂�l��0�Ȃ�~�X(�͈͓��Ń~�X�����特����������)�A1�Ȃ�GOOD�A2�Ȃ�JUST
int GetNoteTiming(void)
{
	int ans = 0;

	for (int i = 0; i < NOTE_MAX; i++)
	{
		if (g_Note[i].use)
		{
			// �~�X���ǂ���
			if ((g_Note[i].pos.x > g_Target.pos.x - NOTE_TIMING_MISS) && (g_Note[i].pos.x < g_Target.pos.x + NOTE_TIMING_MISS))
			{
				
				// �W���X�g�^�C�~���O���ǂ���
				if ((g_Note[i].pos.x > g_Target.pos.x - NOTE_TIMING_GOOD) && (g_Note[i].pos.x < g_Target.pos.x + NOTE_TIMING_GOOD))
				{
					// �ǂ�
					ans++;

					if ((g_Note[i].pos.x > g_Target.pos.x - NOTE_TIMING_JUST) && (g_Note[i].pos.x < g_Target.pos.x + NOTE_TIMING_JUST))
					{
						// �ō�
						ans++;
					}
				}

				g_Note[i].use = FALSE;
				break;
			}
		}
	}

	// �ō��̃^�C�~���O�������犽���𗬂�
	if (ans >= 2) PlaySound(SOUND_LABEL_SE_cheers02);

	// �ǂ��ȏ�̎��ɃG�t�F�N�g�𔭐�(�G�t�F�N�g�̎�ނ̓����_��)
	if (ans >= 1) SetTimingEffect(rand() % TEFFECT_TYPE_MAX);

	// �ǂ��ȏ�ł���΃R���{��ݒ�
	if (ans >= 1) 
	{
		SetTimingHas(TRUE);
		ResetComboTime();	// �R���{�p�����Ԃ����Z�b�g
	}
	else SetTimingHas(FALSE);

	return ans;
}