#include <math.h>

//#include "PietteTech_DHT/PietteTech_DHT.h"  // Uncomment if building in IDE
#include "PietteTech_DHT.h"  // Uncommend if building using CLI

#include "rest_client.h"

RestClient rest_client = RestClient("marsserver.herokuapp.com");
String rest_response;

const bool SHOW_DEBUG_INFO = true;

#define DHTTYPE  DHT22       // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN   3           // Digital pin for communications

//declaration
void dht_wrapper(); // must be declared before the lib initialization

// Lib instantiate
PietteTech_DHT DHT(DHTPIN, DHTTYPE, dht_wrapper);


/*
  WIRING FOR THE CO2 Sensor
  Black = GND
  Red = 3v3
  Blue = Analog Signal
*/

#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interval(in milisecond) between each samples in

#define         DC_GAIN                      (8.5)   //define the DC gain of amplifier
#define         ZERO_POINT_VOLTAGE           (0.324) //define the output of the sensor in volts when the concentration of CO2 is 400PPM
#define         REACTION_VOLTAGE             (0.020) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2

int             co2_pin = A1;

float           CO2Curve[3]  =  { 2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTAGE/(2.602-3))};
int             co2_percentage;
float           co2_volts;


float humidity;
float dewPoint;
float temperature;

Timer publishTimer( 60000, publishCurrentState );


// Server Sensor Ids
int co2SensorId = 1;
int humiditySensorId = 1;
int dewPointSensorId = 1;
int tempSensorId = 1;


void setup()
{
    Time.zone(-5);

    publishTimer.start();

    Serial.begin(9600);

    setupServerSensorIds();

    //while (!Serial.available()) {
    //    Serial.println("Press any key to start.");
    //    delay (1000);
    //}

    //Serial.println("DHT Example program using DHT.acquireAndWait");
    //Serial.print("LIB version: ");
    //Serial.println(DHTLIB_VERSION);
    //Serial.println("---------------");

    publishCurrentState();


}

// This wrapper is in charge of calling
// must be defined like this for the lib work
void dht_wrapper() {
    DHT.isrCallback();
}

void loop()
{
  //publishCurrentState( );
  //delay( 10000 );
}



void setupServerSensorIds()
{
  String myID = System.deviceID();

  Serial.println( "Device ID:" );
  Serial.println( myID );

  //  Decomp - Studio 3
  if( myID.equals( "2a0018000947353138383138" ) )
  {
    Serial.println( "Matches" );

    co2SensorId = 23;
    humiditySensorId = 28;
    dewPointSensorId = 32;
    tempSensorId = 36;
  }

  //  Plant Layer Ambient
  if( myID.equals( "1a003b000647353138383138" ) )
  {
    Serial.println( "Matches" );

    co2SensorId = 37;
    humiditySensorId = 38;
    dewPointSensorId = 39;
    tempSensorId = 41;

  }

}



void publishCurrentState( )
{
  //String stateStr = String( currentState );
  //Particle.publish( "currentState", stateStr );
  readSensors();

  printDebugInfo();

  postReadingToServer( DHT.getHumidity(), "humidity", humiditySensorId );
  postReadingToServer( DHT.getDewPoint(), "dewPoint", dewPointSensorId );
  postReadingToServer( DHT.getCelsius(), "temperature", tempSensorId );
  postReadingToServer( co2_percentage, "co2" , co2SensorId );

}

void readSensors()
{
    readTemperature();

    humidity = DHT.getHumidity();
    dewPoint = DHT.getDewPoint();
    temperature = DHT.getCelsius();

    // Get the CO2 concentration;
    co2_volts = MGRead( co2_pin );
    co2_percentage = MGGetPercentage(co2_volts,CO2Curve);

}



void postReadingToServer( int reading, char* reading_type, int sensor_id )
{
  time_t time = Time.now();

  String timeString = String( Time.format(time, TIME_FORMAT_ISO8601_FULL) );
  timeString = timeString.replace("T", " ");

  String post_data = "sensor_value=" + String( reading ) + "&timestamp=" + timeString + "&sensor_id=" + String( sensor_id )  ;  ;

  Serial.println( "INT POST:" );
  Serial.println( post_data );

  int statusCode = rest_client.post("/new_data/", post_data, &rest_response);

}

void postReadingToServer( float reading, char* reading_type, int sensor_id )
{
  time_t time = Time.now();

  String timeString = String( Time.format(time, TIME_FORMAT_ISO8601_FULL) );
  timeString = timeString.replace("T", " ");

  String post_data = "sensor_value=" + String( reading ) + "&timestamp=" + timeString + "&sensor_id=" + String( sensor_id )  ;  ;

  Serial.println( "FLOAT POST:" );
  Serial.println( post_data );

  int statusCode = rest_client.post("/new_data/", post_data, &rest_response);

}


void postReadingToServer( double reading, char* reading_type, int sensor_id )
{
  time_t time = Time.now();

  String timeString = String( Time.format(time, TIME_FORMAT_ISO8601_FULL) );
  timeString = timeString.replace("T", " ");

  String post_data = "sensor_value=" + String( reading ) + "&timestamp=" + timeString + "&sensor_id=" + String( sensor_id )  ;  ;

  Serial.println( "FLOAT POST:" );
  Serial.println( post_data );

  int statusCode = rest_client.post("/new_data/", post_data, &rest_response);

}



void readTemperature()
{
  Serial.print("Retrieving information from sensor: ");
  Serial.print("Read sensor: ");

  int result = DHT.acquireAndWait( 1000 );

  switch (result) {
      case DHTLIB_OK:
          Serial.println("OK");
          break;
      case DHTLIB_ERROR_CHECKSUM:
          Serial.println("Error\n\r\tChecksum error");
          break;
      case DHTLIB_ERROR_ISR_TIMEOUT:
          Serial.println("Error\n\r\tISR time out error");
          break;
      case DHTLIB_ERROR_RESPONSE_TIMEOUT:
          Serial.println("Error\n\r\tResponse time out error");
          break;
      case DHTLIB_ERROR_DATA_TIMEOUT:
          Serial.println("Error\n\r\tData time out error");
          break;
      case DHTLIB_ERROR_ACQUIRING:
          Serial.println("Error\n\r\tAcquiring");
          break;
      case DHTLIB_ERROR_DELTA:
          Serial.println("Error\n\r\tDelta time to small");
          break;
      case DHTLIB_ERROR_NOTSTARTED:
          Serial.println("Error\n\r\tNot started");
          break;
      default:
          Serial.println("Unknown error");
          break;
  }




}


void printDebugInfo()
{
  if( !SHOW_DEBUG_INFO ) return;

  Serial.print( "Co2 Percentage:" );
  Serial.println( co2_percentage );

  Serial.print("Humidity (%): ");
  Serial.println(DHT.getHumidity(), 2);

  Serial.print("Temperature (oC): ");
  Serial.println(DHT.getCelsius(), 2);

  Serial.print("Temperature (oF): ");
  Serial.println(DHT.getFahrenheit(), 2);

  Serial.print("Temperature (K): ");
  Serial.println(DHT.getKelvin(), 2);

  Serial.print("Dew Point (oC): ");
  Serial.println(DHT.getDewPoint());

  Serial.print("Dew Point Slow (oC): ");
  Serial.println(DHT.getDewPointSlow());


}



/*****************************  MGRead *********************************************
Input:   mg_pin - analog channel
Output:  output of CO2 Sensor
Remarks: This function reads the output of CO2 Sensor
************************************************************************************/
float MGRead(int mg_pin)
{
  int i;
  float v=0;

  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    v += analogRead(mg_pin);
    delay(READ_SAMPLE_INTERVAL);
  }
  v = (v/READ_SAMPLE_TIMES) *5/ 4095 ;

  return v;
}
/*****************************  MQGetPercentage **********************************
Input:   volts   - CO2 Sensor output measured in volts
         pcurve  - pointer to the curve of the target gas
Output:  ppm of the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm)
         of the line could be derived if y(MG-811 output) is provided. As it is a
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic
         value.
************************************************************************************/
int  MGGetPercentage(float volts, float *pcurve)
{
  if ((volts/DC_GAIN )>=ZERO_POINT_VOLTAGE) {
    return -1;
  }
  else {
    return pow(10, ((volts/DC_GAIN)-pcurve[1])/pcurve[2]+pcurve[0]);
  }
}
