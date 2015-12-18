#pragma once

class AgentSocket : public TcpSocket
{
public:
	AgentSocket();
	~AgentSocket();

public:
	int agentID;
	int hostIP;
	int token;

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
	void SendAgentIDResponse(int agentID, bool alreadyRunning);
	void SendProcessListResponse();
	void SendCounterListResponse(bool isMachine);
	void SendStartRecord(bool isMachine, int totalRecordTime, int responseTime, int interval, long long int delay);
	void SendStopRecord(bool isMachine);
	void SendProcessCommand(agent::ProcessCommandType type, std::string& processName);
	void SendCounterCommand(agent::CounterCommandType type, std::string& counterName, bool isMachine);

	int FindAgentID(int hostip);

	void AddProcessName(std::string& processName);
	void DeleteProcessName(std::string& processName);

	void AddCounterName(std::string& counterName, bool isMachine);
	void DeleteCounterName(std::string& counterName, bool isMachine);


private:
	MemPooler<CPacket> *packetPoolManager;
	int position;
	int remainBytes;

	bool isReady;
};