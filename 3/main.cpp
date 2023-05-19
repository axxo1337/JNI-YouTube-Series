#include <thread>
#include <cstdio>

#include <Windows.h>

#include "JNI.h"

void MainThread(HMODULE module)
{
    p_jni = std::make_unique<JNI>();

    /*
    jclass mouse_class{ p_jni->GetInterface()->FindClass("org/lwjgl/input/Mouse")};

    if (mouse_class == nullptr)
    {
        printf("Failed to get Mouse class\n");
        MessageBoxA(0, "ERROR", "Check console", MB_ICONERROR);
        FreeLibrary(module); 
    }

    jmethodID is_button_down_id{ p_env->GetStaticMethodID(mouse_class, "isButtonDown", "(I)Z") };

    if (is_button_down_id == 0)
    {
        printf("Failed to get is_button_down id\n");
        MessageBoxA(0, "ERROR", "Check console", MB_ICONERROR);
        FreeLibrary(module);
    }
    */

    while (!GetAsyncKeyState(VK_END))
    {
        /*
        static jint arg{ 0 };

        if (p_env->CallStaticBooleanMethodA(mouse_class, is_button_down_id, (jvalue*)&arg))
        {
            printf("Mouse clicked!\n");
        }
        */
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