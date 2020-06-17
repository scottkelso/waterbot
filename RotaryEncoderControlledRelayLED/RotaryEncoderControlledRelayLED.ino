const int relay = 5;

volatile boolean TurnDetected;  // need volatile for Interrupts
volatile boolean rotationdirection;  // CW or CCW rotation

int RotaryPosition = 0;    // To store Stepper Motor Position
int PrevPosition;     // Previous Rotary position Value to check accuracy
int StepsToTake;      // How much to move Stepper

const int pinCLK = 2;   // Generating interrupts using CLK signal
const int pinDT = 3;    // Reading DT signal
const int pinSW = 4;    // Reading Push Button switch

// Interrupt routine runs if CLK goes from HIGH to LOW
void isr ()  {
  delay(4);  // delay for Debouncing
  if (digitalRead(pinCLK))
    rotationdirection = digitalRead(pinDT);
  else
    rotationdirection = !digitalRead(pinDT);
  TurnDetected = true;
}

void checkForButtonPressed() {
  if (!(digitalRead(pinSW))) {   // check if button is pressed
    Serial.println("Button Pressed");
    flashLED();
    
    if (RotaryPosition == 0) {  // check if button was already pressed
      
    } else {
      RotaryPosition = 0; // Reset position to ZERO
      Serial.print("Rotary Position Reset: ");
      Serial.println(RotaryPosition);
    }
  }
}

void checkForRotation() {
  // Runs if rotation was detected
  if (TurnDetected)  {
    PrevPosition = RotaryPosition; // Save previous position in variable
    if (rotationdirection) { // CW turn
      RotaryPosition = RotaryPosition + 1;
      Serial.print("CW  - Rotary Position: ");
      Serial.println(RotaryPosition);
      flashLED();
    } else { // CCW turn
      RotaryPosition = RotaryPosition - 1;
      Serial.print("CCW - Rotary Position: ");
      Serial.println(RotaryPosition);
      flashLED();
    }

    TurnDetected = false;  // do NOT repeat IF loop until new rotation detected
  }
}

void checkRotaryEncoder () {
  checkForButtonPressed();
  checkForRotation();
}

void flashLED () {
  digitalWrite(relay, HIGH);
  delay(100);
  digitalWrite(relay, LOW);
}

void setup() {
  // relay
  Serial.begin(9600);
  pinMode(relay, OUTPUT);

  // rotary encoder
  pinMode(pinCLK,INPUT);
  pinMode(pinDT,INPUT);  
  pinMode(pinSW,INPUT);
  digitalWrite(pinSW, HIGH); // Pull-Up resistor for switch
  attachInterrupt (0,isr,FALLING); // interrupt 0 always connected to pin 2 on Arduino UNO
}

void loop() {
  checkRotaryEncoder();
}
