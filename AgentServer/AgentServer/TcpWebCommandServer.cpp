#include "stdafx.h"

extern AgentApp* agentApp;

TcpWebCommandServer::TcpWebCommandServer(WORD Port, int ThreadPoolSize, int SocketPoolSize)
:
proactor_(NULL), acceptor_(NULL), receiver_(NULL), sender_(NULL), disconnector_(NULL),
port_(Port), id(0), threadPoolSize_(ThreadPoolSize), socketPoolSize_(SocketPoolSize), redisManager(REDISIP, 6379)
{
	InitializeCriticalSectionAndSpinCount(&listLock, 4000);
}

TcpWebCommandServer::~TcpWebCommandServer()
{
	DeleteCriticalSection(&listLock);

	if (proactor_ != NULL) delete proactor_;
	if (acceptor_ != NULL) delete acceptor_;
	if (receiver_ != NULL) delete receiver_;
	if (sender_ != NULL) delete sender_;
	if (disconnector_ != NULL) delete disconnector_;
}

void TcpWebCommandServer::Start()
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
		WebCommandSocket* socket = new WebCommandSocket();
		socket->Init();
		socket->InitAct(proactor_, acceptor_, disconnector_, NULL, sender_, receiver_);

		acceptor_->Register(*socket, 0);
	}
	heartbeatThread = std::thread(&TcpWebCommandServer::HeartbeatCheck, this);
}

void TcpWebCommandServer::AddSocket(WebCommandSocket* socket)
{
	EnterCriticalSection(&listLock);
	webCommandList.push_back(socket);
	LeaveCriticalSection(&listLock);
}

void TcpWebCommandServer::DeleteSocket(WebCommandSocket* socket)
{
	EnterCriticalSection(&listLock);
	webCommandList.remove(socket);
	LeaveCriticalSection(&listLock);
}

WebCommandSocket* TcpWebCommandServer::GetSocketByID(int id)
{
	EnterCriticalSection(&listLock);
	for (WebCommandSocket* socket : webCommandList)
	{
		if (socket->token == id)
		{
			LeaveCriticalSection(&listLock);
			return socket;
		}
	}
	LeaveCriticalSection(&listLock);
	return NULL;
}

void TcpWebCommandServer::HeartbeatCheck()
{
	while (!agentApp->isEnd){
		EnterCriticalSection(&listLock);
#ifdef HEARTBEAT
		PRINT("[TcpWebCommandServer] **heartbeat check / player cnt : %d\n", webCommandList.size());
#endif
		for (std::list<WebCommandSocket*>::iterator iter = webCommandList.begin();
			iter != webCommandList.end(); ++iter)
		{
			(*iter)->healthCheck = false;

			/* msg send */

			(*iter)->SendHealthCheck();
		}
		LeaveCriticalSection(&listLock);

		std::this_thread::sleep_for(std::chrono::seconds(agentApp->heartbeatTime));
		if (agentApp->isEnd) return;

		EnterCriticalSection(&listLock);
		int cnt = 0;
		for (std::list<WebCommandSocket*>::iterator iter = webCommandList.begin();
			iter != webCommandList.end(); ++iter)
		{
			if ((*iter)->healthCheck == false){
				(*iter)->Disconnect();
				cnt++;
			}
		}
#ifdef HEARTBEAT
		PRINT("[TcpWebCommandServer] **heartbeat check clear / player cnt : %d\n", webCommandList.size() - cnt);
#endif
		LeaveCriticalSection(&listLock);
	}
}