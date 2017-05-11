# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Post',
            fields=[
                ('id', models.AutoField(serialize=False, primary_key=True, auto_created=True, verbose_name='ID')),
                ('created', models.DateTimeField(auto_now_add=True)),
                ('temperature', models.IntegerField()),
                ('humidity', models.IntegerField()),
            ],
        ),
        migrations.CreateModel(
            name='Sensor',
            fields=[
                ('id', models.AutoField(serialize=False, primary_key=True, auto_created=True, verbose_name='ID')),
                ('mac_addr', models.CharField(max_length=32)),
                ('name', models.CharField(max_length=64)),
                ('interval', models.IntegerField()),
            ],
        ),
        migrations.AddField(
            model_name='post',
            name='sensor',
            field=models.ForeignKey(to='api.Sensor'),
        ),
    ]
