#pragma once

class Sender : public Actor
{
public:
	Sender(){};

public:
	void ProcEvent(Act* act, DWORD bytes_transferred);
	void ProcError(Act* act, DWORD error);

	void Init(Proactor* proactor);
};