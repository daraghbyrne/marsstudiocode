#include <math.h>

#include "rest_client.h"
#include "OneWire.h"
#include "spark-dallas-temperature.h"

RestClient rest_client = RestClient("marsserver.herokuapp.com");
String rest_response;

const bool SHOW_DEBUG_INFO = true;

Timer publishTimer( 60000, publishCurrentState );


// Server Sensor Ids
int ambientLightSensorId = 1;
int q1SoilMoistureSensorId = 1;
int q1SoilTemperatureSensorId = 1;
int q2SoilMoistureSensorId = 1;
int q2SoilTemperatureSensorId = 1;


// Pin and Readings for each sensor

int q1SoilMostureSensorPin = A0;
int q1SoilMoistureReading = 0;

int q2SoilMostureSensorPin = A2;
int q2SoilMoistureReading = 0;

int q1SoilTemperatureSensorPin = D0;
double q1SoilTemperatureReading = 0;

int q2SoilTemperatureSensorPin = D2;
double q2SoilTemperatureReading = 0;

int luminanceSensorPin = A4;
int luminanceReading = 0;


// -----------------
// Read temperature
// -----------------

// Data wire is plugged into port 0 on the Arduino
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWireQ1( q1SoilTemperatureSensorPin );
OneWire oneWireQ2( q2SoilTemperatureSensorPin );

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature dallasQ1( &oneWireQ1 );
DallasTemperature dallasQ2( &oneWireQ2 );


void setup()
{
    Time.zone(-5);

    setupServerSensorIds();

    // setup the library
    dallasQ1.begin();
    dallasQ2.begin();

    publishTimer.start();

    Serial.begin(9600);

    publishCurrentState();

    Serial.println( "Beginning ");

}

void setupServerSensorIds()
{
  String myID = System.deviceID();

  Serial.println( "Device ID:" );
  Serial.println( myID );

  //  Studio 7
  if( myID.equals( "3b003b000247353138383138" ) )
  {
    Serial.println( "Matches" );

    ambientLightSensorId = 18;
    q1SoilMoistureSensorId = 11;
    q1SoilTemperatureSensorId = 15;
    q2SoilMoistureSensorId = 12;
    q2SoilTemperatureSensorId = 16;
  }

  //  Studio 8
  if( myID.equals( "180035000247353138383138" ) )
  {
    Serial.println( "Matches" );

    ambientLightSensorId = 17;
    q1SoilMoistureSensorId = 9;
    q1SoilTemperatureSensorId = 13;
    q2SoilMoistureSensorId = 10;
    q2SoilTemperatureSensorId = 14;
  }

}


void loop()
{
  //publishCurrentState( );
  //delay( 10000 );
}


void publishCurrentState( )
{
  //String stateStr = String( currentState );
  //Particle.publish( "currentState", stateStr );
  readSensors();

  printDebugInfo();

  //post as int
  postReadingToServer( (luminanceReading*1.0f/ 40.95f), ambientLightSensorId );

  postReadingToServer( (q1SoilMoistureReading*1.0f/ 40.95f), q1SoilMoistureSensorId );
  postReadingToServer( (q2SoilMoistureReading*1.0f/ 40.95f), q2SoilMoistureSensorId );

  postReadingToServer( q1SoilTemperatureReading, q1SoilTemperatureSensorId );
  postReadingToServer( q2SoilTemperatureReading, q2SoilTemperatureSensorId );


}

void readSensors()
{
  // get the temperature in Celcius
  float tempQ1C = dallasQ1.getTempCByIndex(0);
  // convert to double
  q1SoilTemperatureReading = (double)tempQ1C;

  // get the temperature in Celcius
  float tempQ2C = dallasQ2.getTempCByIndex(0);
  // convert to double
  q2SoilTemperatureReading = (double)tempQ2C;

  q1SoilMoistureReading = analogRead( q1SoilMostureSensorPin );
  q2SoilMoistureReading = analogRead( q2SoilMostureSensorPin );


  luminanceReading = analogRead( luminanceSensorPin );


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

void postReadingToServer( float reading, char* reading_type )
{
  time_t time = Time.now();

  String timeString = String( Time.format(time, TIME_FORMAT_ISO8601_FULL) );
  timeString = timeString.replace("T", " ");

  String post_data = "sensor_value=" + String( reading ) + "&timestamp=" + timeString + "&sensor_id=1"  ;

  int statusCode = rest_client.post("/new_data/", post_data, &rest_response);

}





void printDebugInfo()
{
  if( !SHOW_DEBUG_INFO ) return;

  Serial.print( "Soil Moisture Q1:" );
  Serial.println( q1SoilMoistureReading );
  Serial.print( "Soil Moisture Q2:" );
  Serial.println( q2SoilMoistureReading );


  Serial.print( "Soil Temp Q1:" );
  Serial.println( q1SoilTemperatureReading );
  Serial.print( "Soil Temp Q2:" );
  Serial.println( q2SoilTemperatureReading );

  Serial.print( "luminanceReading:" );
  Serial.println( luminanceReading );

}
