#include <Arduino.h>
#include <SPI.h>

#include <BMP180I2C.h>
#include <GUVA_S12SD.h>
#include <LCD_I2C.h>
#include <MQ135.h>
#include <RtcDS1302.h>
#include <SD.h>
#include <ThreeWire.h>

#include "MOTOR.h"
#include "STU.h"

#define BAUD_RATE 9600
#define AER_PIN A0
#define UV_PIN A1
#define CS_PIN 2
#define STU_PIN 3
#define MOTOR_11 4
#define MOTOR_12 5
#define MOTOR_21 6
#define MOTOR_22 7
#define RST_PIN 8
#define DAT_PIN 9
#define CLK_PIN 10
#define SNZ_PRESIUNE 0x77
#define PAUZA 5000

BMP180I2C sensorPresiune = BMP180I2C(SNZ_PRESIUNE);
File cardSd;
GUVAS12SD uv(UV_PIN);
LCD_I2C lcd(0x27, 16, 2);
MOTOR motor = MOTOR(MOTOR_11, MOTOR_12, MOTOR_21, MOTOR_22);
MQ135 sensorAer = MQ135(AER_PIN);
RtcDS1302<ThreeWire> Rtc(wire);
String btVal;
STU stu = STU(STU_PIN);
ThreeWire wire(DAT_PIN, CLK_PIN, RST_PIN);

void afisareValoriCard(byte temperatura, byte umiditate, float co2, float presiune, float uvIndex);
void afisareValoriLCD(byte temperetura, byte umiditate, float co2, float presiune, float uvIndex);
void afisareValoriSerial(byte temperatura, byte umiditate, float co2, float presiune, float uvIndex);
void functionare();
void initPresiune();
void printTime(RtcDateTime now);
void printTimeLcd(RtcDateTime now);
void procesareUV(float uvIndex);

void setup()
{
  lcd.backlight();
  lcd.begin();
  motor.setup();
  Rtc.Begin();
  Serial.begin(BAUD_RATE);
  stu.begin();

  if (!Serial || !SD.begin(CS_PIN) || !sensorPresiune.begin())
  {
    while (true)
      ;
  }

  sensorPresiune.resetToDefaults();
  sensorPresiune.setSamplingMode(BMP180MI::MODE_UHR);
}

void loop()
{
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
  delay(25);
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

void procesareUV(float uvIndex)
{
  switch ((int)uvIndex)
  {
  case 0:
    lcd.print("nu este soare");
    break;
  case 1:
    lcd.print("scazut");
    break;
  case 2:
    lcd.print("scazut");
    break;
  case 3:
    lcd.print("mediu");
    break;
  case 4:
    lcd.print("mediu");
    break;
  case 5:
    lcd.print("mediu");
    break;
  case 6:
    lcd.print("ridicat");
    break;
  case 7:
    lcd.print("ridicat");
    break;
  case 8:
    lcd.print("foarte ridicat");
    break;
  case 9:
    lcd.print("foarte ridicat");
    break;
  case 10:
    lcd.print("foarte ridicat");
    break;

  default:
    lcd.print("extrem ridicat");
    break;
  }
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

  delay(PAUZA);
}

void printTimeLcd(RtcDateTime now)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("data: ");
  lcd.print(now.Day());
  lcd.print("/");
  lcd.print(now.Month());
  lcd.print("/");
  lcd.print(now.Year());

  lcd.setCursor(0, 1);
  lcd.print("ora: ");
  lcd.print(now.Hour());
  lcd.print(":");
  lcd.print(now.Minute());
  lcd.print(":");
  lcd.print(now.Second());

  delay(PAUZA);
  lcd.clear();
}

void afisareValoriLCD(byte umiditate, byte temperatura, float co2, float presiune, float uvIndex)
{
  // umiditate
  lcd.clear();
  lcd.print("umiditate : ");
  lcd.print(umiditate);
  // tempratura
  lcd.setCursor(0, 1);
  lcd.print("temperatura : ");
  lcd.print(temperatura);
  delay(PAUZA);

  // calitatea aerului
  lcd.clear();
  lcd.print("cal. aer : ");
  lcd.print(co2);
  // presiune atmosferica
  lcd.setCursor(0, 1);
  lcd.print("presiune : ");
  lcd.print(presiune);
  delay(PAUZA);

  // index UV
  lcd.clear();
  lcd.print("index UV : ");
  lcd.print(uvIndex);
  // mesaj
  lcd.setCursor(0, 1);
  procesareUV(uvIndex);
  
  delay(PAUZA);
  lcd.clear();
}

void printTime(RtcDateTime now)
{
  cardSd.print("data : ");
  cardSd.print(now.Day());
  cardSd.print("/");
  cardSd.print(now.Month());
  cardSd.print("/");
  cardSd.println(now.Year());
  cardSd.print(" ");

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
  byte status = 0;

  status = stu.readRHT(&umiditate, &temperatura);
  if (status)
  {
    Serial.print("Eroare : ");
    Serial.println(status);
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
    printTime(now);

    afisareValoriCard(umiditate, temperatura, co2, presiune, uvIndex);
    cardSd.println();

    cardSd.close();
  }

  printTimeLcd(now);
  afisareValoriLCD(umiditate, temperatura, co2, presiune, uvIndex);
  delay(PAUZA / 2);
}