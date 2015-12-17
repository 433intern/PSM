#include "stdafx.h"

LogicHandle::LogicHandle()
{
	isEnd = false;
	InitializeCriticalSection(&operationLock);
}
LogicHandle::~LogicHandle()
{
	DeleteCriticalSection(&operationLock);
}

void LogicHandle::EnqueueOper(CPacket* msg)
{
	EnterCriticalSection(&operationLock);
	operationQueue.push(msg);
	LeaveCriticalSection(&operationLock);
}

void LogicHandle::Process()
{
	while (!isEnd)
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
}
