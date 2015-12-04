#pragma once

class LogicHandle
{
	std::queue<CPacket *> operationQueue;
	CRITICAL_SECTION operationLock;
public:
	LogicHandle();
	~LogicHandle();

	void EnqueueOper(CPacket* msg);
	void Process();

	bool isEnd;
};

