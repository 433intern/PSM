#include "stdafx.h"

AgentClientApp* agentClientApp;

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3){
		printf("error command\n");
		return 0;
	}

	Helper h;
	h.UpdateProcessList();

	agentClientApp = new AgentClientApp(inet_addr(argv[1]), atoi(argv[2]));
	agentClientApp->Init();
	agentClientApp->Start();

	
	system("pause");
	return 0;
}