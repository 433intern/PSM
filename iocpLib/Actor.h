#pragma once

class Actor
{
public:
	Actor()
	{
		proactor_ = NULL;
	}

	virtual void ProcEvent(Act* act, DWORD bytes_transferred) = 0;
	virtual void ProcError(Act* act, DWORD error) = 0;

public:
	Proactor* proactor_;

};