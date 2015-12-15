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

    psm.redisJob.GetMachineValue_recentHourAVG(None, 0, "TotalCpuTime", 10)
    return render(request, "hello.html")


def ServerMain(request, hostip):
    if not hostip.isdigit():
        response = render(request, "hello.html")

    r = psm.redisJob.GetRedisClient()
    agent = psm.redisJob.GetAgentInfo(r, hostip)
    psm.redisJob.GetMachineCounterList(r, agent)

    return render(request, "server_detail.html", {})