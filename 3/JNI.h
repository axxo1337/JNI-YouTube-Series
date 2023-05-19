#pragma once

#include <string>
#include <map>

#include <memory>

#include <Windows.h>
#include <jni.h>

#include "CMouse.h"

class JNI final
{
public:
	JNI()
	{
		/* Initialize jvm & jni interface */
		{
			jint result = JNI_GetCreatedJavaVMs(&p_jvm, 1, nullptr);

			if (result != 0)
			{
				printf("Failed to get jvm\n");
				MessageBoxA(0, "ERROR", "Check console", MB_ICONERROR);
			}

			p_jvm->AttachCurrentThread((void**)&p_env, nullptr);
		}

		/* Initialize classes */
		{
			cmouse = std::make_shared<CMouse>(p_env);
		}

		is_init = true;
	}

	~JNI()
	{
		p_jvm->DetachCurrentThread();

		is_init = false;
	}

	/* Decode signatures of methods with no object as argument or return type (except String) */
	std::string DecodeSignature(std::string method);

	bool IsInit()
	{
		return is_init;
	}
	
	JavaVM* GetJVM()
	{
		return p_jvm;
	}

	JNIEnv* GetInterface()
	{
		return p_env;
	}
public:
	std::shared_ptr<CMouse> cmouse;
private:
	JavaVM* p_jvm;
	JNIEnv* p_env;

	bool is_init{ false };
};

inline std::unique_ptr<JNI> p_jni;
