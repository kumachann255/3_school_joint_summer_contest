//=============================================================================
//
// ���f������ [player.cpp]
// Author : 
//
//=============================================================================
#include <time.h>
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "enemy.h"
#include "sky_enemy.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"
#include "collision.h"
#include "target.h"
#include "targetObj.h"
#include "rockOn.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_SKY_ENEMY		"data/MODEL/m.obj"			// �ǂݍ��ރ��f����
#define	MODEL_SKY_ENEMY_PARTS	"data/MODEL/torus.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_COLLISION	"data/MODEL/collisionBox.obj"	// �ǂݍ��ރ��f����

#define	VALUE_MOVE				(2.0f)							// �ړ���
#define	VALUE_ROTATE			(XM_PI * 0.02f)					// ��]��

#define SKY_ENEMY_SHADOW_SIZE	(1.0f)							// �e�̑傫��
#define SKY_ENEMY_OFFSET_Y		(30.0f)							// �G�l�~�[�̑��������킹��

#define SKY_ENEMY_PARTS_MAX		(2)								// �G�l�~�[�̃p�[�c�̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static SKY_ENEMY		g_SkyEnemy[MAX_SKY_ENEMY];						// �v���C���[
static ENEMY			g_Collision[MAX_SKY_ENEMY];			// �����蔻��p�̃{�b�N�X

static SKY_ENEMY		g_Parts[MAX_SKY_ENEMY][SKY_ENEMY_PARTS_MAX];		// �v���C���[�̃p�[�c�p

static VERTEX_3D		*g_Vertex = NULL;				// 
static MODEL			g_Collision_Vertex;	// 

static BOOL		g_Load = FALSE;




// �v���C���[�̊K�w�A�j���[�V�����f�[�^
// �v���C���[�̓������E�ɓ������Ă���A�j���f�[�^
static INTERPOLATION_DATA move_tbl_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};



//=============================================================================
// ����������
//=============================================================================
HRESULT InitSkyEnemy(void)
{
	LoadModel(MODEL_ENEMY_COLLISION, &g_Collision[0].model);

	for (int i = 0; i < MAX_SKY_ENEMY; i++)
	{

		LoadModel(MODEL_SKY_ENEMY, &g_SkyEnemy[i].model);
		g_SkyEnemy[i].load = TRUE;

		g_SkyEnemy[i].radius1 = (float)(150 + rand() % 250);					// �T�[�N���P�̔��a	rand�֐���int�^�Ȃ̂�float�^�ɃL���X�g���Ă���

		g_SkyEnemy[i].angle1 = (float)(rand() % 360) * (XM_2PI / 360);		// �T�[�N���P�̊p�x	rand�֐���int�^�Ȃ̂�float�^�ɃL���X�g���Ă���
		g_SkyEnemy[i].angle2 = (float)(rand() % 360) * (XM_2PI / 360);		// �T�[�N���Q�̊p�x rand�֐���int�^�Ȃ̂�float�^�ɃL���X�g���Ă���


		// �G�l�~�[���o�����鏉���ʒu
		g_SkyEnemy[i].pos = { g_SkyEnemy[i].radius1 * cosf(g_SkyEnemy[i].angle1),	// X
						 g_SkyEnemy[i].radius1 * sinf(g_SkyEnemy[i].angle2),	// Y
						 g_SkyEnemy[i].radius1 * sinf(g_SkyEnemy[i].angle1) };	// Z

		g_SkyEnemy[i].rot = { 0.0f, 0.0f, 0.0f };
		g_SkyEnemy[i].scl = { 1.0f, 1.0f, 1.0f };

		g_SkyEnemy[i].circle1_spd = (float)((rand() % 2) + 5) / 1000.0f;			// �T�[�N��1���ړ�����X�s�[�h rand�֐���int�^�Ȃ̂�float�^�ɃL���X�g���Ă���
		g_SkyEnemy[i].circle2_spd = (float)((rand() % 1) + 3) / 100.0f;			// �T�[�N��2���ړ�����X�s�[�h rand�֐���int�^�Ȃ̂�float�^�ɃL���X�g���Ă���


		g_SkyEnemy[i].size = SKY_ENEMY_SIZE;	// �����蔻��̑傫��

		g_SkyEnemy[i].use = FALSE;

		g_SkyEnemy[i].spawn = 0;

		g_SkyEnemy[i].stay_count = 0;
		g_SkyEnemy[i].move_count = 0;

		g_SkyEnemy[i].target = FALSE;


		g_Collision[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Collision[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		GetModelDiffuse(&g_Collision[i].model, &g_Collision[i].diffuse[0]);
		g_Collision[i].use = FALSE;			// TRUE:�����Ă�


		// �����Ńv���C���[�p�̉e���쐬���Ă���
		XMFLOAT3 pos = g_SkyEnemy[i].pos;
		pos.y -= (SKY_ENEMY_OFFSET_Y - 0.1f);
		g_SkyEnemy[i].shadowIdx = CreateShadow(pos, SKY_ENEMY_SHADOW_SIZE, SKY_ENEMY_SHADOW_SIZE);
		//          ��
		//        ���̃����o�[�ϐ������������e��Index�ԍ�

		// �K�w�A�j���[�V�����p�̏���������
		g_SkyEnemy[i].parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������
		g_Parts[i][0].parent = &g_SkyEnemy[i];		// �e���Z�b�g
		g_Parts[i][1].parent = &g_SkyEnemy[i];		// �e���Z�b�g



		// �p�[�c�̏�����
		for (int j = 0; j < SKY_ENEMY_PARTS_MAX; j++)
		{
			g_Parts[i][j].use = FALSE;

			// �ʒu�E��]�E�X�P�[���̏����ݒ�
			g_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// �e�q�֌W
			g_Parts[i][j].parent = &g_SkyEnemy[i];		// �� �����ɐe�̃A�h���X������
		//	g_Parts[�r].parent= &g_Player;		// �r��������e�͖{�́i�v���C���[�j
		//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

			// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
			g_Parts[i][j].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			g_Parts[i][j].move_time = 0.0f;	// ���s���Ԃ��N���A
			g_Parts[i][j].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

			// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
			g_Parts[i][j].load = 0;
		}

		g_Parts[i][0].use = TRUE;
		g_Parts[i][0].tbl_adr = move_tbl_right;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][0].tbl_size = sizeof(move_tbl_right) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][0].load = 1;
		LoadModel(MODEL_SKY_ENEMY_PARTS, &g_Parts[i][0].model);

		g_Parts[i][1].use = TRUE;
		g_Parts[i][1].tbl_adr = move_tbl_left;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][1].tbl_size = sizeof(move_tbl_left) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][1].load = 1;
		LoadModel(MODEL_SKY_ENEMY_PARTS, &g_Parts[i][1].model);

		g_Load = TRUE;
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

	return S_OK;

}



//=============================================================================
// �I������
//=============================================================================
void UninitSkyEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_SKY_ENEMY; i++)
	{
		// ���f���̉������
		if (g_SkyEnemy[i].load)
		{
			UnloadModel(&g_SkyEnemy[i].model);
			g_SkyEnemy[i].load = FALSE;
		}

	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSkyEnemy(void)
{

	CAMERA *cam = GetCamera();
	for (int i = 0; i < MAX_SKY_ENEMY; i++)
	{
		g_SkyEnemy[i].spawn++;

		if (g_SkyEnemy[i].spawn == 100 * i)
		{
			g_SkyEnemy[i].use = TRUE;
		}

		g_SkyEnemy[i].move_count++;


		// 1�ڂ̃T�[�N��
		XMFLOAT3 first_circle;

		first_circle.x = sinf(g_SkyEnemy[i].angle1) * g_SkyEnemy[i].radius2;		// 1�ڂ̉~��2�ڂ̉~�������@�~�^���������_�i���S�_�j��2�ڂ̉~�^���̔��a
		first_circle.z = cosf(g_SkyEnemy[i].angle1) * g_SkyEnemy[i].radius2;		// 1�ڂ̉~��2�ڂ̉~�������@�~�^���������_�i���S�_�j��2�ڂ̉~�^���̔��a



		// 2�ڂ̃T�[�N��
		XMFLOAT3 second_circle;
		g_SkyEnemy[i].radius2 = g_SkyEnemy[i].radius1 + cosf(g_SkyEnemy[i].angle2) * 50;		// ���݂̈ʒu����̃T�[�N���Q�̔��a�����߂�

		second_circle.y = sinf(g_SkyEnemy[i].angle2) * 50;								// �T�C���J�[�u�@���@���̔��a������



		if (g_SkyEnemy[i].radius1 >= 150)					// �T�[�N��1�͈̔͊O�ɂ���ꍇ
		{
			g_SkyEnemy[i].radius1 -= 0.02f;					// �T�[�N��1�Ɍ������Ĉ꒼���ɗ���@�G�l�~�[�̃|�b�v����ʒu�̓��[���h���W�̌��_�𒆐S�Ƃ��ĎO�p�֐��ŊǗ����Ă���̂ŁA���a���k�߂Ă����΃T�[�N���P�Ɍ������Ă���
		}
		else if (g_SkyEnemy[i].radius1 > 0.0f)				// �T�[�N��1�̓����i���a���O�`�P�T�O�j�̏ꍇ
		{
			g_SkyEnemy[i].angle1 += g_SkyEnemy[i].circle1_spd;	// ��]����
			g_SkyEnemy[i].radius1 -= 0.02f;					// ���a��Z�����čs���ăv���C���[�ɋ߂Â��Ă���悤�ɂ��Ă���

			g_SkyEnemy[i].angle2 += g_SkyEnemy[i].circle2_spd;	// ��]����
			g_SkyEnemy[i].pos.y = second_circle.y;
		}
		else											// ���a���O�ȉ��ɂȂ����ꍇ
		{
			g_SkyEnemy[i].radius1 = 0.0f;					// 0��菬�����Ȃ��Ă��܂��Ǝ~�܂��Ă��̂Ŕ��a��0�ɂ��Ă���
		}

		if (g_SkyEnemy[i].move_count <= 60)
		{

			g_SkyEnemy[i].pos.x = first_circle.x;			// �J�E���g������Y�����S�ɉ~�^��
			g_SkyEnemy[i].pos.z = first_circle.z;			// �J�E���g������Y�����S�ɉ~�^��



		}
		else if (g_SkyEnemy[i].move_count >= 60)
		{

			g_SkyEnemy[i].move_count = 0;
		}

		g_SkyEnemy[i].rot.y = g_SkyEnemy[i].angle1;				// ��Ƀv���C���[�̌����������悤�ɂ���


		// �����蔻��悤�̃{�b�N�X�ƈʒu�����L����
		g_Collision[i].pos = g_SkyEnemy[i].pos;





#ifdef _DEBUG
		if (GetKeyboardPress(DIK_R))
		{
			g_SkyEnemy[i].pos.z = g_SkyEnemy[i].pos.x = 0.0f;
			g_SkyEnemy[i].rot.y = g_SkyEnemy[i].dir = 0.0f;
		}
#endif


		CAMERA *camera = GetCamera();
		TARGETOBJ *targetObj = GetTargetObj();
		TARGET *target = GetTarget();

		// ���C�L���X�g���đ����̍��������߂�
		XMFLOAT3 hitPosition;				// ��_
		hitPosition = g_SkyEnemy[i].pos;	// �O�ꂽ���p�ɏ��������Ă���
		bool ans = RayHitEnemySky(targetObj[0].pos, camera->pos, &hitPosition, i);

		if ((ans) && (!g_SkyEnemy[i].target))
		{
			//g_SkyEnemy[i].use = FALSE;
			g_SkyEnemy[i].target = TRUE;
			g_Collision[i].use = FALSE;
			target[0].enemyNum[target[0].count] = i;
			target[0].count++;

			SetRockOn();
		}


		//g_Player.angle += 0.01f;
		//g_Player.rot.y = GetCamera()->rot.y = g_Player.angle;

		//// ���͂̂����������փv���C���[���������Ĉړ�������
		//g_Player.pos.z = 0.0f + cosf(g_Player.angle) * 100;
		//g_Player.pos.x = 0.0f + sinf(g_Player.angle) * 100;





		//// ���C�L���X�g���đ����̍��������߂�
		//XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// �Ԃ������|���S���̖@���x�N�g���i�����j
		//XMFLOAT3 hitPosition;								// ��_
		//hitPosition.y = g_Air.pos.y - AIR_OFFSET_Y;	// �O�ꂽ���p�ɏ��������Ă���
		//bool ans = RayHitField(g_Air.pos, &hitPosition, &normal);
		//g_Air.pos.y = hitPosition.y + AIR_OFFSET_Y;
		//g_Air.pos.y =AIR_OFFSET_Y;


		// �e���v���C���[�̈ʒu�ɍ��킹��
		XMFLOAT3 pos = g_SkyEnemy[i].pos;
		pos.y -= (SKY_ENEMY_OFFSET_Y - 0.1f);
		SetPositionShadow(g_SkyEnemy[i].shadowIdx, pos);



		{	// �|�C���g���C�g�̃e�X�g
			LIGHT *light = GetLightData(1);
			XMFLOAT3 pos = g_SkyEnemy[i].pos;
			pos.y += 20.0f;

			light->Position = pos;
			light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			light->Type = LIGHT_TYPE_POINT;
			light->Enable = TRUE;
			SetLightData(1, light);
		}


		////////////////////////////////////////////////////////////////////////
		//// �p������
		////////////////////////////////////////////////////////////////////////

		//XMVECTOR vx, nvx, up;
		//XMVECTOR quat;
		//float len, angle;

		//// �Q�̃x�N�g���̊O�ς�����ĔC�ӂ̉�]�������߂�
		//g_Air.upVector = normal;
		//up = { 0.0f, 1.0f, 0.0f, 0.0f };
		//vx = XMVector3Cross(up, XMLoadFloat3(&g_Air.upVector));

		//// ���߂���]������N�H�[�^�j�I�������o��
		//nvx = XMVector3Length(vx);
		//XMStoreFloat(&len, nvx);
		//nvx = XMVector3Normalize(vx);
		//angle = asinf(len);
		//quat = XMQuaternionRotationNormal(nvx, angle);

		//// �O��̃N�H�[�^�j�I�����獡��̃N�H�[�^�j�I���܂ł̉�]�����炩�ɂ���
		//quat = XMQuaternionSlerp(XMLoadFloat4(&g_Air.quaternion), quat, 0.05f);

		//// ����̃N�H�[�^�j�I���̌��ʂ�ۑ�����
		//XMStoreFloat4(&g_Air.quaternion, quat);


		// �K�w�A�j���[�V����
		for (int j = 0; j < SKY_ENEMY_PARTS_MAX; j++)
		{
			// �g���Ă���Ȃ珈������
			if ((g_Parts[i][j].use == TRUE) && (g_Parts[i][j].tbl_adr != NULL))
			{
				// �ړ�����
				int		index = (int)g_Parts[i][j].move_time;
				float	time = g_Parts[i][j].move_time - index;
				int		size = g_Parts[i][j].tbl_size;

				float dt = 1.0f / g_Parts[i][j].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_Parts[i][j].move_time += dt;					// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_Parts[i][j].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_Parts[i][j].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_Parts[i][j].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Parts[i][j].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_Parts[i][j].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_Parts[i][j].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Parts[i][j].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_Parts[i][j].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_Parts[i][j].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Parts[i][j].scl, s0 + scl * time);

			}

#ifdef _DEBUG	// �f�o�b�O����\������
		PrintDebugProc("g_SkyEnemy[%d]:X:%f Y:%f Z:%f\n", i, g_SkyEnemy[i].pos.x, g_SkyEnemy[i].pos.y, g_SkyEnemy[i].pos.z);
#endif
		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSkyEnemy(void)
{
	for (int i = 0; i < MAX_SKY_ENEMY; i++)
	{
		if (g_SkyEnemy[i].use == TRUE)
		{


			// �J�����O����
			SetCullingMode(CULL_MODE_NONE);

			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_SkyEnemy[i].scl.x, g_SkyEnemy[i].scl.y, g_SkyEnemy[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_SkyEnemy[i].rot.x, g_SkyEnemy[i].rot.y + XM_PI, g_SkyEnemy[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			//// �N�H�[�^�j�I���𔽉f
			//XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.quaternion));
			//mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_SkyEnemy[i].pos.x, g_SkyEnemy[i].pos.y, g_SkyEnemy[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_SkyEnemy[i].mtxWorld, mtxWorld);


			// ���f���`��
			DrawModel(&g_SkyEnemy[i].model);



			// �p�[�c�̊K�w�A�j���[�V����
			for (int j = 0; j < SKY_ENEMY_PARTS_MAX; j++)
			{
				// ���[���h�}�g���b�N�X�̏�����
				mtxWorld = XMMatrixIdentity();

				// �X�P�[���𔽉f
				mtxScl = XMMatrixScaling(g_Parts[i][j].scl.x, g_Parts[i][j].scl.y, g_Parts[i][j].scl.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// ��]�𔽉f
				mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, g_Parts[i][j].rot.y, g_Parts[i][j].rot.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

				// �ړ��𔽉f
				mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				if (g_Parts[i][j].parent != NULL)	// �q����������e�ƌ�������
				{
					mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
					// ��
					// g_Player.mtxWorld���w���Ă���
				}

				XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

				// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
				if (g_Parts[i][j].use == FALSE) continue;

				// ���[���h�}�g���b�N�X�̐ݒ�
				SetWorldMatrix(&mtxWorld);


				// ���f���`��
				DrawModel(&g_Parts[i][j].model);

			}

			// �R���W�����p�̃{�b�N�X�̕`��
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Collision[i].scl.x, g_Collision[i].scl.y, g_Collision[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Collision[i].rot.x, g_Collision[i].rot.y + XM_PI, g_Collision[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Collision[i].pos.x, g_Collision[i].pos.y, g_Collision[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);
			XMStoreFloat4x4(&g_Collision[i].mtxWorld, mtxWorld);

			// ���f���`��
			DrawModel(&g_Collision[0].model);


		}
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �G�l�~�[�����擾
//=============================================================================
SKY_ENEMY *GetSkyEnemy(void)
{
	return &g_SkyEnemy[0];
}


BOOL RayHitEnemySky(XMFLOAT3 Pos, XMFLOAT3 CameraPos, XMFLOAT3 *HitPosition, int num)
{
	CameraPos.y = 5.0f;

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
