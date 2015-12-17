#include "stdafx.h"


AgentApp::AgentApp(WORD port)
:isEnd(false), heartbeatTime(10), listenPort(port)
{
}

AgentApp::~AgentApp()
{
	delete agentServer;
	delete webCommandServer;
}

void AgentApp::Init()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int nCPUs = (int)si.dwNumberOfProcessors;

	agentServer = new TcpAgentServer(listenPort, nCPUs * 2, 100);
	webCommandServer = new TcpWebCommandServer(listenPort + 1, nCPUs * 2, 100);
}

void AgentApp::Start()
{
	//redisManager.GetAgentID(123123);
	std::thread logicThread(&LogicHandle::Process, &logicHandle);
	agentServer->Start();
	webCommandServer->Start();

#ifdef HEARTBEAT
	Process();
#endif

	if (webCommandServer->heartbeatThread.joinable()) webCommandServer->heartbeatThread.join();
	if (agentServer->heartbeatThread.joinable()) agentServer->heartbeatThread.join();
	if (logicThread.joinable()) logicThread.join();
}

void AgentApp::Process()
{
	while (!isEnd)
	{
		std::string input;
		//ZeroMemory(temp, sizeof(temp));
		std::getline(std::cin, input);

		if (input == "End")
		{
			End();
			break;
		}
		
		if (input.substr(0, 4) == "padd")
		{
			agentServer->GetAgentSocketByAgentID(0)
				->AddProcessName(input.substr(5, input.size()));
		}
		else if (input.substr(0, 4) == "pdel")
		{
			agentServer->GetAgentSocketByAgentID(0)
				->DeleteProcessName(input.substr(5, input.size()));
		}
		else if (input.substr(0, 4) == "cadd")
		{
			agentServer->GetAgentSocketByAgentID(0)
				->AddCounterName(input.substr(5, input.size()), false);
		}
		else if (input.substr(0, 4) == "cdel")
		{
			agentServer->GetAgentSocketByAgentID(0)
				->DeleteCounterName(input.substr(5, input.size()), false);
		}
		else if (input.substr(0, 4) == "mcadd")
		{
			agentServer->GetAgentSocketByAgentID(0)
				->AddCounterName(input.substr(5, input.size()), true);
		}
		else if (input.substr(0, 4) == "mcdel")
		{
			agentServer->GetAgentSocketByAgentID(0)
				->DeleteCounterName(input.substr(5, input.size()), true);
		}
	}
}

void AgentApp::End()
{
	isEnd = true;
	closesocket(agentServer->listenSocket_.socket_);

	logicHandle.isEnd = true;

	PRINT("all disconnect success!!\nreal end complete\n");
}