#pragma once

class AgentClientSocket : public TcpSocket
{
public:
	AgentClientSocket();
	~AgentClientSocket();

public:
	bool ValidPacket(CPacket *packet);
	void PacketHandling(CPacket *packet);

	void RecvProcess(bool isError, Act* act, DWORD bytes_transferred);
	void SendProcess(bool isError, Act* act, DWORD bytes_transferred);
	void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, Act* act, DWORD bytes_transferred);

	void Bind();
	void Connect(unsigned int ip, WORD port);

public:
	void SendHealthAck();
	void SendAgentIDRequest();
	void SendProcessListRequest();
	void SendCounterListRequest(bool isMachine);
	void SendCurrentProcessList(std::vector<ProcessInfo>& processList);
	void SendRecordResponse(bool isMachine, bool isStart, bool isSuccess, std::string& failMsg);
	void SendAgentReady();

	void SendProcessInfo();
	void SendMachineInfo();


	bool isConnect;

private:
	struct mswsock_s
	{
		LPFN_CONNECTEX ConnectEx;
	} mswsock;

	BOOL LoadMswsock(void);

private:
	MemPooler<CPacket> *packetPoolManager;
	
	int position;
	int remainBytes;
	Query query;
};