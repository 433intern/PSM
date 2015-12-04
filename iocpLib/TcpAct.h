#pragma once

class TcpAct : public Act
{
public:
	TcpAct()
	{
		tcpSocket_ = NULL;
	}

	void Complete(DWORD bytes_transferred)
	{
		actor_->ProcEvent(this, bytes_transferred);
	}

	void Error(DWORD error)
	{
		actor_->ProcError(this, error);
	}

	void Init(Actor* actor, TcpSocket* tcpsocket)
	{
		actor_ = actor;
		tcpSocket_ = tcpsocket;
	}


public:
	TcpSocket* tcpSocket_;
};

