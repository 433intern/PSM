#include "stdafx.h"

#include <time.h>

extern AgentClientApp* agentClientApp;

Query::Query()
: isMachineRecordEnd(true), isProcessRecordEnd(true), cpuCounter("\\Processor(_Total)\\% Processor Time")
{
	InitializeCriticalSectionAndSpinCount(&listLock, 4000);
}

Query::~Query()
{
	for (ProcessCounterEntry* p : processLogList) entryPoolManager->Free(p);
	for (MachineCounterEntry* p : machineLogList) mentryPoolManager->Free(p);
	delete entryPoolManager;
	delete mentryPoolManager;

	DeleteCriticalSection(&listLock);
}

void Query::Init()
{
	entryPoolManager = new MemPooler<ProcessCounterEntry>(1000);
	if (!entryPoolManager)
	{
		ERROR_PRINT("[Query] MemPooler<ProcessCounterEntry> error\n");
		/* error handling */
		return;
	}

	mentryPoolManager = new MemPooler<MachineCounterEntry>(1000);
	if (!mentryPoolManager)
	{
		ERROR_PRINT("[Query] MemPooler<MachineCounterEntry> error\n");
		/* error handling */
		return;
	}


	event_ = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (NULL == event_)
	{
		PRINT("[Query] CreateEvent Error!\n");
	}

	helper.Init();
}

bool Query::AddCounter(std::string counter, bool isMachine)
{
	EnterCriticalSection(&listLock);
	if (isMachine)
	{
		for (std::string pc : machineCounterList)
		{
			if (pc == counter){

				LeaveCriticalSection(&listLock);
				return false;
			}
		}
		machineCounterList.push_back(counter);

		LeaveCriticalSection(&listLock);
		return true;
	}
	else
	{
		for (std::string pc : counterList)
		{
			if (pc == counter){
				LeaveCriticalSection(&listLock);
				return false;
			}
		}
		counterList.push_back(counter);
		LeaveCriticalSection(&listLock);
		return true;
	}
	
}

bool Query::DeleteCounter(std::string counter, bool isMachine)
{
	EnterCriticalSection(&listLock);
	if (isMachine) machineCounterList.remove(counter);
	else counterList.remove(counter);
	LeaveCriticalSection(&listLock);
	return true;
}


bool Query::AddProcess(std::string processName)
{
	EnterCriticalSection(&listLock);
	for (ProcessInfo_Agent checkProcess : checkProcessList)
	{
		if (processName == checkProcess.name)
		{
			LeaveCriticalSection(&listLock);
			return false;
		}
	}

	ProcessInfo_Agent p;
	p.name = processName;
	checkProcessList.push_back(p);

	LeaveCriticalSection(&listLock);
	return true;
}

bool Query::DeleteProcess(std::string processName)
{
	EnterCriticalSection(&listLock);
	std::list<ProcessInfo_Agent>::iterator iter;
	for (iter = checkProcessList.begin(); iter != checkProcessList.end(); iter++)
	{
		if (processName == (*iter).name)
		{
			checkProcessList.erase(iter);
			LeaveCriticalSection(&listLock);
			return true;
		}
	}
	LeaveCriticalSection(&listLock);
	return false;
}

bool Query::IsCheckProcess(std::string processName)
{
	for (ProcessInfo_Agent checkProcess : checkProcessList)
	{
		if (processName == checkProcess.name)
		{
			return true;
		}
	}

	return false;
}

void Query::ClearLogList(bool isMachine)
{
	if (isMachine)
	{
		for (MachineCounterEntry* p : machineLogList)
		{
			p->logs.clear();
			if (!mentryPoolManager->Free(p)) PRINT("[Query] Free Error!\n");
		}
		machineLogList.clear();
	}
	else
	{
		for (ProcessCounterEntry* p : processLogList)
		{
			p->logs.clear();
			if (!entryPoolManager->Free(p)) PRINT("[Query] Free Error!\n");
		}
		processLogList.clear();
	}
}

bool Query::InitCounterInfo(PDH_HQUERY& query, bool isMachine)
{
	PDH_STATUS status;
	ClearLogList(isMachine);

	PRINT("[Query] Update Counter Infos...\n");

	EnterCriticalSection(&listLock);
	if (!isMachine)
	{
		helper.UpdateProcessList();
		int i = 0;

		status = PdhAddCounter(query, cpuCounter.c_str(), 0, &cpuCounterV);
		if (status != ERROR_SUCCESS)
		{
			LeaveCriticalSection(&listLock);
			return false;
		}

		for (ProcessInfo p : helper.processList)
		{
			if (!IsCheckProcess(p.name)) continue;

			for (std::string counterName : counterList)
			{
				ProcessCounterEntry* pce = entryPoolManager->Alloc();
				assert(pce);

				pce->counterName = counterName;

				if (p.number == 0) counterName = "\\Process(" + p.name + ")\\" + counterName;
				else counterName = "\\Process(" + p.name + "#" + std::to_string(p.number) + ")\\" + counterName;

				status = PdhAddCounter(query, counterName.c_str(), 0, &pce->counter);
				if (status != ERROR_SUCCESS)
				{
					LeaveCriticalSection(&listLock);
					return false;
				}

				pce->processName = p.name;
				pce->processID = p.processID;
				pce->logs.clear();
				processLogList.push_back(pce);
			}
		}
	}
	else
	{
		for (std::string counterName : machineCounterList)
		{
			MachineCounterEntry* mce = mentryPoolManager->Alloc();
			assert(mce);

			status = PdhAddCounter(query, counterName.c_str(), 0, &mce->counter);

			if (status != ERROR_SUCCESS)
			{
				LeaveCriticalSection(&listLock);
				return false;
			}

			mce->counterName = counterName;
			mce->logs.clear();
			machineLogList.push_back(mce);
		}
	}
	PRINT("[Query] Done!\n");

	LeaveCriticalSection(&listLock);
	return true;
}
char* Query::GetCurTime()
{
	struct tm t;

	time_t timer = time(NULL); // 현재 시각을 초 단위로 얻기
	localtime_s(&t, &timer); // 초 단위의 시간을 분리하여 구조체에 넣기

	static char s[20];

	sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d",
		t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
		t.tm_hour, t.tm_min, t.tm_sec
		);

	return s;
}

bool Query::StartRecord(bool isMachine, int totalTime, int recordTime, int interval,
	long long int delay)
{
	if (isMachine && isMachineRecordEnd)
	{
		isMachineRecordEnd = false;

		machineRecorder = std::thread(&Query::Record, this, isMachine, totalTime, recordTime, interval, delay);
		machineRecorder.detach();

		return true;
	}
	if (!isMachine && isProcessRecordEnd)
	{
		isProcessRecordEnd = false;

		processRecorder = std::thread(&Query::Record, this, isMachine, totalTime, recordTime, interval, delay);
		processRecorder.detach();

		return true;
	}

	return false;
}

bool Query::StopRecord(bool isMachine)
{
	if (isMachine)
	{
		if (isMachineRecordEnd) return false;
		else
		{
			isMachineRecordEnd = true;
			return true;
		}
	}
	else
	{
		if (isProcessRecordEnd) return false;
		else
		{
			isProcessRecordEnd = true;
			return true;
		}
	}

}

bool hasEnding(std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}

bool Query::Record(bool isMachine, int totalTime, int recordTime, int interval,
	long long int delay)
{
	PRINT("[Query] Record Delay : %lld ...\n", delay);
	std::this_thread::sleep_for(std::chrono::seconds(delay));

	int time_ = 0;

	while ((totalTime > time_) || (totalTime < 0)){
		if (isMachine && isMachineRecordEnd) break;
		if (!isMachine && isProcessRecordEnd) break;

		PDH_HQUERY query;
		PDH_STATUS status;

		status = PdhOpenQuery(NULL, 0, &query);

		if (ERROR_SUCCESS != status)
		{
			PRINT("[Query] PdhOpenQuery Error!\n");
			return false;
		}

		if (!InitCounterInfo(query, isMachine)){
			PRINT("[Query] PdhAddCounter Error!\n");
			ClearLogList(isMachine);
			return false;
		}

		do
		{
			ULONG CounterType;
			ULONG WaitResult;
			PDH_FMT_COUNTERVALUE DisplayValue;

			status = PdhCollectQueryData(query);

			if (status != ERROR_SUCCESS)
			{
				PRINT("[Query] PdhCollectQueryData Error!\n");
				return false;
			}

			status = PdhCollectQueryDataEx(query, interval, event_);
			if (status != ERROR_SUCCESS)
			{
				PRINT("[Query] PdhCollectQueryDataEx Error!\n");
				return false;
			}

			WaitResult = WaitForSingleObject(event_, INFINITE);

			if (WaitResult == WAIT_OBJECT_0)
			{
				if (!isMachine)
				{
					PDH_FMT_COUNTERVALUE CpuValue;
					status = PdhGetFormattedCounterValue(cpuCounterV, PDH_FMT_DOUBLE, &CounterType, &CpuValue);
					if (status != ERROR_SUCCESS)
					{
						PRINT("[Query] PdhGetFormattedCounterValue Error!\n");
						continue;
					}


					for (ProcessCounterEntry* it : processLogList)
					{
						status = PdhGetFormattedCounterValue(it->counter, PDH_FMT_DOUBLE, &CounterType, &DisplayValue);
						if (status != ERROR_SUCCESS)
						{
							PRINT("[Query] PdhGetFormattedCounterValue Error!\n");
							continue;
						}

						time_t timer = time(NULL);
						Log log;
						log.timestamp = (long long int) timer;
						log.value = DisplayValue.doubleValue;

						if (hasEnding(it->counterName, std::string("% Processor Time"))) log.value *= (CpuValue.doubleValue/100);
						it->logs.push_back(log);

						PRINT("[%s] %s[%d] %s: %f\n", GetCurTime(), it->processName.c_str(), it->processID, it->counterName.c_str(), log.value);
					}
				}
				else
				{
					for (MachineCounterEntry* it : machineLogList)
					{
						status = PdhGetFormattedCounterValue(it->counter, PDH_FMT_DOUBLE, &CounterType, &DisplayValue);
						if (status != ERROR_SUCCESS)
						{
							PRINT("[Query] PdhGetFormattedCounterValue Error!\n");
							continue;
						}

						time_t timer = time(NULL);
						Log log;
						log.timestamp = (long long int) timer;
						log.value = DisplayValue.doubleValue;
						it->logs.push_back(log);

						PRINT("[%s] Total_ %s: %f\n", GetCurTime(), it->counterName.c_str(), log.value);
					}
				}
			}
			time_ += interval;
		} while (time_ % recordTime != 0);

		status = PdhCloseQuery(query);

		if (ERROR_SUCCESS != status)
		{
			PRINT("[Query] PdhCloseQuery Error!\n");
			return false;
		}

		if (agentClientApp){
			if (!isMachine) agentClientApp->agentClient->socket->SendProcessInfo();
			else agentClientApp->agentClient->socket->SendMachineInfo();
		}

		if (!agentClientApp->agentClient->socket->isConnect) break;
	}

	if (isMachine) isMachineRecordEnd = true;
	else isProcessRecordEnd = true;

	PRINT("[Query] Record Done!\n");
	return true;
}