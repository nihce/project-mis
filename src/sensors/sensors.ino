#include <LowPower.h> //Sleep lib
#include <HCSR04.h> //Distance sensor lib
#include <SparkFun_ADXL345.h> //Acceleration sensor lib


//-----CONSTANTS-----
#define TIMER1 1000 //namesto delay(1000);
#define TrigPin 8 //HCSR04 sensor trig pin
#define EchoPin 9 //HCSR04 sensor echo pin
#define accelRange 2 //Set accel sensor range, accepted values are 2g, 4g, 8g or 16g

#define DEBUG_STREAM SerialUSB

//-----INITIALIZATION-----
UltraSonicDistanceSensor distanceSensor(TrigPin, EchoPin);  //Initialize distance sensor
ADXL345 adxl = ADXL345(); //I2C communication with accel sensor

//-----GLOBAL VARIABLES-----
double dist, prevDist = 0;
int accelX, accelY, accelZ = 0;
uint16_t itteration = 0; //how long there was no update to server
bool measurementError1 = 0; //distance sensor returned "-1" 100x  (KODA=1)
bool measurementError2 = 0; //accelerometer returned <200 in y axes therefore lid is open   (KODA=2)
bool theftError = 0; //distance changed for more than 20 cm from previous measurement   (KODA=3)
bool interruptFromAccel, interruptFromTimer = 0; //for detemining the source of interrupt TRENUTNO NE RABIMO!!!!!!!

//-----SETUP-----
void setup () {
  DEBUG_STREAM.begin(9600);
  DEBUG_STREAM.print("START SETUP");
  adxl.powerOn(); //Power on the accel sensor
  adxl.setRangeSetting(accelRange); //Accel range settings
  measurement(); //izvede prvo meritev za ugotavljanje zacetne napolnjenosti smetnjaka
  prevDist = dist; //zacetna napolnjenost smetnjaka
  DEBUG_STREAM.print("EXIT SETUP");

}

//-----LOOP-----
void loop () {
  //DEBUG_STREAM.print("LOOP ITERATION START");
  int xx,yy,zz;
  adxl.readAccel(&xx, &yy, &zz); // Read the accelerometer values and store them
  DEBUG_STREAM.print(xx);
  DEBUG_STREAM.print(", ");
  DEBUG_STREAM.print(yy);
  DEBUG_STREAM.print(", ");
  DEBUG_STREAM.println(zz);
  double distance = distanceSensor.measureDistanceCm();
  DEBUG_STREAM.println(distance); // Print the distance in centimeters
  DEBUG_STREAM.println();
  //DEBUG_STREAM.print("LOOP ITERATION END");
  delay(1000);

/*
  measurement();
  DEBUG_STREAM.print("Distance [cm] = ");
  DEBUG_STREAM.println(dist);
  DEBUG_STREAM.print("accelY = ");
  DEBUG_STREAM.println(accelY);
  DEBUG_STREAM.print("measurementError1 = ");
  DEBUG_STREAM.println(measurementError1);
  DEBUG_STREAM.print("measurementError2 = ");
  DEBUG_STREAM.println(measurementError2);
  DEBUG_STREAM.println();
  delay(TIMER1);
*/
/*
  itteration++; //povecaj stevec iteracije za 1
  sleepMinutes(10); //put to sleep for 10 minutes

  measurement();
  if (measurementError2) {
    //pokrov odprt
    for (int i = 1; i < 10; i++) { //ta zanka vnese max 10 min zakasnitve
      delay(60 * TIMER1); //wait 60s
      measurement();
      if (!measurementError2) {
        break; //ce se v 10ih poskusih pokrov zapre nadaljuj z meritvami volumna smeti
      }
      if (i == 9) { //ce je 10krat zapored odprt smetnjak poslji opozorilo
        //na server poslji measurementError2=1
      }
    }
  }
  if (!measurementError2) { //ce je pokrov zaprt spusti noter
    if (measurementError1) {
      //pokrov zaprt + senzor razdalje ne dela
      //na server poslji measurementError1=1
    }
    else {
      //pokrov zaprt + senzor razdalje dela
      if (itteration >= 24) { //ze (24 * 10 minut = 4ure) ni bila poslana posodobitev dolzine na streznik
        //na server poslji dist + vsi errorji so enaki 0
        itteration = 0;
      }
      else{
        //preveri ali se je hitro spremenila razdalja smeti za vec kot 20cm
        //ce ni bilo kraje, ne posiljaj nic in pojdi spat
        if (prevDist - dist > 20.0) {
          theftError = 1;
          //na server poslji dist + theftError=1
          //dist naj bo pri posiljanju uint16_t
          theftError = 0;
        }
      }
      prevDist = dist; //shrani trenutno vrednost volumna za naslednje preverjanje
    }
  }
*/
}



//-----FUNCTIONS-----
void measurement() {
  measurementError1 = 0; //distance sensor returned "-1" 100x
  measurementError2 = 0; //accelerometer returned <200 in y axes therefore lid is open
  dist = -1;

  updateAccel(); //prebere pospesek
  if (accelY < 200) { //preveri ali je v Y smeri odprt pokrov
    measurementError2 = 1; //je 1, ce je pokrov odprt
  }
  else {
    updateDist(); //prebere razdaljo v [cm]
  }
}

//MEASURE ACCEL
void updateAccel() {
  adxl.readAccel(&accelX, &accelY, &accelZ);
}

//MEASURE DISTANCE
void updateDist() {
  int i, tmp, cnt1 = 0;

  while (cnt1 != 20) {
    tmp = distanceSensor.measureDistanceCm();
    if (tmp > 0) { //ce vrne -1 zavrzi meritev
      dist = dist + tmp;
      cnt1++;
    }
    i++;
    if (i == 100) break;
    delay(20);
  }
  if (dist == 0) {
    measurementError1 = 1;
  }
  else {
    dist = round(dist / cnt1);
  }
}

//SLEEP FOR THE SET NUMBER OF MINUTES
//NE DELA Z NOVO ARHITEKTURO M0
/*
void sleepMinutes(int minutes) {
  for (int i = 0; i < 15 * minutes; i++)
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
}
*/
