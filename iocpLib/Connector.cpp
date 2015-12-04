
#include "iocpHeaders.h"

void Connector::ProcEvent(Act* act, DWORD bytes_transferred)
{
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);
	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	assert(proactor_);

	proactor_->Register((HANDLE)(tcpsocket.socket_));

	tcpsocket.ConnProcess(false, act, bytes_transferred);
}

void Connector::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	tcpsocket.ConnProcess(true, act, error);
}

void Connector::Init(Proactor* proactor)
{
	proactor_ = proactor;
}