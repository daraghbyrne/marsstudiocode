from django.contrib import admin

from .models import *

class PhotonAdmin(admin.ModelAdmin):
    exclude = ('last_connection',)
admin.site.register(Photon, PhotonAdmin)

class SensorAdmin(admin.ModelAdmin):
    pass
admin.site.register(Sensor, SensorAdmin)
