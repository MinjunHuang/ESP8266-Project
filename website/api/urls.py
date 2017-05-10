# Api urls
from django.conf.urls import patterns, url


urlpatterns = patterns(
    'api.views',
    url(r'^post', 'add_measurement'),
    url(r'^list/measurements$', 'list_measurements'),
    url(r'^list/sensors$', 'list_sensors'),
    url(r'^put/sensor$', 'put_sensor'),
)
