#include <windows.h>

#include <stdio.h>

LONG IATHook(
	__in_opt void* pImageBase,
	__in_opt char* pszImportDllName,
	__in char* pszRoutineName,
	__in void* pFakeRoutine,
	__out HANDLE* phHook
);

LONG UnIATHook(__in HANDLE hHook);

void* GetIATHookOrign(__in HANDLE hHook);

typedef int(__stdcall* LPFN_WriteFile)(__in HANDLE fFile,
	__in LPCVOID lpBuffer,
	__in DWORD nNumberOfBytesToWrite,
	__out LPDWORD lpNumberOfBytesWritten,
	__in LPOVERLAPPED lpOverlapped);

HANDLE g_hHook_WriteFile = NULL;
//////////////////////////////////////////////////////////////////////////

//int __stdcall Fake_MessageBoxA( __in_opt HWND hWnd , __in_opt char* lpText , __in_opt char* lpCaption , __in UINT uType)
//{
//	LPFN_MessageBoxA fnOrigin = (LPFN_MessageBoxA)GetIATHookOrign(g_hHook_MessageBoxA);
//
//	return fnOrigin(hWnd , "hello hack" , lpCaption , uType);
//}
int __stdcall Fake_WriteFile(
	HANDLE       fFile,
	LPCVOID      lpBuffer,
	DWORD        nNumberOfBytesToWrite,
	LPDWORD      lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
)
{

	LPFN_WriteFile fnOrigin = (LPFN_WriteFile)GetIATHookOrign(g_hHook_WriteFile);
	char content[] = "654321";  //篡改内容 
	DWORD dwWrite;
	return fnOrigin(fFile, &content, strlen(content), &dwWrite, lpOverlapped);
}

int __cdecl wmain(int nArgc, WCHAR** Argv)
{
	do
	{
		UNREFERENCED_PARAMETER(nArgc);
		UNREFERENCED_PARAMETER(Argv);

		IATHook(
			GetModuleHandleW(NULL),
			"kernel32.dll",
			"WriteFile",
			Fake_WriteFile,
			&g_hHook_WriteFile
		);



		//MessageBoxA(NULL , "test" , "caption1" , 0);

		//UnIATHook( g_hHook_MessageBoxA);

		//MessageBoxA(NULL , "test" , "caption2" , 0);

	} while (FALSE);

	return 0;
}