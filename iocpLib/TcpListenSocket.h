#pragma once

class TcpListenSocket
{

public:
	TcpListenSocket();


	void Init(WORD port, int backlogsize = 0);
	void Listen(Proactor* proactor);


public:
	SOCKET		socket_;
	SOCKADDR_IN addr_;

	int			backlogSize_;
};

