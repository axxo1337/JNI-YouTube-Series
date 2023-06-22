#ifndef JNI_H_
#define JNI_H_

#include <memory>

#include <Windows.h>
#include <jni.h>

#include "JNIMapper.h"
#include "rsc.h"
#include "Utils.h"

class JNI final
{
public:
	JNI()
	{
		/* Init main pointers */
		{
			jint result = JNI_GetCreatedJavaVMs(&p_jvm, 1, nullptr);

			if (result != 0)
			{
				printf("[-] JNI() failed to initialize p_jvm\n");
				MessageBoxA(0, "ERROR", "Check console", MB_ICONASTERISK);
			}

			p_jvm->AttachCurrentThread((void**)&p_env, nullptr);

			p_mapper = std::make_unique<JNIMapper>(map, p_env);
		}

		/* Init game classes */
		{
			p_mapper->classes["Minecraft"]->SetInstance(p_mapper->classes["Minecraft"]->fields["theMinecraft"]->GetValueObject());
			p_mapper->classes["WorldClient"]->SetInstance(p_mapper->classes["Minecraft"]->fields["theWorld"]->GetValueObject());
			p_mapper->classes["PlayerControllerMP"]->SetInstance(p_mapper->classes["Minecraft"]->fields["playerController"]->GetValueObject());
		}

		is_init = true;
	}

	~JNI()
	{
		p_jvm->DetachCurrentThread();

		is_init = false;
	}

	bool GetInit()
	{
		return is_init;
	}

	JNIEnv* GetEnv()
	{
		return p_env;
	}

public:
	std::unique_ptr<JNIMapper> p_mapper;

private:
	JavaVM* p_jvm;
	JNIEnv* p_env;

	bool is_init{ false };
};

inline std::unique_ptr<JNI> p_jni;

#endif