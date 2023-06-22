#ifndef JNI_H_
#define JNI_H_

#include <mutex>
#include <memory>
#include <fstream>

#include <jni.h>
#include <JNIMapper/JNIMapper.h>
#include <Windows.h>
#include <RscLink.h>

class JNI final
{
public:
	JNI(const JNI&) = delete;
	void operator=(const JNI&) = delete;

	static void Init(std::shared_ptr<RscLink>& p_rsc_link);
	static void Destroy();
	static JNI* Get();
	static bool IsInit();

	JavaVM* GetVM()
	{
		return p_jvm;
	}

	JNIEnv* GetEnv()
	{
		return p_env;
	}

private:
	JNI(std::shared_ptr<RscLink>& p_rsc_link)
		: p_rsc_link(p_rsc_link)
	{
		bool success{ InitJavaInterface() };

		if (success)
		{
			auto entry{ p_rsc_link->operator[]("mappings.json") };

			// unsafe (I really need to update my RscLinker lib to handle resources differently)
			std::uint8_t* buffer{ new std::uint8_t[entry.size + 1] };

			memcpy(buffer, entry.ptr, entry.size);
			buffer[entry.size] = '\0';

			mapper = std::make_unique<JNIMapper>(buffer, p_env);

			delete buffer;
		}
	}

	bool InitJavaInterface()
	{
		jint result{ JNI_GetCreatedJavaVMs(&p_jvm, 1, nullptr) };

		if (result != 0)
		{
			printf("[-] JNI() failed to initialize p_jvm\n");
			MessageBoxA(0, "Check console", "ERROR", MB_ICONERROR);
			return false;
		}

		p_jvm->AttachCurrentThread((void**)&p_env, nullptr);

		return true;
	}

private:
	static JNI* p_instance;
	static std::mutex mutex;

	std::shared_ptr<RscLink>& p_rsc_link;
	JavaVM* p_jvm;
	JNIEnv* p_env;
	std::unique_ptr<JNIMapper> mapper;
};

#endif