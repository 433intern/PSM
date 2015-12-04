#pragma once

class Disconnector : public Actor
{
public:
	Disconnector(){}

public:
	void ProcEvent(Act* act, DWORD bytes_transferred);
	void ProcError(Act* act, DWORD error);

	void Init(Proactor* proactor);
};