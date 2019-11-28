const byte interruptPin = 2;
volatile byte state = HIGH;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);
}

void loop() {
  digitalWrite(LED_BUILTIN, state);
  delay(1000);
}

void blink() {
  state = !state;
}
