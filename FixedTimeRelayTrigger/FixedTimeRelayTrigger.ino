//www.elegoo.com
//2018.10.24
#include <Wire.h>
#include <DS3231.h>

DS3231 clock;
RTCDateTime dt;

const int relay = 6;

const int WATER_FREQ = 3; //twise a day
const int WATER_DURATION = 15; //mins
const float WATER_SCHEDULE[WATER_FREQ] = {23.7, 23.8, 13.0};

boolean shouldWaterFlag = false;

void shouldWater() {
  int i=0;
  while (!shouldWaterFlag & i < WATER_FREQ) {
    int minute = (WATER_SCHEDULE[i] * 10) - (int(WATER_SCHEDULE[i]) * 10);
    shouldWaterFlag = dt.hour == int(WATER_SCHEDULE[i]) & dt.minute == minute * 6;
    i++;
  }
}

void water() {
  digitalWrite(relay, HIGH);
  Serial.println("Watering Started...");
  while(clock.getDateTime().minute != dt.minute + WATER_DURATION) {
    delay(1000);
  }
  
//  for (int i = WATER_DURATION * 60; i > 0; i--) {
//    delay(1000);
//  }
  digitalWrite(relay, LOW);
  shouldWaterFlag = false;
  delay(1000);
  Serial.println("Completed!");
}

void setup()
{
  Serial.begin(9600);

  Serial.println("Initialize RTC module");
  clock.begin();
  
  // Manual (YYYY, MM, DD, HH, II, SS
  // clock.setDateTime(2016, 12, 9, 11, 46, 00);
  
  // Send sketch compiling time to Arduino
  clock.setDateTime(__DATE__, __TIME__);

  pinMode(relay, OUTPUT);
}

void loop()
{
  dt = clock.getDateTime();
  Serial.print("Raw data: ");
  Serial.print(dt.year);   Serial.print("-");
  Serial.print(dt.month);  Serial.print("-");
  Serial.print(dt.day);    Serial.print(" ");
  Serial.print(dt.hour);   Serial.print(":");
  Serial.print(dt.minute); Serial.print(":");
  Serial.print(dt.second); Serial.println("");

  shouldWater();
  if (shouldWaterFlag) {
    water();
  } else {
    delay(1000);
  }
}
