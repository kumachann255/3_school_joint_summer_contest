//=============================================================================
//
// �J�b�v�Ɠ����蔻�菈�� [cup.cpp]
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
#include "cup.h"
#include "meshfield.h"
#include "speech.h"
#include "enemy.h"
#include "enemyHeli.h"
#include "tutorial.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_CUP			"data/MODEL/newneba00.obj"		// �ǂݍ��ރ��f����
#define	MODEL_CUP_1			"data/MODEL/newneba01.obj"		// �ǂݍ��ރ��f����
#define	MODEL_CUP_2			"data/MODEL/newneba02.obj"		// �ǂݍ��ރ��f����
#define	MODEL_CUP_3			"data/MODEL/newneba03.obj"		// �ǂݍ��ރ��f����
#define	MODEL_CUP_4			"data/MODEL/newneba04.obj"		// �ǂݍ��ރ��f����


#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define CUP_OFFSET_Y		(12.0f)						// �I�u�W�F�N�g�̑��������킹��

#define CUP_LIFE			(150)						// ���j�I�u�W�F�N�g�̕\������
#define CUP_MOVE_TIME_0	(3)								// �˂΂˂΂��L���鎞��
#define CUP_MOVE_TIME_1	(10)							// �L��������ԂŎ~�܂鎞��
#define CUP_MOVE_TIME_2	(10)							// �˂΂˂΂��k�܂鎞��
#define CUP_MOVE_TIME_3	(20)							// �˂΂˂΂��󒆂Ŏ~�܂鎞��

#define CUP_ROT_Y			(XM_PI * 0.04f)				// ��]��

#define CUP_SPEED_Z			(2.0f)						// �ړ���(Z��)
#define CUP_SPEED_X			(8.0f)						// �ړ���(X��)

#define CUP_RANGE_ELEA_X	(161.0f)					// �J�b�v�̈ړ��͈�

#define MAX_CUP_MOVE		(5)							// ���[�t�B���O�̐�



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CUP			g_Cup[MAX_CUP];				// ���j�I�u�W�F�N�g

static BOOL				g_Load = FALSE;

static MODEL			g_Cup_Vertex[MAX_CUP_MOVE];		// ���[�t�B���O�p���f���̒��_��񂪓������f�[�^�z��
static VERTEX_3D		*g_CupVertex = NULL;			// �r���o�߂��L�^����ꏊ

static float			g_time;							// ���[�t�B���O�̌o�ߎ���
static int				g_downCount;					// �����n�߂Ăǂ̂��炢���Ԃ��o�߂�����

static int				g_morphingNum;					// ���[�t�B���O�̔ԍ�

static BOOL				g_cameraOn;						// �J�����̃X�C�b�`

static int				g_stopTime;						// ���e���󒆂Ŏ~�܂鎞��

static float			g_cupSpeed_Z;					// �J�b�v�̈ړ��X�s�[�h(Z��)
static float			g_cupSpeed_X;					// �J�b�v�̈ړ��X�s�[�h(X��)

static float			g_radius;						// �G�l�~�[��]�p
static float			g_rot;							// �G�l�~�[��]�p

static BOOL				g_particleOn;					// TRUE:�����G�t�F�N�g����

//=============================================================================
// ����������
//=============================================================================
HRESULT InitCup(void)
{
	for (int i = 0; i < MAX_CUP; i++)
	{
		LoadModelMorphing(MODEL_CUP, &g_Cup[i].model);
		g_Cup[i].load = TRUE;

		g_Cup[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Cup[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Cup[i].scl = XMFLOAT3(1.3f, 1.3f, 1.3f);
		g_Cup[i].size = CUP_SIZE;
		g_Cup[i].life = 0;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Cup[i].model, &g_Cup[i].diffuse[0]);

		g_Cup[i].use = FALSE;			// TRUE:�����Ă�
		//g_Cup[i].move = FALSE;		// TRUE:���ֈړ�����
	
	}

	// ���[�t�B���O����I�u�W�F�N�g�̓ǂݍ���
	LoadObj(MODEL_CUP, &g_Cup_Vertex[0]);
	LoadObj(MODEL_CUP_1, &g_Cup_Vertex[1]);
	LoadObj(MODEL_CUP_2, &g_Cup_Vertex[2]);
	LoadObj(MODEL_CUP_3, &g_Cup_Vertex[3]);
	LoadObj(MODEL_CUP_4, &g_Cup_Vertex[4]);

	// ���g��z��Ƃ��Ďg�p�ł���悤�Ɏd�l�ύX
	g_CupVertex = new VERTEX_3D[g_Cup_Vertex[0].VertexNum];

	// ����(�r���o��)�̏�����
	for (int i = 0; i < g_Cup_Vertex[0].VertexNum; i++)
	{
		g_CupVertex[i].Position = g_Cup_Vertex[0].VertexArray[i].Position;
		g_CupVertex[i].Diffuse  = g_Cup_Vertex[0].VertexArray[i].Diffuse;
		g_CupVertex[i].Normal   = g_Cup_Vertex[0].VertexArray[i].Normal;
		g_CupVertex[i].TexCoord = g_Cup_Vertex[0].VertexArray[i].TexCoord;
	}

	g_morphingNum = 0;
	g_time = 0.0f;
	g_downCount = 0;
	g_cameraOn = FALSE;
	g_stopTime = 0;

	g_cupSpeed_Z = CUP_SPEED_Z;
	g_cupSpeed_X = CUP_SPEED_X;

	g_radius = 0.0f;
	g_rot	 = 0.0f;
	
	g_particleOn = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitCup(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_CUP; i++)
	{
		if (g_Cup[i].load)
		{
			UnloadModel(&g_Cup[i].model);
			g_Cup[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateCup(void)
{
	for (int i = 0; i < MAX_CUP; i++)
	{
		g_particleOn = FALSE;		//�G�t�F�N�g�t���O���I�t�ɂ���

		if (g_Cup[i].use == TRUE)			// ���̍U���͈͂��g���Ă���H
		{									// Yes
			// ���������炷
			g_Cup[i].life--;

			// �������s�����疢�g�p��
			if (g_Cup[i].life < 0)
			{
				g_Cup[i].use = FALSE;
			}

			// �J�b�v�̈ړ�����
			{
				g_Cup[i].pos.z += g_cupSpeed_Z;
				g_Cup[i].pos.x += g_cupSpeed_X;
				g_Cup[i].rot.y += CUP_ROT_Y;


				if (g_Cup[i].pos.x <= -CUP_RANGE_ELEA_X || g_Cup[i].pos.x >= CUP_RANGE_ELEA_X)
				{
					g_cupSpeed_X *= -1;
				}
			}

			// ����������G�l�~�[�̋�������
			{
				ENEMY *enemy = GetEnemy();		// �G�l�~�[�̃|�C���^�[��������
				ENEMY_HELI *enemyheli = GetEnemyHeli();		// �G�l�~�[�̃|�C���^�[��������

				// ���G�l�~�[�̏���
				for (int j = 0; j < MAX_ENEMY; j++)
				{
					if (enemy[j].cupHit == TRUE)
					{
						// �����_������
						if (enemy[j].cupRot == TRUE)
						{
							enemy[j].rot.x = RamdomFloat(2, 6.28f, -6.28f);
							enemy[j].rot.z = RamdomFloat(2, 6.28f, -6.28f);
							enemy[j].pos.y += RamdomFloat(2, 50.0f, 20.0f);

							g_radius = RamdomFloat(2, 20.0f, 5.0f);
						
							enemy[j].cupRot = FALSE;
						}
						
						// ��]������
						enemy[j].rot.y += CUP_ROT_Y;
						enemy[j].pos.x = g_Cup[i].pos.x + sinf(enemy[j].radian) * g_radius;
						enemy[j].pos.z = g_Cup[i].pos.z + cosf(enemy[j].radian) * g_radius;

						enemy[j].radian += CUP_ROT_Y;

						// �G�l�~�[��|��
						if (g_Cup[0].use == FALSE)
						{
							enemy[j].use = FALSE;
							SetTutorialEnemy(TRUE);
							g_particleOn = TRUE;
						}

					}
				}
			
				// �w���̏���
				for (int j = 0; j < MAX_ENEMY_HELI; j++)
				{
					if (enemyheli[j].cupHit == TRUE)
					{
						// �����_������
						if (enemyheli[j].cupRot == TRUE)
						{
							enemyheli[j].rot.x = RamdomFloat(2, 6.28f, -6.28f);
							enemyheli[j].rot.z = RamdomFloat(2, 6.28f, -6.28f);
							enemyheli[j].pos.y += RamdomFloat(2, 50.0f, 20.0f);

							g_radius = RamdomFloat(2, 20.0f, 5.0f);

							enemyheli[j].cupRot = FALSE;
						}

						// ��]������
						enemyheli[j].rot.y += CUP_ROT_Y;
						enemyheli[j].pos.x = g_Cup[i].pos.x + sinf(enemyheli[j].radian) * g_radius;
						enemyheli[j].pos.z = g_Cup[i].pos.z + cosf(enemyheli[j].radian) * g_radius;

						enemyheli[j].radian += CUP_ROT_Y;

						// �G�l�~�[��|��
						if (g_Cup[0].use == FALSE)
						{
							enemyheli[j].use = FALSE;
							//SetTutorialEnemy(TRUE);
							g_particleOn = TRUE;
						}

					}
				}

			}


			//// �˂΂˂΂̃��[�t�B���O�̏���
			//{
			//	int after, brfore;

			//	// �L����I������k�܂�����փ��[�t�B���O
			//	if (g_Cup[i].life < CUP_LIFE - CUP_MOVE_TIME_0)
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
			//		after = g_Cup[i].after;
			//		brfore = 0;

			//		break;

			//	case 1:
			//		after = 0;
			//		brfore = g_Cup[i].after;

			//		break;
			//	}


			//	// ���[�t�B���O����
			//	for (int p = 0; p < g_Cup_Vertex[0].VertexNum; p++)
			//	{
			//		g_CupVertex[p].Position.x = g_Cup_Vertex[after].VertexArray[p].Position.x - g_Cup_Vertex[brfore].VertexArray[p].Position.x;
			//		g_CupVertex[p].Position.y = g_Cup_Vertex[after].VertexArray[p].Position.y - g_Cup_Vertex[brfore].VertexArray[p].Position.y;
			//		g_CupVertex[p].Position.z = g_Cup_Vertex[after].VertexArray[p].Position.z - g_Cup_Vertex[brfore].VertexArray[p].Position.z;

			//		g_CupVertex[p].Position.x *= g_time;
			//		g_CupVertex[p].Position.y *= g_time;
			//		g_CupVertex[p].Position.z *= g_time;

			//		g_CupVertex[p].Position.x += g_Cup_Vertex[brfore].VertexArray[p].Position.x;
			//		g_CupVertex[p].Position.y += g_Cup_Vertex[brfore].VertexArray[p].Position.y;
			//		g_CupVertex[p].Position.z += g_Cup_Vertex[brfore].VertexArray[p].Position.z;
			//	}

			//	// ���Ԃ�i�߂�
			//	switch (g_morphingNum)
			//	{
			//	case 0:
			//		if (g_time < 1.0f) g_time += 1.0f / CUP_MOVE_TIME_0;

			//		break;

			//	case 1:
			//		if ((g_time < 1.0f) && (g_Cup[i].life < CUP_LIFE - CUP_MOVE_TIME_0 - CUP_MOVE_TIME_1))
			//		{
			//			g_time += 1.0f / CUP_MOVE_TIME_2;
			//			g_Cup[i].shrink = TRUE;
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

			//	if ((g_stopTime >= CUP_MOVE_TIME_3) && (g_downCount < BLASE_DOWN_SPEED))
			//	{
			//		g_Cup[i].pos.y -= CUP_DOWN / BLASE_DOWN_SPEED;
			//		g_downCount++;

			//		g_cameraOn = FALSE;
			//	}
			//	
			//	// ��������͏��ƈꏏ�ɉ��ֈړ�����
			//	if (g_downCount >= BLASE_DOWN_SPEED)
			//	{
			//		g_Cup[i].pos.z += FIELD_SPEED;
			//		g_Cup[i].move = TRUE;
			//	}
			//		

			//	D3D11_SUBRESOURCE_DATA sd;
			//	ZeroMemory(&sd, sizeof(sd));
			//	sd.pSysMem = g_CupVertex;

			//	// ���_�o�b�t�@�ɒl���Z�b�g����
			//	D3D11_MAPPED_SUBRESOURCE msr;
			//	GetDeviceContext()->Map(g_Cup[0].model.VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
			//	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			//	// �S���_���𖈉�㏑�����Ă���̂�DX11�ł͂��̕�����������ł�
			//	memcpy(pVtx, g_CupVertex, sizeof(VERTEX_3D)*g_Cup_Vertex[0].VertexNum);

			//	GetDeviceContext()->Unmap(g_Cup[0].model.VertexBuffer, 0);
			//}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawCup(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;


	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// ���e�X�g��L����
	SetAlphaTestEnable(TRUE);

	for (int i = 0; i < MAX_CUP; i++)
	{
		if (g_Cup[i].use == TRUE)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Cup[i].scl.x, g_Cup[i].scl.y, g_Cup[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Cup[i].rot.x, g_Cup[i].rot.y + XM_PI, g_Cup[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Cup[i].pos.x, g_Cup[i].pos.y, g_Cup[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Cup[i].mtxWorld, mtxWorld);


			// ���f���`��
			DrawModel(&g_Cup[i].model);
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
CUP *GetCup()
{
	return &g_Cup[0];
}


//=============================================================================
// �J�b�v�̔���
//=============================================================================
void SetCup(void)
{
	for (int i = 0; i < MAX_CUP; i++)
	{
		if (g_Cup[i].use == FALSE)
		{
			// SE�̃Z�b�g
			PlaySound(SOUND_LABEL_SE_stickingSound01);

			g_Cup[i].use = TRUE;
			//g_Cup[i].move = FALSE;
			//g_Cup[i].shrink = FALSE;

			//g_Cup[i].pos = pos;
			ATTACKRANGE *AttackR = GetAttackR();
			
			g_Cup[i].pos = AttackR->pos;
			g_Cup[i].pos.y = CUP_OFFSET_Y;
			
			g_Cup[i].rot.y = 0.0f;

			g_Cup[i].life = CUP_LIFE;

			g_particleOn = FALSE;

			//g_Cup[i].after = (rand() % (MAX_CUP_MOVE - 1)) + 1;

			g_cameraOn = TRUE;

			//g_time = 0.0f;
			//g_morphingNum = 0;
			//g_downCount = 0;
			//g_stopTime = 0;

			//g_Cup[i].rot.y = RamdomFloat(2, CUP_ROT, -CUP_ROT);

			// �����o����\��
			//SetBomSpeech();

			return;
		}
	}
}


//=============================================================================
// ���������t���O���擾
//=============================================================================
BOOL GetParticleOn(void)
{
	return g_particleOn;
}

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