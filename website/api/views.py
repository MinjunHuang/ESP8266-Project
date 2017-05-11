from django.shortcuts import render
from django.http import HttpResponse
from rest_framework.decorators import api_view
from rest_framework.response import Response
from api.models import Post
from api.models import Sensor
from api.serializers import PostSerializer
from api.serializers import SensorSerializer
from django.utils.datastructures import MultiValueDictKeyError
import datetime


@api_view(['POST'])
def add_measurement(request):
    temp = request.POST.get('temperature', None)
    hum = request.POST.get('humidity', None)
    mac = request.POST.get('mac', None)
    if mac is None:
        return Response()

    sensors = Sensor.objects.filter(mac_addr=mac)
    if sensors.count() == 0:
        sensor = Sensor(mac_addr=mac, name=mac)
        sensor.save()
    else:
        sensor = sensors[0]
    post = Post(temperature=temp, humidity=hum, sensor=sensor)
    post.save()
    return Response(sensor.interval)

@api_view(['GET'])
def list_measurements(request):
    try:
        beginDate=request.GET['beginDate']
        datetime.datetime.strptime(beginDate, "%Y-%m-%d")
    except (ValueError, MultiValueDictKeyError) as e:
        beginDate='0001-01-01'
    try:
        endDate=request.GET['endDate']
        datetime.datetime.strptime(endDate, "%Y-%m-%d")
    except (ValueError, MultiValueDictKeyError) as e:
        endDate='9999-12-31'

    try:
        mac=request.GET['mac']
    except MultiValueDictKeyError:
        mac=''

    if mac == '':
        posts = Post.objects.filter(created__range=[beginDate, endDate])
    else:
        posts = Post.objects.filter(created__range=[beginDate, endDate], sensor__mac_addr=mac)
    serializer = PostSerializer(posts, many=True)
    return Response(serializer.data)

@api_view(['GET'])
def list_sensors(request):
    sensors = Sensor.objects.all()
    serializer = SensorSerializer(sensors, many=True)
    return Response(serializer.data)

@api_view(["PUT"])
def put_sensor(request):
    mac = request.GET['mac']
    name = request.GET['name']
    interval = request.GET['interval']
    sensor = Sensor.objects.get(mac_addr=mac)
    sensor.name = name
    sensor.interval = interval
    sensor.save()
    return Response()

