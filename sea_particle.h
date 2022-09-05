//=============================================================================
//
// �C�p�[�e�B�N������ [sea_particle.h]
// Author : �F�V�`�O + ��ˏ���
//
//=============================================================================
#pragma once

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

// �p�[�e�B�N���̎��
enum
{
	PARTICLE_TYPE_TAKO,			// �^�R�p
	PARTICLE_TYPE_SAME,			// �T���p
	PARTICLE_TYPE_MAX,
};


struct SEA_PARTICLE
{
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		rot;			// ��]
	XMFLOAT3		scl;			// �X�P�[��
	XMFLOAT3		move;			// �ړ���
	
	MATERIAL		material;		// �}�e���A��
	
	int				type;			// �^�C�v
	int				life;			// ����
	int				pop;			// �\���^�C�~���O
	
	BOOL			use;			// �g�p���Ă��邩�ǂ���

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSeaParticle(void);
void UninitSeaParticle(void);
void UpdateSeaParticle(void);
void DrawSeaParticle(void);

//int SetSeaParticleSetSeaParticle(int type, XMFLOAT3 pos, int life);
//void SetColorSeaParticle(int nIdxSeaParticle, XMFLOAT4 col);

