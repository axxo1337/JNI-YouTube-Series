#ifndef HOOKS_H_
#define HOOKS_H_

#include <mutex>
#include <vector>

#include <MinHook.h>
#include <Windows.h>

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);

class Hooks final
{
public:
	Hooks(const Hooks&) = delete;
	void operator=(const Hooks&) = delete;

	static void Init();
	static void Destroy();
	static Hooks* Get();
	static bool IsInit();

private:
	Hooks()
	{
		p_instance->IntallHooks();
	}

	void PushHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal);
	void IntallHooks();
	void RemoveHooks();
	// Hooked functions definitions
	static bool __stdcall wglSwapBuffers(HDC);

public:
	twglSwapBuffers o_swapbuffers{ nullptr };

private:
	static Hooks* p_instance;
	static std::mutex mutex;

	std::vector<void*> hooks;
};

#endif