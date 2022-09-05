//=============================================================================
//
// �T������ [same.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "same.h"
#include "attackRange.h"
#include "player.h"
#include "enemy.h"
#include "shadow.h"
#include "enemyHeli.h"
#include "tutorial.h"
#include "collision.h"
#include "score.h"
#include "combo.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_SAME		"data/MODEL/shark_hire.obj"			// �ǂݍ��ރ��f����
#define	MODEL_SAME_1	"data/MODEL/shark_1.obj"			// �ǂݍ��ރ��f����
#define	MODEL_SAME_2	"data/MODEL/shark_2.obj"			// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(0.1f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define SAME_SHADOW_SIZE	(0.4f)							// �e�̑傫��
#define SAME_OFFSET_Y		(7.0f)							// �v���C���[�̑��������킹��

#define HIRE_ROT_PI			(100)							// �q���̉�]�p���a�̒���
#define HIRE_ROT_ADDTION	(0.1f)							// �q����Y����]���Z��
#define DOWN_ADDTION_1		(0.9f)							// ���~���Z�ʇ@
#define DOWN_ADDTION_2		(0.8f)							// ���~���Z�ʇA
#define UP_ADDTION			(2.0f)							// �㏸���Z��
#define MAX_UP				(40.0f)							// �㏸�ō��l
#define MAX_DOWN			(-50.0f)						// ���~�Œ�l
#define KUCHI_ROT_ADDTION	(0.05f)							// ���{��]��
#define MAX_KUCHI_ROT		(XM_PI * 0.5f)					// ���{�ő��]��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
SAME				g_Same;							// �v���C���[
SAME				g_sAtama;
SAME				g_sKuchi;

static float		g_radius;						// �G�l�~�[��]�p
static float		g_rot;							// �G�l�~�[��]�p
static BOOL			g_particleOn;					// TRUE:�����G�t�F�N�g����

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSame(void)
{
	LoadModel(MODEL_SAME, &g_Same.model);
	g_Same.load = TRUE;

	g_Same.pos = { 0.0f, SAME_OFFSET_Y, 0.0f };
	g_Same.rot = { 0.0f, 0.0f, 0.0f };
	g_Same.scl = { 10.0f, 10.0f, 10.0f };

	g_Same.angle = 0.0f;			// �ړ��X�s�[�h�N���A
	g_Same.size = SAME_SIZE;	// �����蔻��̑傫��

	g_Same.use = FALSE;

	g_Same.mode = GURUGURU;

	//
	LoadModel(MODEL_SAME_1, &g_sAtama.model);
	g_sAtama.load = TRUE;

	g_sAtama.pos = { 0.0f, SAME_OFFSET_Y, 0.0f };
	g_sAtama.rot = { -0.77f, 0.0f, 0.0f };
	g_sAtama.scl = { 4.0f, 4.0f, 4.0f };

	g_sAtama.angle = 0.0f;			// �ړ��X�s�[�h�N���A
	g_sAtama.size = SAME_SIZE;	// �����蔻��̑傫��

	g_sAtama.use = FALSE;

	g_sAtama.mode = PACKNCYO;

	LoadModel(MODEL_SAME_2, &g_sKuchi.model);
	g_sKuchi.load = TRUE;

	g_sKuchi.pos = { 0.0f, SAME_OFFSET_Y, 0.0f };
	g_sKuchi.rot = { 0.0f, 0.0f, 0.0f };
	g_sKuchi.scl = { 4.0f, 4.0f, 4.0f };

	g_sKuchi.angle = 0.0f;			// �ړ��X�s�[�h�N���A
	g_sKuchi.size = SAME_SIZE;	// �����蔻��̑傫��

	g_sKuchi.use = FALSE;

	g_sKuchi.mode = DOWN_LAST;
	//

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSame(void)
{
	// ���f���̉������
	if (g_Same.load)
	{
		UnloadModel(&g_Same.model);
		g_Same.load = FALSE;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSame(void)
{
	CAMERA *cam = GetCamera();
	ATTACKRANGE *attackR = GetAttackR();
	PLAYER *player = GetPlayer();

	if (g_Same.use == TRUE)
	{
		// �T�����e�@g_Same�ɂ͈ړ�������T�C�g������B
// switch���̂��߁Ag_Same�̈������x�[�X�ɊǗ�����B
		switch (g_Same.mode)
		{
		case GURUGURU:
			g_Same.pos.x = attackR->pos.x - HIRE_ROT_PI * cosf(g_Same.angle);
			g_Same.pos.y = attackR->pos.y;
			g_Same.pos.z = attackR->pos.z - HIRE_ROT_PI * sinf(g_Same.angle);

			g_sAtama.pos = attackR->pos;
			g_sKuchi.pos = attackR->pos;

			g_Same.rot.y -= HIRE_ROT_ADDTION; // �q����Y���ł̉�]
			// �T�[�N���̒��S���甼�a100�����邮���邽�߂̉��Z
			g_Same.angle += VALUE_MOVE;

			if (g_Same.angle > XM_2PI)
			{
				// ���������ʃ��[�h�֑J��
				g_Same.angle = 0.0f;
				g_Same.mode = DOWN_FIRST;
			}
			break;

			//g_Same.pos.x = attackR->pos.x - HIRE_ROT_PI * cosf(g_Same.angle);
			//g_Same.pos.y = attackR->pos.y;
			//g_Same.pos.z = attackR->pos.z - HIRE_ROT_PI * sinf(g_Same.angle);

			//g_sAtama.pos = attackR->pos;
			//g_sKuchi.pos = attackR->pos;

			//// �q���̈ړ�
			//if (GetKeyboardPress(DIK_SPACE) && player->rockOn == FALSE)
			//{
			//	g_Same.use = TRUE;
			//	g_Same.rot.y -= HIRE_ROT_ADDTION; // �q����Y���ł̉�]
			//	// �T�[�N���̒��S���甼�a100�����邮���邽�߂̉��Z
			//	g_Same.angle += VALUE_MOVE;

			//	if (g_Same.angle > XM_2PI)
			//	{
			//		// ���������ʃ��[�h�֑J��
			//		g_Same.angle = 0.0f;
			//		g_Same.mode = DOWN_FIRST;
			//	}
			//}
			//break;


		case DOWN_FIRST:
			// ���~����
			g_Same.pos.x = attackR->pos.x - HIRE_ROT_PI * cosf(g_Same.angle);
			g_Same.pos.y -= DOWN_ADDTION_1;
			g_Same.pos.z = attackR->pos.z - HIRE_ROT_PI * sinf(g_Same.angle);

			g_sAtama.pos.x = attackR->pos.x;
			g_sAtama.pos.y -= DOWN_ADDTION_1;
			g_sAtama.pos.z = attackR->pos.z;
			g_sKuchi.pos.x = attackR->pos.x;
			g_sKuchi.pos.y -= DOWN_ADDTION_1;
			g_sKuchi.pos.z = attackR->pos.z;

			if (g_Same.pos.y < MAX_DOWN)
			{
				g_Same.pos.x = attackR->pos.x;
				g_Same.pos.z = attackR->pos.z;

				// ���~�Œ�l�܂ł�������q���̃��f����FALSE�ɂ��邪switch�p�ɑ��̏��͎c��
				g_Same.rot.y = 0.0f;
				g_Same.mode = PACKNCYO;
				//g_Same.use = FALSE;

				g_sAtama.use = TRUE;
				g_sKuchi.use = TRUE;
			}
			break;

		case PACKNCYO:
			// �㏸���݂�����(�㏸���Ă���͂��̈ʒu�ŌŒ�)
			//g_Same.pos.x = attackR->pos.x;
			g_Same.pos.y += UP_ADDTION;
			//g_Same.pos.z = attackR->pos.z;

			//g_sAtama.pos.x = attackR->pos.x;
			g_sAtama.pos.y += UP_ADDTION;
			//g_sAtama.pos.z = attackR->pos.z;

			g_sKuchi.rot.x -= KUCHI_ROT_ADDTION;
			//g_sKuchi.pos.x = attackR->pos.x;
			g_sKuchi.pos.y += UP_ADDTION;
			//g_sKuchi.pos.z = attackR->pos.z;


			if (g_Same.pos.y > MAX_UP)
			{
				// �ő�㏸�l�܂ł�������V�[���J��
				g_sKuchi.rot.x = -MAX_KUCHI_ROT;	// ���{�̉�]���~�߂ĉ��{�̉�]�ő�l�l����
				g_Same.mode = DOWN_LAST;
			}
			break;

		case DOWN_LAST:
			// ���~����
			g_Same.pos.y -= DOWN_ADDTION_2;
			g_sAtama.pos.y -= DOWN_ADDTION_2;
			g_sKuchi.pos.y -= DOWN_ADDTION_2;

			if (g_Same.pos.y < MAX_DOWN)
			{
				// ���ƌ��̃��f����FALSE�ɂ���
				g_Same.use = FALSE;
				g_sAtama.use = FALSE;
				g_sKuchi.use = FALSE;
				g_sKuchi.rot.x = 0.0f; // ���{�̉�]�ʂ̏�����
				g_Same.mode = GURUGURU;
			}
			break;
		}

		// ����������G�l�~�[�̋�������
		{
			ENEMY *enemy = GetEnemy();		// �G�l�~�[�̃|�C���^�[��������
			ENEMY_HELI *enemyheli = GetEnemyHeli();		// �G�l�~�[�̃|�C���^�[��������

			// ���G�l�~�[�̏���

			// �G�ƃT���I�u�W�F�N�g
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Same.mode == PACKNCYO)
				{
					if (CollisionBC(g_Same.pos, enemy[i].pos, g_Same.size, enemy[i].size))
					{
						if (enemy[i].isHit == TRUE) break;
						if (enemy[i].use == TRUE)
						{
							// �X�R�A�𑫂�
							AddScore(100);

							// �R���{�𑫂�
							AddCombo(1);
							ResetComboTime();
						}
						// �G�L�����N�^�[�͓|�����
						enemy[i].use = FALSE;
					}
				}
			}

			// �w���̏���
			for (int j = 0; j < MAX_ENEMY_HELI; j++)
			{
				if (g_Same.mode == PACKNCYO)
				{
					if (CollisionBC(g_Same.pos, enemyheli[j].pos, g_Same.size, enemyheli[j].size))
					{
						if (enemyheli[j].isHit == TRUE) break;
						if (enemyheli[j].use == TRUE)
						{
							// �X�R�A�𑫂�
							AddScore(100);

							// �R���{�𑫂�
							AddCombo(1);
							ResetComboTime();
						}
						// �G�L�����N�^�[�͓|�����
						enemyheli[j].use = FALSE;
					}
				}
			}
		}

	}

#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("g_Same:�� �� �� ���@Space\n");
	//PrintDebugProc("g_Same:X:%f Y:%f Z:%f\n", g_Same.pos.x, g_Same.pos.y, g_Same.pos.z);
	//PrintDebugProc("g_Sameangle%f\n", g_Same.angle);

#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSame(void)
{

	if (g_Same.use == TRUE)
	{
		// �J�����O����
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Same.scl.x, g_Same.scl.y, g_Same.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Same.rot.x, g_Same.rot.y + XM_PI, g_Same.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Same.pos.x, g_Same.pos.y, g_Same.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Same.mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Same.model);

		// �J�����O�ݒ��߂�
		SetCullingMode(CULL_MODE_BACK);
	}
	if (g_sAtama.use == TRUE)
	{
		// �J�����O����
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_sAtama.scl.x, g_sAtama.scl.y, g_sAtama.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_sAtama.rot.x, g_sAtama.rot.y + XM_PI, g_sAtama.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_sAtama.pos.x, g_sAtama.pos.y, g_sAtama.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_sAtama.mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_sAtama.model);

		// �J�����O�ݒ��߂�
		SetCullingMode(CULL_MODE_BACK);
	}
	if (g_sKuchi.use == TRUE)
	{
		// �J�����O����
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_sKuchi.scl.x, g_sKuchi.scl.y, g_sKuchi.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_sKuchi.rot.x, g_sKuchi.rot.y + XM_PI, g_sKuchi.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_sKuchi.pos.x, g_sKuchi.pos.y, g_sKuchi.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_sKuchi.mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_sKuchi.model);

		// �J�����O�ݒ��߂�
		SetCullingMode(CULL_MODE_BACK);
	}
}


//=============================================================================
// �T�������擾
//=============================================================================
SAME *GetSame(void)
{
	return &g_Same;
}

//=============================================================================
// �T���̔���
//=============================================================================
void SetSame(void)
{
	if (g_Same.use == FALSE && g_Same.mode == GURUGURU)
	{
		g_Same.use = TRUE;

		ATTACKRANGE *attackR = GetAttackR();

		g_Same.pos.x = attackR->pos.x - HIRE_ROT_PI * cosf(g_Same.angle);
		g_Same.pos.y = attackR->pos.y;
		g_Same.pos.z = attackR->pos.z - HIRE_ROT_PI * sinf(g_Same.angle);

		g_sAtama.pos = attackR->pos;
		g_sKuchi.pos = attackR->pos;

		return;
	}
}
