//=============================================================================
//
// �h�[������ [dome.cpp]
// Author : �F�V
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "dome.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_DOME_CITY			"data/MODEL/wakusei.obj"				// �ǂݍ��ރ��f����
#define	MODEL_DOME_UNIVERSE		"data/MODEL/dome_universe.obj"			// �ǂݍ��ރ��f����
#define	SKY_OFFSET_Y			(XM_PI/15000)							// ��]���x

#define DOME_SPEED				(1.0f)					// �h�[�����������鑬�x


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static DOME				g_Dome[MAX_DOME];						// �w�i
static float			g_rot_y;

static BOOL				g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitDome(void)
{
	LoadModel(MODEL_DOME_CITY, &g_Dome[0].model);
	LoadModel(MODEL_DOME_UNIVERSE, &g_Dome[1].model);

	for (int i = 0; i < MAX_DOME; i++)
	{
		g_Dome[i].load = TRUE;

		g_Dome[i].use = TRUE;

		g_Dome[i].pos = XMFLOAT3(0.0f, -800.0f, 0.0f);
		g_Dome[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Dome[i].scl = XMFLOAT3(80.0f, 80.0f, 80.0f);

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Dome[0].model, &g_Dome[0].diffuse[0]);
	}
	
	g_rot_y = SKY_OFFSET_Y;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitDome(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_DOME; i++)
	{
		if (g_Dome[i].load)
		{
			UnloadModel(&g_Dome[i].model);
			g_Dome[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateDome(void)
{
	// �h�[����������
	// �v���C���[����ɏオ���Ă�悤�Ɍ�����
	g_Dome[0].pos.y -= DOME_SPEED;

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawDome(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// �t�H�O����
	SetFogEnable(FALSE);

	for (int i = 0; i < MAX_DOME; i++)
	{
		if (g_Dome[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Dome[i].scl.x, g_Dome[i].scl.y, g_Dome[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Dome[i].rot.x, g_Dome[i].rot.y, g_Dome[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Dome[i].pos.x, g_Dome[i].pos.y, g_Dome[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Dome[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Dome[i].model);
	
	}

	// �t�H�O�ݒ��߂�
	SetFogEnable(FALSE);


	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);

}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
DOME *GetDome()
{
	return &g_Dome[0];
}
