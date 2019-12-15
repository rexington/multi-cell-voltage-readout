#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "LowPower.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define TOP_CHARGE 14600 // top battery voltage
#define BOTTOM_CHARGE 10200 // bottom battery voltage
#define bat1pin A0 // pin for reading voltage of bat0
#define bat2pin A1 // pin for reading voltage of bat1
#define bat3pin A2 // pin for reading voltage of bat2
#define bat4pin A3 // pin for reading voltage of bat3 / overall voltage


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/*
 * Using Smoothing example from Arduino public domain 
 *  * created 22 Apr 2007
 * by David A. Mellis  <dam@mellis.org>
 * modified 9 Apr 2012
 * by Tom Igoe
 * http://www.arduino.cc/en/Tutorial/Smoothing
 * 
 * The rest of the code is pieced together from my experimentation and 
 * various examples/tutorials out there on the World Wide Interwebs (WWI)
 * - Rex Vokey (KE6MT) 12 Aug 2019
 */
 
const int numReadings = 20;

int readings1[numReadings];      // cell 1 readings
int readings2[numReadings];   // cell 2 readings
int readings3[numReadings];   // cell 3 readings
int readings4[numReadings];   // cell 4 readings
int prevMin = 1023; // for recording the minimum total voltage
int readIndex = 0;              // the index of the current reading
int total1 = 0;                  // the running total
int total2 = 0;
int total3 = 0;
int total4 = 0;
int average1 = 0;                // the average
int average2 = 0;
int average3 = 0;
int average4 = 0;
float voltMappedAverage1 = 0;        // mapped average for voltage
float voltMappedAverage2 = 0;        // mapped average for voltage
float voltMappedAverage3 = 0;        // mapped average for voltage
float voltMappedAverage4 = 0;        // mapped average for voltage
float voltMappedAverageAll = 0;        // mapped average for voltage
float minMapped = 0;              // mapped minimum total voltage
float voltage1 = 0;            // floating point voltage
float voltage2 = 0;            // floating point voltage
float voltage3 = 0;            // floating point voltage
float voltage4 = 0;            // floating point voltage
float voltageAll = 0;            // floating point voltage
float minVoltage = 0;         // displayed minimum voltage


void setup() {
  // initialize serial communication with computer:
  Serial.begin(19200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(500);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings1[thisReading] = 0;
    readings2[thisReading] = 0;
    readings3[thisReading] = 0;
    readings4[thisReading] = 0;
  }
}

void loop() {
  // subtract the last reading:
  total1 = total1 - readings1[readIndex];
  total2 = total2 - readings2[readIndex];
  total3 = total3 - readings3[readIndex];
  total4 = total4 - readings4[readIndex];

  
  
  // read from the sensor:
  readings1[readIndex] = analogRead(bat1pin);
  readings2[readIndex] = analogRead(bat2pin);
  readings3[readIndex] = analogRead(bat3pin);
  readings4[readIndex] = analogRead(bat4pin);

  // record the minimum
  if((readings4[readIndex] < prevMin)){
    prevMin = readings4[readIndex];
  }
  
  // add the reading to the total:
  total1 = total1 + readings1[readIndex];
  total2 = total2 + readings2[readIndex];
  total3 = total3 + readings3[readIndex];
  total4 = total4 + readings4[readIndex];
  
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average1 = total1 / numReadings;
  average2 = total2 / numReadings;
  average3 = total3 / numReadings;
  average4 = total4 / numReadings;
  
  // turn into mapped voltage, based on our voltage divider figures
  voltMappedAverage1 = float(map(average1,0,1023,0,5000));
  voltMappedAverage2 = float(map(average2,0,1023,0,10000)-voltMappedAverage1);
  voltMappedAverage3 = float(map(average3,0,1023,0,15681)-(voltMappedAverage1+voltMappedAverage2));
  voltMappedAverage4 = float(map(average4,0,1023,0,15682)-(voltMappedAverage1+voltMappedAverage2+voltMappedAverage3));
  voltMappedAverageAll = float(map(average4,0,1023,0,15682));
  minMapped = float(map(prevMin,0,1023,0,15682));
  
  
  // bring integer down to floating point voltage
  voltage1 = voltMappedAverage1/1000;
  voltage2 = voltMappedAverage2/1000;
  voltage3 = voltMappedAverage3/1000;
  voltage4 = voltMappedAverage4/1000;
  voltageAll = voltMappedAverageAll/1000;
  minVoltage = minMapped/1000;

 
  // Display all the infos

  // display voltage with two decimal places
  display.clearDisplay();
  display.setCursor(30,0);
  display.print(voltageAll,2);
  display.print("v");
  display.setTextSize(1);
  // draw our rectangles
  display.drawRect(0,16,45,11,WHITE);
  display.drawRect(82,16,45,11,WHITE);
  display.drawRect(0,29,45,11,WHITE);
  display.drawRect(82,29,45,11,WHITE);
  // draw lines
  display.drawLine(45,20,45,23,WHITE);
  display.drawLine(127,20,127,23,WHITE);
  display.drawLine(45,32,45,35,WHITE);
  display.drawLine(127,32,127,35,WHITE);
  // display cell 1
  display.setCursor(2,18);
  display.print("1:");
  display.print(voltage1);
  display.print("v ");
  // display cell 2
  display.setCursor(84,18);
  display.print("2:");
  display.print(voltage2);
  display.print("v ");
  // display cell 3
  display.setCursor(2,31);
  display.print("3:");
  display.print(voltage3);
  display.print("v");
  // display cell 4
  display.setCursor(84,31);
  display.print("4:");  
  display.print(voltage4);
  display.print("v");

  // display minimum voltage
  display.setTextSize(2);
  display.setCursor(4,44);
  display.print("min:");
  display.print(minVoltage);
  display.print("v");
  
  display.display();

//  delay(600);
  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF); // 120ms = ~8 reads per second, with the µcontroller powered down in between reads
}
