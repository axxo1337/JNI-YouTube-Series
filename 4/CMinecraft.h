#pragma once

#include <jni.h>

class CMinecraft final
{
public:
	CMinecraft(JNIEnv* p_env)
		: p_env(p_env)
	{
		Init();
	}

	bool Init()
	{
		class_ptr = p_env->FindClass("ave");

		if (class_ptr == nullptr)
		{
			printf("[-] Failed to get %s class pointer\n", name);
			return false;
		}

		jfieldID class_instance_field_id{ p_env->GetStaticFieldID(class_ptr, "S", "Lave;")};

		if (class_instance_field_id == 0)
		{
			printf("[-] Failed to get class instance field id\n");
			return false;
		}

		class_instance = p_env->GetStaticObjectField(class_ptr, class_instance_field_id);

		if (class_instance == nullptr)
		{
			printf("[-] Failed to get %s class instance\n", name);
			return false;
		}
	}

	void SetRightClickDelay(int new_delay)
	{
		static jfieldID mouse_delay_id{ p_env->GetFieldID(class_ptr, "ap", "I") };
		p_env->SetIntField(class_instance, mouse_delay_id, new_delay);
	}

	//
	// Java methods
	//
private:
	jclass class_ptr;
	jobject class_instance;
	JNIEnv* p_env;

	const char name[10] = { "Minecraft" };
};