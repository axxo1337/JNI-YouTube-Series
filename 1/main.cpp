#include <thread>
#include <cstdio>

#include <Windows.h>
#include <jni.h>

void MainThread(HMODULE instance)
{
    JavaVM* p_jvm{ nullptr };
    jint result = JNI_GetCreatedJavaVMs(&p_jvm, 1, nullptr);

    void* p_env{ nullptr }; // JNI interface pointer
    p_jvm->AttachCurrentThread(&p_env, nullptr);

    while (!GetAsyncKeyState(VK_END))
    {

    }

    FreeLibrary(instance);
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
