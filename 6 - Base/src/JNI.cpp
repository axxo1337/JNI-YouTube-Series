#include "JNI.h"

JNI* JNI::p_instance;
std::mutex JNI::mutex;

void JNI::Init(std::shared_ptr<RscLink>& p_rsc_link)
{
	if (p_instance != nullptr)
		return;

	p_instance = new JNI(p_rsc_link);
}

void JNI::Destroy()
{
	if (p_instance == nullptr)
		return;

	p_instance->p_jvm->DetachCurrentThread();

	delete p_instance;
	p_instance = nullptr;
}

JNI* JNI::Get()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (p_instance == nullptr)
		return nullptr; // should init but for practical reasons does not

	return p_instance;
}

bool JNI::IsInit()
{
	return p_instance != nullptr;
}