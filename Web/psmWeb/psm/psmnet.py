
import psm.WebProtocol_pb2
import socket
import struct

def GetSocket():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('127.0.0.1', 9001))

    return s

def SendHealthAck(s):
    length = 0
    type = psm.WebProtocol_pb2.HealthAck

    data = struct.pack('hh', length, type)

    s.send(data)

def SendProcessCommandRequest(s, token, msgtype, name):
    type = psm.WebProtocol_pb2.ProcessCommandRequest

    msg = psm.WebProtocol_pb2.wsProcessCommandRequest()
    msg.token = token
    msg.type = msgtype
    msg.processName = name

    smsg= msg.SerializeToString()
    length = len(smsg)

    data = struct.pack('hh'+str(length)+'s', length, type, smsg)

    s.send(data)

def ProcessCommandResult(data):
    msg = psm.WebProtocol_pb2.swProcessCommandResponse()
    msg.ParseFromString(data)
    print(msg.token)
    print(msg.result)
    print(msg.type)

    return msg.result

def ProcessCommandToAgentServer(token, processname, isadd):
    header_size = 4

    s = GetSocket()

    if isadd : SendProcessCommandRequest(s, token, psm.WebProtocol_pb2.ADDLIST, processname)
    else : SendProcessCommandRequest(s, token, psm.WebProtocol_pb2.DELETELIST, processname)

    result = False
    while True:
        header = s.recv(header_size)

        if len(header)==0 : break
        length, type = struct.unpack('hh', header)

        print(length)
        print(type)

        if length!=0 :
            data = s.recv(length)

            if type==psm.WebProtocol_pb2.HealthCheck:
                print("Received healthcheck")
                SendHealthAck(s)
            elif type==psm.WebProtocol_pb2.ProcessCommandResponse:
                print("Received ProcessCommandResponse")
                result = ProcessCommandResult(data)
                break
    s.close()

    return result



