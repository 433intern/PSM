#pragma once


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

#include "RedisManager.h"
#include "LogicHandle.h"
#include "AgentApp.h"

#include "CPacket.h"
#include "../../lib/AgentProtocol.pb.h"
#include "AgentSocket.h"
#include "TcpAgentServer.h"

