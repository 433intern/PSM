"""psmWeb URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/1.9/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  url(r'^$', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  url(r'^$', Home.as_view(), name='home')
Including another URLconf
    1. Add an import:  from blog import urls as blog_urls
    2. Import the include() function: from django.conf.urls import url, include
    3. Add a URL to urlpatterns:  url(r'^blog/', include(blog_urls))
"""
from django.conf.urls import url
from django.contrib import admin

urlpatterns = [
    url(r'^admin/', admin.site.urls),
    url(r'^helloworld/', 'psm.views.Helloworld'),
    url(r'^main/', 'psm.views.Main'),
    url(r'^index/', 'psm.views.Index'),
    url(r'^mchart/', 'psm.views.MachineChart'),
    url(r'^mredis/', 'psm.views.MRedis'),

    url(r'^pchart/', 'psm.views.ProcessChart'),
    url(r'^predis/', 'psm.views.PRedis'),

    url(r'^addprocess/([^/]+)/([^/]+)/', 'psm.views.AddProcess'),
    url(r'^deleteprocess/([^/]+)/([^/]+)/', 'psm.views.DeleteProcess'),

    url(r'^server_detail/([^/]+)/', 'psm.views.ServerDetail'),
    url(r'^process_detail/([^/]+)/([^/]+)/', 'psm.views.ProcessDetail'),

    url(r'^setting/([^/]+)/', 'psm.views.Setting'),
    url(r'^addcounter/', 'psm.views.AddCounter'),
    url(r'^deletecounter/', 'psm.views.DeleteCounter'),

]
