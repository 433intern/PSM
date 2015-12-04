#include "stdafx.h"

extern AgentClientApp* agentClientApp;

AgentClientSocket::AgentClientSocket()
: position(0), remainBytes(HEADER_SIZE)
{
	packetPoolManager = new MemPooler<CPacket>(10);
	if (!packetPoolManager)
	{
		ERROR_PRINT("[AgentClientSocket] MemPooler<CPacket> error\n");
		/* error handling */
		return;
	}

	if (!LoadMswsock())
	{
		ERROR_PRINT("[AgentClientSocket] Error loading mswsock functions: %d\n", WSAGetLastError());
		return;
	}
}

BOOL AgentClientSocket::LoadMswsock(void){
	SOCKET sock;
	DWORD dwBytes;
	int rc;

	/* Dummy socket needed for WSAIoctl */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		return FALSE;
	{
		GUID guid = WSAID_CONNECTEX;
		rc = WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guid, sizeof(guid),
			&mswsock.ConnectEx, sizeof(mswsock.ConnectEx),
			&dwBytes, NULL, NULL);
		if (rc != 0)
			return FALSE;
	}

	rc = closesocket(sock);
	if (rc != 0)
		return FALSE;

	return TRUE;
}

AgentClientSocket::~AgentClientSocket()
{
	delete packetPoolManager;
}

void AgentClientSocket::Connect(unsigned int ip, WORD port){
	sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);

	int ok = mswsock.ConnectEx(socket_, (SOCKADDR*)&addr, sizeof(addr), NULL, 0, NULL,
		static_cast<OVERLAPPED*>(&act_[TcpSocket::ACT_CONNECT]));
	
	if (ok)
	{
		PRINT("[AgentClientSocket] ConnectEx succeeded immediately\n");
		ConnProcess(false, NULL, 0);
	}

	int error = WSAGetLastError();
	if (ok == FALSE && WSAGetLastError() != ERROR_IO_PENDING)
	{
		ERROR_PRINT("[AgentClientSocket] ConnectEx Error!!! s(%d), err(%d)\n", socket_, error);
	}
	PRINT("[AgentClientSocket] Connect Request..\n");
}

void AgentClientSocket::Bind()
{
	int rc;
	struct sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = 0;
	rc = bind(socket_, (SOCKADDR*)&addr, sizeof(addr));

	if (rc != 0)
	{
		PRINT("[AgentClientSocket] bind failed: %d\n", WSAGetLastError());
		return;
	}
}


void AgentClientSocket::RecvProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[AgentClientSocket] RecvProcess Error : %d\n", WSAGetLastError);
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
		ERROR_PRINT("[AgentClientSocket] RecvProcess : recv buf socket is not available\n");
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
			agentClientApp->logicHandle.EnqueueOper(msg);
		}
	}
	Recv(buf + position, remainBytes);
}

void AgentClientSocket::SendProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[AgentClientSocket] SendProcess : Error : %d\n", WSAGetLastError());
		return;
	}
}

void AgentClientSocket::AcceptProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[AgentClientSocket] AcceptProcess : Error : %d\n", WSAGetLastError());
		return;
	}
}

void AgentClientSocket::DisconnProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[AgentClientSocket] DisconnProcess : Error : %d\n", WSAGetLastError());
		return;
	}


	PRINT("Disconnect Success\n");
}

void AgentClientSocket::ConnProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (isError)
	{
		ERROR_PRINT("[AgentClientSocket] ConnProcess : Error : %d\n", WSAGetLastError());
		return;
	}

	PRINT("Connect Success\n");

	Recv(this->recvBuf_, HEADER_SIZE);
}


bool AgentClientSocket::ValidPacket(CPacket *packet)
{
	return true;
}


void AgentClientSocket::PacketHandling(CPacket *packet)
{
	assert(socket_);
	if (!ValidPacket(packet))
	{
		ERROR_PRINT("[AgentClientSocket] Not Valid Packet!\n");
	}

	switch (packet->type)
	{
	case agent::HealthCheck:
			PRINT("[AgentClientSocket] HealthCheck received\n");
			SendHealthAck();
			break;
	}

	if (!packetPoolManager->Free(packet)) ERROR_PRINT("[AgentSocket] free error!\n");
}

void AgentClientSocket::SendHealthAck()
{
	CPacket packet;
	agent::csHealthAck msg;
	
	packet.length = (short)msg.ByteSize();
	packet.type = (short)agent::HealthAck;
	msg.SerializeToArray((void *)&packet.msg, packet.length);

	Send((char *)&packet, packet.length + HEADER_SIZE);
}