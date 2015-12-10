#pragma once

class TcpAgentServer : public WinSockBase
{
public:
	~TcpAgentServer();
	TcpAgentServer(WORD Port, int ThreadPoolSize, int SocketPoolSize);
	void Start();

public:
	Proactor*		proactor_;
	Acceptor*		acceptor_;
	Receiver*		receiver_;
	Sender*			sender_;
	Disconnector*	disconnector_;

	TcpListenSocket	listenSocket_;

public:
	std::thread heartbeatThread;
	void HeartbeatCheck();

public:
	void AddAgent(AgentSocket* agent);
	void DeleteAgent(AgentSocket* agent);
	AgentSocket* GetAgentSocketByAgentID(int AgentID);

private:
	WORD			port_;
	int				threadPoolSize_;
	int				socketPoolSize_;

	std::list<AgentSocket*> agentList;
	CRITICAL_SECTION agentListLock;

};