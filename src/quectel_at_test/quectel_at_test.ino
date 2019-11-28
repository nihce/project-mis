//includes
#include "quectel_arduino_lib.h"

//macros
#define QUECTEL_SERIAL_RX 8
#define QUECTEL_SERIAL_TX 7

extern quectelArduinoClass quectelArduino;

void setup() {
  
  //init serial communication
  Serial.begin(9600);

  //convert string to hex
  String test = "GET /upload.php?id=7&stamp=7&vol=7 HTTP/1.1\r\nHost: 83.212.127.86\r\nAccept-Encoding: gzip,deflate\r\nAccept: */*\r\n\r\n";
  String output = "";
  unsigned int test_len = test.length();
  Serial.print("string length: ");
  Serial.println(test_len);
  Serial.print("Sizeof char: ");
  Serial.println(sizeof(char));
  const char * test_c = test.c_str();
  int i;
  char buff[2];

  for(i=0; i<test_len; i++)
  {
    // write hex representation of the char to buffer
    sprintf(buff, "%0x", *(test_c + i));
    output += buff[0];
    output += buff[1];
  }
  Serial.print("Output: ");
  Serial.println(output);


  
  if(quectelArduino.init(QUECTEL_SERIAL_RX, QUECTEL_SERIAL_TX, "8", "v-iot", "29341") == false){
    Serial.println("Failed quectel module initialization");
  }
  else Serial.println("Quectel module serial communication successful");
}

void loop() {
  
  delay(3000);
  quectelArduino.at_ping("83.212.127.86", 1000);
  delay(1000);
  quectelArduino.create_socket("test");
  
}
