from rest_framework import serializers
from api.models import Post
from api.models import Sensor


class SensorSerializer(serializers.ModelSerializer):
    class Meta:
        model = Sensor
        fields = ('mac_addr', 'name')

class PostSerializer(serializers.ModelSerializer):
    sensor = SensorSerializer(read_only=True)
    class Meta:
        model = Post
        fields = ('sensor', 'created', 'temperature', 'humidity')
