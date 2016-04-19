from .models import * 
from django.http import HttpResponse, Http404, HttpResponseRedirect
from django.shortcuts import render
from django.views.decorators.csrf import csrf_exempt
from datetime import datetime, timedelta
import csv

def get_csv(request):
    # Create the HttpResponse object with the appropriate CSV header.
    response = HttpResponse(content_type='text/csv')
    response['Content-Disposition'] = 'attachment; filename="marsdata.csv"'

    writer = csv.writer(response)
    for d in DataPoint.objects.order_by('-gathered_at'):
        writer.writerow([str(d.name), str(d.sensor_type), str(d.description), d.value, str(d.units), str(d.gathered_at)])

    return response

def home(request):
    context = {
        'data': DataPoint.objects.order_by('-gathered_at')[:50],
        'images': Image.objects.order_by('-gathered_at')[:50],
        'photons': Photon.objects.order_by('name'),
        'sensors': Sensor.objects.order_by('photon_id', 'sensor_type'),
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