#pragma once

class Receiver : public Actor
{
public:
	Receiver(){};

public:
	void ProcEvent(Act* act, DWORD bytes_transferred);
	void ProcError(Act* act, DWORD error);

	void Init(Proactor* proactor);

};