// **** INCLUDES *****
#include "LowPower.h"

// Use pin 2 as wake up pin
const int wakeUpPin = 2;

void wakeUp()
{
    // Just a handler for the pin interrupt.
    Serial.println("interrupt");
}

void setup()
{
    Serial.begin(9600);
    Serial.println("started");
    // Configure wake up pin as input.
    // This will consumes few uA of current.
    pinMode(wakeUpPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(wakeUpPin), wakeUp, FALLING);
    
    Serial.println("setup finished");
    delay(1000);
}

void loop() 
{
    // Allow wake up pin to trigger interrupt on low.
    attachInterrupt(digitalPinToInterrupt(wakeUpPin), wakeUp, FALLING);
    delay(1000);
    Serial.println("before sleep");
    delay(2000);
    // Enter power down state with ADC and BOD module disabled.
    // Wake up when wake up pin is low.
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    Serial.println("krneki");
    
    // Disable external pin interrupt on wake up pin.
    detachInterrupt(0);
    Serial.println("interrupt detached");
    delay(1000);
    
    // Do something here
    // Example: Read sensor, data logging, data transmission.
}
