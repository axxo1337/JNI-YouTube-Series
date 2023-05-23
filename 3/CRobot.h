#pragma once

#include <jni.h>

class CRobot final
{
public:
	CRobot(JNIEnv* p_env)
		: p_env(p_env)
	{
		Init();
	}

	bool Init()
	{
		class_ptr = p_env->FindClass("java/awt/Robot");

		if (class_ptr == nullptr)
		{
			printf("[-] Failed to get %s class pointer\n", name);
			return false;
		}

		jmethodID constructor_id = p_env->GetMethodID(class_ptr, "<init>", "()V");
		class_instance = p_env->NewObject(class_ptr, constructor_id);

		if (class_instance == nullptr)
		{
			printf("[-] Failed to get %s class instance\n", name);
			return false;
		}
	}

	//
	// Java methods
	//

	void mousePress(jint buttons)
	{
		static jmethodID method_id = p_env->GetMethodID(class_ptr, "mousePress", "(I)V");
		p_env->CallVoidMethodA(class_instance, method_id, (jvalue*)&buttons);
	}

	void mouseRelease(jint buttons)
	{
		static jmethodID method_id = p_env->GetMethodID(class_ptr, "mouseRelease", "(I)V");
		p_env->CallVoidMethodA(class_instance, method_id, (jvalue*)&buttons);
	}

	void delay(jint ms)
	{
		static jmethodID method_id = p_env->GetMethodID(class_ptr, "delay", "(I)V");
		p_env->CallVoidMethodA(class_instance, method_id, (jvalue*)&ms);
	}
private:
	jclass class_ptr;
	jobject class_instance;
	JNIEnv* p_env;

	const char name[7] = {"Robot"};
};