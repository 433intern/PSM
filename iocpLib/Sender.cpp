
#include "iocpHeaders.h"

void Sender::ProcEvent(Act* act, DWORD bytes_transferred)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	tcpsocket.SendProcess(false, act, bytes_transferred);
}

void Sender::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	tcpsocket.SendProcess(true, act, error);
}

void Sender::Init(Proactor* proactor)
{
	proactor_ = proactor;
}