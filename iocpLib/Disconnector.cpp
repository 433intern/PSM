
#include "iocpHeaders.h"

void Disconnector::ProcEvent(Act* act, DWORD bytes_transferred)
{
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);
	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	assert(tcpsocket.disconnector_);

	tcpsocket.DisconnProcess(false, act, bytes_transferred);
	// reuse socket

}

void Disconnector::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	tcpsocket.disconnectCall = false;
	tcpsocket.DisconnProcess(true, act, error);
}

void Disconnector::Init(Proactor* proactor)
{
	proactor_ = proactor;
}