//=============================================================================
//
// ���C������ [main.cpp]
// Author : �F�V�`�O
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "shadow.h"
#include "light.h"
#include "meshfield.h"
#include "meshwall.h"
#include "fieldobj.h"
#include "collision.h"
#include "bullet.h"
#include "score.h"
#include "sound.h"
#include "particle.h"

#include "title.h"
#include "gameCity.h"
#include "gameSea.h"
#include "gameSky.h"
#include "result.h"
#include "endroll.h"
#include "fade.h"

#include "timeUI.h"

#include "logo.h"
#include "logoD.h"
#include "countdown.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"�l�o�l�o�[�����h�{���p�[�e�B�["		// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);


//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

int	g_Mode = MODE_GAME_SKY;					// �N�����̉�ʂ�ݒ�

int g_Stage = stage0;							// ���݂̃X�e�[�W

int g_Score[stage_max] = { 0, 0, 0, 0 };	// �e�X�e�[�W�̃X�R�A��ۑ�

int g_ComboMax = 0;								// �ő�R���{����ۑ�

//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// �E�B���h�E�̍����W
		CW_USEDEFAULT,																		// �E�B���h�E�̏���W
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,
		NULL,
		hInstance,
		NULL);

	// ����������(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitLight();

	InitCamera();

	// ���͏����̏�����
	InitInput(hInstance, hWnd);

	// �T�E���h�̏�����
	InitSound(hWnd);


	// ���C�g��L����
	SetLightEnable(TRUE);

	// �w�ʃ|���S�����J�����O
	SetCullingMode(CULL_MODE_BACK);


	// �t�F�[�h�̏�����
	InitFade();

	// �ŏ��̃��[�h���Z�b�g
	SetMode(g_Mode);	// ������SetMode�̂܂܂ŁI

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	// �I���̃��[�h���Z�b�g
	SetMode(MODE_MAX);


	// �T�E���h�I������
	UninitSound();

	// �J�����̏I������
	UninitCamera();

	//���͂̏I������
	UninitInput();

	// �����_���[�̏I������
	UninitRenderer();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// ���͂̍X�V����
	UpdateInput();

	// ���C�g�̍X�V����
	UpdateLight();

	// �J�����X�V
	UpdateCamera();

	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE_LOGO:	// ���S��ʂ̍X�V
		UpdateLogo();
		break;

	case MODE_TITLE_DirectX:// DirectX�ō���Ă܂��A�s��ʂ̍X�V
		UpdateLogoD();
		break;

	case MODE_TITLE:		// �^�C�g����ʂ̍X�V
		UpdateTitle();
		break;

	case MODE_GAME_CITY:	// �Q�[����ʂ̍X�V
		UpdateGameCity();
		break;

	case MODE_GAME_SEA:	// �Q�[����ʂ̍X�V
		UpdateGameSea();
		break;

	case MODE_GAME_SKY:	// �Q�[����ʂ̍X�V
		UpdateGameSky();
		break;

	case MODE_GAME_COUNT:	// ���̃X�e�[�W�܂ł̃J�E���g��ʂ̍X�V
		UpdateCountDown();
		break;

	case MODE_RESULT:		// ���U���g��ʂ̍X�V
		UpdateResult();
		break;

	case MODE_ENDROLL:		// �G���h���[����ʂ̍X�V
		UpdateEndroll();
		break;
	}

	// �t�F�[�h�����̍X�V
	UpdateFade();


}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	SetCamera();

	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE_LOGO:	// ���S��ʂ̕`��
		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		DrawLogo();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);
		break;

	case MODE_TITLE_DirectX:// DirectX�ō���Ă܂��A�s��ʂ̕`��
		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		DrawLogoD();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);
		break;

	case MODE_TITLE:		// �^�C�g����ʂ̕`��
		SetViewPort(TYPE_FULL_SCREEN);

		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		DrawTitle();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);
		break;

	case MODE_GAME_CITY:			// �Q�[����ʂ̕`��
		DrawGameCity();
		break;

	case MODE_GAME_SEA:			// �Q�[����ʂ̕`��
		DrawGameSea();
		break;

	case MODE_GAME_SKY:			// �Q�[����ʂ̕`��
		DrawGameSky();
		break;

	case MODE_GAME_COUNT:	// ���̃X�e�[�W�܂ł̃J�E���g��ʂ̕`��
		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		DrawCountDown();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);
		break;

	case MODE_RESULT:		// ���U���g��ʂ̕`��
		SetViewPort(TYPE_FULL_SCREEN);

		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		DrawResult();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);
		break;

	case MODE_ENDROLL:		// �G���h���[����ʂ̕`��
		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		DrawEndroll();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);
		break;
	}

	// �t�F�[�h�`��
	DrawFade();

#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif



//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int mode)
{
	// ���[�h��ς���O�ɑS������������������Ⴄ

	// ���S��ʂ̏I������
	UninitLogo();

	// DirectX�ō���Ă܂��A�s��ʂ̏I������
	UninitLogoD();

	// �^�C�g����ʂ̏I������
	UninitTitle();

	// �Q�[����ʂ̏I������
	UninitGameCity();

	// �Q�[����ʂ̏I������
	UninitGameSea();

	// �Q�[����ʂ̏I������
	UninitGameSky();

	// ���̃X�e�[�W�܂ł̃J�E���g��ʂ̏I������
	UninitCountDown();

	// ���U���g��ʂ̏I������
	UninitResult();

	// �G���h���[����ʂ̏I������
	UninitResult();


	g_Mode = mode;	// ���̃��[�h���Z�b�g���Ă���

	switch (g_Mode)
	{
	case MODE_TITLE_LOGO:	// ���S��ʂ̏�����
		InitLogo();
		break;

	case MODE_TITLE_DirectX:// DirectX�ō���Ă܂��A�s��ʂ̏�����
		InitLogoD();
		break;

	case MODE_TITLE:
		// �^�C�g����ʂ̏�����
		InitTitle();
		break;

	case MODE_GAME_CITY:
		// �Q�[����ʂ̏�����
		InitGameCity();
		break;

	case MODE_GAME_SEA:
		// �Q�[����ʂ̏�����
		InitGameSea();
		break;

	case MODE_GAME_SKY:
		// �Q�[����ʂ̏�����
		InitGameSky();
		break;

	case MODE_GAME_COUNT:
		InitCountDown();
		break;

	case MODE_RESULT:
		// ���U���g��ʂ̏�����
		InitResult();
		break;

	case MODE_ENDROLL:
		// �G���h���[����ʂ̏�����
		InitEndroll();
		break;

		// �Q�[���I�����̏���
	case MODE_MAX:
		// �G�l�~�[�̏I������
		UninitEnemy();

		// �v���C���[�̏I������
		UninitPlayer();
		break;
	}
}

//=============================================================================
// ���[�h�̎擾
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}


//=============================================================================
// float�^�ŕ����ɂ��Ή����Ă��郉���_���̒l��Ԃ��֐�
// digits:�����_�ȉ��̌���(0.01f �� 2), max:�~�����ő�l, min:�~�����ŏ��l
//=============================================================================
float RamdomFloat(int digits, float max, float min)
{
	// �����_���������߂̕ϐ����쐬
	int l_digits = 1;
	for (int i = 0; i < digits; i++)
	{
		l_digits *= 10;
	}

	// ��]�Z���邽�߂̒l���쐬
	float surplus = (max - min) * l_digits;

	int random = rand() % (int)surplus;

	float ans;

	// �����̃����_���̒l��float�^�փL���X�g�ƕ����̏���
	ans = (float)random / l_digits + min;

	return ans;
}


//=============================================================================
// ���݂̃X�e�[�W���擾
//=============================================================================
int GetStage(void)
{
	return g_Stage;
}


//=============================================================================
// �X�e�[�W��ύX
//=============================================================================
void SetStage(int stage)
{
	g_Stage = stage;
}


//=============================================================================
// �X�e�[�W���ƂɃX�R�A��ۑ�
//=============================================================================
void SetMainScore(int score)
{
	g_Score[g_Stage] = score;
}


//=============================================================================
// �X�R�A���擾
//=============================================================================
int GetMainScore(int stage)
{
	return g_Score[stage];
}


//=============================================================================
// main�X�R�A��������
//=============================================================================
void ResetMainScore(void)
{
	for (int i = 0; i < stage_max; i++)
	{
		g_Score[i] = 0;
	}

	g_ComboMax = 0;
}


//=============================================================================
// �ő�R���{����ۑ�
//=============================================================================
void SetComboMax(int combo)
{
	if (g_ComboMax < combo)
	{
		g_ComboMax = combo;
	}
}


//=============================================================================
// �ő�R���{�����擾
//=============================================================================
int GetComboMax(void)
{
	return g_ComboMax;
}