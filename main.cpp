//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 熊澤義弘
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
#include "sky_smallmeteor.h"
#include "score.h"
#include "sound.h"
#include "particle.h"

#include "title.h"
#include "opening.h"
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
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"ネバネバーランドボムパーティー"		// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

int	g_Mode = MODE_GAME_SKY;					// 起動時の画面を設定

int g_Stage = stage0;							// 現在のステージ

int g_Score[stage_max] = { 0, 0, 0, 0 };	// 各ステージのスコアを保存

int g_ComboMax = 0;								// 最大コンボ数を保存

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
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
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
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
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitLight();

	InitCamera();

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	// サウンドの初期化
	InitSound(hWnd);


	// ライトを有効化
	SetLightEnable(TRUE);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);


	// フェードの初期化
	InitFade();

	// 最初のモードをセット
	SetMode(g_Mode);	// ここはSetModeのままで！

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// 終了のモードをセット
	SetMode(MODE_MAX);


	// サウンド終了処理
	UninitSound();

	// カメラの終了処理
	UninitCamera();

	//入力の終了処理
	UninitInput();

	// レンダラーの終了処理
	UninitRenderer();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();

	// ライトの更新処理
	UpdateLight();

	// カメラ更新
	UpdateCamera();

	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE_LOGO:	// ロゴ画面の更新
		UpdateLogo();
		break;

	case MODE_TITLE_DirectX:// DirectXで作ってますアピ画面の更新
		UpdateLogoD();
		break;

	case MODE_TITLE:		// タイトル画面の更新
		UpdateTitle();
		break;

	case MODE_OPENING:		// オープニング画面の更新
		UpdateOpening();
		break;

	case MODE_GAME_CITY:	// ゲーム画面の更新
		UpdateGameCity();
		break;

	case MODE_GAME_SEA:	// ゲーム画面の更新
		UpdateGameSea();
		break;

	case MODE_GAME_SKY:	// ゲーム画面の更新
		UpdateGameSky();
		break;

	case MODE_GAME_COUNT:	// 次のステージまでのカウント画面の更新
		UpdateCountDown();
		break;

	case MODE_RESULT:		// リザルト画面の更新
		UpdateResult();
		break;

	case MODE_ENDROLL:		// エンドロール画面の更新
		UpdateEndroll();
		break;
	}

	// フェード処理の更新
	UpdateFade();


}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();

	SetCamera();

	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE_LOGO:	// ロゴ画面の描画
		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawLogo();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_TITLE_DirectX:// DirectXで作ってますアピ画面の描画
		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawLogoD();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_TITLE:		// タイトル画面の描画
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawTitle();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_OPENING:		// オープニング画面の描画
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawOpening();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_GAME_CITY:			// ゲーム画面の描画
		DrawGameCity();
		break;

	case MODE_GAME_SEA:			// ゲーム画面の描画
		DrawGameSea();
		break;

	case MODE_GAME_SKY:			// ゲーム画面の描画
		DrawGameSky();
		break;

	case MODE_GAME_COUNT:	// 次のステージまでのカウント画面の描画
		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawCountDown();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_RESULT:		// リザルト画面の描画
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawResult();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_ENDROLL:		// エンドロール画面の描画
		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawEndroll();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;
	}

	// フェード描画
	DrawFade();

#ifdef _DEBUG
	// デバッグ表示
	DrawDebugProc();
#endif

	// バックバッファ、フロントバッファ入れ替え
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
// モードの設定
//=============================================================================
void SetMode(int mode)
{
	// モードを変える前に全部メモリを解放しちゃう

	// ロゴ画面の終了処理
	UninitLogo();

	// DirectXで作ってますアピ画面の終了処理
	UninitLogoD();

	// タイトル画面の終了処理
	UninitTitle();

	// オープニング画面の終了処理
	UninitOpening();

	// ゲーム画面の終了処理
	UninitGameCity();

	// ゲーム画面の終了処理
	UninitGameSea();

	// ゲーム画面の終了処理
	UninitGameSky();

	// 次のステージまでのカウント画面の終了処理
	UninitCountDown();

	// リザルト画面の終了処理
	UninitResult();

	// エンドロール画面の終了処理
	UninitResult();


	g_Mode = mode;	// 次のモードをセットしている

	switch (g_Mode)
	{
	case MODE_TITLE_LOGO:	// ロゴ画面の初期化
		InitLogo();
		break;

	case MODE_TITLE_DirectX:// DirectXで作ってますアピ画面の初期化
		InitLogoD();
		break;

	case MODE_TITLE:
		// タイトル画面の初期化
		InitTitle();
		break;

	case MODE_OPENING:
		// オープニング画面の初期化
		InitOpening();
		break;

	case MODE_GAME_CITY:
		// ゲーム画面の初期化
		InitGameCity();
		break;

	case MODE_GAME_SEA:
		// ゲーム画面の初期化
		InitGameSea();
		break;

	case MODE_GAME_SKY:
		// ゲーム画面の初期化
		InitGameSky();
		break;

	case MODE_GAME_COUNT:
		InitCountDown();
		break;

	case MODE_RESULT:
		// リザルト画面の初期化
		InitResult();
		break;

	case MODE_ENDROLL:
		// エンドロール画面の初期化
		InitEndroll();
		break;

		// ゲーム終了時の処理
	case MODE_MAX:
		// エネミーの終了処理
		UninitEnemy();

		// プレイヤーの終了処理
		UninitPlayer();
		break;
	}
}

//=============================================================================
// モードの取得
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}


//=============================================================================
// float型で負数にも対応しているランダムの値を返す関数
// digits:小数点以下の桁数(0.01f → 2), max:欲しい最大値, min:欲しい最小値
//=============================================================================
float RamdomFloat(int digits, float max, float min)
{
	// 小数点を消すための変数を作成
	int l_digits = 1;
	for (int i = 0; i < digits; i++)
	{
		l_digits *= 10;
	}

	// 剰余算するための値を作成
	float surplus = (max - min) * l_digits;

	int random = rand() % (int)surplus;

	float ans;

	// 整数のランダムの値をfloat型へキャストと負数の処理
	ans = (float)random / l_digits + min;

	return ans;
}


//=============================================================================
// 現在のステージを取得
//=============================================================================
int GetStage(void)
{
	return g_Stage;
}


//=============================================================================
// ステージを変更
//=============================================================================
void SetStage(int stage)
{
	g_Stage = stage;
}


//=============================================================================
// ステージごとにスコアを保存
//=============================================================================
void SetMainScore(int score)
{
	g_Score[g_Stage] = score;
}


//=============================================================================
// スコアを取得
//=============================================================================
int GetMainScore(int stage)
{
	return g_Score[stage];
}


//=============================================================================
// mainスコアを初期化
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
// 最大コンボ数を保存
//=============================================================================
void SetComboMax(int combo)
{
	if (g_ComboMax < combo)
	{
		g_ComboMax = combo;
	}
}


//=============================================================================
// 最大コンボ数を取得
//=============================================================================
int GetComboMax(void)
{
	return g_ComboMax;
}
