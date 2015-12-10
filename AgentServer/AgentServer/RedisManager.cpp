#include "stdafx.h"

RedisManager::RedisManager(std::string redisIP, unsigned short port)
:redisIP(redisIP), port(port), redis(ioService)
{

}

RedisManager::~RedisManager()
{

}

void RedisManager::Init()
{
	boost::asio::ip::address address = boost::asio::ip::address::from_string(redisIP);
	std::string errmsg;

	if (!redis.connect(address, port, errmsg))
	{
		ERROR_PRINT("Can't connect to redis: %d\n", errmsg);
		return;
	}

	defaultProcessList.push_back("Melon");
	defaultProcessList.push_back("KakaoTalk");

	defaultCounterList.push_back("% Processor Time");
	defaultCounterList.push_back("Working Set - Private");

	defaultMachineCounterList.push_back("\\Processor(_Total)\\% Processor Time");
	defaultMachineCounterList.push_back("\\Memory\\Page Faults/sec");
	defaultMachineCounterList.push_back("\\Memory\\Available KBytes");

	PRINT("[RedisManager] RedisManager Initialize complete!\n");
}

int RedisManager::GetAgentID(int hostIP)
{
	RedisValue result;
	result = redis.command("HMGET", "AgentList", std::to_string(hostIP));
	if (result.isOk())
	{
		std::vector<RedisValue> v = result.toArray();
		if (!v[0].isNull())
		{ 
			int agentID = atoi(v[0].toString().c_str());
			return agentID;
		}
	}

	result = redis.command("HLEN", "AgentList");
	int agentID;
	
	if (result.isOk())
	{
		agentID = result.toInt();
	}
	else
	{
		ERROR_PRINT("[RedisManager] redis command HLEN ERROR\n");
		return -1;
	}

	result = redis.command("HMSET", "AgentList", std::to_string(hostIP), std::to_string(agentID));

	if (!result.isOk())
	{
		ERROR_PRINT("[RedisManager] redis command HMSET ERROR\n");
		return -1;
	}

	return agentID;
}

bool RedisManager::GetProcessList(int agentID, std::vector<std::string>& resultList)
{
	resultList.clear();

	std::string key = std::to_string(agentID) + ":ProcessList";

	RedisValue result;
	result = redis.command("smembers",  key);

	if (result.isOk())
	{
		if (!result.isNull() && result.toArray().size() != 0)
		{
			std::vector<RedisValue> v = result.toArray();
			PRINT("[RedisManager] Agent %d's ProcessList\n", agentID);
			for (RedisValue value : v)
			{
				PRINT("%s\n", value.toString().c_str());
				resultList.push_back(value.toString());
			}
			PRINT("\n");
		}
		else
		{
			for (std::string s : defaultProcessList)
			{
				result = redis.command("sadd", key, s);
				if (!result.isOk())
				{
					ERROR_PRINT("[RedisManager] redis command sadd ERROR\n");
					return false;
				}
				else
				{
					resultList.push_back(s);
				}
			}
		}
	}
	else
	{
		ERROR_PRINT("[RedisManager] redis command smembers ERROR\n");
		return false;
	}

	return true;
}

bool RedisManager::GetCounterList(int agentID, std::vector<std::string>& resultList, bool isMachine)
{
	resultList.clear();

	std::string key;
	if (!isMachine) key = std::to_string(agentID) + ":CounterList";
	else key = std::to_string(agentID) + ":MachineCounterList";

	RedisValue result;
	result = redis.command("smembers", key);

	if (result.isOk())
	{
		if (!result.isNull() && result.toArray().size() != 0)
		{
			std::vector<RedisValue> v = result.toArray();
			PRINT("[RedisManager] Agent %s\n", key.c_str());
			for (RedisValue value : v)
			{
				PRINT("%s\n", value.toString().c_str());
				resultList.push_back(value.toString());
			}
			PRINT("\n");
		}
		else
		{
			if (!isMachine)
			{
				for (std::string s : defaultCounterList)
				{
					result = redis.command("sadd", key, s);
					if (!result.isOk())
					{
						ERROR_PRINT("[RedisManager] redis command sadd ERROR\n");
						return false;
					}
					else
					{
						resultList.push_back(s);
					}
				}
			}
			else
			{
				for (std::string s : defaultMachineCounterList)
				{
					result = redis.command("sadd", key, s);
					if (!result.isOk())
					{
						ERROR_PRINT("[RedisManager] redis command sadd ERROR\n");
						return false;
					}
					else
					{
						resultList.push_back(s);
					}
				}
			}
		}
	}
	else
	{
		ERROR_PRINT("[RedisManager] redis command smembers ERROR\n");
		return false;
	}
	return true;
}

bool RedisManager::SaveCurrentProcessList(int agentID, CPacket* packet)
{
	agent::csCurrentProcessListSend msg;

	std::string key = std::to_string(agentID) + ":CurrentProcessList";
	RedisValue result;

	result = redis.command("del", key);
	if (!result.isOk())
	{
		ERROR_PRINT("[RedisManager] redis command del ERROR\n");
		return false;
	}


	if (msg.ParseFromArray(packet->msg, packet->length))
	{
		PRINT("[AgentSocket] Current Process List Agent %d\n", agentID);
		for (int i = 0; i < msg.processinfo_size(); i++)
		{
			agent::CurrentProcess pi = msg.processinfo(i);

			//PRINT("%s\n", pi.processname().c_str());

			std::string value = "";
			for (int j = 0; j < pi.processid_size(); j++)
			{
				int pid = pi.processid(j);
				//PRINT("%d ", pid);

				value += std::to_string(pid)+" ";
			}

			result = redis.command("hmset", key, pi.processname(), value);
			if (!result.isOk())
			{
				ERROR_PRINT("[RedisManager] redis command hmset ERROR\n");
				return false;
			}
		}
	}

	return true;
}

std::string RedisManager::CounterNameToNewName(const std::string& counterName)
{
	if (counterName == "\\Processor(_Total)\\% Processor Time")
	{
		return std::string("TotalCpuTime");
	}
	else if (counterName == "\\Memory\\Page Faults/sec")
	{
		return std::string("PageFault");
	}
	else if (counterName == "\\Memory\\Available KBytes")
	{
		return std::string("Memory");
	}
	else if (counterName == "% Processor Time")
	{
		return std::string("CpuTime");
	}
	else if (counterName == "Working Set - Private")
	{
		return std::string("Memory");
	}
		
	return std::string("etc");
}

std::string RedisManager::GetCurrentDate(long long int timestamp)
{
	struct tm t;

	time_t timer = (time_t)timestamp;
	localtime_s(&t, &timer);
	static char s[20];

	sprintf(s, "%04d-%02d-%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

	return std::string(s);
}

std::string RedisManager::GetCurrTime(long long int timestamp)
{
	struct tm t;

	time_t timer = (time_t)timestamp;
	localtime_s(&t, &timer);
	static char s[20];

	sprintf(s, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);

	return std::string(s);
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

bool RedisManager::SaveProcessInfo(int agentID, CPacket* packet)
{
	agent::csTotalProcessInfoSend msg;

	std::string	sID = std::to_string(agentID);

	RedisValue result;

	if (msg.ParseFromArray(packet->msg, packet->length))
	{
		for (int i = 0; i < msg.info_size(); i++)
		{
			agent::ProcessInfos pi = msg.info(i);
			
			std::string counterName = CounterNameToNewName(pi.countername());
			std::string prefix = sID + ":" + counterName
				+ ":" + pi.processname() + ":" + std::to_string(pi.processid()) + ":";

			if (pi.logs_size() > 0)
			{
				/*agent::Log temp = pi.logs(0);
				std::string curDate = GetCurrentDate(temp.timestamp());
				std::string curKey = prefix + curDate;

				result = redis.command("lindex", curKey, 0);
				if (!result.isOk())
				{
					ERROR_PRINT("[RedisManager] redis command lindex ERROR\n");
					return false;
				}
				double curValue = -1;
				if (!result.isNull())
				{
					std::string t = result.toString();
					curValue = std::stod(split(t, ' ')[1]);
				}*/

				for (int j = 0; j < pi.logs_size(); j++)
				{
					agent::Log l = pi.logs(j);
					std::string curDate = GetCurrentDate(l.timestamp());
					std::string key = prefix + curDate;
					double value = l.value();

					/*if (GetCurrentDate(l.timestamp()) != curDate)
					{
						std::string curDate = GetCurrentDate(l.timestamp());
						std::string curKey = prefix + curDate;
						curValue = -1;
					}
					if (value == curValue)
					{
						continue;
					}
					curValue = value;*/

					if (counterName == "Memory") value /= 1024;

					std::string v = GetCurrTime(l.timestamp()) + " " + std::to_string(value);
					

					result = redis.command("lpush", key, v);
					if (!result.isOk())
					{
						ERROR_PRINT("[RedisManager] redis command lpush ERROR\n");
						return false;
					}
				}		
			}

			
		}
	}
	return true;
}

bool RedisManager::SaveMachineInfo(int agentID, CPacket* packet)
{
	agent::csTotalMachineInfoSend msg;
	std::string	sID = std::to_string(agentID);

	RedisValue result;

	if (msg.ParseFromArray(packet->msg, packet->length))
	{
		for (int i = 0; i < msg.info_size(); i++)
		{
			agent::MachineInfos mi = msg.info(i);

			std::string counterName = CounterNameToNewName(mi.countername());
			std::string prefix = sID + ":" + counterName + ":Total:";

			if (mi.logs_size() > 0)
			{
				for (int j = 0; j < mi.logs_size(); j++)
				{
					agent::Log l = mi.logs(j);
					std::string curDate = GetCurrentDate(l.timestamp());
					std::string key = prefix + curDate;
					double value = l.value();
					std::string v = GetCurrTime(l.timestamp()) + " " + std::to_string(value);
					result = redis.command("lpush", key, v);
					if (!result.isOk())
					{
						ERROR_PRINT("[RedisManager] redis command lpush ERROR\n");
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool RedisManager::SetProcessName(int agentID, std::string& processName)
{
	std::string key = std::to_string(agentID) + ":ProcessList";

	RedisValue result;
	result = redis.command("sadd", key, processName);

	if (result.isOk()){
		if (result.toInt() == 0) return false;
		else return true;
	}
	return false;
}

bool RedisManager::SetCounterName(int agentID, std::string& counterName, bool isMachine)
{
	std::string key;
	if (!isMachine) key = std::to_string(agentID) + ":CounterList";
	else key = std::to_string(agentID) + ":MachineCounterList";

	RedisValue result;
	result = redis.command("sadd", key, counterName);

	if (result.isOk()){
		if (result.toInt() == 0) return false;
		else return true;
	}
	return false;
}

bool RedisManager::RemProcessName(int agentID, std::string& processName)
{
	std::string key = std::to_string(agentID) + ":ProcessList";

	RedisValue result;
	result = redis.command("srem", key, processName);

	if (result.isOk()){
		if (result.toInt() == 0) return false;
		else return true;
	}
	return false;
}

bool RedisManager::RemCounterName(int agentID, std::string& counterName, bool isMachine)
{
	std::string key;
	if (!isMachine) key = std::to_string(agentID) + ":CounterList";
	else key = std::to_string(agentID) + ":MachineCounterList";

	RedisValue result;
	result = redis.command("srem", key, counterName);

	if (result.isOk()){
		if (result.toInt() == 0) return false;
		else return true;
	}
	return false;
}
