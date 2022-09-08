//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : ��ˏ���
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	//// BGM
	// �^�C�g��
	SOUND_LABEL_BGM_bgm_title_0,		// �N���A��̃^�C�g��BGM
	SOUND_LABEL_BGM_bgm_title_4,		// �ŏ��̃^�C�g��BGM

	// �I�[�v�j���O
	SOUND_LABEL_BGM_bgm_opening_1_2,	// �I�[�v�j���OBGM

	// �`���[�g���A��
	SOUND_LABEL_BGM_bgm_tutorial,		// �`���[�g���A��BGM

	// �X�X�e�[�W
	SOUND_LABEL_BGM_bgm_city_stage_1,	// �X�X�e�[�W�PBGM
	SOUND_LABEL_BGM_bgm_city_stage_2,	// �X�X�e�[�W�QBGM

	// �C�X�e�[�W
	SOUND_LABEL_BGM_bgm_sea_stage_1,	// �C�X�e�[�W�PBGM
	SOUND_LABEL_BGM_bgm_sea_stage_3,	// �C�X�e�[�W�QBGM

	// ��X�e�[�W
	SOUND_LABEL_BGM_bgm_sky_stage_1,	// ��X�e�[�W�PBGM
	SOUND_LABEL_BGM_bgm_sky_stage_3,	// ��X�e�[�W�QBGM

	// ���U���g
	SOUND_LABEL_BGM_bgm_result,			// ���U���gBGM

	// �G���h���[��
	SOUND_LABEL_BGM_bgm_endroll,		// �G���h���[��BGM


	//// SE
	SOUND_LABEL_SE_booingSound01,		// �u�[�C���O��
	SOUND_LABEL_SE_carHorn01,			// �Ԃ̃N���N�V������
	SOUND_LABEL_SE_cheers00,			// ������0
	SOUND_LABEL_SE_cheers01,			// ������1
	SOUND_LABEL_SE_cheers02,			// ������2
	SOUND_LABEL_SE_cheers03,			// ������3
	SOUND_LABEL_SE_cheers04,			// ������4
	SOUND_LABEL_SE_cheers05,			// ������5
	SOUND_LABEL_SE_collisionSound01,	// �Ԃ̏Փˉ�
	SOUND_LABEL_SE_comboSound01,		// �R���{�̉��i�ł���Δ��e�j�􎞂̉��ƌ����j
	SOUND_LABEL_SE_countdownSound01,	// 10�b�O�J�E���g�_�E��
	SOUND_LABEL_SE_damagedSound01,		// �_���[�W��
	SOUND_LABEL_SE_explosionSound01,	// �S�����̔�����
	SOUND_LABEL_SE_gunShot01,			// �e��1
	SOUND_LABEL_SE_gunShot02,			// �e��2
	//SOUND_LABEL_SE_propellerSound01,	// �v���y����
	SOUND_LABEL_SE_selectBomb01,		// �^�C�g���̑I����
	SOUND_LABEL_SE_shrinkingSound01,	// �S�����̎��k��
	//SOUND_LABEL_SE_siren01,				// �p�g�J�[��
	SOUND_LABEL_SE_stickingSound01,		// �S�����̒���t����
	SOUND_LABEL_SE_stirringSound01,		// �S����������
	SOUND_LABEL_SE_throwingSound01,		// �S����������
	SOUND_LABEL_SE_titleClick01,		// �^�C�g�����[�h�I����1
	SOUND_LABEL_SE_titleClick02,		// �^�C�g�����[�h�I����2
	SOUND_LABEL_SE_titleClick03,		// �^�C�g�����[�h�I����3
	SOUND_LABEL_SE_titleClick04,		// �^�C�g�����[�h�I����4
	SOUND_LABEL_SE_titleClick05,		// �^�C�g�����[�h�I����4

	SOUND_LABEL_SE_logo,				// ���S�̊���
	SOUND_LABEL_SE_logoD,				// ���S�̊���
	SOUND_LABEL_SE_rhythm,				// ���Y���̉�
	SOUND_LABEL_SE_timing_miss,			// �~�X�̉�
	SOUND_LABEL_SE_timing_good,			// GOOD�̉�

	// �I�[�v�j���O
	SOUND_LABEL_SE_nextPage,			// �I�[�v�j���O�̃y�[�W���߂��鉹

	// ��X�e�[�W
	SOUND_LABEL_SE_enemy_pop,				// �G�l�~�[�|�b�v��
	SOUND_LABEL_SE_meteor_shot,				// ���e�I���ˉ�
	SOUND_LABEL_SE_meteor_hit,				// ���e�I�q�b�g��
	SOUND_LABEL_SE_skyEnemy_death_0,		// �G�l�~�[���S��
	SOUND_LABEL_SE_skyEnemy_death_1,		// �G�l�~�[���S��
	SOUND_LABEL_SE_skyEnemy_death_2,		// �G�l�~�[���S��

	// �U�����̊|����
	SOUND_LABEL_SE_legato,					// ���K�[�g
	SOUND_LABEL_SE_arpeggio,				// �A���y�W�I
	SOUND_LABEL_SE_tremolo,					// �g������
	SOUND_LABEL_SE_pizzicato,				// �s�`�J�[�g
	SOUND_LABEL_SE_fortissimo,				// �t�H���e�b�V��
	SOUND_LABEL_SE_glissando,				// �O���b�T���h

	SOUND_LABEL_SE_cupRotation,				// �R�[�q�[�J�b�v�̉�鉹

	SOUND_LABEL_SE_enemyPop_sea_0,			// �C�X�e�[�W�̃G�l�~�[�̃|�b�v��
	SOUND_LABEL_SE_enemyPop_sea_1,			// �C�X�e�[�W�̃G�l�~�[�̃|�b�v��

	SOUND_LABEL_SE_samePop,					// �T���̉�

	SOUND_LABEL_SE_piano_do00,				// �s�A�m�� �h
	SOUND_LABEL_SE_piano_re,				// �s�A�m�� ��
	SOUND_LABEL_SE_piano_mi,				// �s�A�m�� �~
	SOUND_LABEL_SE_piano_fa,				// �s�A�m�� �t�@
	SOUND_LABEL_SE_piano_so,				// �s�A�m�� �\
	SOUND_LABEL_SE_piano_ra,				// �s�A�m�� ��
	SOUND_LABEL_SE_piano_si,				// �s�A�m�� �V
	SOUND_LABEL_SE_piano_do,				// �s�A�m�� �h

	SOUND_LABEL_SE_titleVoice,				// �^�C�g���̃{�C�X
	SOUND_LABEL_SE_logoVoice,				// ���S�̃{�C�X

	SOUND_LABEL_MAX,	// MAX
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);
void SetSouceVoice(int label, float volume);

void SetMasterVolume(float volume);
void SetSourceVolume(int label, float volume);
