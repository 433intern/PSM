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
			std::string name(msg.processname());
			int agentID = agentApp->agentServer->redisManager.GetAgentID(msg.token());

			bool result = false;
			if (msg.type() == psmweb::ADDLIST){
				if (agentApp->webCommandServer->redisManager.SetProcessName(agentID, name)){
					AgentSocket* socket = agentApp->agentServer->GetAgentSocketByToken(msg.token());
					if (socket != NULL){
						socket->AddProcessName(name);
					}
					result = true;
				}
			}
			else if (msg.type() == psmweb::DELETELIST){
				if (agentApp->webCommandServer->redisManager.RemProcessName(agentID, name)){
					AgentSocket* socket = agentApp->agentServer->GetAgentSocketByToken(msg.token());
					if (socket != NULL){
						socket->DeleteProcessName(name);
					}
					result = true;
				}
			}
			SendProcessCommandResponse(msg.token(), msg.type(), result, "");
		}
		else{
			ERROR_PRINT("[WebCommandSocket] ProcessCommandRequest error\n");
		}
		break;
	}
	case psmweb::CounterCommandRequest:
	{
		PRINT("[WebCommandSocket] CounterCommandRequest\n");
		psmweb::wsCounterCommandRequest msg;

		if (msg.ParseFromArray(packet->msg, packet->length)){
			bool resultv = false;
			int agentID = agentApp->agentServer->redisManager.GetAgentID(msg.token());
			AgentSocket* socket = agentApp->agentServer->GetAgentSocketByToken(msg.token());

			for (int i = 0; i < msg.countername_size(); i++)
			{
				std::string name(msg.countername(i));
				if (msg.type() == psmweb::CADDLIST){
					std::string result;
					if (agentApp->webCommandServer->redisManager.SetCounterName(agentID, name, msg.ismachine(), result)){
						if (socket != NULL){
							socket->AddCounterName(result, msg.ismachine());
						}
						resultv = true;
					}
				}
				else if (msg.type() == psmweb::CDELETELIST){
					std::string result;
					if (agentApp->webCommandServer->redisManager.RemCounterName(agentID, name, msg.ismachine(), result)){
						if (socket != NULL){
							socket->DeleteCounterName(result, msg.ismachine());
						}
						resultv = true;
					}
				}
			}
			SendCounterCommandResponse(msg.token(), msg.type(), resultv, "");
		}
		else{
			ERROR_PRINT("[WebCommandSocket] CounterCommandRequest error\n");
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

void WebCommandSocket::SendCounterCommandResponse(int token, psmweb::CounterCommandType type, bool success, std::string reason)
{
	PRINT("[WebCommandSocket] SendCounterCommandResponse\n");
	CPacket packet;
	psmweb::swCounterCommandResponse msg;

	msg.set_token(token);
	msg.set_type(type);
	if (reason.size() > 0) msg.set_failreason(reason);
	if (success) msg.set_result(psmweb::Result::SUCCESS);
	else msg.set_result(psmweb::Result::FAILURE);

	packet.length = (short)msg.ByteSize();
	packet.type = (short)psmweb::CounterCommandResponse;
	memset(packet.msg, 0, BUFSIZE);
	msg.SerializeToArray((void *)&packet.msg, packet.length);

	Send((char *)&packet, packet.length + HEADER_SIZE);
}