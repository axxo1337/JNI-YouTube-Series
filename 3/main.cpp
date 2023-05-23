#include <thread>
#include <cstdio>

#include <Windows.h>
#include <jni.h>

#include "JNI.h"
#include "Hooks.h"

void MainThread(HMODULE module)
{
    p_jni = std::make_unique<JNI>();
    p_hooks = std::make_unique<Hooks>();

    while (!GetAsyncKeyState(VK_END))
    {
        if (GetAsyncKeyState(VK_XBUTTON1))
        {
            p_jni->p_crobot->mousePress(16);
            p_jni->p_crobot->mouseRelease(16);
            p_jni->p_crobot->delay(200 + (rand() % 20));
        }
    }

    FreeLibrary(module);
}

bool __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) 
{
    static FILE* p_file{ nullptr };
    static std::thread main_thread;

    if (reason == DLL_PROCESS_ATTACH)
    {
        AllocConsole();
        freopen_s(&p_file, "CONOUT$", "w", stdout);

        main_thread = std::thread([instance] { MainThread(instance); });
        if (main_thread.joinable())
            main_thread.detach();
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        fclose(p_file);
        FreeConsole();
    }

    return true;
}
