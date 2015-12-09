#pragma once

#include "../../redisLib/lib/redissyncclient.h"

class RedisManager
{
public:
	RedisManager(std::string redisIP, unsigned short port);
	~RedisManager();

	void Init();
	int GetAgentID(int hostIP);
	bool GetProcessList(int agentID, std::vector<std::string>& result);
	bool GetCounterList(int agentID, std::vector<std::string>& result, bool isMachine);

private:
	std::string redisIP;
	const unsigned short port;
	boost::asio::io_service ioService;
	RedisSyncClient redis;


	std::vector<std::string> defaultProcessList;
	std::vector<std::string> defaultCounterList;
	std::vector<std::string> defaultMachineCounterList;
};