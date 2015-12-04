#include "stdafx.h"

Helper::Helper()
{

}


Helper::~Helper()
{

}

void Helper::Init()
{

}

int Helper::CheckDuplicateProcessName(const std::string& name)
{
	int cnt = 0;
	for (ProcessInfo p : processList)
	{
		if (p.name == name)
		{
			cnt++;
		}
	}

	return cnt;
}

void Helper::UpdateProcessList()
{
	int i = 0;
	PRINT("[Helper] UpdateProcessList...\n");
	processList.clear();

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot)
	{
		PROCESSENTRY32 ProcessEntry32;
		BOOL bProcessFound;
		ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);
		bProcessFound = Process32First(hSnapshot, &ProcessEntry32);
		while (bProcessFound)
		{
			if (i == 0 || i == 1){
				bProcessFound = Process32Next(hSnapshot, &ProcessEntry32);
				i++;
				continue;
			}
			std::string processName(ProcessEntry32.szExeFile);
			int cnt = CheckDuplicateProcessName(processName.substr(0, processName.length() - 4));

			PRINT("%s#%d [%d]\n", processName.substr(0, processName.length() - 4).c_str(), cnt, ProcessEntry32.th32ProcessID);

			ProcessInfo p;
			p.name = processName.substr(0, processName.length()-4);
			p.processID = ProcessEntry32.th32ProcessID;
			p.number = cnt;

			processList.push_back(p);

			bProcessFound = Process32Next(hSnapshot, &ProcessEntry32);
		}
		CloseHandle(hSnapshot);
	}
	PRINT("[Helper] UpdateProcessList Done\n");
}

