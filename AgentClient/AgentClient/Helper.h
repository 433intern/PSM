#pragma once

#include <tlhelp32.h>

class Helper
{
public:
	Helper();
	~Helper();
	void Init();
	void UpdateProcessList();
	int CheckDuplicateProcessName(const std::string& name);

public:
	std::vector<ProcessInfo> processList;


};