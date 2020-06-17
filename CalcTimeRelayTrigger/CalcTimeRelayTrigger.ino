//www.elegoo.com
//2018.10.24
#include <Wire.h>
#include <DS3231.h>

DS3231 clock;
RTCDateTime dt;

// ****** WATERING SETTINGS ****** //
const int waterFreq = 2; //twise a day
const int waterDuration = 1; //mins
const int MAX_FREQ = 20;
float waterSchedule[waterFreq] = {18};

// ****** OTHER SETTINGS ****** //
int pwaterSchedule = 0;
int TIME_POS_OFFSET = 0;
float TIME_OFFSET = 3.;
float TIME_RANGE = 9.;

const int relay = 8;
boolean shouldWaterFlag = false;

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

void shouldWater() {
  int i=0;
  while (!shouldWaterFlag & i < waterFreq) {
    int minute = (waterSchedule[i] * 10) - (int(waterSchedule[i]) * 10);
    shouldWaterFlag = dt.hour == int(waterSchedule[i]) & dt.minute == minute * 6;
    i++;
  }
}

void water() {
  digitalWrite(relay, HIGH);
  Serial.println("Watering Started...");
  while(clock.getDateTime().minute != dt.minute + waterDuration) {
    delay(1000);
  }
  digitalWrite(relay, LOW);
  shouldWaterFlag = false;
  delay(1000);
  Serial.println("Completed!");
}

void print_waterSchedule() {
  Serial.print("waterSchedule Scheduled: ");

  // loop through the elements of the array
  for (int i = 0; i < waterFreq; i++) {
    Serial.println(waterSchedule[i]);
  }
}

void print_date_time() {
  Serial.print("Raw data: ");
  Serial.print(dt.year);   Serial.print("-");
  Serial.print(dt.month);  Serial.print("-");
  Serial.print(dt.day);    Serial.print(" ");
  Serial.print(dt.hour);   Serial.print(":");
  Serial.print(dt.minute); Serial.print(":");
  Serial.print(dt.second); Serial.println("");
}

void setup()
{
  Serial.begin(9600);

  Serial.println("Initialize RTC module");
  clock.begin();
  
  // Send sketch compiling time to Arduino
  clock.setDateTime(__DATE__, __TIME__);

  pinMode(relay, OUTPUT);

//  calc_watering_waterSchedule();
  print_waterSchedule();
}

void loop()
{
  dt = clock.getDateTime();

  shouldWater();
  if (shouldWaterFlag) {
    water();
  } else {
    delay(1000);
  }
}
