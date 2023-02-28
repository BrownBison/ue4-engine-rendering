#include <Windows.h>
#include <SDK.hpp>
#include <detours/detours.h> /* Add your own copy of detours in this path! */

#define CREATE_HOOK(Detour, Original) DetourTransactionBegin(); DetourUpdateThread(GetCurrentThread()); DetourAttach(&(void*&)Original, Detour); DetourTransactionCommit();
#define REMOVE_HOOK(Detour, Original) DetourTransactionBegin(); DetourUpdateThread(GetCurrentThread()); DetourDetach(&(void*&)Original, Detour); DetourTransactionCommit();

void Render(SDK::UCanvas* Canvas)
{
  // do yo rendering there
}

void (*ProcessEvent)(SDK::UObject* Object, SDK::UFunction* Function, void* Parameters) = decltype(ProcessEvent)((uintptr_t)GetModuleHandleW(0) + /* ProcessEvent offset here */);
void ProcessEventHook(SDK::UObject* Object, SDK::UFunction* Function, void* Parameters)
{
  if (Object->IsA(SDK::AHUD::StaticClass())) 
	{
		auto hud = reinterpret_cast<SDK::AHUD*>(Object);
		Render(hud->Canvas);
	}
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
  CREATE_HOOK(ProcessEventHook, ProcessEvent);

  return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, MainThread, 0, 0, 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
