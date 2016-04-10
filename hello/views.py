from .models import * 
from django.http import HttpResponse, Http404, HttpResponseRedirect
from django.shortcuts import render
from django.views.decorators.csrf import csrf_exempt


def home(request):
    context = {
        'data': DataPoint.objects.all(),
        'images': Image.objects.all(),
        'photons': Photon.objects.all(),
        'sensors': Sensor.objects.all(),
    }
    return render(request, 'home.html', context)
    
def about(request):
    context = {}
    return render(request, 'about.html', context)
    
def control(request):
    context = {}
    return render(request, 'control.html', context)
    
def sensors(request):
    context = {
        'photons': Photon.objects.all(),
        'sensors': Sensor.objects.all(),
    }
    return render(request, 'sensors.html', context)

@csrf_exempt   
def new_data(request):
    if request.method == "POST":
        #sensor_value, timestamp, sensor_id
        d = DataPoint(sensor_id = request.POST['sensor_id'], value = request.POST['sensor_value'], gathered_at = request.POST['timestamp'])
        d.save()
        return HttpResponse()

@csrf_exempt        
def new_image(request):
    if request.method == "POST":
        #image_url, timestamp, sensor_id
        i = Image(sensor_id = request.POST['sensor_id'], image_address = request.POST['image_url'], gathered_at = request.POST['timestamp'])
        i.save()
        return HttpResponse()