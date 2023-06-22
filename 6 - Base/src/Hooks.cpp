#include "Hooks.h"

void Hooks::PushHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal)
{
	hooks.push_back(pTarget);
	MH_CreateHook(pTarget, pDetour, ppOriginal);
}

void Hooks::IntallHooks()
{
	MH_Initialize();

	void* p_swapbuffers = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");

	if (p_swapbuffers != nullptr)
		MH_CreateHook(p_swapbuffers, &wglSwapBuffers, (LPVOID*)&o_swapbuffers);
	else
		printf("[!] Failed to get wglSwapBuffers addr\n");

	MH_EnableHook(MH_ALL_HOOKS);
}

void Hooks::RemoveHooks()
{
	MH_DisableHook(MH_ALL_HOOKS);

	for (auto& hook : hooks)
		MH_RemoveHook(hook);
}

//
// Hooked functions
//

bool __stdcall Hooks::wglSwapBuffers(HDC hdc)
{
	return Hooks::Get()->o_swapbuffers(hdc);
}

//
// Singleton stuff (ignore)
//

Hooks* Hooks::p_instance;
std::mutex Hooks::mutex;

void Hooks::Init()
{
	if (p_instance != nullptr)
		return;

	p_instance = new Hooks();
}

void Hooks::Destroy()
{
	if (p_instance == nullptr)
		return;

	p_instance->RemoveHooks();

	delete p_instance;
	p_instance = nullptr;
}

Hooks* Hooks::Get()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (p_instance == nullptr)
		return nullptr; // should init but for practical reasons does not

	return p_instance;
}

bool Hooks::IsInit()
{
	return p_instance != nullptr;
}