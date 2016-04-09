from django.db import models

class Photon(models.Model):
    name = models.CharField(max_length=150)
    email_association = models.CharField(max_length=150)
    last_connection = models.DateTimeField(null=True)
    location_description = models.CharField(max_length=500)

class Sensor(models.Model):
    photon_id = models.ForeignKey(Photon)
    sensor_type = models.CharField(max_length=150)
    description = models.CharField(max_length=300)
    units = models.CharField(max_length=50)
    
class DataPoint(models.Model):
    sensor_id = models.ForeignKey(Sensor)
    value = models.DecimalField(max_digits=5, decimal_places=2)
    gathered_at = models.DateTimeField('time gathered')
    
class Image(models.Model):
    #a camera is a sensor, but it'll have a dummy photon because it's hooked up to the computer
    sensor_id = models.ForeignKey(Sensor)
    image_address = models.CharField(max_length=500, null=True)
    gathered_at = models.DateTimeField('time gathered')