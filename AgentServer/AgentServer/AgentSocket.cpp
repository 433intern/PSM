#include "stdafx.h"

extern AgentApp* agentApp;

AgentSocket::AgentSocket()
: token(-1), agentID(-1), hostIP(-1), position(0), remainBytes(HEADER_SIZE), healthCheck(true), isReady(false)
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
			msg->ownert = CPacket::ownerType::AGENT;
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
	isReady = false;

	agentID = -1;
	hostIP = -1;
	token = -1;

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

	agentApp->agentServer->redisManager.ChangeAgentState_isOn(token, false);
	agentApp->agentServer->redisManager.ChangeAgentState_stopRecording(token);
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

int AgentSocket::FindAgentID(int token)
{
	return agentApp->agentServer->redisManager.GetAgentID(token);
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
				if (agentApp->agentServer->GetAgentSocketByToken(msg.token()) != NULL){
					PRINT("[AgentSocket] token : %d already running!!\n", msg.token());
					SendAgentIDResponse(-1, true);
					break;
				}

				token = msg.token();
				hostIP = msg.hostip();
				agentApp->agentServer->redisManager.InitAgent(msg.token(), msg.hostip(), msg.ramsize());
				this->agentID = FindAgentID(msg.token());
				PRINT("[AgentSocket] hostID : %d AgentID : %d\n", msg.hostip(), agentID);
				SendAgentIDResponse(agentID, false);
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
			PRINT("[AgentSocket] CurrentProcessListSend received\n");
			if (agentApp->agentServer->redisManager.SaveCurrentProcessList(agentID, packet))
			{
				PRINT("[AgentSocket] Update process list %d\n", agentID);
			}
			break;
		}
		case agent::RecordResponse:
		{
			PRINT("[AgentSocket] RecordResponse received\n");
			agent::csRecordResponse msg;
			if (msg.ParseFromArray(packet->msg, packet->length))
			{
				/* result msg print */
			}
			break;
		}
		case agent::AgentReady:
		{
			PRINT("[AgentSocket] AgentReady received\n");

			agentApp->agentServer->redisManager.ChangeAgentState_startRecording(token, -1, 10, 1, 0);
			SendStartRecord(true, -1, 10, 1, 0);
			SendStartRecord(false, -1, 10, 1, 0);
			break;
		}
		case agent::ProcessInfoSend:
		{
			PRINT("[AgentSocket] ProcessInfoSend received\n");
			if (!agentApp->agentServer->redisManager.SaveProcessInfo(agentID, packet))
			{

			}
			break;
		}
		case agent::MachineInfoSend:
		{
			PRINT("[AgentSocket] MachineInfoSend received\n");
			if (!agentApp->agentServer->redisManager.SaveMachineInfo(agentID, packet))
			{

			}
			break;
		}

		case agent::HealthAck:
#ifdef HEARTBEAT
			PRINT("[AgentSocket] HealthAck received\n");
#endif
			healthCheck = true;
			break;

		default:
			PRINT("[AgentSocket] Invalid Socket\n");
			break;
	}

	if (!packetPoolManager->Free(packet)) ERROR_PRINT("[AgentSocket] free error!\n");
}

void AgentSocket::AddProcessName(std::string& processName)
{
	PRINT("[AgentSocket] AddProcessName Success\n");
	SendProcessCommand(agent::ProcessCommandType::ADDLIST, processName);
}

void AgentSocket::DeleteProcessName(std::string& processName)
{
	PRINT("[AgentSocket] DeleteProcessName Success\n");
	SendProcessCommand(agent::ProcessCommandType::DELETELIST, processName);
}

void AgentSocket::AddCounterName(std::string& counterName, bool isMachine)
{
	PRINT("[AgentSocket] AddCounterName Success\n");
	SendCounterCommand(agent::CounterCommandType::CADDLIST, counterName, isMachine);
}

void AgentSocket::DeleteCounterName(std::string& counterName, bool isMachine)
{
	PRINT("[AgentSocket] DeleteCounterName Success\n");
	SendCounterCommand(agent::CounterCommandType::CDELETELIST, counterName, isMachine);
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

void AgentSocket::SendAgentIDResponse(int agentID, bool alreadyRunning)
{
	PRINT("[AgentSocket] SendAgentIDResponse\n");
	CPacket packet;
	agent::scAgentIDResponse msg;

	msg.set_agentid(agentID);
	msg.set_alreadyrunning(alreadyRunning);

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

	if (agentApp->agentServer->redisManager.GetProcessList(agentID, processList))
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

	if (agentApp->agentServer->redisManager.GetCounterList(agentID, counterList, isMachine))
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

void AgentSocket::SendStartRecord(bool isMachine, int totalRecordTime, int responseTime, int interval, long long int delay)
{
	PRINT("[AgentSocket] SendProcessStartRecord\n");
	CPacket packet;
	agent::scStartRecord msg;

	msg.set_ismachine(isMachine);
	msg.set_totalrecordtime(totalRecordTime);
	msg.set_interval(interval);
	msg.set_responsetime(responseTime);
	msg.set_delay(delay);

	packet.length = (short)msg.ByteSize();
	packet.type = (short)agent::StartRecord;
	memset(packet.msg, 0, BUFSIZE);
	msg.SerializeToArray((void *)&packet.msg, packet.length);

	Send((char *)&packet, packet.length + HEADER_SIZE);
}

void AgentSocket::SendStopRecord(bool isMachine)
{
	PRINT("[AgentSocket] SendProcessStopRecord\n");
	CPacket packet;
	agent::scStopRecord msg;

	msg.set_ismachine(isMachine);

	packet.length = (short)msg.ByteSize();
	packet.type = (short)agent::StopRecord;
	memset(packet.msg, 0, BUFSIZE);
	msg.SerializeToArray((void *)&packet.msg, packet.length);

	Send((char *)&packet, packet.length + HEADER_SIZE);
}

void AgentSocket::SendProcessCommand(agent::ProcessCommandType type, std::string& processName)
{
	PRINT("[AgentSocket] SendProcessCommand\n");
	CPacket packet;
	agent::scProcessCommandRequest msg;

	msg.set_type(type);
	msg.set_processname(processName);

	packet.length = (short)msg.ByteSize();
	packet.type = (short)agent::ProcessCommandRequest;
	memset(packet.msg, 0, BUFSIZE);
	msg.SerializeToArray((void *)&packet.msg, packet.length);

	Send((char *)&packet, packet.length + HEADER_SIZE);
}

void AgentSocket::SendCounterCommand(agent::CounterCommandType type, std::string& counterName, bool isMachine)
{
	PRINT("[AgentSocket] SendCounterCommand\n");
	CPacket packet;
	agent::scCounterCommandRequest msg;

	msg.set_ismachine(isMachine);
	msg.set_type(type);
	msg.set_countername(counterName);

	packet.length = (short)msg.ByteSize();
	packet.type = (short)agent::CounterCommandRequest;
	memset(packet.msg, 0, BUFSIZE);
	msg.SerializeToArray((void *)&packet.msg, packet.length);

	Send((char *)&packet, packet.length + HEADER_SIZE);
}