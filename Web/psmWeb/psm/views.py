from django.shortcuts import render
from django.http import HttpResponse

# Create your views here.


def Helloworld(request):
    return render(request, "hello.html")