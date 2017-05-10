from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^$', views.index, name='index'),
    url(r'^table$', views.table, name='table'),
    url(r'^graph$', views.graph, name='graph'),
    url(r'^sensors$', views.sensors, name='sensors'),
]
