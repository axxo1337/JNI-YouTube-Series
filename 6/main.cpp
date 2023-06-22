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
        jobject world_client = p_jni->p_mapper->classes["Minecraft"]->fields["theWorld"]->GetValueObject();

        static bool was_in_menu{ false };

        if (world_client == nullptr)
        {
            was_in_menu = true;
            continue;
        }
        else if (was_in_menu)
        {
            p_jni->p_mapper->classes["WorldClient"]->SetInstance(world_client);
            was_in_menu = false;
        }

        p_jni->p_mapper->classes["Minecraft"]->fields["rightClickDelayTimer"]->SetValueInt(0.f);

        if (GetAsyncKeyState(VK_XBUTTON2))
        {
            p_jni->p_mapper->classes["Minecraft"]->methods["clickMouse"]->CallVoid();
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
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
