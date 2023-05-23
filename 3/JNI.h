#pragma once

#include <memory>

#include <Windows.h>
#include <jni.h>

#include "CRobot.h"

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
		}

		/* Init game classes */
		{
			p_crobot = std::make_unique<CRobot>(p_env);
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
public:
	std::unique_ptr<CRobot> p_crobot;
private:
	JavaVM* p_jvm;
	JNIEnv* p_env;

	bool is_init{ false };
};

inline std::unique_ptr<JNI> p_jni;
