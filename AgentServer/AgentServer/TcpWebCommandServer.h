#pragma once

class TcpWebCommandServer : public WinSockBase
{
public:
	~TcpWebCommandServer();
	TcpWebCommandServer(WORD Port, int ThreadPoolSize, int SocketPoolSize);
	void Start();

public:
	Proactor*		proactor_;
	Acceptor*		acceptor_;
	Receiver*		receiver_;
	Sender*			sender_;
	Disconnector*	disconnector_;

	TcpListenSocket	listenSocket_;

public:
	void AddSocket(WebCommandSocket* socket);
	void DeleteSocket(WebCommandSocket* socket);

public:
	std::thread heartbeatThread;
	void HeartbeatCheck();
	RedisManager redisManager;

private:
	WORD			port_;
	int				threadPoolSize_;
	int				socketPoolSize_;
	int				id;

	std::list<WebCommandSocket*> webCommandList;
	CRITICAL_SECTION listLock;

};