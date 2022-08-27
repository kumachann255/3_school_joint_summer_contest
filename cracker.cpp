//=============================================================================
//
// �N���b�J�[�Ɠ����蔻�菈�� [cracker.cpp]
// Author : �G�i
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "attackRange.h"
#include "shadow.h"
#include "cracker.h"
#include "meshfield.h"
#include "speech.h"
#include "enemy.h"
#include "enemyHeli.h"
#include "tutorial.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_CRACKER			"data/MODEL/newneba00.obj"		// �ǂݍ��ރ��f����
#define	MODEL_CRACKER_1			"data/MODEL/newneba01.obj"		// �ǂݍ��ރ��f����
#define	MODEL_CRACKER_2			"data/MODEL/newneba02.obj"		// �ǂݍ��ރ��f����
#define	MODEL_CRACKER_3			"data/MODEL/newneba03.obj"		// �ǂݍ��ރ��f����
#define	MODEL_CRACKER_4			"data/MODEL/newneba04.obj"		// �ǂݍ��ރ��f����


#define	VALUE_MOVE				(5.0f)						// �ړ���
#define	VALUE_ROTATE			(XM_PI * 0.02f)				// ��]��

#define CRACKER_OFFSET_Y		(12.0f)						// �I�u�W�F�N�g�̑��������킹��

#define CRACKER_LIFE			(150)						// ���j�I�u�W�F�N�g�̕\������
#define CRACKER_MOVE_TIME_0	(3)								// �˂΂˂΂��L���鎞��
#define CRACKER_MOVE_TIME_1	(10)							// �L��������ԂŎ~�܂鎞��
#define CRACKER_MOVE_TIME_2	(10)							// �˂΂˂΂��k�܂鎞��
#define CRACKER_MOVE_TIME_3	(20)							// �˂΂˂΂��󒆂Ŏ~�܂鎞��

#define CRACKER_ROT				(XM_PI * 0.5f * 0.2f)		// ��]��(X��)
#define CRACKER_ROT_MAX			(XM_PI * 0.5f)				// ��]�ő�l

#define CRACKER_MOVE			(-2.0f)						// �ړ���(Z��)
#define CRACKER_MOVE_TIME		(10)						// �ړ��ő�l

#define MAX_CRACKER_MOVE		(5)							// ���[�t�B���O�̐�



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CRACKER			g_Cracker[MAX_CRACKER];				// ���j�I�u�W�F�N�g

static BOOL				g_Load = FALSE;

static MODEL			g_Cracker_Vertex[MAX_CRACKER_MOVE];		// ���[�t�B���O�p���f���̒��_��񂪓������f�[�^�z��
static VERTEX_3D		*g_CrackerVertex = NULL;			// �r���o�߂��L�^����ꏊ

static float			g_time;							// ���[�t�B���O�̌o�ߎ���
static int				g_downCount;					// �����n�߂Ăǂ̂��炢���Ԃ��o�߂�����

static int				g_morphingNum;					// ���[�t�B���O�̔ԍ�

static BOOL				g_cameraOn;						// �J�����̃X�C�b�`

static int				g_stopTime;						// ���e���󒆂Ŏ~�܂鎞��

static float			g_CrackerMove;					//�N���b�J�[�̈ړ���(Z��)
static int				g_CrackerTime;					//�N���b�J�[�̈ړ���(Z��)
static float			g_CrackerRot;					// �N���b�J�[�̉�]��(X��)

static BOOL				g_particleOn;					// TRUE:�����G�t�F�N�g����

//=============================================================================
// ����������
//=============================================================================
HRESULT InitCracker(void)
{
	for (int i = 0; i < MAX_CRACKER; i++)
	{
		LoadModelMorphing(MODEL_CRACKER, &g_Cracker[i].model);
		g_Cracker[i].load = TRUE;

		g_Cracker[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Cracker[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Cracker[i].scl = XMFLOAT3(1.3f, 1.3f, 1.3f);
		g_Cracker[i].size = CRACKER_SIZE;
		g_Cracker[i].life = 0;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Cracker[i].model, &g_Cracker[i].diffuse[0]);

		g_Cracker[i].use = FALSE;			// TRUE:�����Ă�
		//g_Cracker[i].move = FALSE;		// TRUE:���ֈړ�����
	
	}

	// ���[�t�B���O����I�u�W�F�N�g�̓ǂݍ���
	LoadObj(MODEL_CRACKER, &g_Cracker_Vertex[0]);
	LoadObj(MODEL_CRACKER_1, &g_Cracker_Vertex[1]);
	LoadObj(MODEL_CRACKER_2, &g_Cracker_Vertex[2]);
	LoadObj(MODEL_CRACKER_3, &g_Cracker_Vertex[3]);
	LoadObj(MODEL_CRACKER_4, &g_Cracker_Vertex[4]);

	// ���g��z��Ƃ��Ďg�p�ł���悤�Ɏd�l�ύX
	g_CrackerVertex = new VERTEX_3D[g_Cracker_Vertex[0].VertexNum];

	// ����(�r���o��)�̏�����
	for (int i = 0; i < g_Cracker_Vertex[0].VertexNum; i++)
	{
		g_CrackerVertex[i].Position = g_Cracker_Vertex[0].VertexArray[i].Position;
		g_CrackerVertex[i].Diffuse  = g_Cracker_Vertex[0].VertexArray[i].Diffuse;
		g_CrackerVertex[i].Normal   = g_Cracker_Vertex[0].VertexArray[i].Normal;
		g_CrackerVertex[i].TexCoord = g_Cracker_Vertex[0].VertexArray[i].TexCoord;
	}

	g_morphingNum = 0;
	g_time = 0.0f;
	g_downCount = 0;
	g_cameraOn = FALSE;
	g_stopTime = 0;

	g_CrackerMove = CRACKER_MOVE;
	g_CrackerTime = CRACKER_MOVE_TIME;
	g_CrackerRot = CRACKER_ROT;
	
	g_particleOn = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitCracker(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_CRACKER; i++)
	{
		if (g_Cracker[i].load)
		{
			UnloadModel(&g_Cracker[i].model);
			g_Cracker[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateCracker(void)
{
	for (int i = 0; i < MAX_CRACKER; i++)
	{
		g_particleOn = FALSE;		//�G�t�F�N�g�t���O���I�t�ɂ���

		if (g_Cracker[i].use == TRUE)			// ���̍U���͈͂��g���Ă���H
		{									// Yes
			// ���������炷
			g_Cracker[i].life--;

			// �������s�����疢�g�p��
			if (g_Cracker[i].life < 0)
			{
				g_Cracker[i].use = FALSE;
			}

			// �N���b�J�[�̈ړ�����
			{
				if (g_CrackerTime > 0)
				{
					g_Cracker[i].pos.z += g_CrackerMove;
				}

				g_CrackerTime--;

				// ��]����
				g_Cracker[i].rot.x += g_CrackerRot;

				if (g_Cracker[i].rot.x >= CRACKER_ROT_MAX)
				{
					g_CrackerRot = g_CrackerRot * (-0.2f);
				}
		
				if (g_Cracker[i].rot.x <= 0.0f)
				{
					g_Cracker[i].rot.x = 0.0f;
				}
			
			}


			// ����������G�l�~�[�̋�������
			{
				//ENEMY *enemy = GetEnemy();		// �G�l�~�[�̃|�C���^�[��������
				//ENEMY_HELI *enemyheli = GetEnemyHeli();		// �G�l�~�[�̃|�C���^�[��������

				// ���G�l�~�[�̏���
				//for (int j = 0; j < MAX_ENEMY; j++)
				//{
				//	if (enemy[j].CrackerHit == TRUE)
				//	{
				//		// �����_������
				//		if (enemy[j].CrackerRot == TRUE)
				//		{
				//			enemy[j].rot.x = RamdomFloat(2, 6.28f, -6.28f);
				//			enemy[j].rot.z = RamdomFloat(2, 6.28f, -6.28f);
				//			enemy[j].pos.y += RamdomFloat(2, 50.0f, 20.0f);

				//			g_radius = RamdomFloat(2, 20.0f, 5.0f);
				//		
				//			enemy[j].CrackerRot = FALSE;
				//		}
				//		
				//		// ��]������
				//		enemy[j].rot.y += CRACKER_ROT_Y;
				//		enemy[j].pos.x = g_Cracker[i].pos.x + sinf(enemy[j].radian) * g_radius;
				//		enemy[j].pos.z = g_Cracker[i].pos.z + cosf(enemy[j].radian) * g_radius;

				//		enemy[j].radian += CRACKER_ROT_Y;

				//		// �G�l�~�[��|��
				//		if (g_Cracker[0].use == FALSE)
				//		{
				//			enemy[j].use = FALSE;
				//			SetTutorialEnemy(TRUE);
				//			g_particleOn = TRUE;
				//		}

				//	}
				//}
			
				// �w���̏���
				//for (int j = 0; j < MAX_ENEMY_HELI; j++)
				//{
				//	if (enemyheli[j].CrackerHit == TRUE)
				//	{
				//		// �����_������
				//		if (enemyheli[j].CrackerRot == TRUE)
				//		{
				//			enemyheli[j].rot.x = RamdomFloat(2, 6.28f, -6.28f);
				//			enemyheli[j].rot.z = RamdomFloat(2, 6.28f, -6.28f);
				//			enemyheli[j].pos.y += RamdomFloat(2, 50.0f, 20.0f);

				//			g_radius = RamdomFloat(2, 20.0f, 5.0f);

				//			enemyheli[j].CrackerRot = FALSE;
				//		}

				//		// ��]������
				//		enemyheli[j].rot.y += CRACKER_ROT_Y;
				//		enemyheli[j].pos.x = g_Cracker[i].pos.x + sinf(enemyheli[j].radian) * g_radius;
				//		enemyheli[j].pos.z = g_Cracker[i].pos.z + cosf(enemyheli[j].radian) * g_radius;

				//		enemyheli[j].radian += CRACKER_ROT_Y;

				//		// �G�l�~�[��|��
				//		if (g_Cracker[0].use == FALSE)
				//		{
				//			enemyheli[j].use = FALSE;
				//			//SetTutorialEnemy(TRUE);
				//			g_particleOn = TRUE;
				//		}

				//	}
				//}

			}


			//// �˂΂˂΂̃��[�t�B���O�̏���
			//{
			//	int after, brfore;

			//	// �L����I������k�܂�����փ��[�t�B���O
			//	if (g_Cracker[i].life < CRACKER_LIFE - CRACKER_MOVE_TIME_0)
			//	{
			//		g_morphingNum = 1;
			//	}
			//	else
			//	{
			//		g_morphingNum = 0;
			//	}


			//	// ���[�t�B���O���f���̔ԍ�����
			//	switch (g_morphingNum)
			//	{
			//	case 0:
			//		after = g_Cracker[i].after;
			//		brfore = 0;

			//		break;

			//	case 1:
			//		after = 0;
			//		brfore = g_Cracker[i].after;

			//		break;
			//	}


			//	// ���[�t�B���O����
			//	for (int p = 0; p < g_Cracker_Vertex[0].VertexNum; p++)
			//	{
			//		g_CrackerVertex[p].Position.x = g_Cracker_Vertex[after].VertexArray[p].Position.x - g_Cracker_Vertex[brfore].VertexArray[p].Position.x;
			//		g_CrackerVertex[p].Position.y = g_Cracker_Vertex[after].VertexArray[p].Position.y - g_Cracker_Vertex[brfore].VertexArray[p].Position.y;
			//		g_CrackerVertex[p].Position.z = g_Cracker_Vertex[after].VertexArray[p].Position.z - g_Cracker_Vertex[brfore].VertexArray[p].Position.z;

			//		g_CrackerVertex[p].Position.x *= g_time;
			//		g_CrackerVertex[p].Position.y *= g_time;
			//		g_CrackerVertex[p].Position.z *= g_time;

			//		g_CrackerVertex[p].Position.x += g_Cracker_Vertex[brfore].VertexArray[p].Position.x;
			//		g_CrackerVertex[p].Position.y += g_Cracker_Vertex[brfore].VertexArray[p].Position.y;
			//		g_CrackerVertex[p].Position.z += g_Cracker_Vertex[brfore].VertexArray[p].Position.z;
			//	}

			//	// ���Ԃ�i�߂�
			//	switch (g_morphingNum)
			//	{
			//	case 0:
			//		if (g_time < 1.0f) g_time += 1.0f / CRACKER_MOVE_TIME_0;

			//		break;

			//	case 1:
			//		if ((g_time < 1.0f) && (g_Cracker[i].life < CRACKER_LIFE - CRACKER_MOVE_TIME_0 - CRACKER_MOVE_TIME_1))
			//		{
			//			g_time += 1.0f / CRACKER_MOVE_TIME_2;
			//			g_Cracker[i].shrink = TRUE;
			//		}
			//		break;
			//	}

			//	// ��ꃂ�[�t�B���O�����������玟�̃��[�t�B���O��
			//	if ((g_time >= 1.0f) && (g_morphingNum == 0))
			//	{
			//		g_time = 0.0f;
			//		g_morphingNum++;
			//	}

			//	// ��񃂁[�t�B���O���I�������͂�����Ǝ~�܂�
			//	if ((g_time >= 1.0f) && (g_morphingNum == 1) && (g_downCount < BLASE_DOWN_SPEED))
			//	{
			//		g_stopTime++;
			//	}

			//	if ((g_stopTime >= CRACKER_MOVE_TIME_3) && (g_downCount < BLASE_DOWN_SPEED))
			//	{
			//		g_Cracker[i].pos.y -= CRACKER_DOWN / BLASE_DOWN_SPEED;
			//		g_downCount++;

			//		g_cameraOn = FALSE;
			//	}
			//	
			//	// ��������͏��ƈꏏ�ɉ��ֈړ�����
			//	if (g_downCount >= BLASE_DOWN_SPEED)
			//	{
			//		g_Cracker[i].pos.z += FIELD_SPEED;
			//		g_Cracker[i].move = TRUE;
			//	}
			//		

			//	D3D11_SUBRESOURCE_DATA sd;
			//	ZeroMemory(&sd, sizeof(sd));
			//	sd.pSysMem = g_CrackerVertex;

			//	// ���_�o�b�t�@�ɒl���Z�b�g����
			//	D3D11_MAPPED_SUBRESOURCE msr;
			//	GetDeviceContext()->Map(g_Cracker[0].model.VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
			//	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			//	// �S���_���𖈉�㏑�����Ă���̂�DX11�ł͂��̕�����������ł�
			//	memcpy(pVtx, g_CrackerVertex, sizeof(VERTEX_3D)*g_Cracker_Vertex[0].VertexNum);

			//	GetDeviceContext()->Unmap(g_Cracker[0].model.VertexBuffer, 0);
			//}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawCracker(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;


	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// ���e�X�g��L����
	SetAlphaTestEnable(TRUE);

	for (int i = 0; i < MAX_CRACKER; i++)
	{
		if (g_Cracker[i].use == TRUE)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Cracker[i].scl.x, g_Cracker[i].scl.y, g_Cracker[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Cracker[i].rot.x, g_Cracker[i].rot.y + XM_PI, g_Cracker[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Cracker[i].pos.x, g_Cracker[i].pos.y, g_Cracker[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Cracker[i].mtxWorld, mtxWorld);


			// ���f���`��
			DrawModel(&g_Cracker[i].model);
		}
	}

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);

}

//=============================================================================
// ���j�̎擾
//=============================================================================
CRACKER *GetCracker()
{
	return &g_Cracker[0];
}


//=============================================================================
// �N���b�J�[�̔���
//=============================================================================
void SetCracker(void)
{
	for (int i = 0; i < MAX_CRACKER; i++)
	{
		if (g_Cracker[i].use == FALSE)
		{
			// SE�̃Z�b�g
			PlaySound(SOUND_LABEL_SE_stickingSound01);

			g_Cracker[i].use = TRUE;
			//g_Cracker[i].move = FALSE;
			//g_Cracker[i].shrink = FALSE;

			//g_Cracker[i].pos = pos;
			ATTACKRANGE *AttackR = GetAttackR();
			
			g_Cracker[i].pos = AttackR->pos;
			g_Cracker[i].pos.y = CRACKER_OFFSET_Y;

			g_Cracker[i].rot.x = 0.0f;

			g_Cracker[i].life = CRACKER_LIFE;

			g_CrackerTime = CRACKER_MOVE_TIME;

			g_particleOn = FALSE;

			//g_Cracker[i].after = (rand() % (MAX_CRACKER_MOVE - 1)) + 1;

			g_cameraOn = TRUE;

			//g_time = 0.0f;
			//g_morphingNum = 0;
			//g_downCount = 0;
			//g_stopTime = 0;

			//g_Cracker[i].rot.y = RamdomFloat(2, CRACKER_ROT, -CRACKER_ROT);

			// �����o����\��
			//SetBomSpeech();

			return;
		}
	}
}


//=============================================================================
// ���������t���O���擾
//=============================================================================
//BOOL GetParticleOn(void)
//{
//	return g_particleOn;
//}

//=============================================================================
// �J������؂�ւ��邩�ǂ����̃X�C�b�`��m��֐�
//=============================================================================
//BOOL GetCameraSwitch(void)
//{
//	return g_cameraOn;
//}


//=============================================================================
// �J������؂�ւ��邩�ǂ����̃X�C�b�`
//=============================================================================
//void SetCameraSwitch(BOOL data)
//{
//	g_cameraOn = data;
//}


//=============================================================================
// ���݂̃��[�t�B���O�ԍ����擾
//=============================================================================
//int GetMorphing(void)
//{
//	return g_morphingNum;
//}


//=============================================================================
// ���e���󒆂Ŏ~�܂鎞�Ԃ�Ԃ�
//=============================================================================
//int GetStopTime(void)
//{
//	return g_stopTime;
//}