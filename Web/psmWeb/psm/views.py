from django.shortcuts import render
from django.http import HttpResponse

import json

import psm.util
import psm.redisJob
# Create your views here.


def Index(request):
    list = psm.redisJob.GetAgentListToView()
    return render(request, "server_main.html", {"agentList" : list})


def Helloworld(request):

    psm.redisJob.GetMachineValue_recentHourAVG(0, "TotalCpuTime", 10)
    return render(request, "hello.html")


def ServerMain(request, index):
    if not index.isdigit():
        response = render(request, "hello.html")

    print(index)
    print(type(index))
    return render(request, "server_detail.html")