import redis
import json

import time
import datetime

import psm.util

def GetRedisClient():
    r = redis.StrictRedis(host='localhost', port=6379, db=0)
    return r

def GetCounterValue(r, key, startTime, endTime):
    if r==None : r = GetRedisClient()

    datalist = r.lrange(key, 0, endTime-startTime)
    result = []

    for l in datalist:
        data = l#.decode("utf-8")

        temp = data.split(" ")

        time = temp[0]
        value = temp[1]

        curTime = int(time)

        if startTime <= curTime and endTime > curTime:
            result.append((curTime, float(value)))

        if startTime > curTime: break;

    result.reverse()
    #print(result)
    return result

def GetValueList(r, key, responseTime, interval, curTime):
    endTime = curTime - responseTime
    startTime = endTime - interval

    list = GetCounterValue(r, key, startTime, endTime)

    return list

def GetValueList_detail(r, key, responseTime, interval, curTime):
    endTime = curTime - responseTime
    startTime = endTime - interval
    list = GetCounterValue(r, key, startTime, endTime)

    result = []

    if len(list)==0 : return result

    for i in range(len(list)-1):
        start_t = list[i][0]
        end_t = list[i+1][0]
        for time in range(start_t, end_t):
            if time-start_t <= responseTime : result.append([time, list[i][1]])
            else: result.append([time, 0])

    start_t = list[len(list)-1][0]
    for time in range(start_t, endTime):
        if time-start_t <= responseTime : result.append([time, list[len(list)-1][1]])
        else: result.append([time, 0])

    return result





def GetRecentValue(r, key, responseTime, interval):
    curTime = psm.util.GetCurTime_int()
    endTime = curTime - responseTime
    list = GetValueList(r, key, responseTime, interval, curTime)
    result = 0
    if len(list)==0 : return 0

    total = 0
    for i in range(len(list)-1):
        time = list[i+1][0] - list[i][0]

        if time > responseTime : time = responseTime

        result += list[i][1]*time

        total+= time

    time = endTime - list[len(list)-1][0]

    if time > responseTime : time = responseTime

    result += list[len(list)-1][1]*time
    total+=time

    return round(result/total, 2)

def GetProcessKeys(r, prevkey):
    if r==None : r = GetRedisClient()
    keys = []
    for data in r.keys(prevkey + ":*"):
        keys.append(data)#.decode("UTF-8"))

    print(keys)

    return keys

"""def GetRecentValueTotal(r, prevKey, responseTime, interval):
    curTime = psm.util.GetCurTime_int()
    endTime = curTime - responseTime
    startTime = endTime - interval

    strTime = psm.util.timestampToString(startTime)
    temp = strTime.split(" ")
    s_d = temp[0]

    #curTime = psm.util.timestampToString(s_d, "00:00:00")

    prevkeys = []

    time = psm.util.GetCurTime(s_d, "00:00:00")
    delta = 3600 * 24

    while time < endTime:
        date = psm.util.timestampToString(time).split(" ")[0]
        l = r.keys(prevKey + ":*:" + date)


        key = prevKey + ":" + s_d
        list.extend(GetCounterValue(r, key, s_d, startTime, curTime))
        startTime = curTime
        s_d = psm.util.timestampToString(curTime).split(" ")[0]

        time+=delta"""

def GetMachineValue_recent(r, number, counter, responseTime, interval):
    key = ":".join([ str(number) ,  counter, "Total"])
    return GetRecentValue(r, key, responseTime, interval)

def GetProcessValue_recent(r, number, counter, processName, pid, responseTime, interval):
    key = ":".join([ str(number), counter, processName, str(pid)])
    return GetRecentValue(r, key, responseTime, interval)


def GetProcessPIDs(cpl, name):
    for data in cpl:
        if data['name']==name : return data['pids']
    return []

def GetCheckProcessList(r, agent, curProcessList):
    if r==None : r = GetRedisClient()

    key = str(agent['agentNumber']) + ":ProcessList"
    result = r.smembers(key)

    plist = []
    for data in list(result):
        name = str(data)#.decode("UTF-8"))
        pids = GetProcessPIDs(curProcessList, name)
        cpuValue = 0
        memoryValue = 0

        elem = {}
        for pid in pids:
            cpuValue += GetProcessValue_recent(r, agent['agentNumber'],
                            "CPUTime", name, pid, agent['responseTime'], 60)
            memoryValue += GetProcessValue_recent(r, agent['agentNumber'],
                            "Memory", name, pid, agent['responseTime'], 60)
        elem['name'] = name
        elem['cpuValue'] = cpuValue
        elem['memoryValue'] = round(memoryValue / (agent['ramSize'] / 1024) * 100, 2)

        if len(pids)==0:
            elem['state'] = "Stop"
            elem['color'] = "red"
        else:
            elem['state'] = "Run"
            elem['color'] = "green"

        plist.append(elem)
    return plist




def GetCurrentProcessList(r, agent):
    if r==None : r = GetRedisClient()

    key = str(agent['agentNumber']) + ":CurrentProcessList"
    result = r.hgetall(key)

    list = []

    for k, v in result.items():
        data = {}
        pids = v.split(" ")
        pids.pop()

        data["pids"] = pids
        data["name"] = str(k)#.decode("UTF-8"))
        list.append(data)

    return list



def GetProcessCounterList(r, agent, name):
    if r==None : r = GetRedisClient()

    key = str(agent['agentNumber']) + ":CounterList"
    result = r.smembers(key)

    counterList = []

    for data in list(result):
        jsonData = json.loads(data)#.decode("UTF-8"))

        keys = GetProcessKeys(r, str(agent['agentNumber'])+":"+jsonData['name']+":"+name)
        value = 0
        for key in keys:
            value += GetRecentValue \
                (r, key, agent['responseTime'], 60)
        counterList.append([str(jsonData['name']), str(str(value) + jsonData['unit'])])

    return counterList

def GetMachineCounterList(r, agent):
    if r==None : r = GetRedisClient()

    key = str(agent['agentNumber']) + ":MachineCounterList"
    result = r.smembers(key)

    counterList= []

    for data in list(result):
        jsonData = json.loads(data)
        value = GetMachineValue_recent \
            (r, agent['agentNumber'], jsonData['name'], agent['responseTime'], 60)

        counterList.append( [str(jsonData['name']), str(str(value) + jsonData['unit'])] )

    return counterList



def GetAgentInfo(r, token):
    if r==None : r = GetRedisClient()
    result = r.hmget("AgentList", token)
    if len(result)==0 : return None
    data = json.loads(result[0])#.decode("UTF-8"))
    return data

def GetAgentListToView(r):
    if r==None : r = GetRedisClient()
    result = r.hgetall("AgentList")

    list = []

    for k, v in result.items():
        agent = json.loads(v)#.decode("UTF-8"))
        resultAgent = {}

        state=""
        color="black"
        if agent['isOn'] == True:
            state = "Run"
        else:
            state = "Stop"


        if agent['isRecording'] == True:
            t = agent['startTime'].split(" ")
            if psm.util.GetCurTime_int() > psm.util.GetCurTime(t[0], t[1]):
                state += "(Recording)"
                color = "green"

                resultAgent['recording'] = True
                resultAgent['cpu'] = GetMachineValue_recent \
                    (r, agent['agentNumber'], "CPUTime", agent['responseTime'], 60)

                mv =  GetMachineValue_recent \
                    (r, agent['agentNumber'], "Memory", agent['responseTime'], 60)
                resultAgent['memory'] = round(100 - (mv / (agent['ramSize'] / 1024) * 100), 2)
                resultAgent['disk'] = round(GetMachineValue_recent \
                    (r, agent['agentNumber'], "Disk", agent['responseTime'], 60), 2)

            else:
                state += "(Ready)"

                resultAgent['recording'] = False
                resultAgent['cpu'] = 0
                resultAgent['memory'] = 0
                resultAgent['disk'] = 0
        else:
            state += "(Stop)"
            color = "red"

            resultAgent['recording'] = False
            resultAgent['cpu'] = 0
            resultAgent['memory'] = 0
            resultAgent['disk'] = 0

        resultAgent['state'] = state
        resultAgent['color'] = color
        resultAgent['name'] = agent['agentName']
        ipvalue = agent['hostIP']
        resultAgent['ip'] = psm.util.int2ip(int(ipvalue))
        resultAgent['index'] = agent['agentNumber']
        resultAgent['token'] = k#.decode("UTF-8")

        list.append(resultAgent)
    return list




"""def GetMachineValue_recentHourAVG(number, counter, responseTime):
    time = GetCurTime_() - responseTime
    st = datetime.datetime.fromtimestamp().strftime('%Y-%m-%d %H:%M:%S')"""
