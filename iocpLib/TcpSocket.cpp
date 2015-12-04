
#include "iocpHeaders.h"

TcpSocket::TcpSocket()
{
	socket_ = INVALID_SOCKET;
	acceptor_ = NULL;
}

void TcpSocket::Init()
{
	socket_ = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (socket_ == INVALID_SOCKET)
	{
		printf("WSASocket() Error!!! err(%d)\n", WSAGetLastError());
	}

	disconnectCall = false;
	InitBuf();
}

void TcpSocket::InitBuf()
{
	wsaRecvBuf.buf = recvBuf_;
	wsaRecvBuf.len = BUFSIZE;

	wsaSendBuf.buf = sendBuf_;
	wsaSendBuf.len = BUFSIZE;

	ZeroMemory(recvBuf_, BUFSIZE);
	ZeroMemory(sendBuf_, BUFSIZE);

	ZeroMemory(acceptBuf_, BUFSIZE);
}

void TcpSocket::InitAct(Proactor* proactor, Acceptor* acceptor, Disconnector* disconnector,
	Connector* connector, Sender* sender, Receiver* receiver)
{
	proactor_ = proactor;
	acceptor_ = acceptor;
	disconnector_ = disconnector;
	sender_ = sender;
	receiver_ = receiver;
	connector_ = connector;

	act_[ACT_ACCEPT].Init(acceptor_, this);
	act_[ACT_RECV].Init(receiver_, this);
	act_[ACT_SEND].Init(sender_, this);
	act_[ACT_DISCONNECT].Init(disconnector_, this);
	act_[ACT_CONNECT].Init(connector_, this);
}

void TcpSocket::Recv()
{
	DWORD recvbytes = 0;
	DWORD flags = 0;


	INT ret = WSARecv(socket_, &(wsaRecvBuf), 1, &recvbytes, &flags, static_cast<OVERLAPPED*>(&(act_[ACT_RECV])), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			printf("WSARecv() Error!!! s(%d) err(%d)\n", socket_, error);
			//Disconnect();
		}
	}
}

void TcpSocket::Recv(char* buf, int buflen)
{
	//if (disconnectCall) return;

	DWORD recvbytes = 0;
	DWORD flags = 0;
	wsaRecvBuf.buf = buf;
	wsaRecvBuf.len = buflen;

	INT ret = WSARecv(socket_, &(wsaRecvBuf), 1, &recvbytes, &flags, static_cast<OVERLAPPED*>(&(act_[ACT_RECV])), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			printf("WSARecv() Error!!! s(%d) err(%d)\n", socket_, error);
			//Disconnect();
		}
	}
}

void TcpSocket::Send(char* buf, int buflen)
{
	if (buflen == 0) return;
	DWORD sentbytes = 0;
	wsaSendBuf.buf = buf;
	wsaSendBuf.len = buflen;

	INT ret = WSASend(socket_, &(wsaSendBuf), 1, &sentbytes, 0, static_cast<OVERLAPPED*>(&(act_[ACT_SEND])), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			printf("WSASend() Error!!! s(%d) err(%d)\n", socket_, error);
			//Disconnect();
		}
	}
}

void TcpSocket::Reuse(int size)
{
	disconnectCall = false;
	acceptor_->Register(*this, size);
}

void TcpSocket::Disconnect()
{
	if (disconnectCall) return;

	//PRINT("real disconnect call!\n");
	disconnectCall = true;

	BOOL ret = TransmitFile(
		socket_,
		NULL,
		0,
		0,
		static_cast<OVERLAPPED*>(&(act_[ACT_DISCONNECT])),
		NULL,
		TF_DISCONNECT | TF_REUSE_SOCKET
		);

	if (!ret)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			printf("already disconnected, DisconnectEx Error!!! s(%d), err(%d)\n", socket_, error);
		}
	}
}