#include "stdafx.h"

AgentClientApp* agentClientApp;

int _tmain(int argc, _TCHAR* argv[])
{
	//time_t timer = time(NULL); // 현재 시각을 초 단위로 얻기
	//long long int a = (long long int) timer;
	//printf("time : %lld\n", a);

	if (argc != 3){
		printf("error command\n");
		return 0;
	}

	/*Query q;
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

	q.StartRecord(false, 10, 1, 1, 10);*/
	

	agentClientApp = new AgentClientApp(inet_addr(argv[1]), atoi(argv[2]));
	agentClientApp->Init();
	agentClientApp->Start();

	
	system("pause");
	return 0;
}