//=============================================================================
//
// ���b�N�I���A�C�R������ [rockOn.h]
// Author : 
//
//=============================================================================
#pragma once
#define MAX_ROCKON			(20)		// �ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct ROCKON
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	BOOL				use;				// �g�����ǂ���
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitRockOn(void);
void UninitRockOn(void);
void UpdateRockOn(void);
void DrawRockOn(void);

ROCKON *GetRockOn();
void SetRockOn(void);
void ResetRockOn(void);


