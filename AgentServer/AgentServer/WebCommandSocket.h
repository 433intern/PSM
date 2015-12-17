#pragma once

class WebCommandSocket : public TcpSocket
{
public:
	WebCommandSocket();
	~WebCommandSocket();

public:
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

private:
	MemPooler<CPacket> *packetPoolManager;
	int position;
	int remainBytes;
};