import redis
import json

import time
import datetime

import psm.util

def GetRedisClient():
    r = redis.StrictRedis(host='localhost', port=6379, db=0)
    return r

def GetCounterValue(key, date, startTime, endTime):
    r = GetRedisClient()

    datalist = r.lrange(key, 0, endTime-startTime)

    i= -1 * r.llen(key)

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

def GetMachineValue_recentHourAVG(number, counter, responseTime):
    interval = 20

    endTime = psm.util.GetCurTime_int() - responseTime
    strTime = psm.util.timestampToString(endTime)

    temp = strTime.split(" ")
    f_d = temp[0]
    f_t = temp[1]

    startTime = endTime - interval
    strTime = psm.util.timestampToString(startTime)

    temp = strTime.split(" ")
    s_d = temp[0]
    s_t = temp[1]


    list = []

    if s_d == f_d:
        key = ":".join([ str(number) ,  counter, "Total", f_d])
        list = GetCounterValue(key, f_d, startTime, endTime)
    else:
        key = ":".join([ str(number) ,  counter, "Total", s_d])

        curTime = psm.util.GetCurTime(f_d, "00:00:00")

        list = GetCounterValue(key, s_d, startTime, curTime)

        key = ":".join([ str(number) ,  counter, "Total", f_d])
        list.extend(GetCounterValue(key, f_d, curTime, endTime))

    result = 0
    if len(list)==0 : return 0

    startData = list[0]
    total = 0


    for i in range(len(list)-1):
        time = list[i+1][0] - list[i][0]
        result += list[i][1]*time

        total+= time

    time = endTime - list[len(list)-1][0]
    result += list[len(list)-1][1]*time
    total+=time

    return int(result/total)


def GetMachineCounterList(agentID):
    r = GetRedisClient()
    


def GetAgentListToView():
    r = GetRedisClient()
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
                resultAgent['cpu'] = GetMachineValue_recentHourAVG \
                    (agent['agentNumber'], "TotalCpuTime", agent['responseTime'])

                resultAgent['memory'] = 100
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
        temp = int(k.decode("UTF-8"))
        resultAgent['ip'] = psm.util.int2ip(temp)
        resultAgent['index'] = agent['agentNumber']

        list.append(resultAgent)
    return list




"""def GetMachineValue_recentHourAVG(number, counter, responseTime):
    time = GetCurTime_() - responseTime
    st = datetime.datetime.fromtimestamp().strftime('%Y-%m-%d %H:%M:%S')"""
