//=============================================================================
//
// �؂̏��� [tree.h]
// Author : �F�V�`�O
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_FOBJ		(20)					// �I�u�W�F�N�g�̐�
#define MAX_FOBJ_GUAD	(55)					// �I�u�W�F�N�g�̐�
#define MAX_FOBJ_SIGN	(5)						// �I�u�W�F�N�g�̐�


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct FOBJ
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTree(void);
void UninitTree(void);
void UpdateTree(void);
void DrawTree(void);

FOBJ *GetBilding(void);
