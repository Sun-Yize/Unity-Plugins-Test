#pragma once

#ifdef ADDITION_EXPORTS
#define ADDITION_API __declspec(dllexport)
#else
#define ADDITION_API __declspec(dllimport)
#endif

extern "C" ADDITION_API int add(int a, int b);
