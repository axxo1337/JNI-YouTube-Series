#ifndef JNI_H_
#define JNI_H_

#include <memory>

#include <Windows.h>
#include <jni.h>

#include "CMinecraft.h"
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
		}

		/* Init game classes */
		{
			p_cminecraft = std::make_unique<CMinecraft>(p_env);
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
	std::unique_ptr<CMinecraft> p_cminecraft;
private:
	JavaVM* p_jvm;
	JNIEnv* p_env;

	bool is_init{ false };
};

inline std::unique_ptr<JNI> p_jni;

#endif