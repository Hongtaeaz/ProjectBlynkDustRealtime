#define BLYNK_TEMPLATE_ID "TMPL6CJP8buhA"
#define BLYNK_TEMPLATE_NAME "Project"
#define BLYNK_AUTH_TOKEN "BRgZ4NxaFKBQ1g8Gj_Y0-LvYbOpamUfX"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "internet";
char pass[] = "123456789";

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include "PMS.h"
#include <SoftwareSerial.h>
SoftwareSerial mySerial(D5);
PMS pms(mySerial);
PMS::DATA data;

#define Buzzer D6
#define ON LOW
#define OFF HIGH

unsigned long period = 3000; // 3 seconds interval
unsigned long last_time = 0;
int displayState = 0; // Variable to track which PM value to display
bool check = true;
BlynkTimer timer;
void myTimerEvent()
{
  Blynk.virtualWrite(V5, millis() / 1000);
}

void setup()
{
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, OFF);
  mySerial.begin(9600);
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SSID:internet");
  lcd.setCursor(0, 1);
  lcd.print("PASS:123456789");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();

  if (pms.read(data))
  {
    Serial.print("PM 1.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_1_0);
    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(data.PM_AE_UG_2_5);
    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_10_0);
    Serial.println();

    Blynk.virtualWrite(V1, data.PM_AE_UG_1_0);
    Blynk.virtualWrite(V2, data.PM_AE_UG_2_5);
    Blynk.virtualWrite(V3, data.PM_AE_UG_10_0);
    if (data.PM_AE_UG_2_5 >= 100) {
      if (check == true) {
        check = false;
        for (int Rx = 1; Rx <= 10; Rx++) {
          digitalWrite(Buzzer, ON); delay(500);
          digitalWrite(Buzzer, OFF); delay(500);
        }
      }
    }
  }

  if (millis() - last_time > period)
  {
    last_time = millis();
    lcd.clear();

    // Switch between PM 1.0, PM 2.5, and PM 10.0 display
    switch (displayState)
    {
      case 0:
        lcd.setCursor(2, 0);
        lcd.print("PROJECT DUST");
        lcd.setCursor(0, 1);
        lcd.print("PM 1.0: " + String(data.PM_AE_UG_1_0) + " ug/m3");
        displayState = 1;
        break;
      case 1:
        lcd.setCursor(2, 0);
        lcd.print("PROJECT DUST");
        lcd.setCursor(0, 1);
        lcd.print("PM 2.5: " + String(data.PM_AE_UG_2_5) + " ug/m3");
        displayState = 2;
        break;
      case 2:
        lcd.setCursor(2, 0);
        lcd.print("PROJECT DUST");
        lcd.setCursor(0, 1);
        lcd.print("PM 10.0: " + String(data.PM_AE_UG_10_0) + " ug/m3");
        displayState = 0;
        break;
    }
  }
}
