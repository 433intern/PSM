from django.shortcuts import render
from django.http import HttpResponse

import json

import psm.util
import psm.redisJob
# Create your views here.


def Index(request):
    list = psm.redisJob.GetAgentListToView(None)
    return render(request, "server_main.html", {"agentList" : list})

def Helloworld(request):
    psm.redisJob.GetProcessKeys(None, "9:CPUTime:KakaoTalk")
    return HttpResponse("hh")
    #psm.redisJob.GetMachineValue_recent(None, 0, "TotalCpuTime", 10, 60)
    #return render(request, "hello.html")


def ServerDetail(request, token):
    if not token.isdigit():
        response = render(request, "hello.html")

    r = psm.redisJob.GetRedisClient()
    agent = psm.redisJob.GetAgentInfo(r, token)
    cpList = psm.redisJob.GetCurrentProcessList(r, agent)

    mList = psm.redisJob.GetMachineCounterList(r, agent)

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
    pList = psm.redisJob.GetProcessCounterList(r, agent, name)

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
        return render(request, "hello.html")

    mcl = json.loads(request.POST.get('mcl'))
    token = request.POST.get('token')

    r = psm.redisJob.GetRedisClient()
    agent = psm.redisJob.GetAgentInfo(r, token)
    print(agent)

    return render(request, "chart.html", {"token" : token, "agent" : agent, "mcl" : mcl})