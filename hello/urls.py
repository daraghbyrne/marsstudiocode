from django.conf.urls import url, patterns


urlpatterns = patterns('hello.views',
    url(r'^$', 'home', name='home'),
    url(r'^about/$', 'about'),
    url(r'^sensors/$', 'sensors'),
)