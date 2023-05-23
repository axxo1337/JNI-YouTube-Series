#pragma once

#include <jni.h>

class CMouse final
{
public:
	CMouse(JNIEnv* p_env)
		: p_env(p_env)
	{
		Init();
	}

	~CMouse()
	{
		is_init = false;
	}

	bool Init()
	{
		if (is_init)
			return true;

		class_ptr = p_env->FindClass("org/lwjgl/input/Mouse");

		if (class_ptr == nullptr)
		{
			printf("Failed to get Mouse class\n");
			return false;
		}

		is_init = true;
		return is_init;
	}
private:
	JNIEnv* p_env;
	jclass class_ptr;

	bool is_init{ false };
};
