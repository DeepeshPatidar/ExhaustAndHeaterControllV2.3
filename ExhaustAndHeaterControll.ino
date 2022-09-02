//1.9 Removed condition triggering by minutes. Triggering only by hours. As by using minutes causing wrong conditions and fan and halogen not triggerring.
//1.5 added test function to on/off in 10 s
// V1.3 Added if loop for Fan and Halogen on and off
///Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
//Pin Connection
//RTC module  Arduino Uno 16 and 17
//DHT sensor pin 7
//
//
//

// Comment1_Open- below code is for temperature humidity dewpoint and heat index
double dewPoint(double celsius, double humidity)
{
  // (1) Saturation Vapor Pressure = ESGG(T)
  double RATIO = 373.15 / (273.15 + celsius);
  double RHS = -7.1002108 * (RATIO - 1);
  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / RATIO ))) - 1) ;
  RHS += 8.1328e-3 * (pow(10, (-3.4101410 * (RATIO - 1))) - 1) ;
  RHS += log10(1013.246);

  // factor -3 is to adjust units - Vapor Pressure SVP * humidity
  double VP = pow(10, RHS - 3) * humidity;

  // (2) DEWPOINT = F(Vapor Pressure)
  double T = log(VP / 0.61078); // temp var
  return (241.88 * T) / (17.558 - T);
}
// Comment1_Close- below code is for temperature humidity dewpoint and heat index


#define on LOW
#define off HIGH


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // Pin connection A4 and A5
#include <DHT.h>;
//Constants
#define DHTPIN 9     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//const int ledPin =  13;
// the number of the LED pin will connect here relay to trigger fan
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
//Variables
//          FAN connected on K1 and K2
// FAN ON( hour: minute)  FAN off ( hour : minute ||| Halogen ON( hour: minute)  Halogen off ( hour: minute)
//          |      |                  |      |    |||               |     |                      |     |
//          V      V                  V      V    |||               V     V                      V     V
// int     x=23 , y=24,             w =0,   z= 50,                  p=0,  q=51;         // check parameter input line
int       x = 22,                    w = 6;        //Main line with actual trgger paramaters

//int       a=14 , b=19,            c =14,   d= 21 ,               m=14,  n=20  ;//Test line
// int       a=13 ,                    c =14;         // use same const as for the fan off time as we want to off halogen at the same time when fan off.
//int       d = 23,          e = 24
;
// int       i=14 , j=22,            k =14,   l= 24,                o=14,  t=23   ;  //Test line
//int       i = 13  ,           k = 14 ;      // use same const as for the fan off time as we want to off halogen at the same time when fan off.
// int pushButton = 2;
int ManualSwitch;
int HalogenSwitch, RealHour, RealMinute;
float hum;  //Stores humidity value
float temp; //Stores temperature value
float f, h , dewpoint;

#include "RTClib.h"
RTC_DS3231 rtc;
// char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
// #define DHTPIN 2     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.


void setup() {

  pinMode(11, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  //Below code is for LCD 
  // initialize the LCD
  // digitalWrite(pushButton, HIGH);
  // pinMode(pushButton, INPUT);
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();


  //Setting pin mode as output
  pinMode(12, OUTPUT); pinMode(13, OUTPUT);


  //Setting degital write to HIGH to off the pins(i.e. fan and halogen)
  digitalWrite(12, LOW); digitalWrite(13, LOW);
  //Above code is for LCD 
  //pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
  //Serial.println(F("DHTxx test!"));
  dht.begin();
  //below code is for  DS3231 rtc
  //#ifndef ESP8266
  // while (!Serial); // wait for serial port to connect. Needed for native USB
  //#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
  // rtc.adjust(DateTime(2020, 12, 13, 17, 26, 0));
  //if (rtc.lostPower()) {
  //Serial.println("RTC lost power, let's set the time!");
  // When time needs to be set on a new device, or after a power loss, the
  // following line sets the RTC to the date & time this sketch was compiled
  //rtc.adjust(DateTime(2020, 6, 05, 05, 04, 0));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2020, 6, 05, 3, 24, 0));
  //}

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
 // rtc.adjust(DateTime(2022, 8, 28, 9, 59, 40));
  //if (! rtc.initialized()) {
  //Serial2.println("RTC is NOT running!");
  // following line sets the RTC to the date & time this sketch was compiled
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}
// Yaha tak DS3231 ke liye tha (read last comment line)

void FANON()

{
  digitalWrite(13, HIGH);
  /* Serial.print("Fan ON");
    Serial.print("\n");*/
}

void FANOFF()

{
  digitalWrite(13, LOW);
  /* Serial.print("Fan OFF");
    Serial.print("\n");*/
}

void HALOGENON()

{
  digitalWrite(12, HIGH);
  /* Serial.print("Halogen triggered ON");
    Serial.print("\n");*/
}

void HALOGENOFF()
{

  digitalWrite(12, LOW);
  /*Serial.print("Halogen triggered OFF");
    Serial.print("\n");*/

}

void PrintLCDSerial()
{
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  f = dht.readTemperature(true);
  //float hi = dht.computeHeatIndex(f, h);
  //float hiDegC = dht.convertFtoC(hi);
  dewpoint = dewPoint(temp, hum);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.setCursor(5, 0);
  lcd.print(temp);
  lcd.setCursor(11, 0);
  lcd.print("C");
  // lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Hum :");
  lcd.setCursor(5, 1);
  lcd.print(hum);
  lcd.setCursor(11, 1);
  lcd.print("%RH");
  delay(3000);

  DateTime now = rtc.now();
  RealHour = now.hour();
  RealMinute = now.minute();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DewP:");
  lcd.setCursor(5, 0);
  lcd.print(dewpoint);
  lcd.setCursor(11, 0);
  lcd.print("C");

  //Second line showing time and date
  lcd.setCursor(0, 1);
  lcd.print(RealHour);
  lcd.setCursor(2, 1);
  lcd.print(":");
  lcd.setCursor(3, 1);
  lcd.print(RealMinute);
  lcd.setCursor(6, 1);
  lcd.print(now.day(), DEC);
  lcd.setCursor(8, 1);
  lcd.print("/");
  lcd.setCursor(9, 1);
  lcd.print(now.month(), DEC);
  lcd.setCursor(11, 1);
  lcd.print("/");
  lcd.setCursor(12, 1);
  lcd.print(now.year(), DEC);
  delay(3000);

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(", Temp: ");

  Serial.print(temp);
  //Serial.print("\n");

  Serial.print(", dewpoint:");
  Serial.print(dewpoint);
  Serial.print("\n");
  //Serial.print( Td);

  // DateTime now = rtc.now();
  //from beHIGH the code is for DS3231 till next command line
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(" Time ");
  // Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  // Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  Serial.print("\n");
}

void loop() {

  ManualSwitch = digitalRead(11);
  HalogenSwitch = digitalRead(10);
  DateTime now = rtc.now();
  RealHour = now.hour();
  RealMinute = now.minute();
  if (HalogenSwitch == on)
  {
    if (ManualSwitch == off)
    {
      //Continuous FANON halogen ON
      FANON();
      HALOGENON();

    }
    else
    {
      if (((RealHour >= x) & (RealHour <= (w-1))) )//|| ((RealHour >= d) & (RealHour <= e)))
      {
        //FANON HALOGENON
        FANON();
        HALOGENON();

      }
      else
      {
        //FAN and HAlogen OFF
        FANOFF();
        HALOGENOFF();

      }
    }
  }
  else
  {

    if (ManualSwitch == off)
    {
      //Continuous FANON halogen OFF
      FANON();
      HALOGENOFF();

    }
    else
    {
       if (((RealHour >= x) & (RealHour <= (w-1))) )//|| ((RealHour >= d) & (RealHour <= e)))
      {
        //FANON HALOGENOFF
        FANON();
        HALOGENOFF();

      }
      else
      {
        //FAN and HAlogen OFF
        FANOFF();
        HALOGENOFF();

      }
    }

  }
  PrintLCDSerial();
}
