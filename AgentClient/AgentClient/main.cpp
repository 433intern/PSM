#include "stdafx.h"

AgentClientApp* agentClientApp;

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3){
		printf("error command\n");
		return 0;
	}

	/*Query q;
	q.Init();
	
	ProcessInfo_Agent p;
	p.isOn = true;
	p.name = "chrome";
	q.checkProcessList.push_back(p);

	p.isOn = true;
	p.name = "devenv";
	q.checkProcessList.push_back(p);

	q.counterList.push_back("% Processor Time");
	q.counterList.push_back("Working Set - Private");
	q.Record(50, 1);*/

	agentClientApp = new AgentClientApp(inet_addr(argv[1]), atoi(argv[2]));
	agentClientApp->Init();
	agentClientApp->Start();

	
	system("pause");
	return 0;
}