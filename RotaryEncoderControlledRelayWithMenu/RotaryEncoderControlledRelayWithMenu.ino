#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const int pinCLK = 2;   // Generating interrupts using CLK signal
const int pinDT = 3;    // Reading DT signal
const int pinSW = 4;    // Reading Push Button switch

// https://www.youtube.com/watch?v=uh5dLC6IkQQ
const int relay = 22;
const int relay2 = 24;

volatile boolean TurnDetected;  // need volatile for Interrupts
volatile boolean rotationdirection;  // CW or CCW rotation

int RotaryPosition = 0;    // To store Stepper Motor Position
int PrevPosition;     // Previous Rotary position Value to check accuracy
int StepsToTake;      // How much to move Stepper

unsigned long startTime = millis();

int currentMenu = 1;
int waterFreq = 1; //once per day
int duration = 5; //mins
const int MAX_WATER_FREQ = 50;
const int MAX_DURATION = 30;
boolean exiting = false;

void restartTimer () {
  startTime = millis();
}

unsigned long getTimeElapsed () {
  return millis() - startTime;
}

boolean hasTimedOut () {
  return getTimeElapsed() > 200000;
}

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
      lcd.print(" days");
      while (!buttonPressed()) {
        int rot = getRotation();
        if (rot != 0) {
          waterFreq = constrain(waterFreq + rot, 0, MAX_WATER_FREQ);
          lcd.setCursor(0, 1);
          lcd.print(waterFreq);
          lcd.print(" days");
        }
      }
      break;
    case 2:
      lcd.begin(16, 2);
      lcd.setCursor(0, 0);
      lcd.print("Water duration:");
      lcd.setCursor(0, 1);
      lcd.print(duration);
      lcd.print(" minutes ");
      while (!buttonPressed()) {
        int rot = getRotation();
        if (rot != 0) {
          duration = constrain(duration + rot, 1, MAX_DURATION);
          lcd.setCursor(0, 1);
          lcd.print(duration);
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

// Interrupt routine runs if CLK goes from HIGH to LOW
void isr ()  {
  delay(4);  // delay for Debouncing
  if (digitalRead(pinCLK))
    rotationdirection = digitalRead(pinDT);
  else
    rotationdirection = !digitalRead(pinDT);
  TurnDetected = true;
}

boolean buttonPressed() {
  if (!(digitalRead(pinSW))) {   // check if button is pressed
    Serial.println("Button Pressed");
    flashLED();
    
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
    flashLED();
    TurnDetected = false;  // do NOT repeat IF loop until new rotation detected
  }

  return directionAngle;
}

void flashLED () {
  digitalWrite(relay, HIGH);
  delay(500);
  digitalWrite(relay, LOW);
}

void water () {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Watering Now...");
  
  digitalWrite(relay2, HIGH);
  for (int i = duration * 60; i > 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print(i);
    lcd.print(" seconds  ");
    delay(1000);
  }
  digitalWrite(relay2, LOW);
  
  lcd.setCursor(0, 0);
  lcd.print("Completed!");
  delay(2000);
}

void setup() {  
  // relay
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  pinMode(relay2, OUTPUT);

  // rotary encoder
  pinMode(pinCLK,INPUT);
  pinMode(pinDT,INPUT);  
  pinMode(pinSW,INPUT);
  digitalWrite(pinSW, HIGH); // Pull-Up resistor for switch
  attachInterrupt (0,isr,FALLING); // interrupt 0 always connected to pin 2 on Arduino UNO

  // LCD Screen
  lcd.begin(14, 1);
  lcd.print("WATER BOT    ^^,");
  delay(2000);
  showMenu();
}

void loop() {
  if (buttonPressed()) {
    showSubMenu(currentMenu);
    showMenu();
  }
  
  int rot = getRotation();
  if (rot != 0) {
    currentMenu = constrain(currentMenu + rot, 1, 4);
    showMenu();
  }
}
