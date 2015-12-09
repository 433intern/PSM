#pragma once

class AgentClientApp
{
public:
	int agentID;

	unsigned int ip;
	WORD connectPort;

	LogicHandle logicHandle;
	TcpAgentClient* agentClient;

	bool isEnd;

public:
	AgentClientApp(unsigned int ip, WORD connectPort);
	~AgentClientApp();
	void Init();
	void Start();
	void Process();
	void End();

};