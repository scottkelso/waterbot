int r = 8;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(r, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(r, HIGH);
  delay(4000);
  digitalWrite(r, LOW);
  delay(10000);
}
