#include "pc.h"
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

using namespace std;

//Author SuperNov4

DWORD snapProc(const char *procName)  //Get snapshots with CreateToolHelp32Snapshot() function
{
	HANDLE hSnap;
	DWORD procId = 0;
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,procId);

	if (hSnap != INVALID_HANDLE_VALUE)
	{

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!strcmp(procEntry.szExeFile,procName))
				{
					procId = procEntry.th32ProcessID;


				}
			} while (Process32Next(hSnap, &procEntry));
		}

	}
	return procId;
	CloseHandle(hSnap);


}



int main(int argc, const char argv[])
{
	//Variables

	HANDLE hProcess;
	DWORD processId = 0;
	const char* processName = "Process Name"; //Enter your process Name
	LPCSTR path = "[Dll path]"; // Dll path

	processId = snapProc(processName);

	cout << "   ********SuperNova Dll injector*********\n\n";
	printf(" Process Name >> %s\n",processName);
	cout << "Process Id >> " << processId<<endl;

	if (processId == NULL)
	{
		printf("Process was terminated or you didn't open it !!\n");
		printf("Status code : %d\n", GetLastError());
		exit(-1);

	}
	

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processId);

	if (hProcess == INVALID_HANDLE_VALUE)
	{
		printf("Process was terminated or you didn't open it\n ");
		printf("Status code : %d\n", GetLastError());
		exit(-1);
	}

	void* exec_mem = VirtualAllocEx(hProcess, 0, sizeof(path), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	printf("VirtualAllocEx function working ...\n");

	//WriteProcess

	if (exec_mem)
	{
		BOOL write = WriteProcessMemory(hProcess, exec_mem, path, sizeof(path) + 1, NULL);

		if (write == NULL)
		{
			printf("WriteProcessMemory function failed !!\n");
			printf("Status code : %d\n", GetLastError());
		}
	}
	cout << "Writing dll in process's memory\n";
	//LoadLibrary and Kernel32.dll

	printf("Waiting for thread [!]\n");
	Sleep(5000);

	LPVOID loadLibraryA = GetProcAddress(GetModuleHandle("LoadLibraryA"), "kernel32.dll"); //LoadLibraryA

	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)loadLibraryA,exec_mem, 0, NULL); //Open a thread for resume 

	printf("Thread resuming sucess !!\n");
	printf("Dll located in 0%x", exec_mem); //Path location in memory

	WaitForSingleObject(hThread, INFINITE); //Thread Queue

	VirtualFree(exec_mem, sizeof(path), 0);

	
	return 0;
}