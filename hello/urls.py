from django.conf.urls import url, patterns


urlpatterns = patterns('base.views',
    url(r'^$', 'home', name='home'),
    url(r'^about/$', 'about'),
    url(r'^sensors/$', 'sensors'),
)