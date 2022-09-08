//=============================================================================
//
// �^�[�Q�b�gUI���� [target.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "target.h"
#include "model.h"
#include "enemy.h"
#include "player.h"
#include "camera.h"
#include "targetObj.h"
#include "camera.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(120)			// �Ə��̃T�C�Y��
#define TEXTURE_HEIGHT				(TEXTURE_WIDTH)	// �Ə��̃T�C�Y�c
#define TEXTURE_MAX					(2)				// �e�N�X�`���̐�
#define	VALUE_MOVE					(4.0f)			// �ړ���

#define OFFSET_Y					(6.0f)			// ����
#define OFFSET_OBJ					(10.0f)



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
XMVECTOR Screenpos(XMVECTOR World_Pos);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/target_outsite.png",
	"data/TEXTURE/target_insite.png",

};

static TARGET g_Target[MAX_TARGET];

BOOL		  g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTarget(void)
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

	for (int i = 0; i < MAX_TARGET; i++)
	{
		// �ϐ��̏�����
		g_Target[i].w = TEXTURE_WIDTH;
		g_Target[i].h = TEXTURE_HEIGHT;
		g_Target[i].pos = XMFLOAT3(60.0f, 60.0f, 0.0f);
		g_Target[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Target[i].spd = 4.0f;
		g_Target[i].TexNo = i;
		g_Target[i].use = TRUE;
		g_Target[i].count = 0;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTarget(void)
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
void UpdateTarget(void)
{
	// �U�����@���T�[�N���̏ꍇ�̓X�L�b�v
	if (!GetPlayer()->rockOn) return;

	for (int i = 0; i < MAX_TARGET; i++)
	{
		if (!g_Target[i].use) return;

		// ���[���h���W���X�N���[�����W�֕ϊ�
		TARGETOBJ *targetObj = GetTargetObj();
		XMVECTOR pPos = XMLoadFloat3(&targetObj->pos);
		XMVECTOR ans = Screenpos(pPos);

		XMStoreFloat3(&g_Target[i].pos, ans);

	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTarget(void)
{
	// �U�����@���T�[�N���̏ꍇ�̓X�L�b�v
	if (!GetPlayer()->rockOn) return;

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

	// �^�[�Q�b�g�̕`��
	for (int i = 0; i < MAX_TARGET; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

		SetSpriteColorRotation(g_VertexBuffer, g_Target[i].pos.x, g_Target[i].pos.y, g_Target[i].w, g_Target[i].h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), g_Target[i].rot.z);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}

//=============================================================================
// �Ə������擾
//=============================================================================
TARGET *GetTarget(void)
{
	return &g_Target[0];
}


//=============================================================================
// ���[���h���W���X�N���[�����W�֕ϊ�
//=============================================================================
XMVECTOR Screenpos(XMVECTOR World_Pos)
{
	CAMERA *camera = GetCamera();

	XMVECTOR Eye = XMVectorSet(camera->pos.x, camera->pos.y, camera->pos.z, 0.0f);
	XMVECTOR At = XMVectorSet(camera->at.x, camera->at.y, camera->at.z, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX g_View = XMMatrixLookAtLH(Eye, At, Up);

	//XMMATRIX g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, 0.1f, 1000.0f);
	XMMATRIX g_Projection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	float w = (FLOAT)SCREEN_WIDTH / 2.0f;
	float h = (FLOAT)SCREEN_HEIGHT / 2.0f;
	XMMATRIX viewport = {
	w, 0, 0, 0,
	0, -h, 0, 0,
	0, 0, 1, 0,
	w, h, 0, 1
	};

	// �r���[�ϊ��ƃv���W�F�N�V�����ϊ�
	World_Pos = XMVector3Transform(World_Pos, g_View);
	World_Pos = XMVector3Transform(World_Pos, g_Projection);

	XMFLOAT3 temp;
	XMStoreFloat3(&temp, World_Pos);
	// z�Ŋ�����-1~1�͈̔͂Ɏ��߂�
	// �X�N���[���ϊ�
	const XMVECTOR view_vec = XMVectorSet(temp.x / temp.z, temp.y / temp.z, 1.0f, 1.0f);
	return XMVector3Transform(view_vec, viewport);
}


//=============================================================================
// �^�[�Q�b�g�A�C�R������ʊO�ɏo�Ă��Ȃ������m�F
//=============================================================================
BOOL GetTargetArea(int type)
{
	BOOL ans = TRUE;

	for (int i = 0; i < MAX_TARGET; i++)
	{
		switch (type)
		{
		case left:
			if (g_Target[i].pos.x - TEXTURE_WIDTH * 0.5f < 0.0f)
			{
				ans = FALSE;
			}

			break;

		case right:
			if (g_Target[0].pos.x + TEXTURE_WIDTH * 0.5f > SCREEN_WIDTH)
			{
				ans = FALSE;
			}

			break;

		case up:
			if (g_Target[i].pos.y - TEXTURE_HEIGHT * 0.5f < 0.0f)
			{
				ans = FALSE;
			}

			break;

		case down:
			if (g_Target[i].pos.y + TEXTURE_HEIGHT * 0.5f > SCREEN_HEIGHT)
			{
				ans = FALSE;
			}

			break;
		}
	}

	return ans;
}