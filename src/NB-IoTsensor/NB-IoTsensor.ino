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
//void initHumidityTemperature();
//void initPressureSensor();
int Sodaq_nbIOT;
//void initGPS();
void loop();
void do_flash_led(int pin);
void sendUdp(char * data);

void setup()
{
  //String data = "test";
  //String hex = String(data, HEX);
  //Serial.println(hex);
  
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

  //initHumidityTemperature();
  //initPressureSensor();
  //initGPS();

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

void sendUdp(char * data){
    DEBUG_STREAM.println("sendUdp");
    char buf[2];
    int dataSize = strlen(data);
    String dataHex;
    DEBUG_STREAM.print("data length: ");
    DEBUG_STREAM.println(dataSize);

    for(int i=0; i<strlen(data); i++){
      char c = *(data+i);
      DEBUG_STREAM.println(c);
      sprintf(buf, "%0x", c);
      dataHex+=buf[0];
      dataHex+=buf[1];
     }

     nbiot.sendudp(dataHex, dataSize);
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


  Serial.println("data: ");
  Serial.println(topicdata);
  Serial.println(sizeUDP);

  //POSLJI UDP na IP naslov v knjižnici
  nbiot.sendudp(topicdata, sizeUDP);
}

void loop()
{
  do_flash_led(13);
  //int16_t temperature;
  //int16_t humidity;
  //int16_t pressure;
  //char* temperatureChar;
  //char* humidityChar;
  //char* pressureChar;
  String topic;
  String enotahex;

  delay(10000);
  char data[] = "test";
  sendUdp(&data[0]);
  
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
