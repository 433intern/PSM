
class Log
{
public:
	long long int timestamp;
	double value;
};

class MachineCounterEntry
{
public:
	std::string counterName;
	PDH_HCOUNTER counter;
	std::vector<Log> logs;
};

class ProcessCounterEntry
{
public:
	std::string counterName;
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

	bool operator<(const ProcessInfo &t) const {
		return (name < t.name);
	}
};

class ProcessInfo_Agent
{
public:
	std::string name;
};
