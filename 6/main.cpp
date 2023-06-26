#include <thread>
#include <cstdio>

#include <Windows.h>
#include <jni.h>

#include "JNI.h"
#include "Hooks.h"

constexpr float PI{ 3.1415926f };

struct Vec3
{
    float x, y, z;

    Vec3 operator- (Vec3& o)
    {
        return Vec3{x - o.x, y - o.y, z - o.z};
    }
};

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
        static JNIClassInstance entity_list(p_jni->GetEnv(), p_jni->p_mapper->classes["List"].get(), nullptr);
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

            entity_list.SetInstance(p_jni->p_mapper->classes["WorldClient"]->fields["loadedEntityList"]->GetValueObject());
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
            static float max_distance{ 10.f };

            int entity_list_size{ entity_list.methods["size"]->CallInt()};

            if (!locked)
            {
                for (int i{}; i < entity_list_size; ++i)
                {
                    jobject entity{ entity_list.methods["get"]->CallObject((jvalue*)&i) };

                    /* Check if ent is nullptr or localplayer */
                    if (entity == nullptr || entity == the_player.GetInstance())
                        continue;

                    curr_entity.SetInstance(entity);

                    /* Check if is not item or xp etc and if is dead */
                    if (curr_entity.fields["width"]->GetValueFloat() <= 0.5 || curr_entity.fields["isDead"]->GetValueBoolean())
                        continue;

                    Vec3 localpos{ (float)the_player.fields["posX"]->GetValueDouble(), (float)the_player.fields["posY"]->GetValueDouble(), (float)the_player.fields["posZ"]->GetValueDouble() };
                    Vec3 entpos{ (float)curr_entity.fields["posX"]->GetValueDouble(), (float)curr_entity.fields["posY"]->GetValueDouble(), (float)curr_entity.fields["posZ"]->GetValueDouble() };

                    if (std::sqrtf(std::pow(entpos.x - localpos.x, 2) + std::pow(entpos.y - localpos.y, 2) + std::pow(entpos.z - localpos.z, 2)) <= max_distance)
                        locked = true;
                }
            }

            /* Aim at targeted entity */
            if (locked)
            {
                if (curr_entity.fields["isDead"]->GetValueBoolean() || curr_entity.GetInstance() == the_player.GetInstance())
                {
                    locked = false;
                    goto stopTargeting;
                }
                
                Vec3 localpos{ (float)the_player.fields["posX"]->GetValueDouble(), (float)the_player.fields["posY"]->GetValueDouble(), (float)the_player.fields["posZ"]->GetValueDouble() };
                Vec3 entpos{ (float)curr_entity.fields["posX"]->GetValueDouble(), (float)curr_entity.fields["posY"]->GetValueDouble(), (float)curr_entity.fields["posZ"]->GetValueDouble() };

                float magnitude{ std::sqrtf(std::pow(entpos.x - localpos.x, 2) + std::pow(entpos.y - localpos.y, 2) + std::pow(entpos.z - localpos.z, 2)) };

                if (magnitude > max_distance || magnitude < 1)
                {
                    locked = false;
                    goto stopTargeting;
                }

                Vec3 delta_vec = entpos - localpos;

                float yaw{};

                /* Check quadrants for angle correction */
                if (delta_vec.x < 0.f && delta_vec.z > 0.f || delta_vec.x > 0.f && delta_vec.z > 0.f)
                    yaw = -std::atanf(delta_vec.x / delta_vec.z) * 180.f / PI;
                else
                    yaw = -std::atanf(delta_vec.x / delta_vec.z) * 180.f / PI + 180.f;

                the_player.fields["yaw"]->SetValueFloat(ClampAng(yaw, -360.f, 360.f));
                the_player.fields["pitch"]->SetValueFloat(ClampAng((-atanf((1 - sqrtf(1 - 0.002f * (0.002f * (magnitude * magnitude) + 2 * delta_vec.y))) / (0.002f * magnitude))) * 180 / PI, -90.f, 90.f));
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
