// Deep Sleep Schedular
#define AWAKE_INDICATION_PIN LED_BUILTIN
#define SLEEP_MODE SLEEP_MODE_IDLE

// Rotary Encoder
#define CLK_PIN 2   // Generating interrupts using CLK signal
#define DT_PIN 3    // Reading DT signal
#define SW_PIN 4    // Reading Push Button switch
#define RELAY_PIN 6    // https://www.youtube.com/watch?v=uh5dLC6IkQQ
#define BKL_PIN 23

#include <LiquidCrystal.h>
#include <Wire.h>
#include <DS3231.h>
#include <DeepSleepScheduler.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// ******************************* //
//        ROTARY ENCODER SETUP     //
// ******************************* //

volatile boolean TurnDetected;  // need volatile for Interrupts
volatile boolean rotationdirection;  // CW or CCW rotation
int RotaryPosition = 0;    // To store Stepper Motor Position
int PrevPosition;     // Previous Rotary position Value to check accuracy
int StepsToTake;      // How much to move Stepper

// ******************************* //
//        WATERING SETTINGS        //
// ******************************* //

const int MAX_WATER_FREQ = 50;
const int MAX_DURATION = 30;

int waterFreq = 1; //once per day
int waterDuration = 5; //mins
float waterSchedule[MAX_WATER_FREQ];
int pwaterSchedule = 0;

int TIME_POS_OFFSET = 0;
float TIME_OFFSET = 3.;
float TIME_RANGE = 9.;
boolean shouldWaterFlag = false;

// ******************************* //
//          OTHER SETTINGS         //
// ******************************* //
volatile boolean sleeping = false;
boolean exiting = false;
unsigned long startTime = millis();
int currentMenu = 1;

DS3231 clock;
RTCDateTime dt;


// ******************************* //
//      WATERING SCHEDULAR         //
// ******************************* //
void clear_water_schedule() {
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
  }
  pwaterSchedule=i;
}

void calc_watering_schedule() {
  clear_water_schedule();
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
  dt = clock.getDateTime();
  int i=0;
  while (!shouldWaterFlag & i < waterFreq) {
    int minute = (waterSchedule[i] * 10) - (int(waterSchedule[i]) * 10);
    shouldWaterFlag = dt.hour == int(waterSchedule[i]) & dt.minute == minute * 6;
    i++;
  }
}

void waterBurst () {
  digitalWrite(RELAY_PIN, HIGH);
  delay(500);
  digitalWrite(RELAY_PIN, LOW);
}

void water () {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Watering Now...");
  
  digitalWrite(RELAY_PIN, HIGH);
  for (int i = waterDuration * 60; i > 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print(i);
    lcd.print(" seconds  ");
    delay(1000);
  }
  digitalWrite(RELAY_PIN, LOW);
  
  lcd.setCursor(0, 0);
  lcd.print("Completed!");
  delay(2000);
}

void print_water_schedule() {
  Serial.print("Watering Schedule: ");

  // loop through the elements of the array
  for (int i = 0; i < waterFreq; i++) {
    Serial.println(waterSchedule[i]);
  }
}


// ******************************* //
//             TIME                //
// ******************************* //

void restartTimer () {
  startTime = millis();
}

unsigned long getTimeElapsed () {
  return millis() - startTime;
}

boolean hasTimedOut () {
  return getTimeElapsed() > 200000;
}


// ******************************* //
//                MENU             //
// ******************************* //

void showMenu () {
  switch (currentMenu) {
    case 1:
      lcd.begin(16, 2);
      lcd.setCursor(0, 0);
      lcd.print("* Frequency");
      lcd.setCursor(0, 1);
      lcd.print("  Duration   ...");
      break;
    case 2:
      lcd.begin(16, 2);
      lcd.setCursor(0, 0);
      lcd.print("  Frequency");
      lcd.setCursor(0, 1);
      lcd.print("* Duration   ...");
      break;
    case 3:
      lcd.begin(16, 2);
      lcd.setCursor(0, 0);
      lcd.print("* Water Now");
      lcd.setCursor(0, 1);
      lcd.print("  Exit");
      break;
    case 4:
      lcd.begin(16, 2);
      lcd.setCursor(0, 0);
      lcd.print("  Water Now");
      lcd.setCursor(0, 1);
      lcd.print("* Exit");
      break;
  }
}

void showSubMenu (int option) {
  switch (option) {
    case 1:
      lcd.begin(16, 2);
      lcd.setCursor(0, 0);
      lcd.print("Frequency:");
      lcd.setCursor(0, 1);
      lcd.print(waterFreq);
      lcd.print(" times a day");
      while (!buttonPressed()) {
        int rot = getRotation();
        if (rot != 0) {
          waterFreq = constrain(waterFreq + rot, 0, MAX_WATER_FREQ);
          lcd.setCursor(0, 1);
          lcd.print(waterFreq);
          lcd.print(" times a day");
        }
      }
      calc_watering_schedule();
      print_water_schedule();
      break;
    case 2:
      lcd.begin(16, 2);
      lcd.setCursor(0, 0);
      lcd.print("Water Duration:");
      lcd.setCursor(0, 1);
      lcd.print(waterDuration);
      lcd.print(" minutes ");
      while (!buttonPressed()) {
        int rot = getRotation();
        if (rot != 0) {
          waterDuration = constrain(waterDuration + rot, 1, MAX_DURATION);
          lcd.setCursor(0, 1);
          lcd.print(waterDuration);
          lcd.print(" minutes ");
        }
      }
      break;
    case 3:
      water();
      break;
    case 4:
      lcd.begin(16, 2);
      lcd.setCursor(0, 0);
      lcd.print("WATER BOT  ^^,");
      lcd.setCursor(0, 1);
      lcd.print("going to sleep!");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GOODBYE!");
      delay(2000);
      lcd.noDisplay();
      delay(10000);
      //TODO: Shut off LCD screen
      break;
  }
}


// ******************************* //
//         ROTARY ENCODER          //
// ******************************* //

boolean buttonPressed() {
  if (!(digitalRead(SW_PIN))) {   // check if button is pressed
    Serial.println("Button Pressed");
    waterBurst();
    
    if (RotaryPosition == 0) {  // check if button was already pressed
      
    } else {
      RotaryPosition = 0; // Reset position to ZERO
      Serial.print("Rotary Position Reset: ");
      Serial.println(RotaryPosition);
    }
    return true;
  } else {
    return false;
  }
}

int getRotation() {
  int directionAngle = 0;
  
  if (TurnDetected)  {
    PrevPosition = RotaryPosition; // Save previous position in variable
    if (rotationdirection) { // CW turn
      RotaryPosition = RotaryPosition + 1;
      directionAngle = 1;
    } else { // CCW turn
      RotaryPosition = RotaryPosition - 1;
      directionAngle = -1;
    }
    waterBurst();
    TurnDetected = false;  // do NOT repeat IF loop until new rotation detected
  }

  return directionAngle;
}

void check_inputs() {
  if (buttonPressed()) {
    showSubMenu(currentMenu);
    showMenu();
  }
  
  int rot = getRotation();
  if (rot != 0) {
    currentMenu = constrain(currentMenu + rot, 1, 4);
    showMenu();
  }

  shouldWater();
  if (shouldWaterFlag) {
    water();
  }
}

// Interrupt routine runs if CLK goes from HIGH to LOW
void measure_rotation()  {
  if (digitalRead(CLK_PIN))
    rotationdirection = digitalRead(DT_PIN);
  else
    rotationdirection = !digitalRead(DT_PIN);
  TurnDetected = true;
  Serial.println("Turn detected!");
  check_inputs();
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), isr, FALLING);
}

void isr() {
  delay(4);
  scheduler.scheduleDelayed(measure_rotation);
  // detach interrupt to prevent executing it multiple
  // times when touching more than once.
  detachInterrupt(digitalPinToInterrupt(CLK_PIN));
}

//// Interrupt routine runs if CLK goes from HIGH to LOW
//void isr ()  {
//  delay(4);  // delay for Debouncing
//  if (digitalRead(CLK_PIN))
//    rotationdirection = digitalRead(DT_PIN);
//  else
//    rotationdirection = !digitalRead(DT_PIN);
//  TurnDetected = true;
//}

// ******************************* //
//                MAIN             //
// ******************************* //

void start_clock() {
  clock.begin();
  clock.setDateTime(__DATE__, __TIME__);
}

void welcome() {
  // LCD Screen
  lcd.begin(14, 1);
  lcd.print("WATER BOT    ^^,");
}

void setup() {
  Serial.begin(9600);
  
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(CLK_PIN,INPUT);
  pinMode(DT_PIN,INPUT);  
  pinMode(SW_PIN,INPUT_PULLUP);
  pinMode(BKL_PIN, OUTPUT);
  digitalWrite(BKL_PIN, HIGH);
  
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), isr, FALLING);

  scheduler.schedule(start_clock);
  scheduler.schedule(welcome);
  scheduler.schedule(calc_watering_schedule);
  scheduler.schedule(print_water_schedule);
  //TODO: schedule automatic waterings
  scheduler.scheduleDelayed(showMenu, 2000);
}

void loop() {
  scheduler.execute();
}
