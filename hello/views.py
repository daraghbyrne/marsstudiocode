from .models import * 
from django.http import HttpResponse, Http404, HttpResponseRedirect
from django.shortcuts import render


def home(request):
    context = {
        'data': DataPoint.objects.all(),
        'photons': Photon.objects.all(),
        'sensors': Sensor.objects.all(),
    }
    return render(request, 'home.html', context)
    
def about(request):
    context = {}
    return render(request, 'about.html', context)
    
def sensors(request):
    context = {
        'photons': Photon.objects.all(),
        'sensors': Sensor.objects.all(),
    }
    return render(request, 'sensors.html', context)