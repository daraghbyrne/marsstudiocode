"""
This code is in charge of taking pictures from the 4 cameras
and posting them to the webserver.
"""
import sys
import time
import boto3
import logging
import httplib
import subprocess
import shlex
from logging import handlers
from SimpleCV import Camera
from datetime import datetime

PERIOD = 300.0
IMG_POST_FMT = "image_url={}&timestamp={}&sensor_id=2"
IMG_URL_FMT = "http://{}.s3.amazonaws.com/{}"

class CameraCapture(object):

    def __init__(self):
        self.image_index = 0
        
        # Initialize the cameras
        self.camera = Camera()
        
        # Get S3 instance for 
        self.s3 = boto3.resource('s3')
        self.bucket_name = "edu.cmu.marsstudio"

        # Setup logger
        logger_format_string = '[%(processName)s:%(funcName)s][%(asctime)s][%(levelname)s]%(message)s'
        log_format = logging.Formatter(logger_format_string)
        self.logger = logging.getLogger('')
        self.logger.setLevel(logging.DEBUG)
        fh = logging.handlers.RotatingFileHandler("mars_log_file", maxBytes=(1048576*5), backupCount=7)
        fh.setFormatter(log_format)
        self.logger.addHandler(fh)

    def start(self):
        self.logger.debug("Starting Cameras Process")
        while True:
            try:
                # 1. Capture images from all cameras
                #logger.debug("Capturing Images")
                image = self.camera.getImage()
                # 2. Send them to the remote server
                #logger.debug("Submitting Images")
                self.post_image(image)
            except Exception, e:
                self.logger.warning("Unable to retrieve and send images: "+e.message)
    
            # Wait
            time.sleep(PERIOD)

    def post_image(self, img):
        # Upload image to S3
        img_name = "{}.jpg".format(int(time.time()))
        img.save("/tmp/temp_mars_img.jpg")
        with open("/tmp/temp_mars_img.jpg", 'rb') as f:
            self.s3.Object(self.bucket_name, img_name).put(Body=f)

        # Get the time stamp
        timestamp = datetime.now().isoformat()
        # Create the aws image url
        img_url = IMG_URL_FMT.format(self.bucket_name, img_name)
        # Create the post data
        post_data = IMG_POST_FMT.format(img_url, timestamp)
        # Connect to the server
        """
        http = httplib.HTTPSConnection("marsserver.herokuapp.com")
        # Submit and check the request
        http.request("POST", "new_image", post_data)
        response = http.getresponse()
        if response.status != 200:
            self.logger.warning("Error posting images to webserver. " + response.reason)
        else:
            print img_url
        """
        curl_string = "curl --data \"{}\" -X POST https://marsserver.herokuapp.com/new_image/"
        curl_string = curl_string.format(post_data)
        subprocess.call(shlex.split(curl_string))

if __name__ == "__main__":
    # Start
    cc = CameraCapture()
    cc.start()
