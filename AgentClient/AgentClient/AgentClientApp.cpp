#include "stdafx.h"


AgentClientApp::AgentClientApp(unsigned int ip, WORD connectPort)
:isEnd(false), connectPort(connectPort), ip(ip), agentID(-1)
{
}

AgentClientApp::~AgentClientApp()
{
	delete agentClient;
}

void AgentClientApp::Init()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int nCPUs = (int)si.dwNumberOfProcessors;

	agentClient = new TcpAgentClient(2 * nCPUs);
}

void AgentClientApp::Start()
{
	std::thread logicThread(&LogicHandle::Process, &logicHandle);
	agentClient->Start();

	Process();
	
	if (logicThread.joinable()) logicThread.join();
}

void AgentClientApp::Process()
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

		if (input == "connect")
		{
			agentClient->Connect(ip, connectPort);
			break;
		}
	}
}

void AgentClientApp::End()
{
	isEnd = true;
	closesocket(agentClient->socket->socket_);
	logicHandle.isEnd = true;

	PRINT("all disconnect success!!\nreal end complete\n");
}