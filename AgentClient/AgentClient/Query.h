#pragma once

class Query
{
private:
	const int maxLogCnt;
	HANDLE event_;
	Helper helper;
	MemPooler<ProcessCounterEntry> *entryPoolManager;


public:
	std::vector<ProcessInfo_Agent> checkProcessList;
	std::list<agent::ProcessCounter> counterList;
	std::vector<ProcessCounterEntry*> processLogList;

	Query(const int& maxLogCnt);
	~Query();

	void Init();
	bool Record(int recordTime, int interval);
	bool InitCounterInfo(PDH_HQUERY& query);
	bool IsCheckProcess(std::string processName);
	void ClearLogList();

	void AddCounter(agent::ProcessCounter counter);
	void DeleteCounter(agent::ProcessCounter counter);
};

