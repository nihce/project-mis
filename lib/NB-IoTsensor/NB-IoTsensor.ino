/***************************************************************************
  This is a library for the HTS221 Humidity Temperature Sensor
    Originally written by speirano for SmartEverything
    Adjusted by Gregory Knauff of SODAQ for the NB-IoT shield
  Adjusted by Jan van Loenen to work on Sodaq Explorer and Arduino Leonardo

  Standard I2C-address is 0x5F.

***************************************************************************/

#include <Arduino.h>
#include <stdio.h>
#include <Wire.h>
#include <Sodaq_nbIOT.h>
// #include <SoftwareSerial.h> // Uno
#include "Sodaq_HTS221.h"
#include "Sodaq_LPS22HB.h"
#include "Sodaq_UBlox_GPS.h"


#if defined(ARDUINO_AVR_LEONARDO)
#define DEBUG_STREAM Serial
#define MODEM_STREAM Serial1

#elif defined(ARDUINO_AVR_UNO)
SoftwareSerial softSerial(10, 11); // RX, TX
// You can connect an uartsbee or other board (e.g. 2nd Uno) to connect the softserial.
#define DEBUG_STREAM softSerial
#define MODEM_STREAM Serial

#elif defined(ARDUINO_SODAQ_EXPLORER)
#define DEBUG_STREAM SerialUSB
#define MODEM_STREAM Serial

#elif defined(ARDUINO_SAM_ZERO)
#define DEBUG_STREAM SerialUSB
#define MODEM_STREAM Serial1

#else
#error "Please select a Sodaq ExpLoRer, Arduino Leonardo or add your board."
#endif

Sodaq_nbIOT nbiot;
Sodaq_LPS22HB lps22hb;

uint32_t lat = 0;
uint32_t lon = 0;

void setup();
bool connectToNetwork();
void initHumidityTemperature();
void initPressureSensor();
int Sodaq_nbIOT;
void initGPS();
void loop();
void do_flash_led(int pin);

void setup()
{
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  DEBUG_STREAM.begin(9600);
  MODEM_STREAM.begin(nbiot.getDefaultBaudrate());

  while ((!DEBUG_STREAM) && (millis() < 10000)) {
    // Wait for serial monitor for 10 seconds
  }

  DEBUG_STREAM.println("\r\nSODAQ All Things Talk Arduino Example\r\n");

  nbiot.init(MODEM_STREAM, 7);
  nbiot.setDiag(DEBUG_STREAM);

  delay(2000);
/////////////POVEZOVANJE//////////////////
  while (!connectToNetwork());

  initHumidityTemperature();
  initPressureSensor();
  initGPS();

  digitalWrite(13, LOW);
}
/////////////funcija za povezovanje//////////////////
bool connectToNetwork() {
  if (nbiot.connect("v-iot", "29341")) {
    DEBUG_STREAM.println("Connected succesfully!");
    //nbiot.cscon();
    return true;
  }
  else {
    DEBUG_STREAM.println("Failed to connect!");
    delay(2000);
    return false;
  }
}

////////////////////PRETVORBA PODATKOV V HEX IN DODAJANJE TOPICA IN ENOTE////////////
void Data(char *value, String topicdata, String enotahex)
{ char buf[2];
  int sizeUDP;
  for(int i=0;i<strlen(value);i++)
  {  char c=*(value+i);  // single ascii value
     sprintf(buf,"%0x",c);// convert ascii value in hex
     topicdata+=buf[0];
     topicdata+=buf[1];
  }
  topicdata+=enotahex;
  sizeUDP=topicdata.length()/2;

  //POSLJI UDP na IP naslov v knjižnici
  nbiot.sendudp(topicdata, sizeUDP);
}


////////////////////PRETVORBA PODATKOV V HEX IN DODAJANJE TOPICA IN LATITUDE LONGITUDE////////////
void DataGPS(char *GPSCharLon, char *GPSCharLat, String topicdata)
{ char buf[2];
  int sizeUDP;
  //dodan Latitude: in pretvorba latitude vrednosti v HEX
  topicdata+="4c617469747564653a20";
  for(int i=0;i<strlen(GPSCharLat);i++)
  {  char c=*(GPSCharLat+i);  // single ascii value
     sprintf(buf,"%0x",c);// convert ascii value in hex
     topicdata+=buf[0];
     topicdata+=buf[1];
  }
  //dodan Longitude: in pretvorba longitude vrednosti v HEX
  topicdata+="204c6f6e6769747564653a20";
  for(int i=0;i<strlen(GPSCharLon);i++)
  {  char c=*(GPSCharLon+i);  // single ascii value
     sprintf(buf,"%0x",c);// convert ascii value in hex
     topicdata+=buf[0];
     topicdata+=buf[1];
  }
  sizeUDP=topicdata.length()/2;
  
  //POSLJI UDP na IP naslov v knjižnici
  nbiot.sendudp(topicdata, sizeUDP);
}

void initHumidityTemperature() {
  if (hts221.begin() == false)
  {
    DEBUG_STREAM.println("Error while retrieving WHO_AM_I byte...");
    while (1);
  }
}

void initPressureSensor() {
  lps22hb.begin(0x5D);  //

  if (lps22hb.whoAmI() == false)
  {
    DEBUG_STREAM.println("Error while retrieving WHO_AM_I byte...");
  }
}

void initGPS() {
  sodaq_gps.init(6);
  nbiot.osocket();
  // sodaq_gps.setDiag(DEBUG_STREAM);
}



void loop()
{
  do_flash_led(13);
  int16_t temperature;
  int16_t humidity;
  int16_t pressure;
  char* temperatureChar;
  char* humidityChar;
  char* pressureChar;
  String topic;
  String enotahex;



/////////BRANJE TEMPERATURE////////////
  temperature = hts221.readTemperature() * 100;
  DEBUG_STREAM.println("Temperature x100 : " + (String)temperature);
//temperatura v temperaturaChar
  char cstr1[16];
  temperatureChar=itoa(temperature, cstr1, 10);
  
  //dodajanje decimalne vejice
  if(temperature>=1000){
  temperatureChar[4]=temperatureChar[3];
  temperatureChar[3]=temperatureChar[2];
  temperatureChar[2]='.';
  temperatureChar[5]='\0';
  }
  if(temperature<1000&&temperature>100){
  temperatureChar[3]=temperatureChar[2];
  temperatureChar[2]=temperatureChar[1];
  temperatureChar[1]='.';
  temperatureChar[4]='\0';
  }
  

//topic v HEX ----- Temperatura -->54656d706572617475726120
  topic="54656d706572617475726120";
  enotahex="43"; //C
/////////Pretvorba v HEX in poslji UDP
  Data(temperatureChar, topic, enotahex);

  delay(100);


////////////////////BRANJE VLAGE///////////////
  humidity = hts221.readHumidity()/* * 100*/;
  DEBUG_STREAM.println("Humidity : " + (String)humidity);

//humidity v humidityChar
  char cstr2[16];
  humidityChar=itoa(humidity, cstr2, 10);

//topic v HEX ----- Vlaga -->566c61676120
  topic="566c61676120";
  enotahex="25";//%
/////////Pretvorba v HEX in poslji UDP
  Data(humidityChar, topic, enotahex);

  delay(100);


////////////////////BRANJE Pritiska///////////////////
  pressure = lps22hb.readPressure();
  DEBUG_STREAM.println("Pressure:" + (String)pressure);
  
//pressure v pressureChar
  char cstr3[16];
  pressureChar=itoa(pressure, cstr3, 10);

  
//topic v HEX ----- Pritisk -->5072697469736b20
  topic="5072697469736b20";
  enotahex="685061";//hPa
//Pretvorba v HEX in poslji UDP
  Data(pressureChar, topic, enotahex);


////////////BRANJE GPS////////////IZBERI 10sec
  uint32_t start = millis();
  uint32_t timeout = 1UL * 10 * 1000; // x sec timeout 
  

  DEBUG_STREAM.println(String("waiting for fix ..., timeout=") + timeout + String("ms"));
  if (sodaq_gps.scan(true, timeout)) {

    lat = sodaq_gps.getLat() * 100000;
    lon = sodaq_gps.getLon() * 100000;
//Ustavljanje decimalnih pik v latitude longitude
  char* GPSCharLat;
  char* GPSCharLon;
  String GPSChar;
  char cstr4[16];
  char cstr5[16];
  GPSCharLat=itoa(lat, cstr4, 10);
  GPSCharLon=itoa(lon, cstr5, 10);
  //dodajanje decimalnih vejic
  GPSCharLon[6]=GPSCharLon[5];
  GPSCharLon[5]=GPSCharLon[4];
  GPSCharLon[4]=GPSCharLon[3];
  GPSCharLon[3]=GPSCharLon[2];
  GPSCharLon[2]='.';
  
  GPSCharLat[6]=GPSCharLat[5];
  GPSCharLat[5]=GPSCharLat[4];
  GPSCharLat[4]=GPSCharLat[3];
  GPSCharLat[3]=GPSCharLat[2];
  GPSCharLat[2]='.';
  

//topic v HEX ----- GPS  -->47505320
  topic="475053204944313b";
//////////////Pretvorba v HEX in poslji UDP////////////
  DataGPS(GPSCharLat, GPSCharLon, topic);
  DEBUG_STREAM.println();
  }
  
  else {
    DEBUG_STREAM.println("GPS No Fix");

    //polje UDP data "GPS No Fix" v HEX --->475053204e6f20466978
    String nodata="475053204e6f20466978";
    nbiot.sendudp(nodata, 10);
    }
  
  // Wait some time between messages
  delay(5000); // 1000 = 1 sec
}

void do_flash_led(int pin)
{
  for (size_t i = 0; i < 2; ++i) {
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
  }
}
