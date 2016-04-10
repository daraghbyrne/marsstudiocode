"""
This code is in charge of taking pictures from the 4 cameras
and posting them to the webserver.
"""
import sys
from SimpleCV import Camera
from util import Config, logger
import time
import boto3
from datetime import datetime

PERIOD = 60.0
IMG_POST_FMT = "image_url={}&timestamp={}&sensor_id=2"
IMG_URL_FMT = "http://{}.s3.amazonaws.com/{}"

class CameraCapture(object):

    def __init__(self):
        self.image_index = 0
        
        self.n_cameras = int(Config.get("N_CAMERAS"))

        # Initialize the cameras
        self.cameras = []
        for i in xrange(self.n_cameras):
            try:
                self.cameras.append(Camera(camera_index = i))
            except:
                logger.warning("Error opening camera #"+str(i))
        
        self.n_cameras = len(self.cameras)
        if self.n_cameras == 0:
            logger.warning("Unable to find any cameras")
            exit()

        # Get S3 instance for 
        self.s3 = boto3.resource('s3')
        self.bucket_name = Config.get("BUCKET_NAME")

    def start(self):
        while True:
            try:
                # 1. Capture images from all cameras
                #logger.debug("Capturing Images")
                images = self.get_images()
                # 2. Send them to the remote server
                #logger.debug("Submitting Images")
                self.post_images(images)
            except:
                logger.warning("Unable to retrieve and send images")
    
            # Wait
            time.sleep(PERIOD)

    def get_images(self):
        images = []
        for cam in self.cameras:
            # Get Image from camera
            img = cam.getImage()
            images.append(img)
        return images

    def post_images(self, images):
        for i in xrange(self.n_cameras):
            # Upload image to S3
            img = images[i]
            img_name = Config.get("IMAGES_DIR")+"{}-{}.jpg".format(i, int(time.time()))
            img.save("/tmp/temp_mars_img.jpg")
            with open("/tmp/temp_mars_img.jpg", 'rb') as f:
                self.s3.Object(self.bucket_name, img_name).put(Body=f)

            # Send img url to webserver
            timestamp = datetime.now().isoformat()
            img_url = IMG_URL_FMT.format(self.bucket_name, img_name)
            post_data = IMG_POST_FMT.format(img_url, timestamp)
            http = httplib.HTTPConnection(Config.get("WEBSERVER_HOST_NAME"))
            http.request("POST", Config.get("WEBSERVER_POST_IMG_URI"), post_data)
            response = http.getresponse()
            if response.status != 200:
                logger.warning("Error posting images to webserver. " + response.reason)
            else:
                logger.debug("Image Successful: "+img_url)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "usage: ./camera_process.py <sensor_config> <network config file>"

    # Initialize configuration
    Config.init(sys.argv[1], sys.argv[2])

    # Start
    logger.debug("Starting Cameras Process")
    cc = CameraCapture()
    cc.start()
