#pragma once

#include <Windows.h>

DWORD GetProcId(const char* className);

int Inject(const char* proc, const char* dll);