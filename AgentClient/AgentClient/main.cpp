#include "stdafx.h"

AgentClientApp* agentClientApp;

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3){
		printf("error command\n");
		return 0;
	}

	Query q;
	q.Init();
	
	ProcessInfo_Agent p;
	p.name = "chrome";
	q.checkProcessList.push_back(p);

	p.name = "devenv";
	q.checkProcessList.push_back(p);

	q.counterList.push_back("% Processor Time");
	q.counterList.push_back("Working Set - Private");

	q.machineCounterList.push_back("\\Processor(_Total)\\% Processor Time");
	q.machineCounterList.push_back("\\Memory\\Available KBytes");

	q.Record(1, 1, true);
	q.Record(1, 1, false);
	

	agentClientApp = new AgentClientApp(inet_addr(argv[1]), atoi(argv[2]));
	agentClientApp->Init();
	agentClientApp->Start();

	
	system("pause");
	return 0;
}