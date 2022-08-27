//=============================================================================
//
// �G�l�~�[���f������ [enemy.h]
// Author : �F�V�`�O�{�G�i
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define MAX_ENEMY		(50)					// �G�l�~�[�̐�
#define MAX_ENEMY		(2)					// �G�l�~�[�̐�

#define	ENEMY_SIZE		(30.0f)				// �����蔻��̑傫��

#define ENEMY_OFFSET_Y		(0.0f)			// �G�l�~�[�̑��������킹��

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	float				spd;				// �ړ��X�s�[�h
	float				size;				// �����蔻��̑傫��
	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�
	
	float				zGoal;				// z���W�̃v���C���[�ɋ߂Â��ő勗��

	XMFLOAT3			hitPos;				// �����̍��W
	XMFLOAT3			hitRot;				// �����蔻���A�j���[�V�����p�A����̈ړ���
	BOOL				isHit;				// �������Ă�t���O TRUE:�������Ă���
	int					hitTime;			// �ړ�����
	int					liveCount;			// �|�b�v���Ă���̎��Ԃ��Ǘ�

	int					type;				// �G�l�~�[�̃^�C�v�i0:�p�g�J�[�A1:���o�C
	BOOL				fuchi;				// �������C�g�̃I���I�t

	// �N�H�[�^�j�I��
	XMFLOAT4			quaternion;	// �N�H�[�^�j�I��
	XMFLOAT3			upVector;	// �����������Ă��鏊


};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
BOOL RayHitEnemy(XMFLOAT3 Pos, XMFLOAT3 CameraPos, XMFLOAT3 *HitPosition, int num);
