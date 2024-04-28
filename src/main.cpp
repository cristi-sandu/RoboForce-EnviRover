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
ThreeWire wire(DAT_PIN, CLK_PIN, RST_PIN);
RtcDS1302<ThreeWire> Rtc(wire);
String btVal;
STU stu = STU(STU_PIN);

bool deconectat = true;

void afisareValoriCard(byte temperatura, byte umiditate, float co2, float presiune, float uvIndex);
void afisareValoriCardExcel(byte temperatura, byte umiditate, float co2, float presiune, float uvIndex);
void afisareValoriLCD(byte temperetura, byte umiditate, float co2, float presiune, float uvIndex);
void afisareValoriSerial(byte temperatura, byte umiditate, float co2, float presiune, float uvIndex);
void esteCf(int numar);
void functionare();
void initPresiune();
void printTime(RtcDateTime now);
void printTimeExcel(RtcDateTime now);
void printTimeLcd(RtcDateTime now);
void procesareUV(float uvIndex);
void timer(int ore, int minute, int secunde);

void setup()
{
  lcd.begin();
  lcd.backlight();
  motor.setup();
  Rtc.Begin();
  Serial.begin(BAUD_RATE);
  stu.begin();

  if (!Serial || !SD.begin(CS_PIN) || !sensorPresiune.begin())
  {
    lcd.setCursor(5, 0);
    lcd.print("eroare");

    while (true)
    {
      if (Serial && SD.begin(CS_PIN) && sensorPresiune.begin())
      {
        break;
      }
      delay(100);
    }
  }

  sensorPresiune.resetToDefaults();
  sensorPresiune.setSamplingMode(BMP180MI::MODE_UHR);
  cardSd = SD.open("excel.txt", FILE_WRITE);
  cardSd.close();

  // actualizare ora
  // RtcDateTime currentTime = RtcDateTime(__DATE__, __TIME__);
  // Rtc.SetDateTime(currentTime);
}

void loop()
{
  if (Serial.available() <= 0 && deconectat)
  {
    deconectat = true;
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("deconectat");
  }

  while (Serial.available() > 0)
  {
    deconectat = false;
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("conectat");
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
      functionare();
      break;
    case 'c':
      lcd.clear();
      lcd.print(" calibrare snz.");
      timer(0, 10, 0);
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
    lcd.setCursor(5, 0);
    lcd.print("eroare");
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
    lcd.print("foarte scazut");
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

void esteCf(int numar)
{
  if (numar < 10)
  {
    lcd.print(0);
  }
}

void timer(int ore, int minute, int secunde)
{
  lcd.setCursor(4, 1);
  esteCf(ore);
  lcd.print(ore);
  lcd.print(":");
  esteCf(minute);
  lcd.print(minute);
  lcd.print(":");
  esteCf(secunde);
  lcd.print(secunde);
  delay(1000);

  if (!minute && ore)
  {
    ore--;
    minute = 60;
  }
  if (!secunde && minute)
  {
    minute--;
    secunde = 59;
  }

  for (int i = ore; i >= 0; i--)
  {
    lcd.setCursor(4, 1);
    esteCf(i);
    lcd.print(i);
    lcd.print(":");

    for (int j = minute; j >= 0; j--)
    {
      lcd.setCursor(7, 1);
      esteCf(j);
      lcd.print(j);
      lcd.print(":");

      for (int k = secunde; k >= 0; k--)
      {
        lcd.setCursor(10, 1);
        esteCf(k);
        lcd.print(k);
        delay(1000);
      }
    }
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
  lcd.print("data: ");
  esteCf(now.Day());
  lcd.print(now.Day());
  lcd.print("/");
  esteCf(now.Month());
  lcd.print(now.Month());
  lcd.print("/");
  esteCf(now.Year());
  lcd.print(now.Year());

  lcd.setCursor(0, 1);
  lcd.print(" ora: ");
  esteCf(now.Hour());
  lcd.print(now.Hour());
  lcd.print(":");
  esteCf(now.Minute());
  lcd.print(now.Minute());
  lcd.print(":");
  esteCf(now.Second());
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

void printTimeExcel(RtcDateTime now)
{
  cardSd.print(now.Day());
  cardSd.print("-");
  cardSd.print(now.Month());
  cardSd.print("-");
  cardSd.print(now.Year());
  cardSd.print(" ");

  cardSd.print(now.Hour());
  cardSd.print(":");
  cardSd.print(now.Minute());
  cardSd.print(":");
  cardSd.print(now.Second());
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

void afisareValoriCardExcel(byte temperatura, byte umiditate, float co2, float presiune, float uvIndex)
{
  // umiditate
  cardSd.print(umiditate);
  cardSd.print(" ");

  // temperatura
  cardSd.print(temperatura);
  cardSd.print(" ");

  // co2
  cardSd.print(co2);
  cardSd.print(" ");

  // presiune
  cardSd.print(presiune);
  cardSd.print(" ");

  // uv
  cardSd.print(uvIndex);
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
    lcd.setCursor(5, 0);
    lcd.print("eroare");
  }

  initPresiune();

  float presiune = sensorPresiune.getPressure() / 100000;
  float co2 = sensorAer.getCorrectedPPM(temperatura, umiditate);
  float mV = uv.read();
  float uvIndex = uv.index(mV);

  cardSd = SD.open("excel.txt", FILE_WRITE);
  RtcDateTime now = Rtc.GetDateTime();

  if (cardSd)
  {
    // afisare pentru crearea de grafice in excel
    printTimeExcel(now);
    afisareValoriCardExcel(temperatura, umiditate, co2, presiune, uvIndex);

    cardSd.close();
  }

  printTimeLcd(now);
  afisareValoriLCD(umiditate, temperatura, co2, presiune, uvIndex);

  delay(PAUZA / 2);
}