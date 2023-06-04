#pragma once

#include <Windows.h>
#include "Injector.h"

DWORD GetProcId(const char* className)
{
	HWND windowHandle = FindWindowA(className, 0);

	DWORD pId;
	GetWindowThreadProcessId(windowHandle, &pId);

	return pId;
}

int Inject(const char* procName, const char* dllName)
{
	DWORD pId = GetProcId(procName);

	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, NULL, pId);

	if (proc == 0)
		exit(1);

	char dllPath[MAX_PATH] = { 0 };
	GetFullPathNameA(dllName, MAX_PATH, dllPath, NULL);

	LPVOID memoryAlloc = VirtualAllocEx(proc, NULL, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (memoryAlloc == 0)
		exit(1);

	WriteProcessMemory(proc, memoryAlloc, dllPath, strlen(dllPath) + 1, NULL);

	HANDLE remoteThread = CreateRemoteThread(proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, memoryAlloc, NULL, NULL);

	if (remoteThread != 0)
	{
		WaitForSingleObject(remoteThread, INFINITE);
		CloseHandle(remoteThread);
	}

	VirtualFreeEx(proc, dllPath, 0, MEM_RELEASE);
	CloseHandle(proc);

	return 0;
}

int main()
{
	Inject("notepad", "Dll2.dll");
}