#pragma once

class AgentApp
{
public:
	WORD listenPort;

	RedisManager redisManager;
	LogicHandle logicHandle;
	TcpAgentServer* agentServer;

	bool isEnd;
	const int heartbeatTime;


public:
	AgentApp(WORD port);
	~AgentApp();
	void Init();
	void Start();
	void Process();
	void End();

};