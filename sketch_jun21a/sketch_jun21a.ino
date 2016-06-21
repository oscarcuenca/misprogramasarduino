//calibrador UV
#include <EEPROM.h>
#include <SoftwareSerial.h>
//Set up the Liquid Crystal Display
#define LCDIn 3
#define LCDOut 2
SoftwareSerial mySerialPort(LCDIn, LCDOut);
//LCD Display buffers
char databuff[16];
char dispbuff[16];
// Variables needed for RGB calculations
float Gamma = 1.00;
int MaxIntensity = 255;
float fUv;
float Factor;
int iUv;
//UV light wavelengths between 10 and 400 but we use maximum 400
//Here, we start the Uv Led with 10
int i = 10;
//Uv emissor Led is plugged into these arduino digital pins
const int UltravioletOutPin = 8;
// Receiver UV LED to be tested is plugged into A0
int testPin = A0;
// testPin = 11; (digital) para el sensor IR premontado
// variables to store the value coming from the sensor
int sensorValueTest =0;
int oldTest =0;
int peaknm =0;
//EEPROM start data
int addr=0;
//Music
int notelen = 90;
int dlx = 130;
void setup()
{
pinMode(LCDOut, OUTPUT);
pinMode(LCDIn, INPUT);
//Set the UV LED pins to output
pinMode(UltravioletOutPin, OUTPUT);
// Initialize the LCD display
mySerialPort.begin(9600);
mySerialPort.write(0xFE);
mySerialPort.write(0x01);
// test to see if the UV LED works
makeColor(i);
analogWrite(UltravioletOutPin,255-iUv);
delay(5000);
}
void loop()
{
// set the UV LED to a specific inviseble range UV color
makeColor(i);
analogWrite(UltravioletOutPin, 255-iUv);
delay(500);
// read the sensitivity of the Test LED
sensorValueTest= analogRead(testPin);
if (sensorValueTest > oldTest)
{
oldTest = sensorValueTest;
peaknm = i;
}
// Display the values on the LCD
sprintf(databuff,"CV:%3d Cnm:%3d",sensorValueTest,i);
sprintf(dispbuff,"%-16s",databuff);
mySerialPort.print(dispbuff);
sprintf(databuff,"XV:%3d Xnm:%3d",oldTest, peaknm);
sprintf(dispbuff,"%-16s",databuff);
mySerialPort.print(dispbuff);
writeData();
i++;
// If we've reached the upper limit of 400 nm, play a little melody
if (i>400)
{
for (int f = 0; f<=100; f++)
{
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
delay(10000);
}
}
void writeData()
{
int quotient = i/256;
int mod = i % 256;
EEPROM.write(addr++,quotient);
EEPROM.write(addr++,mod);
quotient = sensorValueTest/256;
mod = sensorValueTest % 256;
EEPROM.write(addr++,quotient);
EEPROM.write(addr++,mod);
}
void makeColor(int lambda)
{
//blue  
if (lambda >= 10 && lambda <= 400)
{
fUv = -(lambda - (float)400.0) / ((float)400.0 - (float)10.0);
//fGreen = (float)0.0;
//fBlue = (float)1.0;
}
else
{
fUv = 0.0;
//fGreen = 0.0;
//fBlue = 0.0;
}
if (lambda >= 10 && lambda <= 400)
{
Factor = 0.3 + 0.7*(lambda - (float)10.0) / ((float)400.0 -
(float)10.0);
}
else
{
Factor = 0.0;
}
iUv = factorAdjust(fUv, Factor, MaxIntensity, Gamma);
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

