//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : �F�V�`�O�{�G�i 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "blast.h"
#include "sea_meshfield.h"
#include "shadow.h"
#include "collision.h"
#include "damageEF.h"
#include "debugproc.h"
#include "player.h"
#include "tako.h"
#include "target.h"
#include "fade.h"
#include "tutorial.h"
#include "timeUI.h"
#include "target.h"
#include "targetObj.h"
#include "rockOn.h"

#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY_00			"data/MODEL/enemy00.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_01			"data/MODEL/enemy01.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_02			"data/MODEL/enemy02.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_03			"data/MODEL/enemy03.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_04			"data/MODEL/enemy04.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_05			"data/MODEL/enemy05.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_06			"data/MODEL/enemy06.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_07			"data/MODEL/enemy07.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_08			"data/MODEL/enemy08.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_COLLISION	"data/MODEL/collisionBox.obj"	// �ǂݍ��ރ��f����


#define ENEMY_TYPE_MAX		(9)							// �G�l�~�[�^�C�v�̍ő吔

#define	VALUE_MOVE			(3.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��

#define ENEMY_POP_Z			(500.0f)					// �G�l�~�[�̏����|�b�v�ʒu(z���W)
#define ENEMY_POP_X			(250)						// �G�l�~�[�̏����|�b�v�͈̔�(x���W)

#define ENEMY_GOAL_Z		(70.0f)						// �G�l�~�[�̃S�[����ʒu(z���W)
#define ENEMY_GOAL_Z_OFFSET	(60)						// �G�l�~�[�̃S�[���ʒu�̗���

#define STAGE_TUTORIAL_MAX_POP	(1)						// �ő�A��ɉ��̃G�l�~�[���o����

#define STAGE0_POP_COUNT	(100)						// �G�l�~�[�̃|�b�v�Ԋu
#define STAGE0_MAX_POP		(20)						// �ő�A��ɉ��̃G�l�~�[���o����

#define STAGE1_POP_COUNT	(70)						// �G�l�~�[�̃|�b�v�Ԋu
#define STAGE1_MAX_POP		(25)						// �ő�A��ɉ��̃G�l�~�[���o����

#define STAGE2_POP_COUNT	(50)						// �G�l�~�[�̃|�b�v�Ԋu
#define STAGE2_MAX_POP		(30)						// �ő�A��ɉ��̃G�l�~�[���o����

#define STAGE3_POP_COUNT	(30)						// �G�l�~�[�̃|�b�v�Ԋu
#define STAGE3_MAX_POP		(45)						// �ő�A��ɉ��̃G�l�~�[���o����


#define ENEMY_HIT_MOVE		(5.0f)						// �����蔻���A�j���[�V�����p�ړ���

#define ENEMY_ATTACK_0		(300)						// �G�l�~�[���_�ł���܂ł̎���
#define ENEMY_ATTACK_1		(120 + ENEMY_ATTACK_0)		// �_�ł������Ȃ�܂ł̎���
#define ENEMY_ATTACK_2		(120 + ENEMY_ATTACK_1)		// �U������܂ł̎���

#define ENEMY_BLINKING0		(50)						// �_�ł̊Ԋu
#define ENEMY_BLINKING1		(14)						// �_�ł̊Ԋu

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetEnemy(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[
static ENEMY			g_Collision[MAX_ENEMY];			// �����蔻��p�̃{�b�N�X

static VERTEX_3D		*g_Vertex = NULL;				// 
static MODEL			g_Collision_Vertex;	// 

static int				g_Stage;

static BOOL				g_Load = FALSE;

static int				count = 0;		// �|�b�v�J�E���g



//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	LoadModel(MODEL_ENEMY_00, &g_Enemy[0].model);
	LoadModel(MODEL_ENEMY_01, &g_Enemy[1].model);
	LoadModel(MODEL_ENEMY_02, &g_Enemy[2].model);
	LoadModel(MODEL_ENEMY_03, &g_Enemy[3].model);
	LoadModel(MODEL_ENEMY_04, &g_Enemy[4].model);
	LoadModel(MODEL_ENEMY_05, &g_Enemy[5].model);
	LoadModel(MODEL_ENEMY_06, &g_Enemy[6].model);
	LoadModel(MODEL_ENEMY_07, &g_Enemy[7].model);
	LoadModel(MODEL_ENEMY_08, &g_Enemy[8].model);

	LoadModel(MODEL_ENEMY_COLLISION, &g_Collision[0].model);

	for (int i = 0; i < MAX_ENEMY; i++)
	{	
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Collision[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);

		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Collision[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��
		g_Enemy[i].target = FALSE;

			// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);
		GetModelDiffuse(&g_Collision[i].model, &g_Collision[i].diffuse[0]);

		//XMFLOAT3 pos = g_Enemy[i].pos;
		//pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		//g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].use = FALSE;			// TRUE:�����Ă�
		g_Collision[i].use = FALSE;			// TRUE:�����Ă�

		g_Enemy[i].hitPos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 0.0f);	// �����̒��S
		g_Enemy[i].hitRot = XMFLOAT3(0.0f, 0.0f, 0.0f);				// �����蔻���A�j���[�V�����p�X�s�[�h
		g_Enemy[i].isHit = FALSE;									// TRUE:�������Ă�
		g_Enemy[i].hitTime = 0;										// �^�C�~���O�Ǘ��p
		g_Enemy[i].liveCount = 0;		// �������Ԃ����Z�b�g
		g_Enemy[i].type = 0;			// �G�l�~�[�^�C�v

		g_Enemy[i].cupHit = FALSE;									// TRUE:�������Ă�
		g_Enemy[i].cupRot = FALSE;									// TRUE:�������Ă�
		g_Enemy[i].sameHit = FALSE;									// TRUE:�������Ă�
		g_Enemy[i].sameRot = FALSE;									// TRUE:�������Ă�
		g_Enemy[i].takoHit = FALSE;									// TRUE:�������Ă�
		g_Enemy[i].takoRot = FALSE;									// TRUE:�������Ă�

		g_Enemy[i].radian = 0.0f;									// ��]��

		g_Enemy[i].fuchi = FALSE;


	}

	LoadObj(MODEL_ENEMY_COLLISION, &g_Collision_Vertex);

	g_Vertex = new VERTEX_3D[g_Collision_Vertex.VertexNum];

	// ������
	for (int i = 0; i < g_Collision_Vertex.VertexNum; i++)
	{
		g_Vertex[i].Position = g_Collision_Vertex.VertexArray[i].Position;
		g_Vertex[i].Diffuse = g_Collision_Vertex.VertexArray[i].Diffuse;
		g_Vertex[i].Normal = g_Collision_Vertex.VertexArray[i].Normal;
		g_Vertex[i].TexCoord = g_Collision_Vertex.VertexArray[i].TexCoord;
	}


	g_Stage = GetStage();

	count = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	//return;

	
	{	// pop����
		count++;
		int useCount = 0;

		switch (g_Stage)
		{
		case stage0:
			// �����̏o�����Ă��邩���m�F
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].use == TRUE) useCount++;
			}

			// ���Ԍo�߂ƃG�l�~�[�̏o��������Ń|�b�v���邩���f
			if ((count % STAGE0_POP_COUNT == 0) && (useCount < STAGE0_MAX_POP))
			{
				SetEnemy();
			}

			break;

		case stage1:
			// �����̏o�����Ă��邩���m�F
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].use == TRUE) useCount++;
			}

			// ���Ԍo�߂ƃG�l�~�[�̏o��������Ń|�b�v���邩���f
			if ((count % STAGE1_POP_COUNT == 0) && (useCount < STAGE1_MAX_POP))
			{
				SetEnemy();
			}

			break;

		case tutorial:
			// �����̏o�����Ă��邩���m�F
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].use == TRUE) useCount++;
			}

			// ���Ԍo�߂ƃG�l�~�[�̏o��������Ń|�b�v���邩���f
			if ((count % STAGE2_POP_COUNT == 0) && (useCount < STAGE_TUTORIAL_MAX_POP))
			{
				SetEnemy();
			}

			break;
		}
	}


	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			// �������Ԃ��J�E���g
			if(g_Stage != tutorial) g_Enemy[i].liveCount++;

			// �U����H����Ă��Ȃ���΍U������
			if (g_Enemy[i].isHit == FALSE)
			{
				// �U������
				if (g_Enemy[i].liveCount > ENEMY_ATTACK_2)
				{	// �U�����s��
					// �������Ԃ����Z�b�g
					g_Enemy[i].liveCount = 0;

					// �������C�g�I�t
					g_Enemy[i].fuchi = FALSE;


					// �U��
					SetDamageEF(TRUE);
					// SetCameraShake(20);
					SetPlayerDamage(1);

				}
				else if(g_Enemy[i].liveCount > ENEMY_ATTACK_1)
				{	// �Ԃ��_�ł������Ȃ�
				
					if (g_Enemy[i].liveCount % ENEMY_BLINKING1 < ENEMY_BLINKING1 / 2)
					{	// �I�u�W�F�N�g��Ԃ�����

						// �������C�g�I��
						g_Enemy[i].fuchi = TRUE;
					}
					else
					{	// �I�u�W�F�N�g�̐F��߂�

						// �������C�g�I�t
						g_Enemy[i].fuchi = FALSE;
					}


				}
				else if (g_Enemy[i].liveCount > ENEMY_ATTACK_0)
				{	// �Ԃ��_�ł���

					if (g_Enemy[i].liveCount % ENEMY_BLINKING0 < ENEMY_BLINKING0 / 2)
					{	// �I�u�W�F�N�g��Ԃ�����

						// �������C�g�I��
						g_Enemy[i].fuchi = TRUE;
					}
					else
					{	// �I�u�W�F�N�g�̐F��߂�

						// �������C�g�I�t
						g_Enemy[i].fuchi = FALSE;
					}
				}
			}


			// �ڕW�n�_�܂œ��B���Ă��Ȃ��ꍇ�Ɉړ�����
			if (g_Enemy[i].pos.z > g_Enemy[i].zGoal)
			{
				BOOL ans = TRUE;
				// ���̃p�g�J�[�Ɠ������Ă��Ȃ������m�F
				for (int p = 0; p < MAX_ENEMY; p++)
				{
					//�G�̗L���t���O���`�F�b�N����
					if ((g_Enemy[p].use == FALSE) || (i == p) || (g_Enemy[p].isHit == TRUE)) continue;

					//BC�̓����蔻��
					if (CollisionBC(g_Enemy[i].pos, g_Enemy[p].pos, g_Enemy[p].size, g_Enemy[p].size) &&
						(g_Enemy[i].pos.z > g_Enemy[p].pos.z))
					{	// �������Ă��Ȃ��ꍇ�Ɉړ�

						ans = FALSE;
						break;
					}
				}

				if ((ans) && (GetTime() > 1))
				{
					g_Enemy[i].pos.z -= VALUE_MOVE;
				}
				else if ((ans) && (GetTime() <= 1))
				{
					g_Enemy[i].pos.z += VALUE_MOVE;
				}
			}


			// �����蔻��悤�̃{�b�N�X�ƈʒu�����L����
			g_Collision[i].pos = g_Enemy[i].pos;

			//=======================================================================
			// ������͓���������̃G�l�~�[�̋����͍U���I�u�W�F�N�g���ōs���܂��傤�I
			//=======================================================================

			// �G�l�~�[�̏����A�j���[�V����
			if (g_Enemy[i].isHit == TRUE)				// �U�����������Ă邩�H
			{											// Yes
				//BOOL ans = TRUE;

				// SE�̒�~
				StopSound(SOUND_LABEL_SE_siren01);

				// �F�����ɖ߂�
				g_Enemy[i].fuchi = FALSE;

				BLAST *blast = GetBlast();		// ���j�I�u�W�F�N�g�̏�����

				// �k�܂鏈��
				if ((blast[0].shrink) && (g_Enemy[i].hitTime > 0))
				{
					g_Enemy[i].pos.x += (g_Enemy[i].hitPos.x - g_Enemy[i].pos.x) / ENEMY_HIT_MOVE;
					g_Enemy[i].pos.y += (g_Enemy[i].hitPos.y - g_Enemy[i].pos.y) / ENEMY_HIT_MOVE;
					g_Enemy[i].pos.z += (g_Enemy[i].hitPos.z - g_Enemy[i].pos.z) / ENEMY_HIT_MOVE;

					// �����_���ɉ�]������
					g_Enemy[i].hitRot.x = RamdomFloat(2, 6.28f, -6.28f);
					g_Enemy[i].hitRot.y = RamdomFloat(2, 6.28f, -6.28f);
					g_Enemy[i].hitRot.z = RamdomFloat(2, 6.28f, -6.28f);

					if (g_Enemy[i].hitTime == 15)
					{
						g_Enemy[i].rot.x += g_Enemy[i].hitRot.x;
						g_Enemy[i].rot.y += g_Enemy[i].hitRot.y;
						g_Enemy[i].rot.z += g_Enemy[i].hitRot.z;
					}

					g_Enemy[i].hitTime--;
				}
				

				// ���e�ƈꏏ�ɗ�������
				BOOL camera = GetCameraSwitch();

				if (camera == FALSE && blast[0].move == FALSE)
				{
					g_Enemy[i].pos.y -= BLAST_DOWN / BLASE_DOWN_SPEED;

					if (g_Enemy[i].pos.y < ENEMY_OFFSET_Y)
					{
						g_Enemy[i].pos.y = ENEMY_OFFSET_Y;
					}
				}
				
				//���e�ƈꏏ�ɉ��ֈړ�����
				if (blast[0].move == TRUE) /*&& (g_Enemy[i].move == TRUE)*/ //&& (g_Enemy[i].hitTime == 0))
				{
					g_Enemy[i].pos.z += FIELD_SPEED+ 2.0f;
				}


				if (blast[0].use == FALSE)
				{
					g_Enemy[i].use = FALSE;
					g_Collision[i].use = FALSE;
				}
			}

			CAMERA *camera = GetCamera();
			TARGETOBJ *targetObj = GetTargetObj();
			TARGET *target = GetTarget();

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 hitPosition;								// ��_
			hitPosition = g_Enemy[i].pos;	// �O�ꂽ���p�ɏ��������Ă���
			bool ans = RayHitEnemy(targetObj[0].pos, camera->pos, &hitPosition, i);

			if ((ans) && (!g_Enemy[i].target) && (g_Enemy[i].use))
			{
				g_Enemy[i].target = TRUE;
				g_Collision[i].use = FALSE;
				target[0].enemyNum[target[0].count] = i;
				target[0].count++;

				SetRockOn();
			}
			////////////////////////////////////////////////////////////////////////
			//// �p������
			////////////////////////////////////////////////////////////////////////

			//XMVECTOR vx, nvx, up;
			//XMVECTOR quat;
			//float len, angle;

			//// �Q�̃x�N�g���̊O�ς�����ĔC�ӂ̉�]�������߂�
			//g_Enemy[i].upVector = normal;
			//up = { 0.0f, 1.0f, 0.0f, 0.0f };
			//vx = XMVector3Cross(up, XMLoadFloat3(&g_Enemy[i].upVector));

			//// ���߂���]������N�H�[�^�j�I�������o��
			//nvx = XMVector3Length(vx);
			//XMStoreFloat(&len, nvx);
			//nvx = XMVector3Normalize(vx);
			//angle = asinf(len);
			//quat = XMQuaternionRotationNormal(nvx, angle);

			//// �O��̃N�H�[�^�j�I�����獡��̃N�H�[�^�j�I���܂ł̉�]�����炩�ɂ���
			//quat = XMQuaternionSlerp(XMLoadFloat4(&g_Enemy[i].quaternion), quat, 0.05f);

			//// ����̃N�H�[�^�j�I���̌��ʂ�ۑ�����
			//XMStoreFloat4(&g_Enemy[i].quaternion, quat);

			//// �e���v���C���[�̈ʒu�ɍ��킹��
			//XMFLOAT3 pos = g_Enemy[i].pos;
			//pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			//SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �t�H�O�L��
	SetFogEnable(TRUE);

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// �������C�g�̐ݒ�
		SetFuchi(g_Enemy[i].fuchi);

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �N�H�[�^�j�I���𔽉f
		XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Enemy[i].quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// ���f���`��
		switch (g_Enemy[i].type)
		{
		case 0:
			DrawModel(&g_Enemy[0].model);
			break;

		case 1:
			DrawModel(&g_Enemy[1].model);
			break;
		
		case 2:
			DrawModel(&g_Enemy[2].model);
			break;
		
		case 3:
			DrawModel(&g_Enemy[3].model);
			break;
		
		case 4:
			DrawModel(&g_Enemy[4].model);
			break;
		
		case 5:
			DrawModel(&g_Enemy[5].model);
			break;
		
		case 6:
			DrawModel(&g_Enemy[6].model);
			break;
		
		case 7:
			DrawModel(&g_Enemy[7].model);
			break;
		
		case 8:
			DrawModel(&g_Enemy[8].model);
			break;
		}

#ifdef _DEBUG	// �f�o�b�O���̂ݕ\��

		// �R���W�����p�̃{�b�N�X�̕`��
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Collision[i].scl.x, g_Collision[i].scl.y, g_Collision[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Collision[i].rot.x, g_Collision[i].rot.y + XM_PI, g_Collision[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �N�H�[�^�j�I���𔽉f
		quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Collision[i].quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Collision[i].pos.x, g_Collision[i].pos.y, g_Collision[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);
		XMStoreFloat4x4(&g_Collision[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Collision[0].model);


		// �������C�g�̐ݒ�
		SetFuchi(FALSE);
#endif

	}
	
	// �t�H�O�L��
	SetFogEnable(FALSE);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}


//=============================================================================
// �G�l�~�[�̔���
//=============================================================================
void SetEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE)
		{
			//SetSourceVolume(SOUND_LABEL_SE_carHorn01, 1.0f);
			// SE�̃Z�b�g
			PlaySound(SOUND_LABEL_SE_siren01);

			g_Enemy[i].use = TRUE;
			g_Enemy[i].pos.z = ENEMY_POP_Z;
			g_Enemy[i].pos.y = ENEMY_OFFSET_Y;
			g_Enemy[i].isHit = FALSE;
			g_Enemy[i].cupHit = FALSE;
			g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

			// x���W�̓����_��
			g_Enemy[i].pos.x = (float)(rand() % ENEMY_POP_X) - ((float)ENEMY_POP_X / 2.0f);
			if (g_Stage == tutorial) g_Enemy[i].pos.x = 0.0f;

			// ���B�n�_�������_��
			g_Enemy[i].zGoal = (float)(rand() % ENEMY_GOAL_Z_OFFSET) + ENEMY_GOAL_Z;
			if (g_Stage == tutorial) g_Enemy[i].zGoal = 150.0f;

			g_Enemy[i].liveCount = 0;

			// �G�l�~�[�̃^�C�v�������_����
			g_Enemy[i].type = rand() % ENEMY_TYPE_MAX;

			// �������C�g�I�t
			g_Enemy[i].fuchi = FALSE;

			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE * 2.0f, ENEMY_SHADOW_SIZE * 2.0f);


			g_Collision[i].use = TRUE;
			g_Collision[i].pos = g_Enemy[i].pos;



			return;
		}
	}
}


BOOL RayHitEnemy(XMFLOAT3 Pos, XMFLOAT3 CameraPos, XMFLOAT3 *HitPosition, int num)
{
	XMFLOAT3 start = CameraPos;
	XMFLOAT3 end = Pos;
	XMFLOAT3 org = *HitPosition;

	// �K�v�����������J��Ԃ�
	for (int i = 0; i < g_Collision_Vertex.VertexNum - 2; i++)
	{
		XMFLOAT3 p0 = g_Vertex[i].Position;
		p0.x += g_Collision[num].pos.x;
		p0.y += g_Collision[num].pos.y;
		p0.z += g_Collision[num].pos.z;

		XMFLOAT3 p1 = g_Vertex[i + 1].Position;
		p1.x += g_Collision[num].pos.x;
		p1.y += g_Collision[num].pos.y;
		p1.z += g_Collision[num].pos.z;

		XMFLOAT3 p2 = g_Vertex[i + 2].Position;
		p2.x += g_Collision[num].pos.x;
		p2.y += g_Collision[num].pos.y;
		p2.z += g_Collision[num].pos.z;

		// �O�p�|���S��������Q�����̓����蔻��
		BOOL ans = RayCast(p0, p2, p1, start, end, HitPosition, &g_Vertex[i].Normal);
		if (ans)
		{
			return TRUE;
		}
	}

	// �����ɂ��������Ă��Ȃ�������
	*HitPosition = org;
	return FALSE;
}


// ���b�N�I������Ă��邩�̏�����
// �U�������Ƃ��ɌĂяo�����
void ResetEnemyTarget(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		g_Enemy[i].target = FALSE;
	}
}

// �X�N���[�����W�����[���h���W�֕ϊ�
//void SetScreenToWorld(void)
//{
//	TARGET *target = GetTarget();
//	float sx, sy;
//	float z = 1.0f;
//	XMFLOAT3 ans;
//	XMMATRIX *view;
//	XMMATRIX *proj;
//
//	D3D11_VIEWPORT vp;
//	vp.Width = (FLOAT)SCREEN_WIDTH;
//	vp.Height = (FLOAT)SCREEN_HEIGHT;
//	vp.MinDepth = 0.0f;
//	vp.MaxDepth = 1.0f;
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//
//	ans.x = sx = target->pos.x;
//	ans.y = sy = target->pos.y;
//	ans.z = z;
//
//	XMMATRIX invMat, inv_proj, inv_view;
//	XMStoreFloat4x4(&g_Camera.mtxProjection, mtxProjection);
//
//	XMMatrixInverse(vp, invMat);
//}

