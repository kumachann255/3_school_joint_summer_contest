#pragma once
//=============================================================================
//
// ���e�I���� [meteor.h]
// Author : �R�c����
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_B_METEOR				(1)		// ��覐΂̍ő吔
#define B_METEOR_SIZE				(100.0f) // 覐΂̓����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct B_METEOR
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	float				speed;				// �ړ��X�s�[�h
	float				time;				// ����
	float				size;
	XMFLOAT3			randMove;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitB_Meteor(void);
void UninitB_Meteor(void);
void UpdateB_Meteor(void);
void DrawB_Meteor(void);

void SetB_Meteor(XMFLOAT3 pos, float rot);

B_METEOR *GetB_Meteor(void);
#pragma once
#pragma once
