import struct

import time
import datetime

import socket


# Create your views here.


def CombineList(l1, l2):
    D = {k: v for k, v in l1}

    for key, value in l2:
        if key in D:
            D[key] += value
        else:
            D[key] = value
    result = list(D.items())

    result.sort()

    return result

def int2ip(addr):
    addr = int(socket.htonl(addr))
    return socket.inet_ntoa(struct.pack("!I", addr))


def GetCurTime(strdate, strTime):
    time1 = int(time.mktime(datetime.datetime.strptime(strdate, "%Y-%m-%d").timetuple()))
    temp = strTime.split(":")
    time2 = int(temp[0]) * 3600 + int(temp[1]) * 60 + int(temp[2])

    return time1+time2

def GetCurTime_int():
    return int(time.time())

def timestampToString(time):
    st = datetime.datetime.fromtimestamp(time).strftime('%Y-%m-%d %H:%M:%S')
    return st
