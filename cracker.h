//=============================================================================
//
// �N���b�J�[�Ɠ����蔻�菈�� [cracker.h]
// Author : �G�i
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_CRACKER			(1)
#define	CRACKER_SIZE			(50.0f)				// �����蔻��̑傫��

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct CRACKER
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			basepos;			// ���f���̈ʒu(�Q�Ɨp)
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
HRESULT InitCracker(void);
void UninitCracker(void);
void UpdateCracker(void);
void DrawCracker(void);

CRACKER *GetCracker(void);
void SetCracker(void);

//BOOL GetParticleOn(void);

BOOL GetCameraSwitch(void);
void SetCameraSwitch(BOOL data);
int GetMorphing(void);
int GetStopTime(void);
