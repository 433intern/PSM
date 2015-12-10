#pragma once

class Query
{
private:
	HANDLE event_;
	Helper helper;
	MemPooler<ProcessCounterEntry> *entryPoolManager;
	MemPooler<MachineCounterEntry> *mentryPoolManager;

	CRITICAL_SECTION listLock;

public:
	std::list<ProcessInfo_Agent> checkProcessList;
	std::list<std::string> counterList;
	std::vector<ProcessCounterEntry*> processLogList;

	std::list<std::string> machineCounterList;
	std::vector<MachineCounterEntry*> machineLogList;

	std::thread machineRecorder;
	std::thread processRecorder;

	bool isMachineRecordEnd;
	bool isProcessRecordEnd;


	Query();
	~Query();

	void Init();
	
	bool StartRecord(bool isMachine, int totalTime, int recordTime, int interval,
					long long int delay);
	bool StopRecord(bool isMachine);

	bool Record(bool isMachine, int totalTime, int recordTime, int interval,
				long long int delay);
	bool InitCounterInfo(PDH_HQUERY& query, bool isMachine);
	bool IsCheckProcess(std::string processName);
	void ClearLogList(bool isMachine);

	bool AddCounter(std::string counter, bool isMachine);
	bool DeleteCounter(std::string counter, bool isMachine);

	bool AddProcess(std::string processName);
	bool DeleteProcess(std::string processName);


	char* GetCurTime();
};

