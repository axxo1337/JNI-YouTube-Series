#include <fstream>
#include <thread>
#include <memory>

#include <Windows.h>
#include <RscLink.h>

#include "JNI.h"
#include "Link.h"
#include "Hooks.h"

static std::shared_ptr<RscLink> p_rsc_link;

void Init();
void Release();

/* Real entry point of the cheat */
void MainThread(HINSTANCE instance)
{
    Init();

    while (!GetAsyncKeyState(VK_END))
    {

    }

    Release();

    FreeLibrary(instance);
}

/* Initialize things here */
void Init()
{
    p_rsc_link = std::make_shared<RscLink>((std::uint8_t*)heading, LINK_HEADING_SIZE, (std::uint8_t*)data, LINK_DATA_SIZE);
    JNI::Init(p_rsc_link);
    Hooks::Init();
}

/* Manually release things here */
void Release()
{
    Hooks::Destroy();
    JNI::Destroy();
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