#include <string>
#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <tlhelp32.h>
using        namespace       std;
DWORD FindProcessId(wstring processname);


BOOL StringToWString(const std::string &str, std::wstring &wstr)
{
	int nLen = (int)str.length();
	wstr.resize(nLen, L' ');
	
	int nResult = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), nLen, (LPWSTR)wstr.c_str(), nLen);
	
	if (nResult == 0)
		{
		    return FALSE;
		}
	
		return TRUE;
}
  //wstring高字节不为0，返回FALSE
 BOOL WStringToString(const std::wstring &wstr, std::string &str)
{
	    int nLen = (int)wstr.length();
	    str.resize(nLen, ' ');

		     int nResult = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wstr.c_str(), nLen, (LPSTR)str.c_str(), nLen, NULL, NULL);
	
		     if (nResult == 0)
		    {
		        return FALSE;
		    }

		     return TRUE;
}

int _tmain(int argc, _TCHAR* argv[]){
	STARTUPINFO StartInfo;
	PROCESS_INFORMATION pinfo;
	if (argc != 3){
		wprintf(L"please input you need deamon exe name\n");
		wprintf(L"please input you need deamon time(s)\n");
		wprintf(L"please input like: deamonProcess xxx.exe 1200\n");
		return 0;
	}
	wstring exeName = argv[1];
	string deamonTimeStr;
	WStringToString(argv[2], deamonTimeStr);
	time_t deamonTime = atoi(deamonTimeStr.c_str());

	memset(&StartInfo, 0, sizeof(StartInfo));
	StartInfo.cb = sizeof(STARTUPINFO);
	StartInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartInfo.wShowWindow = SW_MINIMIZE;

	time_t aliveTime = 0;

	while (true){
		DWORD pid = FindProcessId(exeName);
		if (pid){
			printf("process is alive\n");
			Sleep(1000);
			aliveTime = aliveTime + 1000;
			if (aliveTime > deamonTime * 1000){
				printf("kill process\n");
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
				TerminateProcess(hProcess,-1);
				CloseHandle(hProcess);
				aliveTime = 0;
			}
		}
		else{
			printf("create process\n");
			Sleep(100);
			LPTSTR szCmdline = _tcsdup(exeName.c_str());
			CreateProcess(NULL,
				szCmdline,
				NULL,
				NULL,
				FALSE,
				CREATE_NEW_CONSOLE,
				NULL,
				NULL,
				&StartInfo,
				&pinfo);
			aliveTime = 0;
		}
	}
	return 0;
}


DWORD FindProcessId(wstring processname)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	DWORD result = NULL;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap) return(FALSE);

	pe32.dwSize = sizeof(PROCESSENTRY32); // <----- IMPORTANT

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);          // clean the snapshot object
	//	printf("!!! Failed to gather information on system processes! \n");
		return(NULL);
	}

	do
	{
	//	printf("Checking process %ls\n", pe32.szExeFile);
		wstring ws = wstring(pe32.szExeFile);
		if (processname == ws)
		{
			result = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return result;
}