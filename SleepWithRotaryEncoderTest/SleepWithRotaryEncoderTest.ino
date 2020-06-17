#define AWAKE_INDICATION_PIN LED_BUILTIN
//#define SLEEP_DELAY 30000
//#define INTERRUPT_PIN 2

//TODO: Convert time to milliseconds since startup
#define TASK_TIME 5000

// Rotary Encoder
#define CLK_PIN 2   // Generating interrupts using CLK signal
#define DT_PIN 3    // Reading DT signal
#define SW_PIN 4    // Reading Push Button switch

#include <DeepSleepScheduler.h>
#include <avr/sleep.h>

int i=0;

volatile boolean TurnDetected;  // need volatile for Interrupts
volatile boolean rotationdirection;  // CW or CCW rotation

// Interrupt routine runs if CLK goes from HIGH to LOW
void measureRotation ()  {
  if (digitalRead(CLK_PIN))
    rotationdirection = digitalRead(DT_PIN);
  else
    rotationdirection = !digitalRead(DT_PIN);
  TurnDetected = true;
  scheduler.schedule(ledOn);
}

void ledOn() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED On");
  scheduler.scheduleDelayed(ledOff, 2000);
}

void ledOff() {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED Off");
  attachInterrupt(0, isrInterruptPin, FALLING);
}

void isrInterruptPin() {
  scheduler.schedule(measureRotation);
  // detach interrupt to prevent executing it multiple
  // times when touching more than once.
  detachInterrupt(digitalPinToInterrupt(CLK_PIN));
}

void printIncrementor() {
  i=i+1;
  Serial.print(i);
}

void timedTask() {
  Serial.println("Doing timed task...");
  i=0;
  for (int i = 1; i <= 4; i++) {
    scheduler.scheduleAt(printIncrementor, scheduler.getScheduleTimeOfCurrentTask() + (i*1000));
  }
}


/*
 * Stay awake for set amount of time
 * If no inputs detected then sleep
 * Only wake if:
 *    1) A scheduled task needs executed
 *    2) An input interuption is detected
 */
void setup() {
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);

  // rotary encoder
  pinMode(CLK_PIN,INPUT);
  pinMode(DT_PIN, INPUT);  
  pinMode(SW_PIN, INPUT_PULLUP);

  // 2) An input interuption
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), isrInterruptPin, CHANGE);

//  // 1) A scheduled task
//  scheduler.scheduleAt(timedTask, TASK_TIME);
}

void loop() {
  scheduler.execute();
}
