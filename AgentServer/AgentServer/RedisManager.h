#pragma once

#include "../../redisLib/lib/redissyncclient.h"

class RedisManager
{
public:
	RedisManager(std::string redisIP, unsigned short port);
	~RedisManager();

	void Init();

	bool InitAgent(int hostIP, double ramSize);
	int GetAgentID(int hostIP);
	
	Json::Value GetAgentJVByHostIP(int hostIP);

	bool ChangeAgentState_isOn(int hostIP, bool value);
	bool ChangeAgentState_startRecording(int hostIP, int totalRecordTime,
									int responseTime, int interval, long long int delay);
	bool ChangeAgentState_stopRecording(int hostIP);

	bool GetProcessList(int agentID, std::vector<std::string>& result);
	bool GetCounterList(int agentID, std::vector<std::string>& result, bool isMachine);
	
	bool SaveCurrentProcessList(int agentID, CPacket* packet);
	bool SaveProcessInfo(int agentID, CPacket* packet);
	bool SaveMachineInfo(int agentID, CPacket* packet);

	bool SetProcessName(int agentID, std::string& processName);
	bool SetCounterName(int agentID, std::string& counterName, bool isMachine, std::string& result);

	bool RemProcessName(int agentID, std::string& processName);
	bool RemCounterName(int agentID, std::string& counterName, bool isMachine, std::string& result);

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

	std::unordered_map<std::string, std::string> counterNameMap;
};