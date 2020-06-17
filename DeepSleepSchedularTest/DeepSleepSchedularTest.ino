#define AWAKE_INDICATION_PIN LED_BUILTIN
//#define SLEEP_DELAY 30000
#define INTERRUPT_PIN 2

//TODO: Convert time to milliseconds since startup
#define TASK_TIME 10000

#include <DeepSleepScheduler.h>

int i=0;


void ledOn() {
  digitalWrite(LED_BUILTIN, HIGH);
  scheduler.scheduleDelayed(ledOff, 2000);
}

void ledOff() {
  digitalWrite(LED_BUILTIN, LOW);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), isrInterruptPin, FALLING);
}

void isrInterruptPin() {
  scheduler.schedule(ledOn);
  // detach interrupt to prevent executing it multiple
  // times when touching more than once.
  detachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN));
}

void printIncrementor() {
  i=i+1;
  Serial.print(i);
}

void timedTask() {
  Serial.print("Doing timed task...");
  i=0;
  for (int i = 1; i <= 10; i++) {
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
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  // 1) A scheduled task
  scheduler.scheduleAt(timedTask, TASK_TIME);

  // 2) An input interuption
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), isrInterruptPin, FALLING);
}

void loop() {
  scheduler.execute();
}
