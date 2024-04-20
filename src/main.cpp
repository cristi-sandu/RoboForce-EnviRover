#include <Arduino.h>
/*
#include <Wire.h>

// #include <DHT11.h>
#include <MQ135.h>
#include <BMP180I2C.h>
#include <SimpleDHT.h>

#include "SU.h"
#include "MOTOR.h"
#include "BUZZER.h"
#include "RG_LED.h"

#define TRIG_PIN 11
#define ECHO_PIN 10
#define DISTANTA_MIN 22
#define MOTOR_11 4
#define MOTOR_12 5
#define MOTOR_21 6
#define MOTOR_22 7
#define BUZZER_PIN 2
#define RED_PIN 9
#define GREEN_PIN 8
#define DHT_PIN 3
#define AER_PIN A0
#define SNZ_PRESIUNE 0x77
#define PAUZA 2500

SU senzor = SU(ECHO_PIN, TRIG_PIN);
MOTOR motor = MOTOR(MOTOR_11, MOTOR_12, MOTOR_21, MOTOR_22);
BUZZER buzzer = BUZZER(BUZZER_PIN);
RG_LED rgLed = RG_LED(RED_PIN, GREEN_PIN);
// DHT11 dht11 = DHT11(DHT_PIN);
MQ135 sensorAer = MQ135(AER_PIN);
BMP180I2C sensorPresiune = BMP180I2C(SNZ_PRESIUNE);
SimpleDHT11 dht11;
String btVal;

void teste();
void controlBluetooth();
void afisareValori(float temperatura, float umiditate, float co2    , float presiune);

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  motor.setup();
  sensorPresiune.resetToDefaults();
	sensorPresiune.setSamplingMode(BMP180MI::MODE_UHR);
}

void loop()
{
  // teste();
  // controlBluetooth();
  // float umiditate = dht11.readHumidity();
  // float temperatura = dht11.readTemperature();
  // Serial.println("=================================");
  // Serial.println("Sample DHT11...");

  // read without samples.
  byte temperatura = 0;
  byte umiditate = 0;
  if (dht11.read(DHT_PIN, &temperatura, &umiditate, NULL))
  {
    Serial.print("Read DHT11 failed.");
    return;
  }
  float co2 = sensorAer.getCorrectedPPM(temperatura, umiditate);
  afisareValori((float)umiditate, (float)temperatura, co2      , presiune);
  


  float presiune = sensorPresiune.getPressure();
  afisareValori(umiditate, temperatura, co2, presiune);
  
  delay(PAUZA);
}

void teste()
{
  buzzer.on();
  rgLed.red();
  delay(1000);
  buzzer.off();
  rgLed.green();
  delay(1000);
}

void controlBluetooth()
{
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
      motor.inainte();
      break;
    case 'B':
      motor.inapoi();
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
    }
    btVal = "";
  }
  else
  {
    btVal = "";
  }
}

void afisareValori(float umiditate, float temperatura, float co2   , float presiune)
{
  // umiditate
  Serial.print("Umiditate: ");
  Serial.print(umiditate);
  Serial.println(" %");
  // temperatura
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");
  // calitate aer
  Serial.print("Calitate Aer: ");
  Serial.print(co2);
  Serial.println(" ppm");
  

  
  // presiune
  Serial.print("Presiune: ");
  Serial.print(presiune);
  Serial.println(" Pa");


  delay(1000);
}
*/

#include <MQ135.h>

/*  MQ135 gas sensor
    Datasheet can be found here: https://www.olimex.com/Products/Components/Sensors/SNS-MQ135/resources/SNS-MQ135.pdf

    Application
    They are used in air quality control equipments for buildings/offices, are suitable for detecting of NH3, NOx, alcohol, Benzene, smoke, CO2, etc

    Original creator of this library: https://github.com/GeorgK/MQ135
*/

#define PIN_MQ135 A2

MQ135 mq135_sensor(PIN_MQ135);

float temperature = 30.0; // Assume current temperature. Recommended to measure with DHT22
float humidity = 45.0; // Assume current humidity. Recommended to measure with DHT22

void setup() {
  Serial.begin(9600);
}

void loop() {
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);

  Serial.print("MQ135 RZero: ");
  Serial.print(rzero);
  Serial.print("\t Corrected RZero: ");
  Serial.print(correctedRZero);
  Serial.print("\t Resistance: ");
  Serial.print(resistance);
  Serial.print("\t PPM: ");
  Serial.print(ppm);
  Serial.print("\t Corrected PPM: ");
  Serial.print(correctedPPM);
  Serial.println("ppm");

  delay(300);
}
