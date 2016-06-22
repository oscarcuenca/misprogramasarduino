/*
LED Photometer, based on Analog Input
by Patrick Di Justo
2012 08 30
*/
#include <EEPROM.h>
#include <SD.h>
#include <SoftwareSerial.h>
// Liquid Crystal Display
// Define the LCD pins: We'll be using a serial-based LCD display
// which only required +3.3Volts, GND, and a single data line.
// databuff and displaybuff hold the data to be displayed
#define LCDIn 3
#define LCDOut 5
SoftwareSerial mySerialPort(LCDIn, LCDOut);

// Data Buffers for the LCD
char databuff1[16];
char databuff2[16];
char dispbuff[16];

// LED variables section
// Which Arduino pin goes to which LED
int redPin = A1;
int greenPin =A2;
int bluePin=A3;

// Variables needed for RGB calculations
float Gamma = 1.00;
int MaxIntensity = 255;
float fBlue;
float fGreen;
float fRed;
float Factor;
int iR;
int iG;
int iB;

// variables to store the value coming from the sensor
int peaknm =0;

//Our eyes can generally see light wavelengths between 350 and 700
//Here, we start the RGB Led with 350
int i = 350;

//RGB is plugged into these arduino digital pins
const int redOutPin = 8;
const int greenOutPin = 6;
const int blueOutPin = 4;


// A place to store the values coming from the analog port
int sensorValueRed =0;
int sensorValueGreen =0;
int sensorValueBlue = 0;

//A place to store the maximum value for each LED
int maxRed = 0;
int maxGreen =0;
int maxBlue = 0;



//EEPROM variables
// The record length is 7: 1 byte for the record number, 2 bytes
// each for the 3 LEDs. For each additional LED you add, increase
// the record length by 2.
int record=0;
int reclen = 7;
int addr =0;

// the following variable is long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long timeSinceLastSensorHigh = 0;
int dataWritten = 0;

// music section
int notelen =40;
int dlx = notelen *1.33;

//switch to tell if an SD card is present
int SDPresent = 1;
void setup()
{

//Set the RGB LED pins to output
pinMode(redOutPin, OUTPUT);
pinMode(greenOutPin, OUTPUT);
pinMode(blueOutPin, OUTPUT);

// initialize serial communications at 9600 bps:
Serial.begin(9600);

// Set the Analog Pins
// Why are we setting input pins to output?
// We're doing this to prevent "leakage" from the pins.
// Setting the pins to output activates a pullup resistor,
// which makes it difficult for voltage to come into the Arduino,
// until we're ready for it.
Serial.println("Setting up the Analog Pins");
pinMode(redPin, OUTPUT);
digitalWrite(redPin, LOW);
pinMode(greenPin, OUTPUT);
digitalWrite(greenPin, LOW);
pinMode(bluePin, OUTPUT);
digitalWrite(bluePin, LOW);

// Set up SD card, let us know if SD card is absent
//revisar con el sketch_humedad y temperatura el puerto usado en yun para la SD
pinMode(10, OUTPUT);
if (!SD.begin(4)) delay(10);
SDPresent =0;

//Set up LCD
pinMode(LCDOut, OUTPUT);
mySerialPort.begin(9600);
mySerialPort.write(0xFE);
mySerialPort.write(0x01);
sprintf(databuff1,"Wakeup Test");
sprintf(dispbuff,"%-16s",databuff1);
mySerialPort.print(dispbuff);

//set up EEPROM
record = EEPROM.read(0);
addr = (record * reclen) +1;
Serial.println("BEEP");

// Play music to let user know the gadget is ready
tone(7,294,notelen);
delay(dlx);
tone(7,330,notelen);
delay(dlx);
tone(7,261,notelen);
delay(dlx);
tone(7,131,notelen);
delay(dlx);
tone(7,196,notelen);
delay(dlx);
delay(3000);
}
void loop()
{

// set the RGB LED to a specific color
makeColor(i);
analogWrite(redOutPin, 255-iR);
analogWrite(greenOutPin, 255-iG);
analogWrite(blueOutPin, 255-iB);
delay(1000);


// read the value from the sensor:
/*
Back in setup(), we enabled a pullup resistor on the analog pins, which
made it difficult for electricity to come into the analog pins.
Here, we disable the pullup resistor, wait 500ms for the pin to
stabilize and to get the light
read the voltage coming into the pin, then reenable the pullup
resistor.
*/
pinMode(redPin, INPUT);
delay(10);
Serial.print("Reading red: ");
sensorValueRed= analogRead(redPin);
pinMode(redPin, OUTPUT);
Serial.println(sensorValueRed);
delay(10);
pinMode(greenPin, INPUT);
delay(10);
sensorValueGreen = analogRead(greenPin);
pinMode(greenPin, OUTPUT);
delay(10);
pinMode(bluePin, INPUT);
delay(10);
sensorValueBlue = analogRead(bluePin);
pinMode(bluePin, OUTPUT);
delay(10);
Serial.println("Comparing sensor values...");



// Here we compare each sensor to its maximum value.
// If any of the sensors has reached a new peak, sound a tone
if( (sensorValueRed>maxRed)
|| (sensorValueGreen>maxGreen)
|| (sensorValueBlue>maxBlue))
{
tone(7,maxRed+maxGreen+maxBlue,1000);
timeSinceLastSensorHigh = millis();
}

// Here we reset the old maximum value with a new one, if necessary
if(sensorValueRed>maxRed) maxRed = sensorValueRed;
if(sensorValueGreen>maxGreen) maxGreen = sensorValueGreen;
if(sensorValueBlue>maxBlue) maxBlue = sensorValueBlue;
peaknm = i;

// Display the sensor values on the LCD screen
sprintf(databuff1,"R%3d G%3d B%3d",maxRed,maxGreen,maxBlue);
sprintf(dispbuff,"%-16s",databuff1);
mySerialPort.print(dispbuff);
Serial.print(dispbuff);

// If 10 seconds has gone by without any new measurements, write
// data to storage.
if(millis() > (timeSinceLastSensorHigh + 10000))
{
if(dataWritten ==0)
{
  writeData();
i++;
  // If we've reached the upper limit of 700 nm, 
if (i>700)
{
  delay(10000);
}

if(SDPresent = 1)
{
writeDataToSD(databuff1, databuff2);
}
}
}
}
void writeData()
{
Serial.print("I'm writing data!!!!!!!!!!!!!!!");
record++;
EEPROM.write(0, record);
EEPROM.write(addr++, record);

/*
The problems of data storage:
The analog pins read a value from 0 to 1023. This is 1024 different
values,
and 1024 = 2 ^ 10. It would take 10 bits of data space to safely
store the value from the analog pins. Unfortunately, a standard byte
of data is only 8 bits. How can you fit 10 bits into 8 bits?
You can't.
What we're doing here is splitting the 10 bits of data into two sections,
which can be stored in two bytes of memory space.
*/
int quotient = sensorValueRed/256;
int mod = sensorValueRed % 256;
EEPROM.write(addr++,quotient);
EEPROM.write(addr++,mod);
quotient = sensorValueGreen/256;
mod = sensorValueGreen % 256;
EEPROM.write(addr++,quotient);
EEPROM.write(addr++,mod);
quotient = sensorValueBlue/256;
mod = sensorValueBlue % 256;
EEPROM.write(addr++,quotient);
EEPROM.write(addr++,mod);
dataWritten = 1;
sprintf(databuff1,"EEPROM written");
sprintf(dispbuff,"%-16s",databuff1);
mySerialPort.print(dispbuff);
Serial.println("FINAL BEEP");
tone(7,196,notelen);
delay(dlx);
tone(7,131,notelen);
delay(dlx);
tone(7,261,notelen);
delay(dlx);
tone(7,330,notelen);
delay(dlx);
tone(7,294,notelen);
}
void writeDataToSD(String dataString1, String dataString2)
{

// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another.
File dataFile = SD.open("LEDdata.txt", FILE_WRITE);
// if the file is available, write to it:
if (dataFile)
{
dataFile.print(millis());
dataFile.print(",");
dataFile.println(dataString1);
dataFile.close();
sprintf(databuff1,"SDCard written");
sprintf(dispbuff,"%-16s",databuff1);
mySerialPort.print(dispbuff);
}
}
void makeColor(int lambda)
{
if (lambda >= 350 && lambda <= 439)
{
fRed = -(lambda - (float)440.0) / ((float)440.0 - (float)350.0);
fGreen = (float)0.0;
fBlue = (float)1.0;
}
else if (lambda >= (float)440.0 && lambda <= (float)489.0)
{
fRed = 0.0;
fGreen = (lambda - (float)440.0) / ((float)490.0 - (float)440.0);
fBlue = 1.0;
}
else if (lambda >= (float)490.0 && lambda <= (float)509.0)
{
  fRed = 0.0;
fGreen = 1.0;
fBlue = -(lambda - (float)510.0) / ((float)510.0 - (float)490.0);
}
else if (lambda >= (float)510.0 && lambda <= (float)579.0)
{
fRed = (lambda - (float)510.0) / ((float)580.0 - (float)510.0);
fGreen = 1.0;
fBlue = 0.0;
}
else if (lambda >= (float)580.0 && lambda <= (float)644.0)
{
fRed = 1.0;
fGreen = -(lambda - (float)645.0) / ((float)645.0 - (float)580.0);
fBlue = 0.0;
}
else if (lambda >= 645.0 && lambda <= 780.0)
{
fRed = 1.0;
fGreen = 0.0;
fBlue = 0.0;
}
else
{
fRed = 0.0;
fGreen = 0.0;
fBlue = 0.0;
}
if (lambda >= 350 && lambda <= 419)
{
Factor = 0.3 + 0.7*(lambda - (float)350.0) / ((float)420.0 -
(float)350.0);
}
else if (lambda >= 420 && lambda <= 700)
{
Factor = 1.0;
}
else if (lambda >= 701 && lambda <= 780)
{
Factor = 0.3 + 0.7*((float)780.0 - lambda) / ((float)780.0 -
(float)700.0);
}
else
{
Factor = 0.0;
}
iR = factorAdjust(fRed, Factor, MaxIntensity, Gamma);
iG = factorAdjust(fGreen, Factor, MaxIntensity, Gamma);
iB = factorAdjust(fBlue, Factor, MaxIntensity, Gamma);
}
int factorAdjust(float C, float Factor, int MaxIntensity, float Gamma)
{
  if(C == 0.0)
{
return 0;
}
else
{
return (int) round(MaxIntensity * pow(C * Factor, Gamma));
}
}

