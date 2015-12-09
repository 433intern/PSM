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