#include <thread>
#include <cstdio>

#include <Windows.h>
#include <jni.h>

void MainThread(HMODULE module)
{
    JavaVM* p_jvm{ nullptr };
    jint result = JNI_GetCreatedJavaVMs(&p_jvm, 1, nullptr);

    JNIEnv* p_env{ nullptr }; // JNI interface pointer
    p_jvm->AttachCurrentThread((void**)&p_env, nullptr);

    jclass mouse_class{ p_env->FindClass("org/lwjgl/input/Mouse") };

    if (mouse_class == nullptr)
    {
        printf("Mouse class not found\n");
        FreeLibrary(module);
    }

    jmethodID is_button_down_id{ p_env->GetStaticMethodID(mouse_class, "isButtonDown", "(I)Z") };

    if (is_button_down_id == 0)
    {
        printf("Did not find isButtonDown\n");
        FreeLibrary(module);
    }

    jint button_id{ 0 };

    while (!GetAsyncKeyState(VK_END))
    {
        if (p_env->CallStaticBooleanMethodA(mouse_class, is_button_down_id, (jvalue*)&button_id))
        {
            printf("[+] Mouse pressed\n");
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