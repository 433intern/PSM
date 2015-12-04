#pragma once

#define HEADER_SIZE 4
#define LENGTH_SIZE 2

struct CPacket
{
	short length;
	short type;

	byte msg[BUFSIZE];

	TcpSocket* owner;
};