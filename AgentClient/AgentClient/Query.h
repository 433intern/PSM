#pragma once

class Query
{
private:
	HANDLE event_;
	Helper helper;
	MemPooler<ProcessCounterEntry> *entryPoolManager;

	PDH_HQUERY query;

public:
	std::list<ProcessInfo_Agent> checkProcessList;
	std::list<std::string> counterList;
	std::list<std::string> machineCounterList;
	std::vector<ProcessCounterEntry*> processLogList;

	Query();
	~Query();

	void Init();
	bool Record(int recordTime, int interval);
	bool InitCounterInfo(PDH_HQUERY& query);
	bool IsCheckProcess(std::string processName);
	void ClearLogList();

	bool AddCounter(std::string counter, bool isMachine);
	bool DeleteCounter(std::string counter, bool isMachine);

	bool AddProcess(std::string processName);
	bool DeleteProcess(std::string processName);


	char* GetCurTime();
};

