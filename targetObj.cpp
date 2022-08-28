//=============================================================================
//
// �^�[�Q�b�g�p�̋�I�u�W�F�N�g���� [targetObj.cpp]
// Author : �F�V
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "targetObj.h"
#include "debugproc.h"
#include "target.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_DOME_CITY			"data/MODEL/collisionBox.obj"	// �ǂݍ��ރ��f����

#define MOVE_VALUE_Y			(3.0f)		// �^�[�Q�b�g���c�ɓ������x
#define MOVE_VALUE_X			(0.01f)		// �^�[�Q�b�g�����ɓ������x

#define OBJ_DISTANCE			(700.0f)	// �J��������^�[�Q�b�g�̋���

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static TARGETOBJ		g_TargetObj;
static float			g_rot_y;

static BOOL				g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTargetObj(void)
{
	LoadModel(MODEL_DOME_CITY, &g_TargetObj.model);

	g_TargetObj.load = TRUE;

	g_TargetObj.use = TRUE;

	g_TargetObj.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_TargetObj.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_TargetObj.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
	GetModelDiffuse(&g_TargetObj.model, &g_TargetObj.diffuse[0]);
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTargetObj(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_TARGETOBJ; i++)
	{
		if (g_TargetObj.load)
		{
			UnloadModel(&g_TargetObj.model);
			g_TargetObj.load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTargetObj(void)
{
	if (!GetKeyboardPress(DIK_LSHIFT))
	{
		// �ړ��������Ⴄ
		if (GetKeyboardPress(DIK_LEFT))
		{	// ���ֈړ�

			// �^�[�Q�b�g�A�C�R������ʊO�ɏo�Ă��Ȃ������m�F
			if (GetTargetArea(left)) g_TargetObj.rot.y -= MOVE_VALUE_X;
		}
		if (GetKeyboardPress(DIK_RIGHT))
		{	// �E�ֈړ�

			// �^�[�Q�b�g�A�C�R������ʊO�ɏo�Ă��Ȃ������m�F
			if (GetTargetArea(right)) g_TargetObj.rot.y += MOVE_VALUE_X;
		}
		if (GetKeyboardPress(DIK_UP))
		{	// ��ֈړ�

			// �^�[�Q�b�g�A�C�R������ʊO�ɏo�Ă��Ȃ������m�F
			if (GetTargetArea(up)) g_TargetObj.pos.y += MOVE_VALUE_Y;
		}
		if (GetKeyboardPress(DIK_DOWN))
		{	// ���ֈړ�

			// �^�[�Q�b�g�A�C�R������ʊO�ɏo�Ă��Ȃ������m�F
			if (GetTargetArea(down)) g_TargetObj.pos.y -= MOVE_VALUE_Y;
		}
	}

	// ���͂̂�����������OBJ���ړ�������
	g_TargetObj.pos.x = sinf(g_TargetObj.rot.y) * OBJ_DISTANCE;
	g_TargetObj.pos.z = cosf(g_TargetObj.rot.y) * OBJ_DISTANCE;

#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("g_TargetObj:�� �� �� ���@Space\n");
	PrintDebugProc("g_TargetObj:X:%f Y:%f Z:%f\n", g_TargetObj.pos.x, g_TargetObj.pos.y, g_TargetObj.pos.z);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTargetObj(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	if (g_TargetObj.use == FALSE) return;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_TargetObj.scl.x, g_TargetObj.scl.y, g_TargetObj.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_TargetObj.rot.x, g_TargetObj.rot.y, g_TargetObj.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_TargetObj.pos.x, g_TargetObj.pos.y, g_TargetObj.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_TargetObj.mtxWorld, mtxWorld);

	// ���f���`��
	DrawModel(&g_TargetObj.model);
	
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
TARGETOBJ *GetTargetObj()
{
	return &g_TargetObj;
}
