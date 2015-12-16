import redis
import json

import time
import datetime

import psm.util

def GetRedisClient():
    r = redis.StrictRedis(host='localhost', port=6379, db=0)
    return r

def GetCounterValue(r, key, date, startTime, endTime):
    if r==None : r = GetRedisClient()

    datalist = r.lrange(key, 0, endTime-startTime)
    result = []

    for l in datalist:
        data = l.decode("utf-8")

        temp = data.split(" ")

        time = temp[0]
        value = temp[1]

        curTime = psm.util.GetCurTime(date, time)

        if startTime <= curTime and endTime > curTime:
            result.append((curTime, float(value)))

        if startTime > curTime: break;

    result.reverse()
    #print(result)
    return result

def GetValueList(r, prevKey, responseTime, interval, curTime):
    endTime = curTime - responseTime
    strTime = psm.util.timestampToString(endTime)
    temp = strTime.split(" ")
    f_d = temp[0]
    startTime = endTime - interval
    strTime = psm.util.timestampToString(startTime)
    temp = strTime.split(" ")
    s_d = temp[0]

    if s_d == f_d:
        key = prevKey + ":" + f_d
        list = GetCounterValue(r, key, f_d, startTime, endTime)
    else:
        key = prevKey + ":" + s_d
        curTime = psm.util.GetCurTime(f_d, "00:00:00")
        list = GetCounterValue(r, key, s_d, startTime, curTime)
        key = prevKey + ":" + f_d
        list.extend(GetCounterValue(r, key, f_d, curTime, endTime))

    return list

def GetRecentValue(r, prevKey, responseTime, interval):
    curTime = psm.util.GetCurTime_int()
    endTime = curTime - responseTime
    list = GetValueList(r, prevKey, responseTime, interval, curTime)
    result = 0
    if len(list)==0 : return 0

    total = 0
    for i in range(len(list)-1):
        time = list[i+1][0] - list[i][0]
        result += list[i][1]*time

        total+= time

    time = endTime - list[len(list)-1][0]

    if time > responseTime : time = responseTime

    result += list[len(list)-1][1]*time
    total+=time

    return round(result/total, 2)



def GetMachineValue_recent(r, number, counter, responseTime, interval):
    prevKey = ":".join([ str(number) ,  counter, "Total"])
    return GetRecentValue(r, prevKey, responseTime, interval)

def GetProcessValue_recent(r, number, counter, processName, pid, responseTime, interval):
    prevKey = ":".join([ str(number), counter, processName, str(pid)])
    return GetRecentValue(r, prevKey, responseTime, interval)

def GetProcessPIDs(cpl, name):
    for data in cpl:
        if data[0]==name : return data[1]
    return []

def GetCheckProcessList(r, agent, curProcessList):
    if r==None : r = GetRedisClient()

    key = str(agent['agentNumber']) + ":ProcessList"
    result = r.smembers(key)

    plist = []
    for data in list(result):
        name = data.decode("UTF-8")
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
        pids = (v.decode("UTF-8")).split(" ")
        pids.pop()
        list.append((k.decode("UTF-8"), pids))

    return list



def GetProcessCounterList(r, agent):
    if r==None : r = GetRedisClient()

    key = str(agent['agentNumber']) + ":CounterList"
    result = r.smembers(key)

    counterList = []

    for data in list(result):
        jsonData = json.loads(data.decode("UTF-8"))
        counterList.append(jsonData)

    return counterList

def GetMachineCounterList(r, agent):
    if r==None : r = GetRedisClient()

    key = str(agent['agentNumber']) + ":MachineCounterList"
    result = r.smembers(key)

    counterList= []

    for data in list(result):
        jsonData = json.loads(data.decode("UTF-8"))
        value = GetMachineValue_recent \
            (r, agent['agentNumber'], jsonData['name'], agent['responseTime'], 60)

        counterList.append( [jsonData['name'], str(value) + jsonData['unit']] )

    return counterList



def GetAgentInfo(r, hostip):
    if r==None : r = GetRedisClient()
    result = r.hmget("AgentList", hostip)


    agentInfo = {}

    if len(result)==0 : return None

    data = json.loads(result[0].decode("UTF-8"))
    return data

def GetAgentListToView(r):
    if r==None : r = GetRedisClient()
    result = r.hgetall("AgentList")

    list = []

    for k, v in result.items():
        agent = json.loads(v.decode("UTF-8"))
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
                resultAgent['memory'] = 100 - round(mv / (agent['ramSize'] / 1024) * 100, 2)
                resultAgent['disk'] = 100

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
        ipvalue = k.decode("UTF-8")
        resultAgent['ip'] = psm.util.int2ip(int(ipvalue))
        resultAgent['index'] = agent['agentNumber']
        resultAgent['ip_int'] = ipvalue

        list.append(resultAgent)
    return list




"""def GetMachineValue_recentHourAVG(number, counter, responseTime):
    time = GetCurTime_() - responseTime
    st = datetime.datetime.fromtimestamp().strftime('%Y-%m-%d %H:%M:%S')"""
