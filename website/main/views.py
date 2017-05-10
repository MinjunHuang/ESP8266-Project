from django.http import HttpResponse
from django.template import loader
from django.template import RequestContext
from django.shortcuts import redirect
from django.shortcuts import render_to_response

def index(request):
    return redirect('table')

def table(request):
    template = loader.get_template('main/table.html')
    return HttpResponse(template.render())

def graph(request):
    template = loader.get_template('main/graph.html')
    return HttpResponse(template.render())

def sensors(request):
    return render_to_response('main/sensors.html',
            context_instance=RequestContext(request),
            )
