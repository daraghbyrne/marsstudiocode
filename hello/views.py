from .models import * 
from django.http import HttpResponse, Http404, HttpResponseRedirect
from django.shortcuts import render
from django.views.decorators.csrf import csrf_exempt
from datetime import datetime, timedelta
import csv
from .images2gif import Image as ImagePackage
from .images2gif import writeGif
import os
import urllib.request
import json

def get_csv(request):
    # Create the HttpResponse object with the appropriate CSV header.
    response = HttpResponse(content_type='text/csv')
    response['Content-Disposition'] = 'attachment; filename="marsdata.csv"'

    writer = csv.writer(response)
    writer.writerow(["Photon Name", "Sensor Type", "Sensor Description", "Sensor Value", "Sensor Units", "Date and Time"])
    for d in DataPoint.objects.order_by('-gathered_at'):
        writer.writerow([str(d.sensor_id.photon_id.name), str(d.sensor_id.sensor_type), str(d.sensor_id.description), d.value, str(d.sensor_id.units), str(d.gathered_at)])

    return response

def home(request):
    context = {
        'data': DataPoint.objects.order_by('-gathered_at')[:30],
        'images': Image.objects.order_by('-gathered_at')[:20],
        'photons': Photon.objects.order_by('name'),
        'sensors': Sensor.objects.order_by('photon_id', 'sensor_type'),
        'data_pts': json.dumps([float(d[0]) for d in DataPoint.objects.order_by('-gathered_at').values_list('value')[:30]]),
        'data_dates': json.dumps([str(d[0]) for d in DataPoint.objects.order_by('-gathered_at').values_list('gathered_at')[:30]]),
    }
    return render(request, 'home.html', context)
    
def about(request):
    context = {}
    return render(request, 'about.html', context)
    
def time_lapse(request):
    #FROM: http://stackoverflow.com/questions/753190/programmatically-generate-video-or-animated-gif-in-python
    file_names = Image.objects.all().values_list('image_address')[:50]

    images = []
    for fn in file_names:
        with urllib.request.urlopen(fn[0]) as response:
            images.append(ImagePackage.open(response))
    
    ## writeGif(filename, images, duration=0.1, loops=0, dither=1)
    ##    Write an animated gif from the specified images.
    ##    images should be a list of numpy arrays of PIL images.
    ##    Numpy images of type float should have pixels between 0 and 1.
    ##    Numpy images of other types are expected to have values between 0 and 255.
    
    filename = "marsgif.gif"
    #writeGif(filename, images)
    
    context = {
        'filename': ("../" + filename)
    }
    return render(request, 'time_lapse.html', context)
    
def pictures(request):
    context = {
        'images': Image.objects.order_by('-gathered_at'),
    }
    return render(request, 'pictures.html', context)
    
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
        #sensor_value, timestamp, sensor_id)
        sen = Sensor.objects.get(id = int(request.POST.get('sensor_id')))
        d = DataPoint(sensor_id = sen, value = float(request.POST.get('sensor_value', 0.0)), gathered_at = (request.POST.get('timestamp', 0)))
        d.save()
        return HttpResponse()

@csrf_exempt        
def new_image(request):
    if request.method == "POST":
        #image_url, timestamp, sensor_id
        sen = Sensor.objects.get(id = int(request.POST.get('sensor_id', 0)))
        i = Image(sensor_id = sen, image_address = request.POST.get('image_url', 0), gathered_at = (request.POST.get('timestamp', 0)))
        i.save()
        return HttpResponse()