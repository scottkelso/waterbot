// Rotary Encoder
#define CLK_PIN 2   // Generating interrupts using CLK signal
#define DT_PIN 3    // Reading DT signal
#define SW_PIN 4    // Reading Push Button switch

volatile boolean TurnDetected;  // need volatile for Interrupts
volatile boolean rotationdirection;  // CW or CCW rotation

// Interrupt routine runs if CLK goes from HIGH to LOW
void isr ()  {
  delay(4);  // delay for Debouncing
  if (digitalRead(CLK_PIN))
    rotationdirection= digitalRead(DT_PIN);
  else
    rotationdirection= !digitalRead(DT_PIN);
  TurnDetected = true;
}

void setup() {
  Serial.begin(9600);

  // rotary encoder
  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);  
  pinMode(SW_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(CLK_PIN), isr, FALLING);
}

void loop() {
  if (TurnDetected)  {
    Serial.println("Turn detected!");
    TurnDetected = false;
  }
}
