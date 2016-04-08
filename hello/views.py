from .models import * 
from django.http import HttpResponse, Http404, HttpResponseRedirect
from django.template import loader
from django.shortcuts import get_object_or_404, render
from django.db import transaction
from django.shortcuts import render


def home(request):
    context = {
        'data': DataPoint.objects.all(),
        'photons': Photon.objects.all(),
        'sensors': Sensor.objects.all(),
    }
    return render(request, 'templates/home.html', context)
    
def about(request):
    context = {}
    return render(request, 'templates/about.html', context)
    
def sensors(request):
    context = {
        'photons': Photon.objects.all(),
        'sensors': Sensor.objects.all(),
    }
    return render(request, 'templates/sensors.html', context)