from django.shortcuts import render
from django.http import HttpResponse

import json

import psm.util
import psm.redisJob
import psm.psmnet
# Create your views here.


def Index(request):
    list = psm.redisJob.GetAgentListToView(None)
    return render(request, "server_main.html", {"agentList" : list})

def Helloworld(request):
    return HttpResponse("no url! error page")
    #psm.redisJob.GetMachineValue_recent(None, 0, "TotalCpuTime", 10, 60)
    #return render(request, "hello.html")

def Main(request):
    return render(request, "index.html")


def ServerDetail(request, token):
    if not token.isdigit():
        response = HttpResponse("no url! error page")

    r = psm.redisJob.GetRedisClient()
    agent = psm.redisJob.GetAgentInfo(r, token)
    cpList = psm.redisJob.GetCurrentProcessList(r, agent)

    mList = psm.redisJob.GetMachineCounterList(r, agent, True)

    checkList = psm.redisJob.GetCheckProcessList(r, agent, cpList)

    psm.redisJob.GetCurrentProcessList(r, agent)
    return render(request, "server_detail.html",
                  {"token" : token, "agent" : agent, "mcl" : mList, "cpl" : cpList, "checkl" : checkList})


def ProcessDetail(request, token, name):
    if not token.isdigit() or not name.isalpha():
        response = render(request, "hello.html")

    print(token)
    print(name)

    r = psm.redisJob.GetRedisClient()
    agent = psm.redisJob.GetAgentInfo(r, token)
    pList = psm.redisJob.GetProcessCounterList(r, agent, name, True)

    jv = {}
    print(pList)

    jv["pList"] = pList
    jv["process"] = name
    jv["token"] = token

    return HttpResponse(json.dumps(jv))

def MRedis(request):
    if request.method != 'POST':
        jv = {}
        return HttpResponse(json.dumps(jv))


    curTime = int(request.POST.get('curTime'))
    responseTime = int(request.POST.get('responseTime'))
    interval = int(request.POST.get('interval'))
    mcl = request.POST.getlist('mcl[]')
    agentNumber = request.POST.get('agentNumber')

    datas = []
    r = psm.redisJob.GetRedisClient()

    for counter in mcl:
        key = ":".join([ str(agentNumber) ,  counter, "Total"])
        data = psm.redisJob.GetValueList(r, key, responseTime, interval, curTime)

        jv = {}
        jv['name'] = counter
        jv['data'] = data
        datas.append(jv)

    return HttpResponse(json.dumps(datas))

def MachineChart(request):
    if request.method != 'POST':
        return HttpResponse("no url! error page")

    mcl = json.loads(request.POST.get('mcl'))
    mcl = map(lambda x: str(x), mcl)
    token = request.POST.get('token')

    r = psm.redisJob.GetRedisClient()
    agent = psm.redisJob.GetAgentInfo(r, token)

    print(request.POST.get('mcl'))
    print(mcl)

    return render(request, "chart.html", {"agentName" : agent['agentName'], "token" : token, "agent" : json.dumps(agent), "mcl" : mcl})

def PRedis(request):
    if request.method != 'POST':
        jv = {}
        return HttpResponse(json.dumps(jv))


    curTime = int(request.POST.get('curTime'))
    responseTime = int(request.POST.get('responseTime'))
    interval = int(request.POST.get('interval'))
    pcl = request.POST.getlist('pcl[]')
    agentNumber = request.POST.get('agentNumber')
    name = request.POST.get('name')

    datas = []
    r = psm.redisJob.GetRedisClient()

    for counter in pcl:
        prevkey = ":".join([ str(agentNumber) ,  counter,  name])
        keys = psm.redisJob.GetProcessKeys(r, prevkey)

        totaldata = []
        for key in keys:
            data = psm.redisJob.GetValueList_detail(r, key, responseTime, interval, curTime)
            totaldata = psm.util.CombineList(totaldata, data)

        jv = {}
        jv['counter'] = counter
        jv['data'] = totaldata
        datas.append(jv)

    return HttpResponse(json.dumps(datas))


def ProcessChart(request):
    if request.method != 'POST':
        return render(request, "hello.html")

    pcl = json.loads(request.POST.get('pcl'))
    pcl = map(lambda x: str(x), pcl)
    token = request.POST.get('token')
    name = request.POST.get('name')

    r = psm.redisJob.GetRedisClient()
    agent = psm.redisJob.GetAgentInfo(r, token)
    print(agent)

    return render(request, "chart_p.html", {"agentName" : agent['agentName'], "name" : name, "token" : token, "agent" : json.dumps(agent), "pcl" : pcl})

def AddProcess(request, token, processName):
    result = psm.psmnet.ProcessCommandToAgentServer(int(token), str(processName), True)
    return HttpResponse(result)

def DeleteProcess(request, token, processName):
    result = psm.psmnet.ProcessCommandToAgentServer(int(token), str(processName), False)
    return HttpResponse(result)

def AddCounter(request):
    token = int(request.POST.get('token'))
    ismachine = (request.POST.get('ismachine') == u'true')
    cl = request.POST.getlist('cl[]')

    print(ismachine)

    result = psm.psmnet.CounterCommandToAgentServer(token, True, ismachine, cl)
    return HttpResponse(result)

def DeleteCounter(request):
    token = int(request.POST.get('token'))
    ismachine = (request.POST.get('ismachine') == u'true')
    cl = request.POST.getlist('cl[]')

    result = psm.psmnet.CounterCommandToAgentServer(token, False, ismachine, cl)

    return HttpResponse(result)



def Setting(request, token):
    r = psm.redisJob.GetRedisClient()
    agent = psm.redisJob.GetAgentInfo(r, token)
    mcl = psm.redisJob.GetMachineCounterList(r, agent, False)
    pcl = psm.redisJob.GetProcessCounterList(r, agent, "", False)
    amcl = psm.redisJob.GetAddableCounterList(True, mcl)
    apcl = psm.redisJob.GetAddableCounterList(False, pcl)

    return render(request, "server_detail2.html", {"token" : token, "mcl" : mcl, "pcl" : pcl, "amcl" : amcl, "apcl" :apcl})





