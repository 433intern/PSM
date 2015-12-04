#include "stdafx.h"

extern AgentApp* agentApp;

AgentSocket::AgentSocket(int agentID)
: agentID(agentID), position(0), remainBytes(HEADER_SIZE), healthCheck(true)
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


void AgentSocket::PacketHandling(CPacket *packet)
{
	assert(socket_);
	if (!ValidPacket(packet))
	{
		ERROR_PRINT("[AgentSocket] Not Valid Packet!\n");
	}

	switch (packet->type)
	{
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