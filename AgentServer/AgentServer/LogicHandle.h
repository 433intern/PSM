#pragma once

class LogicHandle
{
	std::queue<CPacket *> operationQueue;
	CRITICAL_SECTION operationLock;
	HANDLE event_;

public:
	LogicHandle();
	~LogicHandle();

	void EnqueueOper(CPacket* msg);
	void Process();

	bool isEnd;
};

