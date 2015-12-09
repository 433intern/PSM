#include "stdafx.h"

#include <time.h>

Query::Query()
{

}

Query::~Query()
{
	for (ProcessCounterEntry* p : processLogList) entryPoolManager->Free(p);
	delete entryPoolManager;
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


	event_ = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (NULL == event_)
	{
		PRINT("[Query] CreateEvent Error!\n");
	}

	helper.Init();
}

bool Query::AddCounter(std::string counter)
{
	for (std::string pc : counterList)
	{
		if (pc == counter) return false;
	}
	counterList.push_back(counter);
	return true;
}

bool Query::DeleteCounter(std::string counter)
{
	counterList.remove(counter);
	return true;
}


bool Query::AddProcess(std::string processName)
{
	for (ProcessInfo_Agent checkProcess : checkProcessList)
	{
		if (processName == checkProcess.name)
		{
			return false;
		}
	}

	ProcessInfo_Agent p;
	p.name = processName;
	checkProcessList.push_back(p);

	return true;
}

bool Query::DeleteProcess(std::string processName)
{
	std::list<ProcessInfo_Agent>::iterator iter;
	for (iter = checkProcessList.begin(); iter != checkProcessList.end(); iter++)
	{
		if (processName == (*iter).name)
		{
			checkProcessList.erase(iter);
			return true;
		}
	}
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

void Query::ClearLogList()
{
	for (ProcessCounterEntry* p : processLogList)
	{
		p->logs.clear();
		if (!entryPoolManager->Free(p)) PRINT("[Query] Free Error!\n");
	}

	processLogList.clear();
}

bool Query::InitCounterInfo(PDH_HQUERY& query)
{
	PDH_STATUS status;
	ClearLogList();
	helper.UpdateProcessList();

	PRINT("[Query] Update Counter Infos...\n");

	int i = 0;

	for (ProcessInfo p : helper.processList)
	{
		if (!IsCheckProcess(p.name)) continue;

		for (std::string counterName : counterList)
		{
			ProcessCounterEntry* pce = entryPoolManager->Alloc();
			assert(pce);

			if (p.number == 0) counterName = "\\Process(" + p.name + ")\\" + counterName;
			else counterName = "\\Process(" + p.name + "#" + std::to_string(p.number) + ")\\" + counterName;

			status = PdhAddCounter(query, counterName.c_str(), 0, &pce->counter);
			
			
			if (status != ERROR_SUCCESS)
			{
				return false;
			}
			
			pce->processName = p.name;
			pce->processID = p.processID;

			processLogList.push_back(pce);

		}
	}

	PRINT("[Query] Done!\n");

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

bool Query::Record(int recordTime, int interval)
{
	
	PDH_STATUS status;

	status = PdhOpenQuery(NULL, 0, &query);

	if (ERROR_SUCCESS != status)
	{
		PRINT("[Query] PdhOpenQuery Error!\n");
		return false;
	}

	if (!InitCounterInfo(query)){
		PRINT("[Query] PdhAddCounter Error!\n");
		ClearLogList();
		return false;
	}

	
	int time = 0;
	while (recordTime > time)
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
			for (ProcessCounterEntry* it : processLogList)
			{
				status = PdhGetFormattedCounterValue(it->counter, PDH_FMT_DOUBLE, &CounterType, &DisplayValue);
				if (status != ERROR_SUCCESS)
				{
					PRINT("[Query] PdhGetFormattedCounterValue Error!\n");
					return false;
				}

				Log log;
				log.timestamp = std::string(GetCurTime());
				log.time = time;
				log.value = DisplayValue.doubleValue;
				it->logs.push_back(log);

				PRINT("[%s] %s[%d] : %f\n", GetCurTime(), it->processName.c_str(), it->processID, log.value/1024);
			}
		}
		time++;
	}
	
	return true;
}