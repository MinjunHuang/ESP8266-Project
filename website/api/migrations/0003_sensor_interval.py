# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('api', '0002_remove_sensor_interval'),
    ]

    operations = [
        migrations.AddField(
            model_name='sensor',
            name='interval',
            field=models.IntegerField(default=30),
            preserve_default=False,
        ),
    ]
