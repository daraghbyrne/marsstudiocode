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

int uvSensorPin = A3;
int uvSensorReading = 0;

int luminanceSensorPin = A2;
int luminanceReading = 0;

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


Timer publishTimer( 60000, publishCurrentState );

void setup()
{
  Time.zone(-5);

  Particle.variable("soil_moisture", &soilMoistureReading, INT);
  Particle.variable("soil_temp", &soilProbeTemp, DOUBLE);

  // setup the library
  dallas.begin();


  publishTimer.start();

  Serial.begin( 9600 );

}


void loop()
{

  //readSensors();


  //delay( 10000 );

}

void publishCurrentState( )
{
  //String stateStr = String( currentState );
  //Particle.publish( "currentState", stateStr );
  readSensors();

  printDebugInfo();

  postReadingToServer( soilMoistureReading );
  postReadingToServer( uvSensorReading );
  postReadingToServer( luminanceReading );

}

void readSensors()
{
    soilMoistureReading = analogRead( soilMostureSensorPin );
    uvSensorReading = analogRead( uvSensorPin );
    luminanceReading = analogRead( luminanceSensorPin );

    // Request temperature conversion (traditional)
    dallas.requestTemperatures();

    // get the temperature in Celcius
    float tempC = dallas.getTempCByIndex(0);
    // convert to double
    soilProbeTemp = (double)tempC;

    // Get the CO2 concentration;
    co2_volts = MGRead( co2_pin );
    co2_percentage = MGGetPercentage(co2_volts,CO2Curve);

}



void postReadingToServer( int reading )
{
  time_t time = Time.now();

  String timeString = String( Time.format(time, TIME_FORMAT_ISO8601_FULL) );
  timeString = timeString.replace("T", " ");

  String post_data = "sensor_value=" + String( reading ) + "&timestamp=" + timeString + "&sensor_id=1"  ;

  int statusCode = rest_client.post("/new_data/", post_data, &rest_response);

  //Serial.println( post_data );
  //Serial.print("Status code from server: ");
  //Serial.println( statusCode );
  //Serial.print("Response body from server: ");
  //Serial.println(rest_response);
}


void printDebugInfo()
{
  if( !SHOW_DEBUG_INFO ) return;

  Serial.print( "Soil Moisture:" );
  Serial.println( soilMoistureReading );

  Serial.print( "UV:" );
  Serial.println( uvSensorReading );

  Serial.print( "luminanceReading:" );
  Serial.println( luminanceReading );


  Serial.print( "Soil Moisture:" );
  Serial.println( soilMoistureReading );

  Serial.print( "Co2 Percentage:" );
  Serial.println( co2_percentage );

  // Print out
  Serial.print( "Soil Probe Temp (C): ");
  Serial.println( soilProbeTemp );

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
