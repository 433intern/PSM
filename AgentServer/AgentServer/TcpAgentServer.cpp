#include "stdafx.h"

extern AgentApp* agentApp;

TcpAgentServer::TcpAgentServer(WORD Port, int ThreadPoolSize, int SocketPoolSize)
:proactor_(NULL), acceptor_(NULL), receiver_(NULL), sender_(NULL), disconnector_(NULL),
port_(Port), threadPoolSize_(ThreadPoolSize), socketPoolSize_(SocketPoolSize), redisManager(REDISIP, 6379)
{
	InitializeCriticalSectionAndSpinCount(&agentListLock, 4000);
}

TcpAgentServer::~TcpAgentServer()
{
	DeleteCriticalSection(&agentListLock);

	if (proactor_ != NULL) delete proactor_;
	if (acceptor_ != NULL) delete acceptor_;
	if (receiver_ != NULL) delete receiver_;
	if (sender_ != NULL) delete sender_;
	if (disconnector_ != NULL) delete disconnector_;
}

void TcpAgentServer::Start()
{
	redisManager.Init();
	proactor_ = new Proactor;
	acceptor_ = new Acceptor;
	receiver_ = new Receiver;
	sender_ = new Sender;
	disconnector_ = new Disconnector;

	// Proactor initialize
	proactor_->Init(threadPoolSize_);
	// Listen sock initialize
	listenSocket_.Init(port_);
	// Listen start
	listenSocket_.Listen(proactor_);
	// Acceptor initialize - Manage socket pool with Disconnector
	acceptor_->Init(&listenSocket_, proactor_);
	// Receiver initialize - Manager user Pool, data transmission, parsing data
	receiver_->Init(proactor_);
	// Disconnector initialize  - Manage socket pool with Acceptor
	disconnector_->Init(proactor_);
	// Sender initialize 
	sender_->Init(proactor_);

	// Create Socket pool 
	for (int i = 0; i < socketPoolSize_; i++)
	{
		AgentSocket* socket = new AgentSocket();
		socket->Init();
		socket->InitAct(proactor_, acceptor_, disconnector_, NULL, sender_, receiver_);

		acceptor_->Register(*socket, 0);
	}
	heartbeatThread = std::thread(&TcpAgentServer::HeartbeatCheck, this);
}

void TcpAgentServer::AddAgent(AgentSocket* agent)
{
	EnterCriticalSection(&agentListLock);
	agentList.push_back(agent);
	LeaveCriticalSection(&agentListLock);
}

void TcpAgentServer::DeleteAgent(AgentSocket* agent)
{
	EnterCriticalSection(&agentListLock);
	agentList.remove(agent);
	LeaveCriticalSection(&agentListLock);
}

AgentSocket* TcpAgentServer::GetAgentSocketByAgentID(int AgentID)
{
	EnterCriticalSection(&agentListLock);
	for (AgentSocket* ag : agentList)
	{
		if (ag->agentID == AgentID)
		{
			LeaveCriticalSection(&agentListLock);
			return ag;
		}
	}
	LeaveCriticalSection(&agentListLock);
	return NULL;
}

AgentSocket* TcpAgentServer::GetAgentSocketByToken(int token)
{
	EnterCriticalSection(&agentListLock);
	for (AgentSocket* ag : agentList)
	{
		if (ag->token == token)
		{
			LeaveCriticalSection(&agentListLock);
			return ag;
		}
	}
	LeaveCriticalSection(&agentListLock);
	return NULL;
}



void TcpAgentServer::HeartbeatCheck()
{
	while (!agentApp->isEnd){
		EnterCriticalSection(&agentListLock);
#ifdef HEARTBEAT
		PRINT("[ClientServer] **heartbeat check / player cnt : %d\n", agentList.size());
#endif
		for (std::list<AgentSocket*>::iterator iter = agentList.begin();
			iter != agentList.end(); ++iter)
		{
			(*iter)->healthCheck = false;

			/* msg send */

			(*iter)->SendHealthCheck();
		}
		LeaveCriticalSection(&agentListLock);

		std::this_thread::sleep_for(std::chrono::seconds(agentApp->heartbeatTime));
		if (agentApp->isEnd) return;

		EnterCriticalSection(&agentListLock);
		int cnt = 0;
		for (std::list<AgentSocket*>::iterator iter = agentList.begin();
			iter != agentList.end(); ++iter)
		{
			if ((*iter)->healthCheck == false){
				(*iter)->Disconnect();
				cnt++;
			}
		}
#ifdef HEARTBEAT
		PRINT("[ClientServer] **heartbeat check clear / player cnt : %d\n", agentList.size() - cnt);
#endif
		LeaveCriticalSection(&agentListLock);
	}
}