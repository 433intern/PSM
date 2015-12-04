
#include "iocpHeaders.h"

TcpListenSocket::TcpListenSocket()
{
	backlogSize_ = 0;
}

void TcpListenSocket::Init(WORD port, int backlogsize)
{
	printf("Listen Socket Init %d\n", port);
	socket_ = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	addr_.sin_family = AF_INET;
	addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_.sin_port = htons(port);

	backlogSize_ = backlogsize;
}

void TcpListenSocket::Listen(Proactor* proactor)
{
	proactor->Register(reinterpret_cast<HANDLE>(socket_));

	bind(socket_, (SOCKADDR*)&addr_, sizeof(addr_));
	listen(socket_, backlogSize_);
}
