#pragma once

class WinSockBase
{
public:
	WinSockBase()
	{
		WSADATA	wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{

		}
	}

	virtual ~WinSockBase()
	{
		WSACleanup();
	}

};