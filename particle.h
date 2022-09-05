//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : �F�V�`�O
//
//=============================================================================
#pragma once

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

// �p�[�e�B�N���̎��
enum
{
	PARTICLE_TYPE_CUP,			// �J�b�v�p
	PARTICLE_TYPE_CUP_NOTE,		// �J�b�v�����p
	PARTICLE_TYPE_BLAST,		// �l�o�l�o�p
	PARTICLE_TYPE_MAX,
};


struct PARTICLE
{
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		rot;			// ��]
	XMFLOAT3		scl;			// �X�P�[��
	XMFLOAT3		move;			// �ړ���
	
	MATERIAL		material;		// �}�e���A��

	int				nIdxShadow;		// �eID
	
	int				texNo;			// �e�N�X�`���ԍ�
	int				type;			// �^�C�v
	int				life;			// ����
	
	float			pop;			// �\���^�C�~���O
	
	BOOL			use;			// �g�p���Ă��邩�ǂ���

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

int SetParticle(int type, int texNo, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 move, XMFLOAT4 col, float pop, int life);
void SetColorParticle(int nIdxParticle, XMFLOAT4 col);

