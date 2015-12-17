#include "stdafx.h"

extern AgentApp* agentApp;

WebCommandSocket::WebCommandSocket()
: token(-1), position(0), remainBytes(HEADER_SIZE), healthCheck(true)
{
	packetPoolManager = new MemPooler<CPacket>(10);
	if (!packetPoolManager)
	{
		PRINT("[WebCommandSocket] MemPooler<CPacket> error\n");
		/* error handling */
		return;
	}
}

WebCommandSocket::~WebCommandSocket()
{
	delete packetPoolManager;
}

void WebCommandSocket::RecvProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[WebCommandSocket] RecvProcess Error : %d\n", WSAGetLastError);
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
		ERROR_PRINT("[WebCommandSocket] RecvProcess : recv buf socket is not available\n");
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

void WebCommandSocket::SendProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[WebCommandSocket] SendProcess : Error : %d\n", WSAGetLastError());
		return;
	}
}

void WebCommandSocket::AcceptProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[WebCommandSocket] AcceptProcess : Error : %d\n", WSAGetLastError());
		return;
	}
	PRINT("[WebCommandSocket] connect success, %d\n", this->socket_);
	agentApp->webCommandServer->AddSocket(this);

	healthCheck = true;
	position = 0;
	remainBytes = HEADER_SIZE;
	token = -1;

	Recv(recvBuf_, HEADER_SIZE);
}

void WebCommandSocket::DisconnProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[WebCommandSocket] DisconnProcess : Error : %d\n", WSAGetLastError());
		return;
	}

	PRINT("[WebCommandSocket] disconnect success, %d\n", this->socket_);

	agentApp->webCommandServer->DeleteSocket(this);
}

void WebCommandSocket::ConnProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[WebCommandSocket] ConnProcess : Error : %d\n", WSAGetLastError());
		return;
	}
}


bool WebCommandSocket::ValidPacket(CPacket *packet)
{
	return true;
}

void WebCommandSocket::PacketHandling(CPacket *packet)
{
	assert(socket_);
	if (!ValidPacket(packet))
	{
		ERROR_PRINT("[WebCommandSocket] Not Valid Packet!\n");
	}

	switch (packet->type)
	{
	default:
		PRINT("[WebCommandSocket] Invalid Socket\n");
		break;
	}

	if (!packetPoolManager->Free(packet)) ERROR_PRINT("[WebCommandSocket] free error!\n");
}

void WebCommandSocket::SendHealthCheck()
{
	PRINT("[WebCommandSocket] SendHealthCheck\n");
	CPacket packet;
	psmweb::wsHealthCheck msg;

	packet.length = (short)msg.ByteSize();
	packet.type = (short)psmweb::HealthCheck;
	memset(packet.msg, 0, BUFSIZE);
	msg.SerializeToArray((void *)&packet.msg, packet.length);

	Send((char *)&packet, packet.length + HEADER_SIZE);
}