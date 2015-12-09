#pragma once

class AgentSocket : public TcpSocket
{
public:
	AgentSocket();
	~AgentSocket();

public:
	int agentID;
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
	void SendCounterListResponse();

	int FindAgentID(int hostip);


private:
	MemPooler<CPacket> *packetPoolManager;
	int position;
	int remainBytes;
};