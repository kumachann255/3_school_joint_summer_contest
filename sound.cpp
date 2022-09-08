//=============================================================================
//
// サウンド処理 [sound.cpp]追加
// Author : 大塚勝亮
//
//=============================================================================
#include "sound.h"
#include <DirectXMath.h>

//*****************************************************************************
// パラメータ構造体定義
//*****************************************************************************
typedef struct
{
	char *pFilename;	// ファイル名
	int nCntLoop;		// ループカウント
} SOUNDPARAM;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
IXAudio2 *g_pXAudio2 = NULL;								// XAudio2オブジェクトへのインターフェイス
IXAudio2MasteringVoice *g_pMasteringVoice = NULL;			// マスターボイス
IXAudio2SubmixVoice *g_apSubmixVoice;						// サブミックスボイス

IXAudio2SourceVoice *g_apSourceVoice[SOUND_LABEL_MAX] = {};	// ソースボイス
BYTE *g_apDataAudio[SOUND_LABEL_MAX] = {};					// オーディオデータ
DWORD g_aSizeAudio[SOUND_LABEL_MAX] = {};					// オーディオデータサイズ

// 各音素材のパラメータ
SOUNDPARAM g_aParam[SOUND_LABEL_MAX] =
{
	//// BGM
	// タイトル
	{ (char*)"data/BGM/bgm_title_0.wav", -1 },	// タイトルBGM
	{ (char*)"data/BGM/bgm_title_4.wav", -1 },	// タイトルBGM

	// オープニング
	{ (char*)"data/BGM/bgm_opening_1_2.wav", -1 },	// タイトルBG

	// チュートリアル
	{ (char*)"data/BGM/bgm_tutorial.wav", -1 },	// ステージBGM1

	// 街ステージ
	{ (char*)"data/BGM/bgm_city_stage_1.wav", -1 },	// ステージBGM1
	{ (char*)"data/BGM/bgm_city_stage_2.wav", -1 },	// ステージBGM1

	// 海ステージ
	{ (char*)"data/BGM/bgm_sea_stage_1.wav", -1 },	// ステージBGM1
	{ (char*)"data/BGM/bgm_sea_stage_3.wav", -1 },	// ステージBGM2

	// 空ステージ
	{ (char*)"data/BGM/bgm_sky_stage_1.wav", -1 },	// 空ステージBGMサンプル１
	{ (char*)"data/BGM/bgm_sky_stage_3.wav", -1 },	// 空ステージBGMサンプル３

	// リザルト
	{ (char*)"data/BGM/bgm_result.wav", -1 },	// タイトルBG

	// エンドロール
	{ (char*)"data/BGM/bgm_endroll.wav", -1 },	// エンドロールBGM


	//// SE
	{ (char*)"data/SE/booingSound01.wav", 0 },		// ブーイング音
	{ (char*)"data/SE/carHorn01.wav", 0 },			// 車のクラクション音
	{ (char*)"data/SE/Cheers00.wav", 0 },			// 歓声音0
	{ (char*)"data/SE/Cheers01.wav", 0 },			// 歓声音1
	{ (char*)"data/SE/Cheers02.wav", 0 },			// 歓声音2
	{ (char*)"data/SE/Cheers03.wav", 0 },			// 歓声音3
	{ (char*)"data/SE/Cheers04.wav", 0 },			// 歓声音4
	{ (char*)"data/SE/Cheers05.wav", 0 },			// 歓声音5
	{ (char*)"data/SE/collisionSound01.wav", 0 },	// 車の衝突音
	{ (char*)"data/SE/comboSound01.wav", 0 },		// コンボの音（できれば爆弾破裂時の音と交換）
	{ (char*)"data/SE/countdownSound01.wav", 0 },	// 10秒前カウントダウン
	{ (char*)"data/SE/damagedSound01.wav", 0 },		// ダメージ音
	{ (char*)"data/SE/explosionSound01.wav", 0 },	// 粘着物の爆発音
	{ (char*)"data/SE/gunShot01.wav", 0 },			// 銃声1
	{ (char*)"data/SE/gunShot02.wav", 0 },			// 銃声2
	//{ (char*)"data/SE/propellerSound01.wav", -1 },	// プロペラ音
	{ (char*)"data/SE/selectBomb01.wav", 0 },		// タイトル選択音
	{ (char*)"data/SE/shrinkageSound01.wav", 0 },	// 粘着物の収縮音
	//{ (char*)"data/SE/siren01.wav", 0 },			// パトカー音
	{ (char*)"data/SE/stickingSound01.wav", 0 },	// 粘着物の張り付き音
	{ (char*)"data/SE/stirringSound01.wav", 0 },	// 粘着かき混ぜ
	{ (char*)"data/SE/throwingSound01.wav", 0 },	// 粘着物投擲音
	{ (char*)"data/SE/titleClick01.wav", 0 },		// タイトルモード選択音1
	{ (char*)"data/SE/titleClick02.wav", 0 },		// タイトルモード選択音2
	{ (char*)"data/SE/titleClick03.wav", 0 },		// タイトルモード選択音3
	{ (char*)"data/SE/titleClick04.wav", 0 },		// タイトルモード選択音4
	{ (char*)"data/SE/titleClick05.wav", 0 },		// タイトルモード選択音5

	{ (char*)"data/SE/logo.wav", 0 },				// 
	{ (char*)"data/SE/logoD.wav", 0 },				// 
	{ (char*)"data/SE/rhythm.wav", 0 },				// リズムの音
	{ (char*)"data/SE/timing_miss.wav", 0 },		// ミスの音
	{ (char*)"data/SE/timing_good.wav", 0 },		// GOODの音

	// オープニング
	{ (char*)"data/SE/nextPage.wav", 0 },			// ページをめくる音

	// 空ステージ
	{ (char*)"data/SE/enemy_pop.wav", 0 },				// エネミーポップ音
	{ (char*)"data/SE/meteor_shot.wav", 0 },			// メテオ発射音
	{ (char*)"data/SE/meteor_hit.wav", 0 },				// メテオヒット音
	{ (char*)"data/SE/skyEnemy_death_0.wav", 0 },		// エネミー死亡音
	{ (char*)"data/SE/skyEnemy_death_1.wav", 0 },		// エネミー死亡音
	{ (char*)"data/SE/skyEnemy_death_2.wav", 0 },		// エネミー死亡音




};

//=============================================================================
// 初期化処理
//=============================================================================
BOOL InitSound(HWND hWnd)
{
	HRESULT hr;

	// COMライブラリの初期化
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2オブジェクトの作成
	hr = XAudio2Create(&g_pXAudio2, 0);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2オブジェクトの作成に失敗！", "警告！", MB_ICONWARNING);

		// COMライブラリの終了処理
		CoUninitialize();

		return FALSE;
	}
	
	// マスターボイスの生成
	hr = g_pXAudio2->CreateMasteringVoice(&g_pMasteringVoice);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "マスターボイスの生成に失敗！", "警告！", MB_ICONWARNING);

		if(g_pXAudio2)
		{
			// XAudio2オブジェクトの開放
			g_pXAudio2->Release();
			g_pXAudio2 = NULL;
		}

		// COMライブラリの終了処理
		CoUninitialize();

		return FALSE;
	}

	// サブミックスボイスの生成
	hr = g_pXAudio2->CreateSubmixVoice(&g_apSubmixVoice, 2, 44800);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "サブミックスの生成に失敗！", "警告！", MB_ICONWARNING);
		return FALSE;
	}

	// サウンドデータの初期化
	for(int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// バッファのクリア
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// サウンドデータファイルの生成
		hFile = CreateFile(g_aParam[nCntSound].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(1)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// ファイルポインタを先頭に移動
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(2)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
	
		// WAVEファイルのチェック
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(1)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(2)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		if(dwFiletype != 'EVAW')
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(3)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
	
		// フォーマットチェック
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(1)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(2)", "警告！", MB_ICONWARNING);
			return FALSE;
		}

		// オーディオデータ読み込み
		hr = CheckChunk(hFile, 'atad', &g_aSizeAudio[nCntSound], &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(1)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		g_apDataAudio[nCntSound] = (BYTE*)malloc(g_aSizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, g_apDataAudio[nCntSound], g_aSizeAudio[nCntSound], dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
	
		// ソースボイスの生成
		hr = g_pXAudio2->CreateSourceVoice(&g_apSourceVoice[nCntSound], &(wfx.Format));
		if(FAILED(hr))
		{
			MessageBox(hWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);
			return FALSE;
		}

		// バッファの値設定
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = g_aSizeAudio[nCntSound];
		buffer.pAudioData = g_apDataAudio[nCntSound];
		buffer.Flags      = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount  = g_aParam[nCntSound].nCntLoop;

		// オーディオバッファの登録
		g_apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);

	}

	SetSourceVolume(SOUND_LABEL_BGM_bgm_sea_stage_1, 0.2f);
	SetSourceVolume(SOUND_LABEL_BGM_bgm_sea_stage_3, 0.2f);


	SetSourceVolume(SOUND_LABEL_SE_booingSound01, 0.5f);
	//SetSourceVolume(SOUND_LABEL_SE_carHorn01, 0.25f);
	//SetSourceVolume(SOUND_LABEL_SE_propellerSound01, 0.3f);
	//SetSourceVolume(SOUND_LABEL_SE_siren01, 0.5f);
	SetSourceVolume(SOUND_LABEL_SE_stickingSound01, 1.5f);
	SetSourceVolume(SOUND_LABEL_SE_titleClick04, 0.25f);


	// 空ステージのBGMとSEの音量調整
	SetSourceVolume(SOUND_LABEL_BGM_bgm_sky_stage_1, 0.8f);
	SetSourceVolume(SOUND_LABEL_BGM_bgm_sky_stage_3, 0.4f);

	SetSourceVolume(SOUND_LABEL_SE_enemy_pop,		 1.0f);
	SetSourceVolume(SOUND_LABEL_SE_meteor_shot,		 1.0f);
	SetSourceVolume(SOUND_LABEL_SE_meteor_hit,		 1.0f);
	SetSourceVolume(SOUND_LABEL_SE_skyEnemy_death_0, 1.0f);
	SetSourceVolume(SOUND_LABEL_SE_skyEnemy_death_1, 1.0f);
	SetSourceVolume(SOUND_LABEL_SE_skyEnemy_death_2, 1.0f);

	return TRUE;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSound(void)
{
	// 一時停止
	for(int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if(g_apSourceVoice[nCntSound])
		{
			// 一時停止
			g_apSourceVoice[nCntSound]->Stop(0);
	
			// ソースボイスの破棄
			g_apSourceVoice[nCntSound]->DestroyVoice();
			g_apSourceVoice[nCntSound] = NULL;
	
			// オーディオデータの開放
			free(g_apDataAudio[nCntSound]);
			g_apDataAudio[nCntSound] = NULL;
		}
	}
	
	// マスターボイスの破棄
	g_pMasteringVoice->DestroyVoice();
	g_pMasteringVoice = NULL;
	
	if(g_pXAudio2)
	{
		// XAudio2オブジェクトの開放
		g_pXAudio2->Release();
		g_pXAudio2 = NULL;
	}
	
	// COMライブラリの終了処理
	CoUninitialize();
}

//=============================================================================
// セグメント再生(再生中なら停止)
//=============================================================================
void PlaySound(int label)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = g_aSizeAudio[label];
	buffer.pAudioData = g_apDataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = g_aParam[label].nCntLoop;

	// 状態取得
	g_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		g_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		g_apSourceVoice[label]->FlushSourceBuffers();
	}

	// ソースボイスの出力をサブミックスボイスに切り替え
	XAUDIO2_SEND_DESCRIPTOR send = { 0, g_apSubmixVoice };
	XAUDIO2_VOICE_SENDS sendlist = { 1, &send };
	g_apSourceVoice[label]->SetOutputVoices(&sendlist);

	// オーディオバッファの登録
	g_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	g_apSourceVoice[label]->Start(0);

}

//=============================================================================
// セグメント停止(ラベル指定)
//=============================================================================
void StopSound(int label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
	g_apSourceVoice[label]->GetState(&xa2state);
	if(xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		g_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		g_apSourceVoice[label]->FlushSourceBuffers();
	}
}

//=============================================================================
// セグメント停止(全て)
//=============================================================================
void StopSound(void)
{
	// 一時停止
	for(int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if(g_apSourceVoice[nCntSound])
		{
			// 一時停止
			g_apSourceVoice[nCntSound]->Stop(0);
		}
	}
}

//=============================================================================
// チャンクのチェック
//=============================================================================
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;
	
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}
	
	while(hr == S_OK)
	{
		if(ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if(ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch(dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize  = dwChunkDataSize;
			dwChunkDataSize = 4;
			if(ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if(SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if(dwChunkType == format)
		{
			*pChunkSize         = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if(dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}
	
	return S_OK;
}

//=============================================================================
// チャンクデータの読み込み
//=============================================================================
HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;
	
	if(SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if(ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}
	
	return S_OK;
}

//=============================================================================
// ソースボイスの音量調整（0.0f ~ 1.0fで調整）
//=============================================================================
void SetSouceVoice(int label, float volume)
{
	g_apSourceVoice[label]->SetVolume(volume);
	return;
}

//=============================================================================
// マスターボイスの音量調節(0.0f ~ 1.0fで調整)
//=============================================================================
void SetMasterVolume(float volume)
{
	volume *= volume;
	g_pMasteringVoice->SetVolume(volume);
	return;
}


//=============================================================================
// ソースボイスの音量調整(0.0f ~ 1.0fで調整)
//=============================================================================
void SetSourceVolume(int label, float volume)
{
	g_apSourceVoice[label]->SetVolume(volume);
	return;
}