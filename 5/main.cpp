#include <thread>
#include <cstdio>

#include <Windows.h>
#include <jni.h>

#include "JNI.h"
#include "Hooks.h"
#include "JNIMapper.h"
#include "rsc.h"

void MainThread(HMODULE module)
{
    p_jni = std::make_unique<JNI>();
    p_hooks = std::make_unique<Hooks>();

    JNIMapper mapper(map, p_jni->GetEnv());

    mapper.classes["Minecraft"]->SetInstance(mapper.classes["Minecraft"]->fields["theMinecraft"]->GetValueObject());

    while (!GetAsyncKeyState(VK_END))
    {
        mapper.classes["Minecraft"]->fields["rightClickDelayTimer"]->SetValueInt(0.f);

        if (GetAsyncKeyState(VK_XBUTTON1))
        {
            mapper.classes["Minecraft"]->methods["clickMouse"]->CallVoid();
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
