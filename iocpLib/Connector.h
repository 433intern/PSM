#pragma once

class Connector : public Actor
{
public:
	Connector(){}

public:
	void ProcEvent(Act* act, DWORD bytes_transferred);
	void ProcError(Act* act, DWORD error);

	void Init(Proactor* proactor);
};