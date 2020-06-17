//www.elegoo.com
//2018.10.24
#include <Wire.h>
#include <DS3231.h>

DS3231 clock;
RTCDateTime dt;

const int MAX_FREQ = 20;

int waterFreq = 2; //twise per day
float waterDuration = 15.; //mins
float waterSchedule[MAX_FREQ];
int pwaterSchedule = 0;

int TIME_POS_OFFSET = 0;
float TIME_OFFSET = 3.;
float TIME_RANGE = 9.;

void clear_waterSchedule() {
  for (int i=0; i<waterFreq; i++) {
    waterSchedule[i]=0;
  }
  pwaterSchedule=0;
  TIME_POS_OFFSET=0;
}

float divider_size(int cuts) {
  return TIME_RANGE / (cuts + 1);
}

void divider(int cuts) {
  float count=0.;
  float dividerSize = TIME_RANGE / float(cuts + 1);
  int i=0;
  for (i; i<cuts; i++) {
    
    count += dividerSize;
    waterSchedule[i + pwaterSchedule] = count + TIME_POS_OFFSET + TIME_OFFSET;
    
    Serial.print("DEBUG: in slot: ");
    Serial.println(i+pwaterSchedule);
    Serial.print("DEBUG: Storing time: ");
    Serial.println(waterSchedule[i+pwaterSchedule]);
    Serial.println("");
  }
  pwaterSchedule=i;
}

void calc_watering_waterSchedule() {
  clear_waterSchedule();
  int half_freq = waterFreq / 2;
  if (waterFreq % 2 == 0) {
    divider(half_freq);
    TIME_POS_OFFSET = 9;
    divider(half_freq);
  } else {
    divider((waterFreq + 1) / 2);
    TIME_POS_OFFSET = 9;
    divider(half_freq);
  }
}

void print_waterSchedule() {
  Serial.print("waterSchedule Scheduled: ");

  // loop through the elements of the array
  for (int i = 0; i < waterFreq; i++) {
    Serial.println(waterSchedule[i]);
  }
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
  /*
  Tips:This command will be executed every time when Arduino restarts. 
       Comment this line out to store the memory of DS3231 module
  */

  calc_watering_waterSchedule();
  print_waterSchedule();
}

void loop()
{
//  dt = clock.getDateTime();

  // For leading zero look to DS3231_dateformat example

//  Serial.print("Raw data: ");
//  Serial.print(dt.year);   Serial.print("-");
//  Serial.print(dt.month);  Serial.print("-");
//  Serial.print(dt.day);    Serial.print(" ");
//  Serial.print(dt.hour);   Serial.print(":");
//  Serial.print(dt.minute); Serial.print(":");
//  Serial.print(dt.second); Serial.println("");

//  delay(1000);
}
