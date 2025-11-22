#include <Windows.h>
#include <thread>
#include <string>
#include <cstdio>
#include <filesystem>
#include <mnsb.h>

using namespace std;

/* Manosaba Plugin Loader v0.0.0 by wakanameko */
const wchar_t version[] = L"0.0.0";
const wchar_t name[] = L"Manosaba Plugin Loader";
const wchar_t author[] = L"wakanameko";

static HMODULE gameAsm = nullptr;	// store GameAssembly.dll

static HMODULE g_realUnityPlayer = nullptr;
using UnityMainFunc = int(*)(HINSTANCE, HINSTANCE, LPWSTR, int);
UnityMainFunc UnityMainOriginal = nullptr;

typedef void(*GameInitializer_Start_fn)(void* self);
GameInitializer_Start_fn GameInitializer_Start_original = nullptr;
const unsigned int GameInitializer_Start_RVA = 0x30FB70;	// Decompiled RVA from GameAssembly.dll in manosaba v1.1.0

// Plugin
typedef void(*TitileUI_Activate_fn)(void* self);
TitileUI_Activate_fn TitleUI_Activate = nullptr;

void TitleUI_ActivateHook(void* self) {
	printf("--- TitleUI_ActivateHook called! ---\n");

	// 関数呼び出しの一例
	const int UnlockAllStilsRVA = 0x353110;	// WitchTrials.Views.UnlockAllStils():void
	const void* UnlockAllStilsAddr = (void*)((uintptr_t)gameAsm + UnlockAllStilsRVA);
	typedef void(*UnlockAllStils_fn)();
	UnlockAllStils_fn UnlockAllStils = (UnlockAllStils_fn)UnlockAllStilsAddr;

	printf("[INFO]  Calling WitchTrials.Views.UnlockAllStils():void\n");
	UnlockAllStils();
	printf("[INFO]  Success!\n");

	TitleUI_Activate(self);
}
bool dummyPlugin() {
	printf("--- dummyPlugin called! ---\n");

	const wchar_t PluginName[] = L"Unlock All Stils";
	const int PluginVersion = 1;

	// フックする関数を設定
	const int TitileUI_ActivateRVA = 0x3652E0;	// WitchTrials.Views.TitleUI Activate():void
	const void* TitleUI_ActivateAddr = (void*)((uintptr_t)gameAsm + TitileUI_ActivateRVA);
	TitleUI_Activate = (TitileUI_Activate_fn)TitleUI_ActivateAddr;
	MH_Initialize();
	MH_CreateHook(
		(LPVOID)TitleUI_ActivateAddr,
		(LPVOID)TitleUI_ActivateHook,
		(LPVOID*)&TitleUI_Activate
	);
	MH_EnableHook((LPVOID)TitleUI_ActivateAddr);

	return true;
}
// end of Pluguin

void hook_GameInitializer_Start(void* self) {	// 多分型違うけど一旦 void*
    printf("--- hook_GameInitializer_Start called! ---\n");

    // プラグイン読み込む処理ここに書く
    dummyPlugin();

    printf("[INFO]  Hacks are patched!\n");

    // 必ずオリジナルを呼ぶ
    GameInitializer_Start_original(self);
}

bool startHook() {
	printf("--- startHook: Called ---\n");
	try {
		// Load GameAssembly.dll
		while (!gameAsm) {	// inspect until loaded ASM
			printf("[INFO]  Finding GameASM...\n");
			gameAsm = GetModuleHandleW(L"GameAssembly.dll");
			Sleep(5);
		}
		printf("[INFO]  startHook: GameAssembly.dll loaded: %p\n", gameAsm);

		// get base address
		uintptr_t baseAddr = (uintptr_t)gameAsm;

		// calc function address
		void* funcAddrGameInitializer_Start = (void*)(baseAddr + GameInitializer_Start_RVA);
		printf("[DEBUG] GameInitializer.Start() address = %p\n", funcAddrGameInitializer_Start);

		// init MinHook
		MH_Initialize();
		MH_CreateHook(
			(LPVOID)funcAddrGameInitializer_Start,
			(LPVOID)hook_GameInitializer_Start,
			(LPVOID*)&GameInitializer_Start_original
		);
		MH_EnableHook((LPVOID)funcAddrGameInitializer_Start);

		printf("[INFO]  il2cpp_init hook injected successfully.\n");

		return true;
	}
	catch (...) {
		MessageBoxW(nullptr, L"Unhandled exception in StartCoreCLR()", L"MNSB proxy dll", MB_OK);
		return false;
	}
}

// Reference: https://github.com/DDLCPlus/UnityPlayer/blob/main/dllmain.cpp
extern "C" __declspec(dllexport)
int UnityMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmdLine, int nCmdShow) {
    if (!UnityMainOriginal) {
        g_realUnityPlayer = LoadLibraryA("UnityPlayer_original.dll");
        if (!g_realUnityPlayer) {
            MessageBoxW(nullptr, L"Couldn't find UnityPlayer_original.dll.", L"MNSB proxy dll", MB_OK);
            return -1;
        }

        UnityMainOriginal = (UnityMainFunc)GetProcAddress(g_realUnityPlayer, "UnityMain");

        if (!UnityMainOriginal) {
            MessageBoxW(nullptr, L"Couldn't find UnityMain original export! Try re-installing the game.", L"MNSB proxy dll", MB_OK);
            return -1;
        } 

        std::thread(startHook).detach();
    }

    return UnityMainOriginal(hInstance, hPrevInstance, cmdLine, nCmdShow);
}

void setupConsole() {
    if (GetConsoleWindow() != NULL) { return; }

    if (AllocConsole()) {
        FILE* pFile;
        freopen_s(&pFile, "CONOUT$", "w", stdout);
        freopen_s(&pFile, "CONOUT$", "w", stderr);
		// join title
		std::wstring title = name;
		title += L" v";
		title += std::wstring(version);		// std初めて使ったんだけどwstringって何やねんクソ便利やん
        SetConsoleTitleW(title.c_str());

        printf("--- %S Console was Initialized ---\n", std::wstring(name).c_str());
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
		setupConsole();
        DisableThreadLibraryCalls(hModule);
    }
    return TRUE;
}
