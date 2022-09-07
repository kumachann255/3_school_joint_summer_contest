//=============================================================================
//
// �C�p�[�e�B�N������ [sea_particle.h]
// Author : �F�V�`�O + ��ˏ���
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

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
	int				tex_No;			// �g�p�e�N�X�`���ԍ�
	float			speed;			// �ړ��X�s�[�h
	float			time;			// ����
	BOOL			use;			// �g�p���Ă��邩�ǂ���

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSeaParticle(void);
void UninitSeaParticle(void);
void UpdateSeaParticle(void);
void DrawSeaParticle(void);

void SetSeaParticleTako();
void SetSeaParticleSame();
void SetSeaBonb(XMFLOAT3 pos);

void SetColorSeaParticle(int nIdxSeaParticle, XMFLOAT4 col);

