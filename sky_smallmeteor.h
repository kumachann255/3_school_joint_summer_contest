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
#define	MAX_S_METEOR				(25)		// ��覐΂̍ő吔
#define S_METEOR_SIZE				(70.0f) // 覐΂̓����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct S_METEOR
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

	int					ModelPattern;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitS_Meteor(void);
void UninitS_Meteor(void);
void UpdateS_Meteor(void);
void DrawS_Meteor(void);

void SetS_Meteor(XMFLOAT3 pos, float rot);

S_METEOR *GetS_Meteor(void);
#pragma once
