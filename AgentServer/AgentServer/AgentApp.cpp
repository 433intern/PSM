#include "stdafx.h"


AgentApp::AgentApp(WORD port)
:isEnd(false), heartbeatTime(10), listenPort(port)
{
}

AgentApp::~AgentApp()
{
	delete agentServer;
}

void AgentApp::Init()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int nCPUs = (int)si.dwNumberOfProcessors;

	agentServer = new TcpAgentServer(listenPort, nCPUs * 2, 100);
}

void AgentApp::Start()
{
	std::thread logicThread(&LogicHandle::Process, &logicHandle);
	agentServer->Start();

#ifdef HEARTBEAT
	Process();
#endif

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
	}
}

void AgentApp::End()
{
	isEnd = true;
	closesocket(agentServer->listenSocket_.socket_);

	logicHandle.isEnd = true;

	PRINT("all disconnect success!!\nreal end complete\n");
}