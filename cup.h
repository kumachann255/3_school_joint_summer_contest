//=============================================================================
//
// �J�b�v�Ɠ����蔻�菈�� [cup.h]
// Author : �G�i
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_CUP			(1)
#define	CUP_SIZE			(50.0f)				// �����蔻��̑傫��

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct CUP
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	float				size;				// �����蔻��̑傫��
	int					life;				// ���j�̎c�莞��
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitCup(void);
void UninitCup(void);
void UpdateCup(void);
void DrawCup(void);

CUP *GetCup(void);
void SetCup(void);

BOOL GetCupParticleOn(void);

BOOL GetCameraSwitch(void);
void SetCameraSwitch(BOOL data);
int GetMorphing(void);
int GetStopTime(void);
