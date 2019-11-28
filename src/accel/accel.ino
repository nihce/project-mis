#include <SparkFun_ADXL345.h> 

ADXL345 adxl = ADXL345();             // I2C COMMUNICATION
const byte interruptPin = 2;

void setup() {
  Serial.begin(9600);
  Serial.println("BEGIN SETUP");

  pinMode(5, INPUT); //TESTNO IZPISOVANJE INTERRUPTA
  
  adxl.powerOn();
  adxl.setRangeSetting(2);           // Give the range settings
                                      // Accepted values are 2g, 4g, 8g or 16g
  
  adxl.setActivityXYZ(1,1,1);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(10);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)

  // Setting all interupts to take place on INT1 pin
  adxl.setImportantInterruptMapping(0, 0, 0, 1, 0);     // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);" 
                                                        // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
                                                        // This library may have a problem using INT2 pin. Default to INT1 pin.

  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.InactivityINT(0);
  adxl.ActivityINT(1);
  adxl.FreeFallINT(0);
  adxl.doubleTapINT(0);
  adxl.singleTapINT(0);
  
  //adxl.setInterruptLevelBit(1); // positive logic
  pinMode(interruptPin, INPUT);
  
  //attachInterrupt(digitalPinToInterrupt(interruptPin), ADXL_ISR, RISING);
  

  Serial.println("END SETUP");
}

void loop(){
  Serial.println(digitalRead(5));
  delay(5);
}

void ADXL_ISR() {
  
  // getInterruptSource clears all triggered actions after returning value
  // Do not call again until you need to recheck for triggered actions
  //byte interrupts = adxl.getInterruptSource(); // zaradi tega obvisi, gleda na pospeskometru kaj je sprozilo interrupt
  
  Serial.println("*** INSIDE ADXL_ISR ***"); 
  
  // Activity
  //if(adxl.triggered(interrupts, ADXL345_ACTIVITY)){
  //  Serial.println("*** ACTIVITY ***"); 
  //   //add code here to do when activity is sensed
  //}
}

