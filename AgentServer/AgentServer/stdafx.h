#pragma once


#define REDISIP "127.0.0.1"

#include <wchar.h>
#include <cstdio>
#include <tchar.h>
#include <list>
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <climits>
#include <thread>
#include <queue>
#include <stdio.h>
#include <hash_map>
#include <unordered_map>

#include <json/json.h>


#include "../../iocpLib/iocpHeaders.h"

class AgentApp;
class RedisManager;
struct CPacket;
class AgentSocket;
class TcpAgentServer;
class LogicHandle;

#define HEARTBEAT
#define ERROR_PRINT printf
#define PRINT printf

class WebCommandSocket;
class TcpWebCommandServer;

#include "RedisManager.h"
#include "LogicHandle.h"
#include "AgentApp.h"



#include "CPacket.h"
#include "../../lib/AgentProtocol.pb.h"
#include "AgentSocket.h"
#include "TcpAgentServer.h"

#include "../../lib/WebProtocol.pb.h"
#include "WebCommandSocket.h"
#include "TcpWebCommandServer.h"
