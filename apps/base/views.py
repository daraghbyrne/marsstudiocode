"""Views for the base app"""

from .models import * 
from .forms import *
from django.http import HttpResponse, Http404, HttpResponseRedirect
from django.template import loader
from django.shortcuts import get_object_or_404, render
from django.db import transaction
from django.shortcuts import render


def home(request):

    return render(request, 'base/home.html')
    
def about(request):

    return render(request, 'base/about.html')
    
def sensors(request):

    return render(request, 'base/sensors.html')