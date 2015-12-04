#include "stdafx.h"

TcpAgentClient::TcpAgentClient(int ThreadPoolSize)
{
	proactor_ = NULL;
	receiver_ = NULL;
	sender_ = NULL;
	disconnector_ = NULL;
	connector_ = NULL;

	threadPoolSize_ = ThreadPoolSize;
}

TcpAgentClient::~TcpAgentClient()
{
	if (proactor_ != NULL) delete proactor_;
	if (connector_ != NULL) delete connector_;
	if (receiver_ != NULL) delete receiver_;
	if (sender_ != NULL) delete sender_;
	if (disconnector_ != NULL) delete disconnector_;
}

void TcpAgentClient::Start()
{
	proactor_ = new Proactor;
	receiver_ = new Receiver;
	sender_ = new Sender;
	disconnector_ = new Disconnector;
	connector_ = new Connector;

	// Proactor initialize
	proactor_->Init(threadPoolSize_);
	connector_->Init(proactor_);
	// Receiver initialize - Manager user Pool, data transmission, parsing data
	receiver_->Init(proactor_);
	// Disconnector initialize  - Manage socket pool with Acceptor
	disconnector_->Init(proactor_);
	// Sender initialize 
	sender_->Init(proactor_);

	CreateConnectSocket();
}

void TcpAgentClient::CreateConnectSocket(){
	socket = new AgentClientSocket();
	socket->Init();
	socket->InitAct(proactor_, NULL, disconnector_, connector_, sender_, receiver_);

	socket->Bind();

	proactor_->Register((HANDLE)socket->socket_);
}

void TcpAgentClient::Connect(unsigned int ip, WORD port){
	socket->Connect(ip, port);
}