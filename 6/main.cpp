#include <thread>
#include <cstdio>

#include <Windows.h>
#include <jni.h>

#include "JNI.h"
#include "Hooks.h"

constexpr float PI{ 3.1415926f };

float ClampAng(float angle, float min, float max)
{
    if (angle >= 90.f)
        angle -= 360.f;

    return std::clamp(angle, min, max);
}

void MainThread(HMODULE module)
{
    p_jni = std::make_unique<JNI>();
    p_hooks = std::make_unique<Hooks>();

    while (!GetAsyncKeyState(VK_END))
    {
        static JNIClassInstance entity_set(p_jni->GetEnv(), p_jni->p_mapper->classes["Set"].get(), nullptr);
        static JNIClassInstance entity_set_it(p_jni->GetEnv(), p_jni->p_mapper->classes["Iterator"].get(), nullptr);
        static JNIClassInstance curr_entity(p_jni->GetEnv(), p_jni->p_mapper->classes["Entity"].get(), nullptr);
        static JNIClassInstance the_player(p_jni->GetEnv(), p_jni->p_mapper->classes["EntityPlayerSP"].get(), nullptr);
        static bool was_in_menu{ true };

        jobject world_client = p_jni->p_mapper->classes["Minecraft"]->fields["theWorld"]->GetValueObject();


        if (world_client == nullptr)
        {
            was_in_menu = true;
            continue;
        }
        else if (was_in_menu)
        {
            p_jni->p_mapper->classes["WorldClient"]->SetInstance(world_client);

            entity_set.SetInstance(p_jni->p_mapper->classes["WorldClient"]->fields["entityList"]->GetValueObject());
            p_jni->p_mapper->classes["PlayerControllerMP"]->SetInstance(p_jni->p_mapper->classes["Minecraft"]->fields["playerController"]->GetValueObject());
            curr_entity.SetInstance(nullptr);
            the_player.SetInstance(p_jni->p_mapper->classes["Minecraft"]->fields["thePlayer"]->GetValueObject());

            was_in_menu = false;
        }

        /* In case the player was not loaded yet */
        if (the_player.GetInstance() == nullptr || the_player.methods["getHealth"]->CallFloat() == 0.f)
        {
            the_player.SetInstance(p_jni->p_mapper->classes["Minecraft"]->fields["thePlayer"]->GetValueObject());
            continue;
        }

        /* Misc features */
        {
            p_jni->p_mapper->classes["Minecraft"]->fields["rightClickDelayTimer"]->SetValueInt(0.f);

            if (GetAsyncKeyState(VK_XBUTTON2))
            {
                p_jni->p_mapper->classes["Minecraft"]->methods["clickMouse"]->CallVoid();
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }
        }

        /* Aimbot */
        {
            static bool locked{ false };

            entity_set_it.SetInstance(entity_set.methods["iterator"]->CallObject());

            while (!locked && entity_set_it.methods["hasNext"]->CallBoolean())
            {
                jobject entity{ entity_set_it.methods["next"]->CallObject() };

                /* Check if ent is nullptr */
                if (entity == nullptr)
                    break;

                /* Check if localplayer */
                if (entity == the_player.GetInstance())
                    continue;

                curr_entity.SetInstance(entity);

                /* Check if is not item or xp etc and if is dead */
                if (curr_entity.fields["width"]->GetValueFloat() <= 0.5 || curr_entity.fields["isDead"]->GetValueBoolean())
                    continue;

                double src_x{ the_player.fields["posX"]->GetValueDouble() }, src_y{ the_player.fields["posY"]->GetValueDouble() },
                    src_z{ the_player.fields["posZ"]->GetValueDouble() };

                double dst_x{ curr_entity.fields["posX"]->GetValueDouble() }, dst_y{ curr_entity.fields["posY"]->GetValueDouble() }, 
                    dst_z{ curr_entity.fields["posZ"]->GetValueDouble() };

                static double distance{ 3 };

                if (std::sqrt(std::pow(dst_x - src_x, 2) + std::pow(dst_y - src_y, 2) + std::pow(dst_y - src_y, 2)) <= distance)
                    locked = true;
            }

            /* Aim at targeted entity */
            if (locked)
            {
                if (curr_entity.fields["isDead"]->GetValueBoolean() || curr_entity.GetInstance() == the_player.GetInstance())
                {
                    locked = false;
                    goto stopTargeting;
                }
                
                double src_x{ the_player.fields["posX"]->GetValueDouble() }, src_y{ the_player.fields["posY"]->GetValueDouble() },
                    src_z{ the_player.fields["posZ"]->GetValueDouble() };

                double dst_x{ curr_entity.fields["posX"]->GetValueDouble() }, dst_y{ curr_entity.fields["posY"]->GetValueDouble() },
                    dst_z{ curr_entity.fields["posZ"]->GetValueDouble() };


                double magnitude{ std::sqrt(std::pow(dst_x - src_x, 2) + std::pow(dst_y - src_y, 2) + std::pow(dst_y - src_y, 2)) };

                if (magnitude > 10 || magnitude < 1)
                {
                    locked = false;
                    goto stopTargeting;
                }

                double dlt_x{ dst_x - src_x }, dlt_y{ dst_y - src_y }, dlt_z{ dst_z - src_z };

                float yaw{};

                if (dlt_x < 0.f && dlt_z > 0.f || dlt_x > 0.f && dlt_z > 0.f)
                    yaw = -std::atanf(dlt_x / dlt_z) * 180.f / PI;
                else
                    yaw = -std::atanf(dlt_x / dlt_z) * 180.f / PI + 180.f;

                float pitch{ (-atanf((1 - sqrtf(1 - 0.002f * (0.002f * (magnitude * magnitude) + 2 * dlt_y))) / (0.002f * magnitude))) * 180 / PI };

                the_player.fields["yaw"]->SetValueFloat(ClampAng(yaw, -360, 360));
                the_player.fields["pitch"]->SetValueFloat(ClampAng(pitch, -90, 90));
            }
        stopTargeting:
            {}
        }
    }

    p_hooks->Remove();

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
