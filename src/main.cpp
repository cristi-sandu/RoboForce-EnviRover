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
#define BT_STATUS_PIN A2
#define CS_PIN 2
#define STU_PIN 3
#define MOTOR_11 4
#define MOTOR_12 5
#define MOTOR_21 6
#define MOTOR_22 7
#define RST_PIN 8
#define DAT_PIN 9
#define CLK_PIN 10
#define LCD 0x27
#define LINII_LCD 2
#define COLOANE_LCD 16
#define SNZ_PRESIUNE 0x77
#define PAUZA 5000

BMP180I2C sensorPresiune = BMP180I2C(SNZ_PRESIUNE);
File cardSd;
GUVAS12SD uv(UV_PIN);
LCD_I2C lcd(LCD, COLOANE_LCD, LINII_LCD);
MOTOR motor = MOTOR(MOTOR_11, MOTOR_12, MOTOR_21, MOTOR_22);
MQ135 sensorAer = MQ135(AER_PIN);
ThreeWire wire(DAT_PIN, CLK_PIN, RST_PIN);
RtcDS1302<ThreeWire> Rtc(wire);
String btVal;
STU stu = STU(STU_PIN);

void afisareTimp(RtcDateTime now, char optiune);
void afisareValori(byte temperatura, byte umiditate, float co2, float presiune, float uvIndex, char optiune);
void checkBluetooth(bool status);
void esteCf(int numar);
void functionare();
void initPresiune();
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
  pinMode(BT_STATUS_PIN, INPUT);

  if (!Serial || !SD.begin(CS_PIN) || !sensorPresiune.begin())
  {
    lcd.clear();
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
  checkBluetooth(digitalRead(BT_STATUS_PIN));

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
      functionare();
      break;
    case 'c':
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("calibrare snz.");
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

void checkBluetooth(bool status)
{
  if (status == false)
  {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("stare bt");
    lcd.setCursor(3, 1);
    lcd.print("deconectat");
  }
  else
  {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("stare bt");
    lcd.setCursor(4, 1);
    lcd.print("conectat");
  }
}

void initPresiune()
{
  if (!sensorPresiune.measurePressure())
  {
    lcd.clear();
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
    lcd.setCursor(0, 1);
    lcd.print("extrem de scazut");
    break;
  case 1:
  case 2:
    lcd.setCursor(5, 1);
    lcd.print("scazut");
    break;
  case 3:
  case 4:
  case 5:
    lcd.setCursor(5, 1);
    lcd.print("mediu");
    break;
  case 6:
  case 7:
    lcd.setCursor(4, 1);
    lcd.print("ridicat");
    break;
  case 8:
  case 9:
  case 10:
    lcd.setCursor(1, 1);
    lcd.print("foarte ridicat");
    break;

  default:
    lcd.setCursor(1, 1);
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

void afisareTimp(RtcDateTime now, char optiune)
{
  switch (optiune)
  {
  case 'c':
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
    break;
  case 'e':
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
    break;
  case 'l':
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
    break;
  }
}

void afisareValori(byte umiditate, byte temperatura, float co2, float presiune, float uvIndex, char optiune)
{
  switch (optiune)
  {
  case 'c':
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
    break;
  case 'e':
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
    break;
  case 'l':
    // umiditate
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("umid : ");
    esteCf(umiditate);
    lcd.print(umiditate);
    lcd.print("%");
    // tempratura
    lcd.setCursor(3, 1);
    lcd.print("temp : ");
    esteCf(temperatura);
    lcd.print(temperatura);
    lcd.print("C");
    delay(PAUZA);

    // calitatea aerului
    lcd.clear();
    lcd.setCursor(1, 0);
    esteCf(co2);
    lcd.print("CO2 : ");
    lcd.print(co2);
    lcd.print("ppm");
    // presiune atmosferica
    lcd.setCursor(0, 1);
    lcd.print("p : ");
    esteCf(presiune);
    lcd.print(presiune);
    lcd.print("*10^3Pa");
    delay(PAUZA);

    // index UV
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("index UV : ");
    lcd.print((int)uvIndex);
    // mesaj
    procesareUV(uvIndex);

    delay(PAUZA);
    lcd.clear();
    break;
  case 's':
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
    break;
  }
}

void functionare()
{
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("inregistrare");
  lcd.setCursor(5, 1);
  lcd.print("valori");

  byte temperatura = 0;
  byte umiditate = 0;
  byte status = 0;

  status = stu.readRHT(&umiditate, &temperatura);
  if (status)
  {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("eroare");
  }

  initPresiune();

  float presiune = sensorPresiune.getPressure() / 1000;
  float co2 = sensorAer.getCorrectedPPM(temperatura, umiditate);
  float mV = uv.read();
  float uvIndex = uv.index(mV);

  delay(PAUZA / 2);

  cardSd = SD.open("excel.txt", FILE_WRITE);
  RtcDateTime now = Rtc.GetDateTime();

  if (cardSd)
  {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("scriere pe");
    lcd.setCursor(6, 1);
    lcd.print("card");

    // afisare pentru crearea de grafice in excel
    afisareTimp(now, 'e');
    afisareValori(umiditate, temperatura, co2, presiune, uvIndex, 'e');

    cardSd.close();
  }

  delay(PAUZA / 2);

  afisareTimp(now, 'l');
  afisareValori(umiditate, temperatura, co2, presiune, uvIndex, 'l');
}