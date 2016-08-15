#include <Bridge.h>
#include <HttpClient.h>
#include <YunClient.h>
#include <YunServer.h>
#include <DHT.h>
#include <Process.h>
 
#define DHTPIN 2     // pin de salida
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  
  Bridge.begin();
  Serial.begin(9600);
// Serial.println("DHT11 test!");

dht.begin();
//while(!Serial);

}
void loop() {
// Initialize the client library
  HttpClient client;
int humedad = dht.readHumidity();
int temperatura = dht.readTemperature();


// Make a HTTP request:
//char buffer[90];//no se, no he contado

//client.get(String("http://wi-sen.esy.es/dht11/sensorarduino.php?temperature=") + t + String("&humidity=") + h);
//http://forum.arduino.cc/index.php?topic=366236.msg2528237#msg2528237
//client.get(buffer);
//  if (isnan(temperatura) || isnan(humedad)) {
//  Serial.println("Failed to read from DHT");
//} else {
    Process p;              
    p.begin("/root/curl.sh");      
    p.addParameter(String(temperatura)); 
    p.addParameter(String(humedad)); 
    p.run();
    //Serial.print("Humedad: ");
    //Serial.print(humedad);
    //Serial.print(" %\t");
    //Serial.print("Temperatura: ");
    //Serial.print(temperatura);
    //Serial.println(" *C");
//}
 delay(200000);   
//Serial.flush();
}
