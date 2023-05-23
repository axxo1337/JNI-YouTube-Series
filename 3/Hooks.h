#pragma once

#include <memory>

#include <MinHook.h>

class Hooks
{
public:
	Hooks()
	{
		MH_Initialize();

		is_init = true;
	}

	~Hooks()
	{
		is_init = false;
	}

	bool GetInit()
	{
		return is_init;
	}
private:
	bool is_init{ false };
};

inline std::unique_ptr<Hooks> p_hooks;