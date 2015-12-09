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

	processQuery.Init();
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

	SendAgentIDRequest();
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
		case agent::AgentIDResponse:
		{
			PRINT("[AgentClientSocket] AgentIDResponse received\n");
			agent::scAgentIDResponse msg;
			if (msg.ParseFromArray(packet->msg, packet->length))
			{
				PRINT("[AgentClientSocket] AgentID : %d\n", msg.agentid());
				agentClientApp->agentID = msg.agentid();

				SendProcessListRequest();
			}
			
			break;
		}
		case agent::ProcessListResponse:
		{
			PRINT("[AgentClientSocket] ProcessListResponse received\n");
			agent::scProcessListResponse msg;
			if (msg.ParseFromArray(packet->msg, packet->length))
			{
				PRINT("[AgentClientSocket] ProcessList : %d\n", msg.processname_size());
				for (int i = 0; i < msg.processname_size(); i++)
				{
					PRINT("%s\n", msg.processname(i).c_str());
					processQuery.AddProcess(msg.processname(i));
				}
			}

			SendCounterListRequest();
			break;
		}
		case agent::CounterListResponse:
		{
			PRINT("[AgentClientSocket] CounterListResponse received\n");
			agent::scCounterListResponse msg;
			if (msg.ParseFromArray(packet->msg, packet->length))
			{
				PRINT("[AgentClientSocket] CounterList : %d\n", msg.countername_size());
				for (int i = 0; i < msg.countername_size(); i++)
				{
					PRINT("%s\n", msg.countername(i).c_str());
					processQuery.AddCounter(msg.countername(i));
				}
			}
			break;
		}
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

void AgentClientSocket::SendAgentIDRequest()
{
	PRINT("[AgentSocket] SendAgentIDRequest\n");
	CPacket packet;
	agent::csAgentIDRequest msg;

	PHOSTENT hostinfo;
	char hostname[50];
	int tempAddr;
	memset(hostname, 0, sizeof(hostname));

	int nError = gethostname(hostname, sizeof(hostname));
	if (nError == 0)
	{
		hostinfo = gethostbyname(hostname);
		struct in_addr* temp = (struct in_addr*)hostinfo->h_addr_list[0];
		tempAddr = temp->S_un.S_addr;

		msg.set_hostip(tempAddr);

		packet.length = (short)msg.ByteSize();
		packet.type = (short)agent::AgentIDRequest;
		msg.SerializeToArray((void *)&packet.msg, packet.length);

		Send((char *)&packet, packet.length + HEADER_SIZE);
	}
	else
	{
		ERROR_PRINT("[AgentClientSocket] Can't Send AgentIDRequest packet\n");
	}
	
}

void AgentClientSocket::SendProcessListRequest()
{
	PRINT("[AgentSocket] SendProcessListRequest\n");
	CPacket packet;
	agent::csProcessListRequest msg;

	packet.length = (short)msg.ByteSize();
	packet.type = (short)agent::ProcessListRequest;
	msg.SerializeToArray((void *)&packet.msg, packet.length);

	Send((char *)&packet, packet.length + HEADER_SIZE);
}

void AgentClientSocket::SendCounterListRequest()
{
	PRINT("[AgentSocket] SendProcessListRequest\n");
	CPacket packet;
	agent::csCounterListRequest msg;

	packet.length = (short)msg.ByteSize();
	packet.type = (short)agent::CounterListRequest;
	msg.SerializeToArray((void *)&packet.msg, packet.length);

	Send((char *)&packet, packet.length + HEADER_SIZE);
}