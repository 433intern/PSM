#pragma once

class Acceptor : public Actor
{
public:
	Acceptor(){};

public:
	void ProcEvent(Act* act, DWORD bytes_transferred);
	void ProcError(Act* act, DWORD error);

	void Init(TcpListenSocket* tcplistensocket, Proactor* proactor);
	void Register(TcpSocket& acceptsocket, int size);

public:
	TcpListenSocket*	tcpListenSocket_;
};
