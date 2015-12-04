#include "stdafx.h"

Query::Query(const int& maxLogCnt)
:maxLogCnt(maxLogCnt)
{

}

Query::~Query()
{

}

void Query::Init()
{
	entryPoolManager = new MemPooler<ProcessCounterEntry>(10);
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

void Query::AddCounter(agent::ProcessCounter counter)
{
	counterList.push_back(counter);
}

void Query::DeleteCounter(agent::ProcessCounter counter)
{
	counterList.remove(counter);
}

bool Query::IsCheckProcess(std::string processName)
{
	for (ProcessInfo_Agent checkProcess : checkProcessList)
	{
		if (processName == checkProcess.name)
		{
			checkProcess.isOn = true;
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
	ClearLogList();
	helper.UpdateProcessList();

	PRINT("[Query] Update Counter Infos...\n");

	for (ProcessInfo p : helper.processList)
	{
		if (!IsCheckProcess(p.name)) continue;

		for (agent::ProcessCounter counter : counterList)
		{
			std::string counterName = "";

			switch (counter)
			{
			case agent::ProcessCounter::IO_DATA_BYTES_SEC:
				counterName = "IO Data Bytes/sec";
				break;
			case agent::ProcessCounter::KERNEL_TIME:
				counterName = "% Privileged Time";
				break;
			case agent::ProcessCounter::THREAD_COUNT:
				counterName = "Thread Count";
				break;
			case agent::ProcessCounter::TOTAL_CPU_TIME:
				counterName = "% Processor Time";
				break;
			case agent::ProcessCounter::USER_TIME:
				counterName = "% User Time";
				break;
			case agent::ProcessCounter::VIRTUAL_BYTES:
				counterName = "Virtual Bytes";
				break;
			case agent::ProcessCounter::WORKING_SET_PRIVATE:
				counterName = "Working Set - Private";
				break;
			}

			PDH_STATUS status;
			ProcessCounterEntry* pce = entryPoolManager->Alloc();

			if (p.number == 0) counterName = "\\Process(" + p.name + ")\\" + counterName;
			else counterName = "\\Process(" + p.name + "#" + std::to_string(p.number) + ")\\" + counterName;

			status = PdhAddCounter(query, counterName.c_str(), 0, &pce->counter);
			
			if (status != ERROR_SUCCESS)
			{
				return false;
			}
			
			pce->processName = p.name;
			pce->processID = p.processID;
		}
	}

	PRINT("[Query] Done!\n");

	return true;
}


bool Query::Record(int recordTime, int interval)
{
	PDH_HQUERY query;
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
				log.time = time;
				log.value = DisplayValue.doubleValue;
				it->logs.push_back(log);
			}
		}
		time++;
	}
	
	return true;
}