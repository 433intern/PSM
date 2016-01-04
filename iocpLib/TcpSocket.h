#pragma once

class TcpSocket
{

public:
	TcpSocket();

public:
	void Init();
	void InitBuf();
	void InitAct(Proactor* proactor, Acceptor* acceptor, Disconnector* disconnector,
		Connector* connector, Sender* sender, Receiver* receiver);

public:
	bool Recv();
	bool Recv(char* buf, int buflen);
	bool Send(char* buf, int buflen);
	void Reuse(int size);
	void Disconnect();
	void Connect();

	virtual void RecvProcess(bool isError, Act* act, DWORD bytes_transferred) = 0;
	virtual void SendProcess(bool isError, Act* act, DWORD bytes_transferred) = 0;
	virtual void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred) = 0;
	virtual void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred) = 0;
	virtual void ConnProcess(bool isError, Act* act, DWORD bytes_transferred) = 0;

public:
	enum ACT_TYPE
	{
		ACT_SEND = 0,
		ACT_RECV,
		ACT_ACCEPT,
		ACT_DISCONNECT,
		ACT_CONNECT,
		ACT_TYPE_CNT
	};

	TcpAct			act_[ACT_TYPE_CNT];

public:
	SOCKET			socket_;
	SOCKADDR_IN		addr_;

public:
	char			acceptBuf_[BUFSIZE]; //may not use

	char			recvBuf_[BUFSIZE];
	char			sendBuf_[BUFSIZE]; //may not use

	WSABUF			wsaRecvBuf;
	WSABUF			wsaSendBuf;

public:
	Proactor*		proactor_;
	Acceptor*		acceptor_;
	Disconnector*	disconnector_;
	Sender*			sender_;
	Receiver*		receiver_;
	Connector* connector_;

	bool disconnectCall;

};
