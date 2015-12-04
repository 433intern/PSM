
class Log
{
public:
	DWORD time;
	double value;
};

class ProcessCounterEntry
{
public:
	std::string processName;
	PDH_HCOUNTER counter;
	int processID;
	std::vector<Log> logs;
};

class ProcessInfo
{
public:
	std::string name;
	int processID;
	int number;
};

class ProcessInfo_Agent
{
public:
	std::string name;
	bool isOn;
};
