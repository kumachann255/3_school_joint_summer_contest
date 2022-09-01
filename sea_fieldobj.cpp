//=============================================================================
//
// �C�I�u�W�F�N�g�̏��� [sea_fieldobj.cpp]
// Author : ��ˏ���
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "debugproc.h"
#include "model.h"
#include "sea_fieldobj.h"
#include "meshfield.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_NAMI			"data/MODEL/kuzira_nami.obj"	// �ǂݍ��ރ��f����
#define	MODEL_KUZIRA		"data/MODEL/kuzira.obj"			// �ǂݍ��ރ��f����
#define	MODEL_SHIO			"data/MODEL/kuzira_sio00.obj"	// �ǂݍ��ރ��f����

#define	VALUE_ROTATE			(0.01f)						// ��]���Z��
#define	VALUE_POSY				(5.0f)						// �㉺���Z��
#define	MAX_FRONT_ROT			(0.15f)						// �ő�O��]�l
#define	MAX_BACK_ROT			(-0.65f)					// �ő���]�l
#define	MAX_LOWPOS_Y			(-100.0f)					// �ő剺�~�l

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static SEA_FOBJ				g_Nami[MAX_SEA_FOBJ_SIGN];		// �g
static SEA_FOBJ				g_Kuzira[MAX_SEA_FOBJ_SIGN];	// �N�W��
static SEA_FOBJ				g_Shio[MAX_SEA_FOBJ_SIGN];		// ��

static BOOL				g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitSeaFieldObj(void)
{
	LoadModel(MODEL_KUZIRA, &g_Kuzira[0].model);
	LoadModel(MODEL_NAMI, &g_Nami[0].model);
	LoadModel(MODEL_SHIO, &g_Shio[0].model);

	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		LoadModel(MODEL_NAMI, &g_Nami[i].model);
		g_Nami[i].load = TRUE;
		g_Nami[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Nami[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Nami[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Nami[i].moveFlag = TRUE;
		g_Nami[i].use = TRUE;			// TRUE:�����Ă�
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Nami[i].model, &g_Nami[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		LoadModel(MODEL_KUZIRA, &g_Kuzira[i].model);
		g_Kuzira[i].load = TRUE;
		g_Kuzira[i].pos = XMFLOAT3(-450.0f + 200.0f * i, 0.0f, 650.0f);
		g_Kuzira[i].rot = XMFLOAT3(0.0f + -0.157f * i, 1.57f, 0.0f);
		g_Kuzira[i].scl = XMFLOAT3(3.5f - 0.5f * i, 3.5f - 0.5f * i, 3.5f - 0.5f * i);
		g_Kuzira[i].moveFlag = TRUE;
		g_Kuzira[i].use = TRUE;			// TRUE:�����Ă�
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Kuzira[i].model, &g_Kuzira[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		LoadModel(MODEL_SHIO, &g_Shio[i].model);
		g_Shio[i].load = TRUE;
		g_Shio[i].pos = XMFLOAT3(-450.0f + 200.0f * i, 0.0f - 4.0f * i, 650.0f);
		g_Shio[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Shio[i].scl = XMFLOAT3(2.0f - 0.25f * i, 2.0f - 0.25f * i, 2.0f - 0.25f * i);
		g_Shio[i].moveFlag = TRUE;
		g_Shio[i].use = TRUE;			// TRUE:�����Ă�
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Shio[i].model, &g_Shio[i].diffuse[0]);
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSeaFieldObj(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Nami[i].load)
		{
			UnloadModel(&g_Nami[i].model);
			g_Nami[i].load = FALSE;
		}
	}

	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Kuzira[i].load)
		{
			UnloadModel(&g_Kuzira[i].model);
			g_Kuzira[i].load = FALSE;
		}
	}

	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Shio[i].load)
		{
			UnloadModel(&g_Shio[i].model);
			g_Shio[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSeaFieldObj(void)
{
	// �g�̏���
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{

	}

	// �N�W���̏���
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		switch (g_Kuzira[i].moveFlag)
		{
		case TRUE:
			g_Kuzira[i].rot.x += VALUE_ROTATE;
			if (g_Kuzira[i].rot.x > MAX_FRONT_ROT)
			{
				g_Kuzira[i].moveFlag = FALSE;
			}
			break;

		case FALSE:
			g_Kuzira[i].rot.x -= VALUE_ROTATE;
			if (g_Kuzira[i].rot.x < MAX_BACK_ROT)
			{
				g_Kuzira[i].moveFlag = TRUE;
			}
			break;
		}
	}
	// ���̏���
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		switch (g_Shio[i].moveFlag)
		{
		case TRUE:
			g_Shio[i].pos.y += VALUE_POSY;
			if (g_Shio[i].pos.y > 0.0f)
			{
				g_Shio[i].moveFlag = FALSE;
			}
			break;

		case FALSE:
			g_Shio[i].pos.y -= VALUE_POSY;
			if (g_Shio[i].pos.y < MAX_LOWPOS_Y)
			{
				g_Shio[i].moveFlag = TRUE;
			}
			break;
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		PrintDebugProc("g_Nami[%d].pos.y:%f\n", i, g_Nami[i].pos.y);
		PrintDebugProc("g_Kuzira[%d].rot.x:%f\n", i, g_Kuzira[i].rot.x);
		PrintDebugProc("g_Shio[%d].pos.y:%f\n", i , g_Shio[i].pos.y);
	}

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSeaFieldObj(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �g�̕`��
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Nami[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Nami[i].scl.x, g_Nami[i].scl.y, g_Nami[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Nami[i].rot.x, g_Nami[i].rot.y + XM_PI, g_Nami[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Nami[i].pos.x, g_Nami[i].pos.y, g_Nami[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Nami[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Nami[0].model);

	}

	// �N�W���̕`��
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Kuzira[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Kuzira[i].scl.x, g_Kuzira[i].scl.y, g_Kuzira[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Kuzira[i].rot.x, g_Kuzira[i].rot.y + XM_PI, g_Kuzira[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Kuzira[i].pos.x, g_Kuzira[i].pos.y, g_Kuzira[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Kuzira[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Kuzira[0].model);

	}

	// ���̕`��
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Shio[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Shio[i].scl.x, g_Shio[i].scl.y, g_Shio[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Shio[i].rot.x, g_Shio[i].rot.y + XM_PI, g_Shio[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Shio[i].pos.x, g_Shio[i].pos.y, g_Shio[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Shio[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Shio[0].model);

	}
}