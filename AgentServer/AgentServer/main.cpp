#include "stdafx.h"

AgentApp* agentApp;

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 2){
		printf("error command\n");
		return 0;
	}

	agentApp = new AgentApp(atoi(argv[1]));
	agentApp->Init();
	agentApp->Start();


	system("pause");
	return 0;
}