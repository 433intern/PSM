#pragma once

#pragma comment(lib, "ws2_32")
#pragma comment(lib,"mswsock.lib")  

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define BUFSIZE 8192

#include <WinSock2.h>
#include <MSWSock.h>
#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include <list>

class Act;
class Actor;
class Proactor;
class TcpAct;

#include "targetver.h"
#include "Act.h"
#include "Proactor.h"
#include "Actor.h"

class Acceptor;
class Receiver;
class Sender;
class Disconnector;
class Connector;
class TcpListenSocket;
class TcpSocket;

class WinSockBase;

#include "MemPooler.h"

#include "Acceptor.h"
#include "Disconnector.h"
#include "Receiver.h"
#include "Sender.h"
#include "Connector.h"

#include "TcpAct.h"
#include "TcpListenSocket.h"
#include "TcpSocket.h"

#include "WinSockBase.h"