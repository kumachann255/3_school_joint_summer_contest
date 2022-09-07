//=============================================================================
//
// ���e�I���� [meteor.cpp]
// Author : �F�V�`�O
//
//=============================================================================
#include <time.h>
#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "sky_smallmeteor.h"
#include "shadow.h"
#include "player.h"
#include "attackRange.h"
#include "blast.h"
#include "target.h"
#include "sky_enemy.h"
#include "sound.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_METEOR_1			"data/MODEL/meteo_01.obj"		// �ǂݍ��ރ��f����
#define	MODEL_METEOR_2			"data/MODEL/meteo_02.obj"		// �ǂݍ��ރ��f����
#define	MODEL_METEOR_3			"data/MODEL/meteo_03.obj"		// �ǂݍ��ރ��f����
#define	MODEL_METEOR_4			"data/MODEL/meteo_04.obj"		// �ǂݍ��ރ��f����
#define	MODEL_METEOR_5			"data/MODEL/meteo_05.obj"		// �ǂݍ��ރ��f����
#define	MODEL_METEOR_6			"data/MODEL/meteo_06.obj"		// �ǂݍ��ރ��f����


#define	VALUE_MOVE				(5.0f)				// �ړ���

#define S_METEOR_SPEED			(0.015f)			// �{���̑��x

#define S_METEOR_SPEED_MAX		(40.0f)			// ���t���[���Ń��e�I���G�l�~�[�ɂԂ��邩
#define S_METEOR_SPEED_MIN		(30.0f)				// ���t���[���Ń��e�I���G�l�~�[�ɂԂ��邩
#define S_METEOR_POP_DISTANCE	(100.0f)
#define	S_METEOR_POP_RAND		(50.0f)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static S_METEOR			g_sMeteor[MAX_S_METEOR];				// �U���͈�

static BOOL				g_Load = FALSE;

static XMFLOAT3			control0, control1, control2;	// ����_

static XMFLOAT3			rot;	// ��]

//=============================================================================
// ����������
//=============================================================================
HRESULT InitS_Meteor(void)
{
	//srand(time(NULL));

	LoadModel(MODEL_METEOR_1, &g_sMeteor[0].model);
	LoadModel(MODEL_METEOR_2, &g_sMeteor[1].model);
	LoadModel(MODEL_METEOR_3, &g_sMeteor[2].model);
	LoadModel(MODEL_METEOR_4, &g_sMeteor[3].model);
	LoadModel(MODEL_METEOR_5, &g_sMeteor[4].model);
	LoadModel(MODEL_METEOR_6, &g_sMeteor[5].model);

	for (int i = 0; i < MAX_S_METEOR; i++)
	{
		g_sMeteor[i].load = TRUE;

		g_sMeteor[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_sMeteor[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_sMeteor[i].scl = XMFLOAT3(4.0f, 4.0f, 4.0f);

		g_sMeteor[i].speed = S_METEOR_SPEED;			// �ړ��X�s�[�h�N���A
		g_sMeteor[i].time = 0.0f;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		//GetModelDiffuse(&g_sMeteor[i].model, &g_sMeteor[i].diffuse[0]);

		//// �F�������ς���
		//XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 0.5f };

		//// �F���Z�b�g
		//SetModelDiffuse(&g_sMeteor[i].model, 0, color);

		g_sMeteor[i].size = S_METEOR_SIZE;
		g_sMeteor[i].randMove = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_sMeteor[i].ModelPattern = 0;

		g_sMeteor[i].use = FALSE;			// TRUE:�����Ă�


		g_Load = TRUE;
	}
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitS_Meteor(void)
{
	for (int i = 0; i < MAX_S_METEOR; i++)
	{
		if (g_Load == FALSE) return;

		if (g_sMeteor[i].load)
		{
			UnloadModel(&g_sMeteor[i].model);
			g_sMeteor[i].load = FALSE;
		}
	}
	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateS_Meteor(void)
{
	TARGET *target = GetTarget();

	for (int i = 0; i < MAX_S_METEOR; i++)
	{
		if (g_sMeteor[i].use == TRUE)			// ���̍U���͈͂��g���Ă���H
		{									// Yes

			g_sMeteor[i].pos.x += g_sMeteor[i].randMove.x;
			g_sMeteor[i].pos.z += g_sMeteor[i].randMove.z;
			g_sMeteor[i].pos.y += g_sMeteor[i].randMove.y;

			//g_sMeteor[i].pos.x += (target[0].targetPos[i].x - g_sMeteor[i].pos.x) * 0.9f;
			//g_sMeteor[i].pos.x += (target[0].targetPos[i].z - g_sMeteor[i].pos.z) * 0.9f;
			//g_sMeteor[i].pos.x += (target[0].targetPos[i].y - g_sMeteor[i].pos.y) * 0.9f;

			//g_sMeteor[i].pos.x = target[0].targetPos[i].x;
			//g_sMeteor[i].pos.x = target[0].targetPos[i].z;
			//g_sMeteor[i].pos.x = target[0].targetPos[i].y;

			if (g_sMeteor[i].pos.y < -300.0f)
			{
				g_sMeteor[i].use = FALSE;
			}


			// ��]����
			g_sMeteor[i].rot.x += rot.x;
			g_sMeteor[i].rot.y += rot.y;


#ifdef _DEBUG	// �f�o�b�O����\������
			PrintDebugProc("g_sMeteor[%d]:X:%f Y:%f Z:%f\n", i, g_sMeteor[i].pos.x, g_sMeteor[i].pos.y, g_sMeteor[i].pos.z);
#endif
		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawS_Meteor(void)
{

	for (int i = 0; i < MAX_S_METEOR; i++)
	{

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// �J�����O����
		SetCullingMode(CULL_MODE_NONE);

		if (g_sMeteor[i].use == FALSE) return;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_sMeteor[i].scl.x, g_sMeteor[i].scl.y, g_sMeteor[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_sMeteor[i].rot.x, g_sMeteor[i].rot.y + XM_PI, g_sMeteor[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_sMeteor[i].pos.x, g_sMeteor[i].pos.y, g_sMeteor[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_sMeteor[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_sMeteor[g_sMeteor[i].ModelPattern].model);

		// �J�����O�ݒ��߂�
		SetCullingMode(CULL_MODE_BACK);
	}


}


void SetS_Meteor(XMFLOAT3 pos, float rot)
{
	TARGET *target = GetTarget();
	SKY_ENEMY *enemy = GetSkyEnemy();

	for (int i = 0; i < target->count; i++)
	{
		if (!g_sMeteor[i].use)
		{
			g_sMeteor[i].use = TRUE;

			PlaySound(SOUND_LABEL_SE_meteor_shot);

			g_sMeteor[i].ModelPattern = rand() % 2;

			// �����ʒu���K��ʒu���班�������_���v�f��������
			g_sMeteor[i].pos.x = (pos.x - sinf(rot) * S_METEOR_POP_DISTANCE) + RamdomFloat(2, S_METEOR_POP_RAND, -S_METEOR_POP_RAND);
			g_sMeteor[i].pos.z = (pos.z + cosf(rot) * S_METEOR_POP_DISTANCE) + RamdomFloat(2, S_METEOR_POP_RAND, -S_METEOR_POP_RAND);
			g_sMeteor[i].pos.y = 300.0f;

			//g_sMeteor[i].randMove.x = RamdomFloat(2, 2.0f, -2.0f);
			//g_sMeteor[i].randMove.z = RamdomFloat(2, 2.0f, -2.0f);
			//g_sMeteor[i].randMove.y = RamdomFloat(2, 6.0f, 4.0f);

			float time = RamdomFloat(2, S_METEOR_SPEED_MAX, S_METEOR_SPEED_MIN);
			g_sMeteor[i].randMove.x = (enemy[target[0].enemyNum[i]].pos.x - g_sMeteor[i].pos.x) / time;
			g_sMeteor[i].randMove.z = (enemy[target[0].enemyNum[i]].pos.z - g_sMeteor[i].pos.z) / time;
			g_sMeteor[i].randMove.y = (enemy[target[0].enemyNum[i]].pos.y - g_sMeteor[i].pos.y) / time;
			//g_sMeteor[i].randMove.y = RamdomFloat(2, 6.0f, 4.0f);
			//g_sMeteor[i].randMove.y = RamdomFloat(2, 2.0f, 1.0f);



			//return;
		}

	}

	target[0].count = 0;
	//target[0].use = FALSE;
}


S_METEOR *GetS_Meteor(void)
{
	return &g_sMeteor[0];
}