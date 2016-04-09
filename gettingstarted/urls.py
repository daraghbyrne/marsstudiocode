from django.conf.urls import include, url

from django.contrib import admin
admin.autodiscover()

import hello.views

# Examples:
# url(r'^$', 'gettingstarted.views.home', name='home'),
# url(r'^blog/', include('blog.urls')),

urlpatterns = [
    url(r'^$', hello.views.home, name='home'),
    url(r'^about/$', hello.views.about, name='about'),
    url(r'^sensors/$', hello.views.sensors, name='sensors'),
    url(r'^control/$',hello.views.control, name='control'),
    # Admin
    url(r'^admin/', include(admin.site.urls)),
    # Getting data from server
    url(r'^new_data/$',hello.views.new_data, name='new_data'),
    url(r'^new_image/$',hello.views.new_image, name='new_image'),
]
