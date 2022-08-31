//=============================================================================
//
// ���f������ [tako.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"
#include "enemy.h"
#include "enemyHeli.h"
#include "tako.h"
#include "shadow.h"
#include "rockOn.h"
#include "target.h"
#include "collision.h"
#include "score.h"
#include "combo.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_TAKO		"data/MODEL/tako_1.obj"			// �ǂݍ��ރ��f����
#define	MODEL_TAKO_1	"data/MODEL/tako_2.obj"			// �ǂݍ��ރ��f����
#define	MODEL_TAKO_2	"data/MODEL/tako_3.obj"			// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(0.1f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define TAKO_SHADOW_SIZE	(0.4f)							// �e�̑傫��
#define TAKO_OFFSET_Y		(7.0f)							// �v���C���[�̑��������킹��

#define TAKO_SCL_INIT		(0.1f)							// �^�R�̑傫���̏����l�ݒ�
#define TAKO_POS_Y			(50.0f)							// �^�R�̍��������l�ݒ�
#define TAKO_POS_CATCH		(70.0f)							// �^�R�̃L���b�`�A�N�V�����������̍���
#define TAKO_POS_ADDITION	(0.5f)							// �^�R�̃|�W�V�������Z�p
#define VALUE_HOMING		(0.1f)							// �^�R�����񂹒l
#define FRONT_COLLISION		(20.0f)							// ��O�ŏ����ʒu�̓����蔻��l
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
TAKO				g_Tako[MAX_TAKO];							// �v���C���[
TAKO				g_Tako_middle[MAX_TAKO];					// �v���C���[
TAKO				g_Tako_last[MAX_TAKO];					// �v���C���[


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTako(void)
{
	for (int i = 0; i < MAX_TAKO; i++)
	{
		LoadModel(MODEL_TAKO, &g_Tako[i].model);
		g_Tako[i].load = TRUE;

		g_Tako[i].pos = { 0.0f, TAKO_OFFSET_Y, 0.0f };
		g_Tako[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Tako[i].scl = { TAKO_SCL_INIT, TAKO_SCL_INIT, TAKO_SCL_INIT };

		g_Tako[i].angle = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Tako[i].size = TAKO_SIZE;	// �����蔻��̑傫��
		g_Tako[i].rockOnNum = 0;

		g_Tako[i].use = FALSE;

		g_Tako[i].mode = WAIT;

		//

		LoadModel(MODEL_TAKO_1, &g_Tako_middle[i].model);
		g_Tako_middle[i].load = TRUE;

		g_Tako_middle[i].pos = { 0.0f, TAKO_OFFSET_Y, 0.0f };
		g_Tako_middle[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Tako_middle[i].scl = { 1.0f, 1.0f, 1.0f };

		g_Tako_middle[i].angle = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Tako_middle[i].size = TAKO_SIZE;	// �����蔻��̑傫��

		g_Tako_middle[i].use = FALSE;

		LoadModel(MODEL_TAKO_2, &g_Tako_last[i].model);
		g_Tako_last[i].load = TRUE;

		g_Tako_last[i].pos = { 0.0f, TAKO_OFFSET_Y, 0.0f };
		g_Tako_last[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Tako_last[i].scl = { 1.0f, 1.0f, 1.0f };

		g_Tako_last[i].angle = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Tako_last[i].size = TAKO_SIZE;	// �����蔻��̑傫��

		g_Tako_last[i].use = FALSE;

	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTako(void)
{
	for (int i = 0; i < MAX_TAKO; i++)
	{
		// ���f���̉������
		if (g_Tako[i].load)
		{
			UnloadModel(&g_Tako[i].model);
			g_Tako[i].load = FALSE;
		}
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTako(void)
{
	//return;

	CAMERA *cam = GetCamera();
	PLAYER *player = GetPlayer();
	ENEMY  *enemy = GetEnemy();
	ROCKON *rockOn = GetRockOn();

	for (int i = 0; i < MAX_TAKO; i++)
	{
		//if (g_Tako[i].use == FALSE)
		//	continue;

		//�T�����e�@g_Tako�ɂ͈ړ�������T�C�g������B
		switch (g_Tako[i].mode)
		{
		case WAIT:
			g_Tako[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
			g_Tako[i].pos.y += TAKO_POS_Y;
			g_Tako_middle[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
			g_Tako_middle[i].pos.y += TAKO_POS_Y;
			g_Tako_last[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
			g_Tako_last[i].pos.y += TAKO_POS_Y;

			if (GetKeyboardPress(DIK_SPACE) && player->rockOn == TRUE)
			{
				//g_Tako[i].scl = { TAKO_SCL_INIT, TAKO_SCL_INIT, TAKO_SCL_INIT };
				g_Tako_middle[i].use = TRUE;
				g_Tako[i].mode = ZUZHO;
			}
			break;

		case ZUZHO:

			g_Tako[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
			g_Tako[i].pos.y += TAKO_POS_CATCH;
			g_Tako_middle[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
			g_Tako_middle[i].pos.y += TAKO_POS_CATCH;
			g_Tako_last[i].pos = rockOn[g_Tako[i].rockOnNum].pos;
			g_Tako_last[i].pos.y += TAKO_POS_CATCH;
			g_Tako[i].mode = CATCH;
			break;

		case CATCH:
			g_Tako[i].pos.x = rockOn[g_Tako[i].rockOnNum].pos.x;
			g_Tako[i].pos.y -= TAKO_POS_ADDITION;
			g_Tako[i].pos.z = rockOn[g_Tako[i].rockOnNum].pos.z;
			g_Tako_middle[i].pos.x = rockOn[g_Tako[i].rockOnNum].pos.x;
			g_Tako_middle[i].pos.y -= TAKO_POS_ADDITION;
			g_Tako_middle[i].pos.z = rockOn[g_Tako[i].rockOnNum].pos.z;
			g_Tako_last[i].pos.x = rockOn[g_Tako[i].rockOnNum].pos.x;
			g_Tako_last[i].pos.y -= TAKO_POS_ADDITION;
			g_Tako_last[i].pos.z = rockOn[g_Tako[i].rockOnNum].pos.z;


			if (g_Tako[i].pos.y < 0.0f)
			{
				g_Tako_middle[i].use = FALSE;
				g_Tako_last[i].use = TRUE;
				g_Tako[i].mode = RELEASE;
			}
			break;

		case RELEASE:

			XMVECTOR pos1 = XMLoadFloat3(&cam->pos);
			XMVECTOR pos2 = XMLoadFloat3(&g_Tako[i].pos);
			pos2 += (pos1 - pos2) * VALUE_HOMING;
			XMStoreFloat3(&g_Tako[i].pos, pos2);
			XMVECTOR pos3 = XMLoadFloat3(&cam->pos);
			XMVECTOR pos4 = XMLoadFloat3(&g_Tako_last[i].pos);
			pos2 += (pos1 - pos2) * VALUE_HOMING;
			XMStoreFloat3(&g_Tako_last[i].pos, pos2);

			if (g_Tako[i].pos.z < cam->pos.z + FRONT_COLLISION)
			{
				g_Tako_last[i].use = FALSE;
				g_Tako[i].use = FALSE;
				g_Tako[i].mode = WAIT;
				ResetRockOn();

				ReleaseShadow(enemy[i].shadowIdx);

				// �X�R�A�𑫂�
				AddScore(100);

				// �R���{�𑫂�
				AddCombo(1);
				ResetComboTime();

			}
			break;
		}

		// ����������G�l�~�[�̋�������
		{
			ENEMY *enemy = GetEnemy();		// �G�l�~�[�̃|�C���^�[��������
			ENEMY_HELI *enemyheli = GetEnemyHeli();		// �G�l�~�[�̃|�C���^�[��������

			// ���G�l�~�[�̏���

			// �G�ƃT���I�u�W�F�N�g
			for (int j = 0; j < MAX_ENEMY; j++)
			{
				if (g_Tako[i].mode == RELEASE)
				{
					if (CollisionBC(g_Tako[i].pos, enemy[j].pos, g_Tako[i].size, enemy[j].size))
					{
						if (enemy[j].isHit == TRUE) break;
						// �G�L�����N�^�[�͓|�����
						enemy[j].use = FALSE;
					}
				}
			}

			// �w���̏���
			for (int j = 0; j < MAX_ENEMY_HELI; j++)
			{
				if (g_Tako[i].mode == RELEASE)
				{
					if (CollisionBC(g_Tako[i].pos, enemyheli[j].pos, g_Tako[i].size, enemyheli[j].size))
					{
						if (enemyheli[j].isHit == TRUE) break;
						// �G�L�����N�^�[�͓|�����
						enemyheli[j].use = FALSE;
					}
				}
			}
		}
	}





#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Enemy:�� �� �� ���@Space\n");
	//PrintDebugProc("Enemy:X:%f Y:%f Z:%f\n", g_Tako[i].pos.x, g_Tako[i].pos.y, g_Tako[i].pos.z);
	//PrintDebugProc("Enemyangle%f\n", g_Tako[i].angle);

#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTako(void)
{
	for (int i = 0; i < MAX_TAKO; i++)
	{
		if (g_Tako[i].use == TRUE)
		{
			// �J�����O����
			SetCullingMode(CULL_MODE_NONE);

			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Tako[i].scl.x, g_Tako[i].scl.y, g_Tako[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Tako[i].rot.x, g_Tako[i].rot.y + XM_PI, g_Tako[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Tako[i].pos.x, g_Tako[i].pos.y, g_Tako[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Tako[i].mtxWorld, mtxWorld);

			// ���f���`��
			DrawModel(&g_Tako[i].model);

			// �J�����O�ݒ��߂�
			SetCullingMode(CULL_MODE_BACK);
		}

		if (g_Tako_middle[i].use == TRUE)
		{
			// �J�����O����
			SetCullingMode(CULL_MODE_NONE);

			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Tako_middle[i].scl.x, g_Tako_middle[i].scl.y, g_Tako_middle[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Tako_middle[i].rot.x, g_Tako_middle[i].rot.y + XM_PI, g_Tako_middle[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Tako_middle[i].pos.x, g_Tako_middle[i].pos.y, g_Tako_middle[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Tako_middle[i].mtxWorld, mtxWorld);

			// ���f���`��
			DrawModel(&g_Tako_middle[i].model);

			// �J�����O�ݒ��߂�
			SetCullingMode(CULL_MODE_BACK);
		}

		if (g_Tako_last[i].use == TRUE)
		{
			// �J�����O����
			SetCullingMode(CULL_MODE_NONE);

			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Tako_last[i].scl.x, g_Tako_last[i].scl.y, g_Tako_last[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Tako_last[i].rot.x, g_Tako_last[i].rot.y + XM_PI, g_Tako_last[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Tako_last[i].pos.x, g_Tako_last[i].pos.y, g_Tako_last[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Tako_last[i].mtxWorld, mtxWorld);

			// ���f���`��
			DrawModel(&g_Tako_last[i].model);

			// �J�����O�ݒ��߂�
			SetCullingMode(CULL_MODE_BACK);
		}
	}
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
TAKO *GetTako(void)
{
	return &g_Tako[0];
}

//=============================================================================
//�^�R�̔���
//=============================================================================
void SetTako(void)
{
	ROCKON *rockOn = GetRockOn();

	for (int i = 0; i < MAX_ROCKON; i++)
	{
		if (rockOn[i].use == TRUE)
		{

			//for (int j = 0; j < MAX_ROCKON; j++)
			//{
			//	if (rockOn[j].use == TRUE)
			//	{
			//		g_Tako[i].use = TRUE;
			//		g_Tako[i].pos = rockOn[j].pos;
			//		//g_Tako_middle[i].pos = rockOn[j].pos;
			//		//g_Tako_last[i].pos = rockOn[j].pos;
			//		break;
			//	}
			//}
			for (int j = 0; j < MAX_TAKO; j++)
			{
				if (g_Tako[j].use == FALSE)
				{
					g_Tako[j].use = TRUE;
					g_Tako[j].pos = rockOn[i].pos;
					g_Tako[j].pos.y += 5.0f;
					g_Tako[j].rockOnNum = i;

					g_Tako_middle[i].pos = rockOn[j].pos;
					g_Tako_last[i].pos = rockOn[j].pos;
					break;
				}
			}
		}
	}
}
