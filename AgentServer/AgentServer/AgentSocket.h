#pragma once

class AgentSocket : public TcpSocket
{
public:
	AgentSocket();
	~AgentSocket();

public:
	int agentID;
	int hostIP;

	bool healthCheck;

	bool ValidPacket(CPacket *packet);
	void PacketHandling(CPacket *packet);

	void RecvProcess(bool isError, Act* act, DWORD bytes_transferred);
	void SendProcess(bool isError, Act* act, DWORD bytes_transferred);
	void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, Act* act, DWORD bytes_transferred);


public:
	void SendHealthCheck();
	void SendAgentIDResponse(int agentID);
	void SendProcessListResponse();
	void SendCounterListResponse(bool isMachine);
	void SendStartRecord(bool isMachine, int totalRecordTime, int responseTime, int interval, long long int delay);
	void SendStopRecord(bool isMachine);
	void SendProcessCommand(agent::ProcessCommandType type, std::string& processName);
	void SendCounterCommand(agent::CounterCommandType type, std::string& counterName, bool isMachine);

	int FindAgentID(int hostip);

	bool AddProcessName(std::string& processName);
	bool DeleteProcessName(std::string& processName);

	bool AddCounterName(std::string& counterName, bool isMachine);
	bool DeleteCounterName(std::string& counterName, bool isMachine);


private:
	MemPooler<CPacket> *packetPoolManager;
	int position;
	int remainBytes;

	bool isReady;
};