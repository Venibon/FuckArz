#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "main.h"
#include <cstring>
#include <thread>

SAMPFUNCS *SF = new SAMPFUNCS();
bool CALLBACK incomingRPC(stRakNetHookParams* params);
void onShowDialog(int id, int style, char* title, char* text);

void __stdcall mainloop(){
	static bool initialized = false;
	if (!initialized){
		if (GAME && GAME->GetSystemState() == eSystemState::GS_PLAYING_GAME && SF->getSAMP()->IsInitialized()){
			initialized = true;
			SF->getRakNet()->registerRakNetCallback(RakNetScriptHookType::RAKHOOK_TYPE_INCOMING_RPC, incomingRPC);
		}
	}
}

bool CALLBACK incomingRPC(stRakNetHookParams* params){
	int RPC = params->packetId;
	BitStream* bs = params->bitStream;
	if (RPC == 61) {
		INT16 dialogId; // INT16 wDialogID
		UINT8 dialogStyle; // UINT8 bDialogStyle
		UINT8 titleLength; // UINT8 bTitleLength
		char title[256]; // char[] szTitle
		UINT8 button1Length; // UINT8 bButton1Len
		char button1[256]; // char[] szButton1
		UINT8 button2Length; // UINT8 bButton2Len
		char button2[256]; // char[] szButton2
		char text[4096]; // CSTRING szInfo[4096]

		bs->ResetReadPointer();
		bs->Read(dialogId);
		bs->Read(dialogStyle);
		bs->Read(titleLength);
		bs->Read(title, titleLength);
		title[titleLength] = '\0';
		bs->Read(button1Length);
		bs->Read(button1, button1Length);
		button1[button1Length] = '\0';
		bs->Read(button2Length);
		bs->Read(button2, button2Length);
		button2[button2Length] = '\0';
		SF->getRakNet()->DecodeString(text, 4096, bs);
		onShowDialog(dialogId, dialogStyle, title, text);
	}

	return true;
};

bool inputActive() {
	return SF->getSAMP()->getScoreboard()->iIsEnabled ? true : false;
}

void onShowDialog(int id, int style, char* title, char* text) {
	std::thread([=]() {
		bool inputActive = SF->getSAMP()->getInput()->iInputEnabled;
		if (id == 26190 && !inputActive) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			SF->getSAMP()->getDialog()->Close(1);
		}
	}).detach();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
	if (dwReasonForCall == DLL_PROCESS_ATTACH)
		SF->initPlugin(mainloop, hModule);
	return TRUE;
}