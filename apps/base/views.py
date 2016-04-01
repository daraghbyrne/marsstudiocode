"""Views for the base app"""

from .models import * 
from .forms import *
from django.http import HttpResponse, Http404, HttpResponseRedirect
from django.template import loader
from django.shortcuts import get_object_or_404, render
from django.db import transaction
from django.shortcuts import render


def home(request):
    context = {}
    return render(request, 'base/home.html', context)
    
def about(request):
    context = {}
    return render(request, 'base/about.html', context)
    
def sensors(request):
    context = {}
    return render(request, 'base/sensors.html', context)