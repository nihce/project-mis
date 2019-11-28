/********************************************************************************
*
*
*
********************************************************************************/

////////////////////// includes /////////////////////////////////////////////////
#include <Arduino.h>
#include <stdio.h>
#include <Wire.h>
#include <Sodaq_nbIOT.h>

///////////////////// macros ///////////////////////////////////////////////////
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
#error "Please select a suitable board"
#endif

#define SENSOR_ID   "1234"

///////////////////// global variables /////////////////////////////////////////
Sodaq_nbIOT nbiot;
int Sodaq_nbIOT;

//for sendudp
const String urlBegin = "474554202f75706c6f61642e7068703f"; //GET /upload.php?
const String urlEnd = "20485454502f312e310d0a486f73743a2038332e3231322e3132372e38360d0a4163636570742d456e636f64696e673a20677a69702c6465666c6174650d0a4163636570743a202a2f2a0d0a0d0a";   // HTTP/1.1\r\nHost: 83.212.127.86\r\nAccept-Encoding: gzip,deflate\r\nAccept: */*\r\n\r\n
const int combinedSize = 94; //combined number of bytes int urlBegin and urlEnd

//////////////////// function prototypes ///////////////////////////////////////
bool connectToNetwork();
void sendUdp(const char * data);
bool sendAlarm(int alarmCode);
bool sendMeasurementData(int volume);

//////////////////// setup and main loop ///////////////////////////////////////
void setup()
{
  //led
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  DEBUG_STREAM.begin(9600);
  MODEM_STREAM.begin(nbiot.getDefaultBaudrate());

  while ((!DEBUG_STREAM) && (millis() < 10000)) {
    // Wait for serial monitor for 10 seconds
  }

  DEBUG_STREAM.println("\r\nNB-IoT module initialization started\r\n");

  nbiot.init(MODEM_STREAM, 7);
  nbiot.setDiag(DEBUG_STREAM);

  delay(2000);
  //connect to network
  while (!connectToNetwork());

  //open local socket
  nbiot.osocket();
  
  digitalWrite(13, LOW);
}

void loop()
{
  
}

//////////////////// function definitions //////////////////////////////////////

//connects to the NB-IoT network
bool connectToNetwork() 
{
    if (nbiot.connect("v-iot", "29341"))
    {
        DEBUG_STREAM.println("Connected succesfully!");
        //nbiot.cscon();
        return true;
    }
    else
    {
        DEBUG_STREAM.println("Failed to connect!");
        delay(2000);
        return false;
    }
}

void sendUdp(const char * data)
{
    DEBUG_STREAM.print("sendUdp ");
    DEBUG_STREAM.println(data);
    char buf[2];
    int dataSize;
    String dataHex;

    //convert the data to hex format
    for(int i=0; i<strlen(data); i++){
    char c = *(data+i);
    sprintf(buf, "%0x", c);
    dataHex+=buf[0];
    dataHex+=buf[1];
    }

    //DEBUG_STREAM.print("converted: ");
    //DEBUG_STREAM.println(dataHex);
    
    //add the fixed part
    String getRequest = urlBegin;
    getRequest+=dataHex;
    getRequest+=urlEnd;
    dataSize = getRequest.length()/2; //preveri ce je prava vrednost
    DEBUG_STREAM.print("data size: ");
    DEBUG_STREAM.println(dataSize);
    DEBUG_STREAM.println(getRequest);

    //send multiple datagrams?
    nbiot.sendudp(getRequest, dataSize);

    //wait for response?
}

bool sendAlarm(int alarmCode)
{
  String requestData = "id="+String(SENSOR_ID)+"&flags="+String(alarmCode)+"&volume=0";
  const char * ptr = requestData.c_str();

  sendUdp(ptr);
}

bool sendMeasurementData(int volume)
{
  String requestData = "id="+String(SENSOR_ID)+"&flags=0&volume="+String(volume);
  const char * ptr = requestData.c_str();
  
  //send the data to server
  sendUdp(ptr);
}
