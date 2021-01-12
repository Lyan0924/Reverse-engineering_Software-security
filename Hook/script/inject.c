#pragma warning(disable:4996)
#include <windows.h>
#include<stdio.h>
#include <tchar.h>

BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath) {

	HANDLE hProcess = NULL;

	HANDLE hThread = NULL;

	HMODULE hMod = NULL;

	LPVOID pRemoteBuf = NULL;  // 存储在目标进程申请的内存地址

	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);  // 存储DLL文件路径所需的内存空间大小

	LPTHREAD_START_ROUTINE pThreadProc;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))) {

		printf(L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());

		return FALSE;

	}

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);  // 在目标进程空间中申请内存
	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);  // 向在目标进程申请的内存空间中写入DLL文件的路径
	hMod = GetModuleHandle(L"kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");  // 获得LoadLibrary()函数的地址
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;

}


int main() {
	char szDllPath[] = "hook.dll";
	printf("请输入进程号:");
	DWORD dwPID;
	scanf("%u", &dwPID);
	if (InjectDll((DWORD)_tstol(dwPID), szDllPath))
		printf("InjectDll(\"%s\") success\n", szDllPath);
	else
		printf(L"InjectDll(\"%s\") failed\n", szDllPath);
	return 0;
}
