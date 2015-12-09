#include "stdafx.h"

extern AgentApp* agentApp;

AgentSocket::AgentSocket()
: agentID(-1), position(0), remainBytes(HEADER_SIZE), healthCheck(true)
{
	packetPoolManager = new MemPooler<CPacket>(10);
	if (!packetPoolManager)
	{
		PRINT("[AgentSocket] MemPooler<CPacket> error\n");
		/* error handling */
		return;
	}
}

AgentSocket::~AgentSocket()
{
	delete packetPoolManager;
}

void AgentSocket::RecvProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[AgentSocket] RecvProcess Error : %d\n", WSAGetLastError);
		Disconnect();
		return;
	}

	if (bytes_transferred == 0)
	{
		Disconnect();
		return;
	}

	this->position += bytes_transferred;
	this->remainBytes -= bytes_transferred;

	if (this->socket_ == NULL){
		ERROR_PRINT("[AgentSocket] RecvProcess : recv buf socket is not available\n");
		return;
	}

	char* buf = this->recvBuf_;

	if (this->position < HEADER_SIZE)
	{
		if (position == 0) remainBytes = HEADER_SIZE;
	}
	else
	{
		if (position == HEADER_SIZE && remainBytes == 0)
			memcpy(&remainBytes, buf, LENGTH_SIZE);

		if (remainBytes == 0)
		{
			short length;
			memcpy(&length, buf, LENGTH_SIZE);

			position = 0;
			remainBytes = HEADER_SIZE;

			CPacket* msg = packetPoolManager->Alloc();

			memcpy(msg, buf, HEADER_SIZE + length);
			msg->owner = this;
			/* packet Handling */
			agentApp->logicHandle.EnqueueOper(msg);
		}
	}
	Recv(buf + position, remainBytes);
}

void AgentSocket::SendProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[AgentSocket] SendProcess : Error : %d\n", WSAGetLastError());
		return;
	}
}

void AgentSocket::AcceptProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[AgentSocket] AcceptProcess : Error : %d\n", WSAGetLastError());
		return;
	}
	PRINT("[AgentSocket] connect success, %d\n", this->socket_);
	agentApp->agentServer->AddAgent(this);

	healthCheck = true;
	position = 0;
	remainBytes = HEADER_SIZE;

	Recv(recvBuf_, HEADER_SIZE);
}

void AgentSocket::DisconnProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[AgentSocket] DisconnProcess : Error : %d\n", WSAGetLastError());
		return;
	}

	PRINT("[AgentSocket] disconnect success, %d\n", this->socket_);
	agentApp->agentServer->DeleteAgent(this);
}

void AgentSocket::ConnProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[AgentSocket] ConnProcess : Error : %d\n", WSAGetLastError());
		return;
	}
}


bool AgentSocket::ValidPacket(CPacket *packet)
{
	return true;
}

int AgentSocket::FindAgentID(int hostip)
{
	return agentApp->redisManager.GetAgentID(hostip);
}

void AgentSocket::PacketHandling(CPacket *packet)
{
	assert(socket_);
	if (!ValidPacket(packet))
	{
		ERROR_PRINT("[AgentSocket] Not Valid Packet!\n");
	}

	switch (packet->type)
	{
		case agent::AgentIDRequest:
		{
			PRINT("[AgentSocket] AgentIDRequest received\n");
			agent::csAgentIDRequest msg;
			if (msg.ParseFromArray(packet->msg, packet->length))
			{
				this->agentID = FindAgentID(msg.hostip());
				PRINT("[AgentSocket] hostID : %d\n", msg.hostip());
				SendAgentIDResponse(agentID);
			}
			break;
		}
		case agent::ProcessListRequest:
		{
			PRINT("[AgentSocket] ProcessListRequest received\n");
			SendProcessListResponse();
			break;
		}
		case agent::CounterListRequest:
		{
			agent::csCounterListRequest msg;
			PRINT("[AgentSocket] CounterListRequest received\n");
			if (msg.ParseFromArray(packet->msg, packet->length))
			{
				SendCounterListResponse(msg.ismachine());
			}
			break;
		}
		case agent::CurrentProcessListSend:
		{
		    agent::csCurrentProcessListSend msg;
			PRINT("[AgentSocket] CurrentProcessListSend received\n");
			if (msg.ParseFromArray(packet->msg, packet->length))
			{
				PRINT("[AgentSocket] Current Process List Agent %d\n", agentID);
				for (int i = 0; i < msg.processinfo_size(); i++)
				{
					agent::CurrentProcess pi = msg.processinfo(i);
					
					PRINT("%s\n", pi.processname().c_str());
					for (int j = 0; j < pi.processid_size(); j++)
					{
						int pid = pi.processid(j);
						PRINT("%d ", pid);
					}
					PRINT("\n");
				}
			}
			break;
		}
		case agent::HealthAck:
#ifdef HEARTBEAT
			PRINT("[AgentSocket] HealthAck received\n");
#endif
			healthCheck = true;
			break;
	}

	if (!packetPoolManager->Free(packet)) ERROR_PRINT("[AgentSocket] free error!\n");
}

void AgentSocket::SendHealthCheck()
{
	PRINT("[AgentSocket] SendHealthCheck\n");
	CPacket packet;
	agent::scHealthCheck msg;
	
	packet.length = (short)msg.ByteSize();
	packet.type = (short)agent::HealthCheck;
	memset(packet.msg, 0, BUFSIZE);
	msg.SerializeToArray((void *)&packet.msg, packet.length);

	Send((char *)&packet, packet.length + HEADER_SIZE);
}

void AgentSocket::SendAgentIDResponse(int agentID)
{
	PRINT("[AgentSocket] SendAgentIDResponse\n");
	CPacket packet;
	agent::scAgentIDResponse msg;

	msg.set_agentid(agentID);

	packet.length = (short)msg.ByteSize();
	packet.type = (short)agent::AgentIDResponse;
	memset(packet.msg, 0, BUFSIZE);
	msg.SerializeToArray((void *)&packet.msg, packet.length);

	Send((char *)&packet, packet.length + HEADER_SIZE);
}

void AgentSocket::SendProcessListResponse()
{
	PRINT("[AgentSocket] SendProcessListResponse\n");
	CPacket packet;
	agent::scProcessListResponse msg;

	std::vector<std::string> processList;

	if (agentApp->redisManager.GetProcessList(agentID, processList))
	{
		for (std::string process : processList)
		{
			std::string* p = msg.add_processname();
			*p = process;
		}

		packet.length = (short)msg.ByteSize();
		packet.type = (short)agent::ProcessListResponse;
		memset(packet.msg, 0, BUFSIZE);
		msg.SerializeToArray((void *)&packet.msg, packet.length);

		Send((char *)&packet, packet.length + HEADER_SIZE);
	}
	else
	{
		ERROR_PRINT("[AgentSocket] SendProcessListResponse ERROR\n");
	}
}

void AgentSocket::SendCounterListResponse(bool isMachine)
{
	PRINT("[AgentSocket] SendCounterListResponse\n");
	CPacket packet;
	agent::scCounterListResponse msg;

	msg.set_ismachine(isMachine);

	std::vector<std::string> counterList;

	if (agentApp->redisManager.GetCounterList(agentID, counterList, isMachine))
	{
		for (std::string counter : counterList)
		{
			std::string* p = msg.add_countername();
			*p = counter;
		}

		packet.length = (short)msg.ByteSize();
		packet.type = (short)agent::CounterListResponse;
		memset(packet.msg, 0, BUFSIZE);
		msg.SerializeToArray((void *)&packet.msg, packet.length);

		Send((char *)&packet, packet.length + HEADER_SIZE);
	}
	else
	{
		ERROR_PRINT("[AgentSocket] SendCounterListResponse ERROR\n");
	}
}