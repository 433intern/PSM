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

	defaultProcessList.push_back("chrome");
	defaultProcessList.push_back("devenv");

	defaultCounterList.push_back("% Processor Time");
	defaultCounterList.push_back("Working Set - Private");

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

			PRINT("%s\n", pi.processname().c_str());

			std::string value = "";
			for (int j = 0; j < pi.processid_size(); j++)
			{
				int pid = pi.processid(j);
				PRINT("%d ", pid);

				value += std::to_string(pid)+" ";
			}
			PRINT("\n");

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