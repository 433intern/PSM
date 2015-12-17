#include "stdafx.h"

extern AgentApp* agentApp;

WebCommandSocket::WebCommandSocket()
: position(0), remainBytes(HEADER_SIZE), healthCheck(true)
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
			msg->ownert = CPacket::ownerType::WEBCOMMAND;
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
	case psmweb::HealthAck:
		PRINT("[WebCommandSocket] HealthAck\n");
		this->healthCheck = true;
		break;
	case psmweb::ProcessCommandRequest:
	{
		PRINT("[WebCommandSocket] ProcessCommandRequest\n");
		psmweb::wsProcessCommandRequest msg;
		
		if (msg.ParseFromArray(packet->msg, packet->length)){
			AgentSocket* socket = agentApp->agentServer->GetAgentSocketByToken(msg.token());
			std::string name(msg.processname());
			if (socket != NULL){
				if (msg.type() == psmweb::ADDLIST){
					SendProcessCommandResponse(msg.token(), msg.type(), \
						socket->AddProcessName(name), "");
				}
				else if (msg.type() == psmweb::DELETELIST){
					SendProcessCommandResponse(msg.token(), msg.type(), \
						socket->DeleteProcessName(name), "");
				}
			}
		}
		else{
			ERROR_PRINT("[WebCommandSocket] ProcessCommandRequest error\n");
		}
		break;
	}

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

void WebCommandSocket::SendProcessCommandResponse(int token, psmweb::ProcessCommandType type, bool success, std::string reason)
{
	PRINT("[WebCommandSocket] SendProcessCommandResponse\n");
	CPacket packet;
	psmweb::swProcessCommandResponse msg;

	msg.set_token(token);
	msg.set_type(type);
	if (reason.size() > 0) msg.set_failreason(reason);
	if (success) msg.set_result(psmweb::Result::SUCCESS);
	else msg.set_result(psmweb::Result::FAILURE);

	packet.length = (short)msg.ByteSize();
	packet.type = (short)psmweb::ProcessCommandResponse;
	memset(packet.msg, 0, BUFSIZE);
	msg.SerializeToArray((void *)&packet.msg, packet.length);

	Send((char *)&packet, packet.length + HEADER_SIZE);
}