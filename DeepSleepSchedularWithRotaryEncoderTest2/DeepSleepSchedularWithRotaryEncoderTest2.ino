#define AWAKE_INDICATION_PIN LED_BUILTIN
#define SLEEP_MODE SLEEP_MODE_IDLE
#define TASK_TIME 5000

// Rotary Encoder
#define CLK_PIN 2   // Generating interrupts using CLK signal
#define DT_PIN 3    // Reading DT signal
#define SW_PIN 4    // Reading Push Button switch

#include <DeepSleepScheduler.h>

volatile boolean TurnDetected;  // need volatile for Interrupts
volatile boolean rotationdirection;  // CW or CCW rotation

void ledOn() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED On");
  scheduler.scheduleDelayed(ledOff, 2000);
}

void ledOff() {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED Off");
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), isrInterruptPin, FALLING);
}

// Interrupt routine runs if CLK goes from HIGH to LOW
void measureRotation ()  {
  if (digitalRead(CLK_PIN))
    rotationdirection = digitalRead(DT_PIN);
  else
    rotationdirection = !digitalRead(DT_PIN);
  TurnDetected = true;
  Serial.println("Turn detected!");
  scheduler.schedule(ledOn);
}

void isrInterruptPin() {
  scheduler.scheduleOnce(measureRotation);
  // detach interrupt to prevent executing it multiple
  // times when touching more than once.
  detachInterrupt(digitalPinToInterrupt(CLK_PIN));
}

/*
 * Work should be done under only one of two conditions
 *   1) Rotary Encoder movement sends interrupt signal
 *   2) Task is previously set for a time
 */
void setup() {
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);

  // rotary encoder
  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);  
  pinMode(SW_PIN, INPUT_PULLUP);

  // 1) Rotary Encoder movement sends interrupt signal
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), isrInterruptPin, FALLING);

  // 2) Task is previously set for a time
  scheduler.scheduleAt(ledOn, TASK_TIME);
}

void loop() {
  scheduler.execute();
}
