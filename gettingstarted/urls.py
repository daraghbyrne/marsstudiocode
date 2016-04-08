from django.conf.urls import include, url

from django.contrib import admin
admin.autodiscover()

import hello.views

# Examples:
# url(r'^$', 'gettingstarted.views.home', name='home'),
# url(r'^blog/', include('blog.urls')),

urlpatterns = [
    url(r'^$', 'helllo.views.home', name='home'),
    url(r'^about/$', 'hello.views.about', name='about'),
    url(r'^sensors/$', 'hello.views.sensors', name='sensors'),
    # Admin
    url(r'^admin/', include(admin.site.urls)),
]
