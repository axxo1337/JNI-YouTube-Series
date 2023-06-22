#ifndef UTILS_H_
#define UTILS_H_

#include <cassert>

#include <Windows.h>

#define myassert(condition) \
	assert(condition); \
	MessageBoxA(0, "An error occured check console", "ERROR", MB_ICONERROR)

#endif