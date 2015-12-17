#include "stdafx.h"

LogicHandle::LogicHandle()
{
	isEnd = false;
	InitializeCriticalSection(&operationLock);

	event_ = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (NULL == event_)
	{
		PRINT("[LogicHandle] CreateEvent Error!\n");
	}
}
LogicHandle::~LogicHandle()
{
	DeleteCriticalSection(&operationLock);
	CloseHandle(event_);
}

void LogicHandle::EnqueueOper(CPacket* msg)
{
	EnterCriticalSection(&operationLock);
	operationQueue.push(msg);
	LeaveCriticalSection(&operationLock);

	SetEvent(event_);
}

void LogicHandle::Process()
{
	while (!isEnd)
	{
		DWORD ret = WaitForSingleObject(event_, INFINITE);
		if (ret == WAIT_OBJECT_0)
		{
			if (operationQueue.size() <= 0)
			{
				continue;
			}
			CPacket* packet = nullptr;

			EnterCriticalSection(&operationLock);
			if (operationQueue.size() > 0)
			{
				packet = operationQueue.front();
				operationQueue.pop();
			}
			LeaveCriticalSection(&operationLock);

			if (packet != nullptr)
			{
				if (packet->ownert == CPacket::ownerType::AGENT){
					AgentSocket *socket = static_cast<AgentSocket *>(packet->owner);
					socket->PacketHandling(packet);
				}
				else if (packet->ownert == CPacket::ownerType::WEBCOMMAND){
					WebCommandSocket* socket = static_cast<WebCommandSocket *>(packet->owner);
					socket->PacketHandling(packet);
				}

			}
		}
		else
		{
			PRINT("[LogicHandle] wait event Error!\n");
		}
	}
}
