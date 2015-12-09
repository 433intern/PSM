#pragma once

#include "../../redisLib/lib/redissyncclient.h"

class RedisManager
{
public:
	RedisManager(std::string redisIP, unsigned short port);
	~RedisManager();

	void Init();
	int GetAgentID(int hostIP);

private:
	std::string redisIP;
	const unsigned short port;
	boost::asio::io_service ioService;
	RedisSyncClient redis;
};