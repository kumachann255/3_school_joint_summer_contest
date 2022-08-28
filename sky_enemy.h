#pragma once
//=============================================================================
//
// ���f������ [sky_enemy.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_SKY_ENEMY		(10)					// �G�l�~�[�̐�

#define	SKY_ENEMY_SIZE		(30.0f)				// �����蔻��̑傫��

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct SKY_ENEMY
{
	XMFLOAT3			pos;		// �|���S���̈ʒu
	XMFLOAT3			rot;		// �|���S���̌���(��])
	XMFLOAT3			scl;		// �|���S���̑傫��(�X�P�[��)

	XMFLOAT4X4			mtxWorld;	// ���[���h�}�g���b�N�X

	BOOL				load;
	DX11_MODEL			model;		// ���f�����

	float				circle1_spd;		// �ړ��X�s�[�h
	float				circle2_spd;
	float				dir;		// ����
	float				size;		// �����蔻��̑傫��
	int					shadowIdx;	// �e��Index

	BOOL				use;

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	INTERPOLATION_DATA	*tbl_adr;	// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tbl_size;	// �o�^�����e�[�u���̃��R�[�h����
	float				move_time;	// ���s����

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	SKY_ENEMY				*parent;	// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �N�H�[�^�j�I��
	XMFLOAT4			quaternion;	// �N�H�[�^�j�I��
	XMFLOAT3			upVector;	// �����������Ă��鏊

	float angle1;
	float angle2;

	float radius1;
	float radius2;

	int stay_count;
	int move_count;

	int spawn;

	int EnemyType;

	BOOL				isHit;				// �������Ă�t���O TRUE:�������Ă���
	int					hitTime;			// �ړ�����
	int					liveCount;			// �|�b�v���Ă���̎��Ԃ��Ǘ�

	int					type;				// �G�l�~�[�̃^�C�v�i0:�p�g�J�[�A1:���o�C
	BOOL				fuchi;				// �������C�g�̃I���I�t

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSkyEnemy(void);
void UninitSkyEnemy(void);
void UpdateSkyEnemy(void);
void DrawSkyEnemy(void);

SKY_ENEMY *GetSkyEnemy(void);
void SetSkyEnemy(void);
