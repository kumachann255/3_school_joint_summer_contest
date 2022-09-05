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
#include "same.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_NAMI			"data/MODEL/kuzira_nami.obj"	// �ǂݍ��ރ��f����
#define	MODEL_KUZIRA		"data/MODEL/kuzira.obj"			// �ǂݍ��ރ��f����
#define	MODEL_SHIO			"data/MODEL/kuzira_sio00.obj"	// �ǂݍ��ރ��f����
#define	MODEL_HORAGAI		"data/MODEL/kuzira_horage.obj"	// �ǂݍ��ރ��f����
#define	MODEL_SHIMA			"data/MODEL/sima.obj"			// �ǂݍ��ރ��f����
#define	MODEL_HITO			"data/MODEL/enemy08.obj"		// �ǂݍ��ރ��f����

#define VALUE_NAMI				(5.0f)						// �g���Z��
#define VALUE_DOWN_POSY			(0.3f)						// ���~���Z��
#define VALUE_FRONT				(1.0f)						// �O����Z��
#define	VALUE_ROTATE			(0.01f)						// ��]���Z��
#define	VALUE_POSY				(5.0f)						// �㉺���Z��
#define	MAX_FRONT_ROT			(0.574f)					// �ő�O��]�l
#define	MAX_BACK_ROT			(-0.65f)					// �ő���]�l
#define	MAX_LOWPOS_Y			(-100.0f)					// �ő剺�~�l
#define	MAX_LOWPOS_KUZIRA_Y		(50.0f)						// �ő剺�~�l
#define MAX_LOW_NAMI			(-25.0f)					// �g�̍ő剺�~�l
#define KUZIRA_POP_COUNT		(300)						// ������̏o���Ԋu
#define KUZIRA_POP_Z			(550)						// ������̏����|�b�v�ʒu(z���W)
#define KUZIRA_POP_X			(700)						// ������̏����|�b�v�͈̔�(x���W)
#define MAX_HUNSYA_TIME			(25)						// ���̍ő啬�ˎ���

enum {
	wait,
	move,
	hunsya,
	up,
	down,
	left,
	light
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetNami(int i, int mode);
void SetShio(XMFLOAT3 pos);
void SetKuzira(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static SEA_FOBJ				g_Nami[MAX_SEA_WAVE];			// �g
static SEA_FOBJ				g_Kuzira[MAX_SEA_FOBJ_SIGN];	// �N�W��
static SEA_FOBJ				g_Shio[MAX_SEA_FOBJ_SIGN];		// ��
static SEA_FOBJ				g_Horagai;						// �ق�L
static SEA_FOBJ				g_Shima;						// ��
static SEA_FOBJ				g_Hito;							// �l

static BOOL				g_Load = FALSE;

static int popCount = 0;
static int hunsyaTime = 0;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSeaFieldObj(void)
{
	LoadModel(MODEL_KUZIRA, &g_Kuzira[0].model);
	LoadModel(MODEL_NAMI, &g_Nami[0].model);
	LoadModel(MODEL_SHIO, &g_Shio[0].model);
	LoadModel(MODEL_HORAGAI, &g_Horagai.model);
	LoadModel(MODEL_SHIMA, &g_Shima.model);
	LoadModel(MODEL_HITO, &g_Hito.model);

	for (int i = 0; i < MAX_SEA_WAVE; i++)
	{
		LoadModel(MODEL_NAMI, &g_Nami[i].model);
		g_Nami[i].load = TRUE;
		g_Nami[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Nami[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Nami[i].scl = XMFLOAT3(1.0f, 3.0f, 3.0f);
		g_Nami[i].moveFlag = wait;
		g_Nami[i].use = FALSE;			// TRUE:�����Ă�
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Nami[i].model, &g_Nami[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		LoadModel(MODEL_KUZIRA, &g_Kuzira[i].model);
		g_Kuzira[i].load = TRUE;
		g_Kuzira[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Kuzira[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Kuzira[i].scl = XMFLOAT3(3.5f - 0.5f * i, 3.5f - 0.5f * i, 3.5f - 0.5f * i);
		g_Kuzira[i].moveFlag = hunsya;
		g_Kuzira[i].use = FALSE;			// TRUE:�����Ă�
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Kuzira[i].model, &g_Kuzira[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		LoadModel(MODEL_SHIO, &g_Shio[i].model);
		g_Shio[i].load = TRUE;
		g_Shio[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Shio[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Shio[i].scl = XMFLOAT3(3.0f, 6.0f, 3.0f);
		g_Shio[i].moveFlag = up;
		g_Shio[i].use = FALSE;			// TRUE:�����Ă�
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Shio[i].model, &g_Shio[i].diffuse[0]);
	}

	g_Horagai.load = TRUE;
	g_Horagai.pos = XMFLOAT3(20.0f, 10.0f, 20.0f);
	g_Horagai.rot = XMFLOAT3(0.754f, 1.57f, 0.0f);
	g_Horagai.scl = XMFLOAT3(0.5f, 0.5f, 0.5f);
	g_Horagai.moveFlag = wait;
	g_Horagai.use = TRUE;			// TRUE:�����Ă�

	g_Shima.load = TRUE;
	g_Shima.pos = XMFLOAT3(-10.0f, 0.0f, 10.0f);
	g_Shima.rot = XMFLOAT3(0.0f, 0.754f, 0.0f);
	g_Shima.scl = XMFLOAT3(1.125f, 1.125f, 1.125f);
	g_Shima.moveFlag = wait;
	g_Shima.use = TRUE;			// TRUE:�����Ă�

	g_Hito.load = TRUE;
	g_Hito.pos = XMFLOAT3(0.0f, 20.0f, 10.0f);
	g_Hito.rot = XMFLOAT3(0.0f, 3.14f, 0.0f);
	g_Hito.scl = XMFLOAT3(0.75f, 0.75f, 0.75f);
	g_Hito.moveFlag = wait;
	g_Hito.use = TRUE;			// TRUE:�����Ă�

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSeaFieldObj(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_SEA_WAVE; i++)
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

	if (g_Horagai.load)
	{
		UnloadModel(&g_Horagai.model);
		g_Horagai.load = FALSE;
	}

	if (g_Shima.load)
	{
		UnloadModel(&g_Shima.model);
		g_Shima.load = FALSE;
	}

	if (g_Hito.load)
	{
		UnloadModel(&g_Hito.model);
		g_Hito.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSeaFieldObj(void)
{

	popCount++;

	// �N�W���̏���
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{

		// ���Ԍo�߂ƃG�l�~�[�̏o��������Ń|�b�v���邩���f
		if (popCount == KUZIRA_POP_COUNT)
		{
			SetKuzira();
			popCount = 0;
		}

		if (g_Kuzira[i].use == TRUE)
		{
			switch (g_Kuzira[i].moveFlag)
			{
			case hunsya:
				// ���シ��
				hunsyaTime++;
				SetShio(g_Kuzira[i].pos);

				if (hunsyaTime > MAX_HUNSYA_TIME)
				{
					hunsyaTime = 0;
					g_Kuzira[i].moveFlag = up;
				}
				break;

			case up:
				// ���シ��
				g_Kuzira[i].pos.y += VALUE_POSY;
				if (g_Kuzira[i].pos.y > MAX_LOWPOS_KUZIRA_Y)
				{
					g_Kuzira[i].moveFlag = move;
				}
				break;

			case move:
				// x����y���ŉ�]
				g_Kuzira[i].rot.x += VALUE_ROTATE;
				g_Kuzira[i].rot.z += VALUE_ROTATE;

				if (g_Kuzira[i].rot.x > MAX_FRONT_ROT)
				{
					g_Kuzira[i].moveFlag = down;
				}
				break;

			case down:
				// x����y���ŉ�]��������
				g_Kuzira[i].pos.y -= VALUE_POSY;
				g_Kuzira[i].rot.x += VALUE_ROTATE * 2.0f;
				g_Kuzira[i].rot.z += VALUE_ROTATE * 2.0f;
				g_Kuzira[i].pos.z -= VALUE_POSY;

				if (g_Kuzira[i].pos.y < -MAX_LOWPOS_KUZIRA_Y)
				{
					g_Kuzira[i].use = FALSE;
				}
				break;

				//case wait:
				//	g_Kuzira[i].rot.x += VALUE_ROTATE;
				//	if (g_Kuzira[i].rot.x > MAX_FRONT_ROT)
				//	{
				//		g_Kuzira[i].moveFlag = move;
				//	}
				//	break;

				//case move:
				//	g_Kuzira[i].rot.x -= VALUE_ROTATE;
				//	if (g_Kuzira[i].rot.x < MAX_BACK_ROT)
				//	{
				//		g_Kuzira[i].moveFlag = wait;
				//	}
				//	break;
			}
		}
	}

	// �g�̏���
	for (int i = 0; i < MAX_SEA_WAVE; i++)
	{
		SAME *same = GetSame();
		int dir;

		if (g_Nami[i].use == FALSE)
		{
			dir = i % MAX_SEA_WAVE;
			switch (dir)
			{
			case 0:
				SetNami(dir, same->mode);
				break;

			case 1:
				SetNami(dir, same->mode);
				break;
			}
		}
		else if (g_Nami[i].use == TRUE)
		{
			dir = i % MAX_SEA_WAVE;
			switch (dir)
			{
			case 0:
				g_Nami[i].pos.x += -VALUE_FRONT;
				g_Nami[i].pos.z -= VALUE_FRONT;
				g_Nami[i].pos.y += VALUE_DOWN_POSY;
				if (g_Nami[i].pos.y > MAX_LOW_NAMI)
				{
					g_Nami[i].use = FALSE;
				}
				break;

			case 1:
				g_Nami[i].pos.x += VALUE_FRONT;
				g_Nami[i].pos.z -= VALUE_FRONT;
				g_Nami[i].pos.y += VALUE_DOWN_POSY;
				if (g_Nami[i].pos.y > MAX_LOW_NAMI)
				{
					g_Nami[i].use = FALSE;
				}
				break;
			}

		}
	}

	// ���̏���
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Shio[i].use == TRUE)
		{
			switch (g_Shio[i].moveFlag)
			{
			case up:
				g_Shio[i].pos.y += VALUE_POSY;
				if (g_Shio[i].pos.y > 0.0f)
				{
					g_Shio[i].moveFlag = down;
				}
				break;

			case down:
				g_Shio[i].pos.y -= VALUE_POSY;
				if (g_Shio[i].pos.y < 0.0f)
				{
					g_Shio[i].moveFlag = up;
					g_Shio[i].use = FALSE;
				}
				break;
			}
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		PrintDebugProc("g_Nami[%d].pos.y:%f\n", i, g_Nami[i].pos.y);
		PrintDebugProc("g_Kuzira[%d].rot.x:%f\n", i, g_Kuzira[i].rot.x);
		PrintDebugProc("g_Shio[%d].pos.y:%f\n", i , g_Shio[i].pos.y);
	}
	PrintDebugProc("Popcount: %d\n", popCount);


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSeaFieldObj(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �g�̕`��
	for (int i = 0; i < MAX_SEA_WAVE; i++)
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

	// �ق�L�̕`��

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Horagai.scl.x, g_Horagai.scl.y, g_Horagai.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Horagai.rot.x, g_Horagai.rot.y + XM_PI, g_Horagai.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Horagai.pos.x, g_Horagai.pos.y, g_Horagai.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Horagai.mtxWorld, mtxWorld);

	// ���f���`��
	DrawModel(&g_Horagai.model);

	// ���̕`��

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Shima.scl.x, g_Shima.scl.y, g_Shima.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Shima.rot.x, g_Shima.rot.y + XM_PI, g_Shima.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Shima.pos.x, g_Shima.pos.y, g_Shima.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Shima.mtxWorld, mtxWorld);

	// ���f���`��
	DrawModel(&g_Shima.model);

	// �l�̕`��

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Hito.scl.x, g_Hito.scl.y, g_Hito.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Hito.rot.x, g_Hito.rot.y + XM_PI, g_Hito.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Hito.pos.x, g_Hito.pos.y, g_Hito.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Hito.mtxWorld, mtxWorld);

	// ���f���`��
	DrawModel(&g_Hito.model);

}

//=============================================================================
// �g�̔���
//=============================================================================
void SetNami(int i, int mode)
{
	for (int i = 0; i < MAX_SEA_WAVE; i++)
	{
		SAME *same = GetSame();

		if (g_Nami[i].use == FALSE && same->mode == PACKNCYO)
		{
			g_Nami[i].use = TRUE;

			if (i == 1)
			{	// �E�ɐݒu
				g_Nami[i].pos = same->pos;
				g_Nami[i].pos.x += VALUE_NAMI;
				g_Nami[i].pos.y -= VALUE_NAMI;
				g_Nami[i].rot = XMFLOAT3(0.0f, 2.0f, 0.0f);
			}
			else
			{	// ���ɐݒu
				g_Nami[i].pos = same->pos;
				g_Nami[i].pos.x += -VALUE_NAMI;
				g_Nami[i].pos.y -=  VALUE_NAMI;
				g_Nami[i].rot = XMFLOAT3(0.0f, -2.0f, 0.0f);
			}
			return;
		}
	}
}

//=============================================================================
// �N�W���̔���
//=============================================================================
void SetKuzira(void)
{
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Kuzira[i].use == FALSE)
		{
			//SetSourceVolume(SOUND_LABEL_SE_carHorn01, 1.0f);
			// SE�̃Z�b�g

			g_Kuzira[i].use = TRUE;
			g_Kuzira[i].moveFlag = hunsya;
			g_Kuzira[i].pos.z = KUZIRA_POP_Z;
			g_Kuzira[i].pos.y = -MAX_LOWPOS_KUZIRA_Y;
			g_Kuzira[i].rot = XMFLOAT3(-1.57f,0.0f, 0.0f);

			// x���W�̓����_��
			g_Kuzira[i].pos.x = (float)(rand() % KUZIRA_POP_X) - ((float)KUZIRA_POP_X / 2.0f);
			// z�n�_�������_��
			g_Kuzira[i].pos.z = (float)(rand() % KUZIRA_POP_Z) + ((float)KUZIRA_POP_Z / 2.0f);

			return;
		}
	}
}

//=============================================================================
// �N�W���̔���
//=============================================================================
void SetShio(XMFLOAT3 pos)
{
	for (int i = 0; i < MAX_SEA_FOBJ_SIGN; i++)
	{
		if (g_Shio[i].use == FALSE)
		{
			//SetSourceVolume(SOUND_LABEL_SE_carHorn01, 1.0f);
			// SE�̃Z�b�g

			g_Shio[i].use = TRUE;
			g_Shio[i].pos = pos;
			g_Shio[i].pos.y = -20.0f;
			g_Shio[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

			return;
		}
	}
}