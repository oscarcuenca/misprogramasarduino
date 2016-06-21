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
float fIr;
float Factor;
int iIr;
//IR light wavelengths between 700 and 2500 but we use maximum 940
//Here, we start the IR Led with 700
int i = 700;
//IR emissor Led is plugged into these arduino digital pins
const int InfraredOutPin = 8;
// Receiver IR LED to be tested is plugged into A0
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
//Set the IR LED pins to output
pinMode(InfraredOutPin, OUTPUT);
// Initialize the LCD display
mySerialPort.begin(9600);
mySerialPort.write(0xFE);
mySerialPort.write(0x01);
// test to see if the IR LED works
makeColor(i);
analogWrite(InfraredOutPin,255-iIr);
delay(5000);
}
void loop()
{
// set the IR LED to a specific inviseble range IR color
makeColor(i);
analogWrite(InfraredOutPin, 255-iIr);
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
// If we've reached the upper limit of 940 nm, play a little melody
if (i>940)
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
if (lambda >= 700 && lambda <= 940)
{
fIr = -(lambda - (float)940.0) / ((float)940.0 - (float)700.0);
//fGreen = (float)0.0;
//fBlue = (float)1.0;
}
else
{
fIr = 0.0;
//fGreen = 0.0;
//fBlue = 0.0;
}
if (lambda >= 700 && lambda <= 940)
{
Factor = 0.3 + 0.7*(lambda - (float)700.0) / ((float)940.0 -
(float)350.0);
}
else
{
Factor = 0.0;
}
iIr = factorAdjust(fIr, Factor, MaxIntensity, Gamma);
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

