"""
{
  "token_type": "bearer",
  "access_token": "b42568c8da6b363f90e114bfe9dce5b1c27fc3f1",
  "expires_in": 0,
  "refresh_token": "3894e5e88fdacd42e4815481bb186cd7a954d190"
}
"""
import httplib, json
PHOTON_ID = "34003f000747343232363230"
ACCESS_TOKEN = "b42568c8da6b363f90e114bfe9dce5b1c27fc3f1"
API_URL = "api.particle.io"

uri = "/v1/devices/{}/a0".format(PHOTON_ID)
http = httplib.HTTPSConnection(API_URL)

http.request("GET", uri+"?access_token={}".format(ACCESS_TOKEN))
response = http.getresponse()
data = response.read()
data = json.loads(data)
print data["result"]