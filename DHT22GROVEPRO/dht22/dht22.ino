#include <Bridge.h>
#include <HttpClient.h>
#include <YunClient.h>
#include <YunServer.h>
#include <DHT.h>
#include <Process.h>
 
#define DHTPIN 2     // pin de salida
#define DHTTYPE DHT22   // DHT 22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  
  Bridge.begin();
  Serial.begin(9600);
  Serial.println("DHT11 test!");

dht.begin();
while(!Serial);

}
void loop() {
// Initialize the client library
  HttpClient client;
float humidity = dht.readHumidity();
float temperature = dht.readTemperature();


// Make a HTTP request:
//char buffer[90];//no se, no he contado

//client.get(String("http://wi-sen.esy.es/dht11/sensorarduino.php?temperature=") + t + String("&humidity=") + h);
//http://forum.arduino.cc/index.php?topic=366236.msg2528237#msg2528237
//client.get(buffer);
   if (isnan(temperature) || isnan(humidity)) {
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
 delay(28800000);   
Serial.flush();
}
