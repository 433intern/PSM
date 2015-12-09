
class Log
{
public:
	DWORD time;
	std::string timestamp;
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

	bool operator<(const ProcessInfo &t) const {
		return (name < t.name);
	}
};

class ProcessInfo_Agent
{
public:
	std::string name;
};
