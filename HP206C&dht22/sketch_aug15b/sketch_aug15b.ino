/*
 * Demo name   : HP206C and DHT22 USING BASE SHIELD.
 * Usage       : I2C PRECISION BAROMETER AND ALTIMETER [HP206C hopeRF] 
 * Author      : Oliver Wang from Seeed Studio adapted to yun by Oscar Cuenca From Wi-Sen
 * Version     : V0.1
 * Change log  : Add kalman filter 2014/04/04
*/

#include <HP20x_dev.h>
#include "Arduino.h"
#include "Wire.h" 
#include <KalmanFilter.h>
#include <Bridge.h>
#include <HttpClient.h>
#include <YunClient.h>
#include <YunServer.h>
#include <Process.h>
#include <DHT.h>

unsigned char ret = 0;

/* Instance */
KalmanFilter t_filter;    //temperature filter
KalmanFilter p_filter;    //pressure filter
KalmanFilter a_filter;    //altitude filter

#define DHTPIN 2     // pin de salida
#define DHTTYPE DHT22   // DHT 22

DHT dht(DHTPIN, DHTTYPE);
/*
HP206C Becasue it is designed to be connected directly to a micro-controller via the I2C bus we do not need to initialize a pin
*/
void setup()
{  

 
  Bridge.begin();
  Serial.begin(9600);
  Serial.println("****HP20x  & DHT22 temperature by Wi_sen****\n");
  Serial.println("Calculation formula: H = [8.5(101325-P)]/100 \n");

/* Power up,delay 150ms,until voltage is stable */
delay(150);
HP20x.begin();
dht.begin();
while(!Serial);
/* Reset HP20x_dev */
delay(100);

  
  /* Determine HP20x_dev is available or not */
  ret = HP20x.isAvailable();
  if(OK_HP20X_DEV == ret)
  {
    Serial.println("HP20x_dev is available.\n");    
  }
  else
  {
    Serial.println("HP20x_dev isn't available.\n");
  }
  
}
 

void loop()
{
  // Initialize the client library
  HttpClient client;
float humidity = dht.readHumidity();
float temperature = dht.readTemperature();

   if (isnan(temperature) || isnan(humidity))
{
    Serial.println("Failed to read from DHT");
  } else {
    Process p;              
    p.begin("/root/curl.sh");      
    p.addParameter(String(temperature)); 
    p.addParameter(String(humidity)); 
    p.run();
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");
  }

  
    //char display[40];
    if(OK_HP20X_DEV == ret)
    { 
Process b;
b.begin("/root/temperature.sh");
float Temper = HP20x.ReadTemperature();

    Serial.println("------------------\n");
    //long Temper = HP20x.ReadTemperature();
    Serial.println("Temper:");
    float t = Temper/100.0;
    Serial.print(t);    
    Serial.println("C.\n");
    Serial.println("Filter:");
    //long TempFiltered = t_filter(t));
    b.addParameter(String(t));
    b.run();
    Serial.print(t_filter.Filter(t));
    Serial.println("C.\n");
    
 
  Process c; 
   c.begin("root/pressure.sh");   
   float Pressure = HP20x.ReadPressure();
   Serial.println("Pressure:");
    float p = Pressure/100.0;
    Serial.print(p);
    Serial.println("hPa.\n");
    Serial.println("Filter:");
    //long PressFiltered =p_filter(p);
    c.addParameter(String(p));
    c.run();
    Serial.print(p_filter.Filter(p));
    Serial.println("hPa\n");

    Process d;
    d.begin("root/altitude.sh");
    float Altitude = HP20x.ReadAltitude();    
    Serial.println("Altitude:");
    float a = Altitude/100.0;
    Serial.print(a);
    Serial.println("m.\n");
    Serial.println("Filter:");
    //long AltFiltered=a_filter(p);
    d.addParameter(String(a));
    d.run();
    Serial.print(a_filter.Filter(a));
    Serial.println("m.\n");
    Serial.println("------------------\n");
    }
/* 3 times per day */
delay(28800000);   
Serial.flush();
}
