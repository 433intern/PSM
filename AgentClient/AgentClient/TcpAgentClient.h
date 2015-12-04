#pragma once

class TcpAgentClient : public WinSockBase
{
public:
	~TcpAgentClient();
	TcpAgentClient(int ThreadPoolSize);
	void Start();
	void CreateConnectSocket();
	void Connect(unsigned int ip, WORD port);

public:
	Proactor*		proactor_;
	Receiver*		receiver_;
	Sender*			sender_;
	Disconnector*	disconnector_;
	Connector*		connector_;

	AgentClientSocket* socket;

private:
	int				threadPoolSize_;

};