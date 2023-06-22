#ifndef HOOKS_H_
#define HOOKS_H_

#include <memory>

#include <MinHook.h>

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);

class Hooks
{
public:
	Hooks()
	{
		MH_Initialize();

		p_swapbuffers = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");

		if (p_swapbuffers != nullptr)
			MH_CreateHook(p_swapbuffers, &wglSwapBuffers, (LPVOID*)&o_swapbuffers);
		else
			printf("[!] Failed to get wglSwapBuffers addr\n");

		MH_EnableHook(MH_ALL_HOOKS);

		is_init = true;
	}

	~Hooks()
	{
		MH_RemoveHook(p_swapbuffers);

		is_init = false;
	}

	bool GetInit()
	{
		return is_init;
	}

private:
	static bool __stdcall wglSwapBuffers(HDC);

public:
	twglSwapBuffers o_swapbuffers{ nullptr };

private:
	bool is_init{ false };

	void* p_swapbuffers;
};

inline std::unique_ptr<Hooks> p_hooks;

#endif