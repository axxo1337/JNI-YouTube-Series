#include "Hooks.h"

bool __stdcall Hooks::wglSwapBuffers(HDC hdc)
{
	return p_hooks->o_swapbuffers(hdc);
}