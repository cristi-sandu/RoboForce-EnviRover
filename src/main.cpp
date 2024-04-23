#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <BMP180I2C.h>
#include <GUVA_S12SD.h>
#include <LCD_I2C.h>
#include <MQ135.h>
#include <RtcDS1302.h>
#include <SimpleDHT.h>
#include <SD.h>
#include <ThreeWire.h>

#include "MOTOR.h"

#define BAUD_RATE 9600
#define AER_PIN A0
#define UV_PIN A1
#define CS_PIN 2
#define DHT_PIN 3
#define MOTOR_11 4
#define MOTOR_12 5
#define MOTOR_21 6
#define MOTOR_22 7
#define RST_PIN 8
#define DAT_PIN 9
#define CLK_PIN 10
#define SNZ_PRESIUNE 0x77
#define PAUZA 1000

MOTOR motor = MOTOR(MOTOR_11, MOTOR_12, MOTOR_21, MOTOR_22);
MQ135 sensorAer = MQ135(AER_PIN);
BMP180I2C sensorPresiune = BMP180I2C(SNZ_PRESIUNE);
SimpleDHT11 dht11;
String btVal;
LCD_I2C lcd(0x27, 16, 2);
GUVAS12SD uv(UV_PIN);
ThreeWire wire(DAT_PIN, CLK_PIN, RST_PIN);
RtcDS1302<ThreeWire> Rtc(wire);
File cardSd;

void initPresiune();
void afisareValoriSerial(byte temperatura, byte umiditate, float co2, float presiune, float uvIndex);
void afisareValoriLCD(byte temperetura, byte umiditate, float co2, float presiune, float uvIndex);
void printDate(RtcDateTime now);
void printTime(RtcDateTime now);
void printDateLcd(RtcDateTime now);
void printTimeLcd(RtcDateTime now);
void afisareValoriCard(byte temperatura, byte umiditate, float co2, float presiune, float uvIndex);
void functionare();

void setup()
{
  Serial.begin(BAUD_RATE);
  while (!Serial)
    ;
  // Wire.begin();
  Rtc.Begin();

  if (!SD.begin(CS_PIN))
  {
    while (true)
      ;
  }

  if (!sensorPresiune.begin())
  {
    while (true)
      ;
  }

  motor.setup();
  sensorPresiune.resetToDefaults();
  sensorPresiune.setSamplingMode(BMP180MI::MODE_UHR);
  lcd.begin();
  lcd.backlight();
}

void loop()
{
  // teste();
  // controlBluetooth();
  // afisareValoriLCD(umiditate, temperatura, co2, presiune);

  //   byte temperatura = 0;
  //     byte umiditate = 0;

  //     if (dht11.read(DHT_PIN, &temperatura, &umiditate, NULL))
  //     {
  //       Serial.println("Eroare");
  //       return;
  //     }

  //     if (!sensorPresiune.measurePressure())
  //     {
  //       Serial.println("could not start perssure measurement, is a measurement already running?");
  //       return;
  //     }
  //     do
  //     {
  //       delay(100);
  //     } while (!sensorPresiune.hasValue());

  //     float presiune = sensorPresiune.getPressure();
  //     float co2 = sensorAer.getCorrectedPPM(temperatura, umiditate);
  //     float mV = uv.read();
  //     float uvIndex = uv.index(mV);
  // afisareValoriSerial(umiditate, temperatura, co2, presiune, uvIndex);
  cardSd = SD.open("valori.txt", FILE_WRITE);
  cardSd.close();

  while (Serial.available() > 0)
  {
    btVal = btVal + ((char)(Serial.read()));
    delay(2);
  }
  if (0 < String(btVal).length() && 2 >= String(btVal).length())
  {
    Serial.println(String(btVal).length());
    Serial.println(btVal);

    switch (String(btVal).charAt(0))
    {
    case 'F':
      motor.inapoi();
      break;
    case 'B':
      motor.inainte();
      break;
    case 'L':
      motor.stanga();
      break;
    case 'R':
      motor.dreapta();
      break;
    case 'S':
      motor.stop();
      break;
    case 'o':
      int i = 0;

      do
      {
        functionare();
      } while (++i < 3);

      break;
    }
    btVal = "";
  }
  else
  {
    btVal = "";
  }

  // delay(PAUZA);
}

void initPresiune()
{
  if (!sensorPresiune.measurePressure())
  {
    Serial.println("Eroare");
    return;
  }
  do
  {
    delay(100);
  } while (!sensorPresiune.hasValue());
}

void afisareValoriSerial(byte umiditate, byte temperatura, float co2, float presiune, float uvIndex)
{
  Serial.println("Start masuratoare");
  // umiditate
  Serial.print("Umiditate: ");
  Serial.print(umiditate);
  Serial.println(" %");
  // temperatura
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" C");
  // calitate aer
  Serial.print("Calitate Aer: ");
  Serial.print(co2);
  Serial.println(" ppm");
  // presiune
  Serial.print("Presiune: ");
  Serial.print(presiune);
  Serial.println(" Pa");
  Serial.println();
  delay(1000);
}

void afisareValoriLCD(byte umiditate, byte temperatura, float co2, float presiune, float uvIndex)
{
  lcd.print("Umiditate : ");
  lcd.print(umiditate);
  lcd.setCursor(0, 1);
  lcd.print("Temperatura : ");
  lcd.print(temperatura);
  delay(5000);
  lcd.clear();
  lcd.print("Cal. aer : ");
  lcd.print(co2);
  lcd.setCursor(0, 1);
  lcd.print("Presiune : ");
  lcd.print(presiune);
  delay(5000);
  lcd.clear();
  lcd.print("Index UV : ");
  lcd.print(uvIndex);
  delay(5000);
  lcd.clear();
}

void printDateLcd(RtcDateTime now)
{
  lcd.print("data: ");
  lcd.print(now.Day());
  lcd.print("/");
  lcd.print(now.Month());
  lcd.print("/");
  lcd.print(now.Year());
}

void printTimeLcd(RtcDateTime now)
{
  lcd.print("ora: ");
  lcd.print(now.Hour());
  lcd.print(":");
  lcd.print(now.Minute());
  lcd.print(":");
  lcd.print(now.Second());
}
void printDate(RtcDateTime now)
{
  cardSd.print("data : ");
  cardSd.print(now.Day());
  cardSd.print("/");
  cardSd.print(now.Month());
  cardSd.print("/");
  cardSd.println(now.Year());
  cardSd.print(" ");
}

void printTime(RtcDateTime now)
{
  cardSd.print("ora : ");
  cardSd.print(now.Hour());
  cardSd.print(":");
  cardSd.print(now.Minute());
  cardSd.print(":");
  cardSd.println(now.Second());
  cardSd.print(" ");
}

void afisareValoriCard(byte umiditate, byte temperatura, float co2, float presiune, float uvIndex)
{
  // umiditate
  cardSd.print("umiditate : ");
  cardSd.print(umiditate);
  cardSd.print(" ");

  // temperatura
  cardSd.print("temperatura : ");
  cardSd.print(temperatura);
  cardSd.print(" ");

  // co2
  cardSd.print("co2 : ");
  cardSd.print(co2);
  cardSd.print(" ");

  // presiune
  cardSd.print("presiune : ");
  cardSd.print(presiune);
  cardSd.print(" ");

  // uv
  cardSd.print("uv : ");
  cardSd.print(uvIndex);
  cardSd.print(" ");

  cardSd.println();
}

void functionare()
{
  byte temperatura = 0;
  byte umiditate = 0;

  if (dht11.read(DHT_PIN, &temperatura, &umiditate, NULL))
  {
    Serial.println("Eroare");
    return;
  }

  initPresiune();

  float presiune = sensorPresiune.getPressure();
  float co2 = sensorAer.getCorrectedPPM(temperatura, umiditate);
  float mV = uv.read();
  float uvIndex = uv.index(mV);

  cardSd = SD.open("valori.txt", FILE_WRITE);
  RtcDateTime now = Rtc.GetDateTime();

  if (cardSd)
  {
    // data si ora
    printDate(now);
    printTime(now);

    afisareValoriCard(umiditate, temperatura, co2, presiune, uvIndex);
    cardSd.println();

    cardSd.close();
  }
  lcd.clear();

  lcd.setCursor(0, 0);
  printDateLcd(now);

  lcd.setCursor(0, 1);
  printTimeLcd(now);

  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);

  afisareValoriLCD(umiditate, temperatura, co2, presiune, uvIndex);
  delay(PAUZA);
}

/*
// BMP180_I2C.ino
//
// shows how to use the BMP180MI library with the sensor connected using I2C.
//
// Copyright (c) 2018 Gregor Christandl
//
// connect the BMP180 to the Arduino like this:
// Arduino - BMC180
// 5V ------ VCC
// GND ----- GND
// SDA ----- SDA
// SCL ----- SCL

#include <Arduino.h>
#include <Wire.h>

#include <BMP180I2C.h>

#define I2C_ADDRESS 0x77

//create an BMP180 object using the I2C interface
BMP180I2C bmp180(I2C_ADDRESS);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //wait for serial connection to open (only necessary on some boards)
  while (!Serial);

  Wire.begin();

  //begin() initializes the interface, checks the sensor ID and reads the calibration parameters.
  if (!bmp180.begin())
  {
    Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
    while (1);
  }

  //reset sensor to default parameters.
  bmp180.resetToDefaults();

  //enable ultra high resolution mode for pressure measurements
  bmp180.setSamplingMode(BMP180MI::MODE_UHR);
}

void loop() {
  // put your main code here, to run repeatedly:

  delay(1000);

  //start a temperature measurement
  if (!bmp180.measureTemperature())
  {
    Serial.println("could not start temperature measurement, is a measurement already running?");
    return;
  }

  //wait for the measurement to finish. proceed as soon as hasValue() returned true.
  do
  {
    delay(100);
  } while (!bmp180.hasValue());

  Serial.print("Temperature: ");
  Serial.print(bmp180.getTemperature());
  Serial.println(" degC");

  //start a pressure measurement. pressure measurements depend on temperature measurement, you should only start a pressure
  //measurement immediately after a temperature measurement.
  if (!bmp180.measurePressure())
  {
    Serial.println("could not start perssure measurement, is a measurement already running?");
    return;
  }

  //wait for the measurement to finish. proceed as soon as hasValue() returned true.
  do
  {
    delay(100);
  } while (!bmp180.hasValue());

  Serial.print("Pressure: ");
  Serial.print(bmp180.getPressure());
  Serial.println(" Pa");
}
*/