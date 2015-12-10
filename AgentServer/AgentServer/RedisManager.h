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
	
	bool SaveCurrentProcessList(int agentID, CPacket* packet);
	bool SaveProcessInfo(int agentID, CPacket* packet);
	bool SaveMachineInfo(int agentID, CPacket* packet);

	bool SetProcessName(int agentID, std::string& processName);
	bool SetCounterName(int agentID, std::string& counterName, bool isMachine);

	bool RemProcessName(int agentID, std::string& processName);
	bool RemCounterName(int agentID, std::string& counterName, bool isMachine);

	std::string CounterNameToNewName(const std::string& counterName);
	std::string GetCurrentDate(long long int timestamp);
	std::string GetCurrTime(long long int timestamp);

private:
	std::string redisIP;
	const unsigned short port;
	boost::asio::io_service ioService;
	RedisSyncClient redis;


	std::vector<std::string> defaultProcessList;
	std::vector<std::string> defaultCounterList;
	std::vector<std::string> defaultMachineCounterList;
};