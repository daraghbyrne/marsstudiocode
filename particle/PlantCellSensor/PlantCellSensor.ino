#include <math.h>
// This #include statement was automatically added by the Spark IDE.
#include "OneWire.h"

// This #include statement was automatically added by the Spark IDE.
#include "spark-dallas-temperature.h"

#include "rest_client.h"

RestClient rest_client = RestClient("marsserver.herokuapp.com");
String rest_response;

const bool SHOW_DEBUG_INFO = true;

int soilMostureSensorPin = A0;
int soilMoistureReading = 0;

int luminanceSensorPin = A2;
int luminanceReading = 0;

// Server Sensor Ids
int ambientLightSensorId = 1;
int soilMoistureSensorId = 1;
int soilTemperatureSensorId = 1;

// -----------------
// Read temperature
// -----------------

// Data wire is plugged into port 0 on the Arduino
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire( D0 );

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature dallas( &oneWire );

// Create a variable that will store the temperature value
double soilProbeTemp = 0.0;
double soilProbeTempF = 0.0;

Timer publishTimer( 60000, publishCurrentState );

void setup()
{
  Time.zone(-5);

  setupServerSensorIds();

  // setup the library
  dallas.begin();

  publishTimer.start();

  Serial.begin( 9600 );

  publishCurrentState( );
}

void setupServerSensorIds()
{
  String myID = System.deviceID();

  Serial.println( "Device ID:" );
  Serial.println( myID );

  //  Studio 7
  if( myID.equals( "2f002f001047353138383138" ) )
  {
    Serial.println( "Matches" );

    ambientLightSensorId = 42;
    soilMoistureSensorId = 43;
    soilTemperatureSensorId = 44;
  }


}


void loop()
{
}

void publishCurrentState( )
{
  //String stateStr = String( currentState );
  //Particle.publish( "currentState", stateStr );
  readSensors();

  printDebugInfo();

  postReadingToServer( (luminanceReading*1.0f/ 40.95f), ambientLightSensorId );
  postReadingToServer( (soilMoistureReading*1.0f/ 40.95f), soilMoistureSensorId );
  postReadingToServer( soilProbeTemp, soilTemperatureSensorId );


}

void readSensors()
{
    soilMoistureReading = analogRead( soilMostureSensorPin );
    luminanceReading = analogRead( luminanceSensorPin );

    // Request temperature conversion (traditional)
    dallas.requestTemperatures();

    // get the temperature in Celcius
    float tempC = dallas.getTempCByIndex(0);
    // convert to double
    soilProbeTemp = (double)tempC;
}



void postReadingToServer( int reading, int sensor_id )
{
  time_t time = Time.now();

  String timeString = String( Time.format(time, TIME_FORMAT_ISO8601_FULL) );
  timeString = timeString.replace("T", " ");

  String post_data = "sensor_value=" + String( reading ) + "&timestamp=" + timeString + "&sensor_id=" + String( sensor_id )  ;

  Serial.println( "INT POST:" );
  Serial.println( post_data );

  int statusCode = rest_client.post("/new_data/", post_data, &rest_response);

}

void postReadingToServer( double reading, int sensor_id )
{
  time_t time = Time.now();

  String timeString = String( Time.format(time, TIME_FORMAT_ISO8601_FULL) );
  timeString = timeString.replace("T", " ");

  String post_data = "sensor_value=" + String( reading ) + "&timestamp=" + timeString + "&sensor_id=" + String( sensor_id )  ;

  Serial.println( "FLOAT POST:" );
  Serial.println( post_data );

  int statusCode = rest_client.post("/new_data/", post_data, &rest_response);

}


void printDebugInfo()
{
  if( !SHOW_DEBUG_INFO ) return;

  Serial.print( "Soil Moisture:" );
  Serial.println( soilMoistureReading );

  Serial.print( "luminanceReading:" );
  Serial.println( luminanceReading );
  // Print out
  Serial.print( "Soil Probe Temp (C): ");
  Serial.println( soilProbeTemp );

}
