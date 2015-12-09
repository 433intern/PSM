#pragma once

#include <wchar.h>
#include <algorithm>
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

#include "../../iocpLib/iocpHeaders.h"

#include <Pdh.h>
#pragma comment(lib, "pdh.lib")

class Log;
class ProcessCounterEntry;
class ProcessInfo;
class ProcessInfo_Agent;
class Helper;
class Query;

#include "../../lib/AgentProtocol.pb.h"

#include "Informations.h"
#include "Helper.h"
#include "Query.h"

class AgentClientApp;
struct CPacket;
class AgentClientSocket;
class TcpAgentClient;
class LogicHandle;

#define ERROR_PRINT printf
#define PRINT printf

#include "LogicHandle.h"
#include "AgentClientApp.h"

#include "CPacket.h"

#include "AgentClientSocket.h"
#include "TcpAgentClient.h"

