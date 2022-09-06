//=============================================================================
//
// ���b�N�I���A�C�R������ [rockOn.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "rockOn.h"
#include "debugproc.h"
#include "target.h"
#include "sky_enemy.h"
#include "enemy.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ROCKON		"data/MODEL/rockOnIcon.obj"	// �ǂݍ��ރ��f����

#define	ROCKON_ROT_SPEED	(0.01f)		// ��]���x


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static struct ROCKON	g_RockOn[MAX_ROCKON];
static float			g_rot_y;

static BOOL				g_Load = FALSE;
static int				g_max;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitRockOn(void)
{
	LoadModel(MODEL_ROCKON, &g_RockOn[0].model);

	for (int i = 0; i < MAX_ROCKON; i++)
	{
		g_RockOn[i].load = TRUE;

		g_RockOn[i].use = TRUE;

		g_RockOn[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_RockOn[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_RockOn[i].scl = XMFLOAT3(10.0f, 10.0f, 10.0f);

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_RockOn[i].model, &g_RockOn[i].diffuse[0]);

		g_RockOn[i].load = TRUE;

		g_RockOn[i].use = FALSE;

		g_RockOn[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_RockOn[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_RockOn[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_RockOn[i].model, &g_RockOn[i].diffuse[0]);

	}
	
	g_max = MAX_ROCKON;
	if (GetMode() == MODE_GAME_SEA) g_max = MAX_ROCKON_SEA;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitRockOn(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ROCKON; i++)
	{
		if (g_RockOn[i].load)
		{
			UnloadModel(&g_RockOn[i].model);
			g_RockOn[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateRockOn(void)
{
	TARGET *target = GetTarget();

	if (GetMode() != MODE_GAME_SKY)
	{
		ENEMY *enemy = GetEnemy();

		for (int i = 0; i < g_max; i++)
		{
			if (g_RockOn[i].use)
			{
				g_RockOn[i].pos = enemy[target[0].enemyNum[i]].pos;

				g_RockOn[i].rot.x += ROCKON_ROT_SPEED;
				g_RockOn[i].rot.z += ROCKON_ROT_SPEED;

				//if (!enemy[target[0].enemyNum[i]].use) g_aRockOn[i].bUse = FALSE;
			}
		}
	}
	else 
	{
		SKY_ENEMY *enemy = GetSkyEnemy();

		for (int i = 0; i < g_max; i++)
		{
			if (g_RockOn[i].use)
			{
				g_RockOn[i].pos = enemy[target[0].enemyNum[i]].pos;

				g_RockOn[i].rot.x += ROCKON_ROT_SPEED;
				g_RockOn[i].rot.z += ROCKON_ROT_SPEED;

				//if (!enemy[target[0].enemyNum[i]].use) g_aRockOn[i].bUse = FALSE;
			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawRockOn(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < g_max; i++)
	{
		if (g_RockOn[i].use == FALSE) return;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_RockOn[i].scl.x, g_RockOn[i].scl.y, g_RockOn[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_RockOn[i].rot.x, g_RockOn[i].rot.y, g_RockOn[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_RockOn[i].pos.x, g_RockOn[i].pos.y, g_RockOn[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_RockOn[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_RockOn[0].model);
	}
}


//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ROCKON *GetRockOn()
{
	return &g_RockOn[0];
}


void SetRockOn(void)
{
	for (int i = 0; i < g_max; i++)
	{
		if (!g_RockOn[i].use)
		{
			g_RockOn[i].use = TRUE;

			return;
		}
	}
}


// ���b�N�I���^�[�Q�b�g�̈ꊇ�폜
void ResetRockOn(void)
{
	for (int i = 0; i < g_max; i++)
	{
		g_RockOn[i].use = FALSE;
	}
}