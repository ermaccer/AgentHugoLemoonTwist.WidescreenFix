// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "MemoryMgr.h"
#include "Camera.h"

using namespace Memory::VP;

int gResolutionX, gResolutionY;
float gAspectRatio;

void Set2DScale()
{
	float x = *(float*)0x50FBE8;


	float _4_3 = 4.0f / 3.0f;
	float scaleRatio = gAspectRatio / _4_3;

	x *= scaleRatio;

	Patch<float>(0x50FBE8, x);
}


void __declspec(naked) Set2DScale_Hook()
{
	static int jmpContinue = 0x405EDE;

	_asm pushad

	Set2DScale();

	_asm {
		popad
		mov		[ebp - 4], ebx
		mov     ds:0x50FBEC, eax
		jmp jmpContinue
	}
}


void __declspec(naked) SetAspectRatio_Hook()
{
	static int jmpContinue = 0x46980E;
	static Camera* pCamera = nullptr;

	_asm {
		mov pCamera, esi
		pushad
	}
	if (pCamera)
		pCamera->m_fAspectRatio = gAspectRatio;

	_asm {
		popad
		jmp jmpContinue
	}

}


void Init()
{
	// disable writing 800x600 on startup
	Nop(0x405EFC, 86);

	gResolutionX = GetPrivateProfileInt(L"Video Card (Adapter 0)", L"Width", 800, L".\\Game.ini");
	gResolutionY = GetPrivateProfileInt(L"Video Card (Adapter 0)", L"Height", 600, L".\\Game.ini");

	gAspectRatio = (float)gResolutionX / (float)gResolutionY;

	InjectHook(0x469804, SetAspectRatio_Hook, PATCH_JUMP);
	InjectHook(0x405ED6, Set2DScale_Hook, PATCH_JUMP);
}



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Init();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

