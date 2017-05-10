from django.db import models

class Sensor(models.Model):
    mac_addr = models.CharField(max_length=32)
    name = models.CharField(max_length=64)

class Post(models.Model):
    created = models.DateTimeField(auto_now_add=True)
    temperature = models.IntegerField()
    humidity = models.IntegerField()
    sensor = models.ForeignKey(Sensor, on_delete=models.CASCADE)
