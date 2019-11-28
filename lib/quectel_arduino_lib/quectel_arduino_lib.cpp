
#include "quectel_arduino_lib.h"

/*
************************************** BASIC METHODS *************************************************************
*
*/
int quectelArduinoClass::send_at_command(String command){

    //flush the response buffer
    int i = 0;
    while(atResponseBuffer[i] != '\0'){
        atResponseBuffer[i] = '\0';
        i++;
    }

    command = command + "\r";

    const char * buf = command.c_str();

    //send data
    quectelSerial->listen();
    quectelSerial->write(buf);

    //wait for response
    delay(300);

    return quectelSerial->available();
}

/*
* @brief
* @param in: timeout - timeout in milliseconds
*/
char * quectelArduinoClass::get_at_response(unsigned int timeout){

    int nSymbols = quectelSerial->available();
    unsigned int start_time = millis();

    while(nSymbols == 0 && millis() - start_time < timeout)
    {
        delay(100);
        nSymbols = quectelSerial->available();
    }
    delay(100);
    nSymbols = quectelSerial->available();

    if(nSymbols){
        for(int i = 0; i<nSymbols; i++){

            atResponseBuffer[i] = quectelSerial->read();
        }
    }

    return &atResponseBuffer[0];
}

/*
* @brief removes the unnecessary ASCII characters from atResponseBuffer
* 
*/
char * quectelArduinoClass::crop_at_response(void){

    int i = 0;
    int a;
    int tmpBuffIdx = 0;

    while(atResponseBuffer[i] != '\0'){

        char c = atResponseBuffer[i];
        
        if(c!=13 && c!=10 && c!=6 && c!= 3){
            atResponseBuffer[tmpBuffIdx] = c;

            tmpBuffIdx ++;
        }
        i ++;
    }

    //flush the rest
    for(a=tmpBuffIdx; a<i; a++){
        atResponseBuffer[a] = '\0';
    }

    //Serial.print("atResponseBufferSize: ");
    //Serial.println(atResponseBuffer);

    return &atResponseBuffer[0];
}

void quectelArduinoClass::sendCommandAndPrintResp(String command, unsigned int delayTime, unsigned int timeout){

    Serial.println(command);
    send_at_command(command);
    delay(delayTime);
    char * resp = get_at_response(timeout);
    Serial.println(crop_at_response());
}

/*
************************************* UDP COMMUNICATION ******************************************************* 
*
*/
unsigned int quectelArduinoClass::create_socket(String port){
    /*  
    //TCP
    //create TCP socket
    sendCommandAndPrintResp("AT+NSOCR=STREAM,6,1234,2", 3000); //ustvari socket, ALI TCP DELUJE?

    //connect to TCP server
    //String command = ;
    sendCommandAndPrintResp("AT+NSOCO=0,83.212.127.86,1024", 3000);

    //send tcp
    //sendCommandAndPrintResp();

    //close session
    sendCommandAndPrintResp("AT+NSOCR=0",1000);
    */
    
    //AT+CGATT? - pove ce je povezan v omrezje
    //sendCommandAndPrintResp("AT+CGATT=1", 5000, 0);
    sendCommandAndPrintResp("AT+CGATT?", 0, TYP_DELAY);

    //UDP
    sendCommandAndPrintResp("AT+NSOCR=DGRAM,17,1235,1", 3000, 0); //open UDP socket
    //sendCommandAndPrintResp("AT+NSOST=0,83.212.127.86,80,20,5175656374656C204E422D496F542044656D6F0D", 3000); //send some data
    //sendCommandAndPrintResp("AT+NSOST=0,83.212.127.86,80,2,AB30", 3000);
    //sendCommandAndPrintResp("AT+NSOST=0,83.212.127.86,80,20,5175656374656C204E422D496F542044656D6F0D,1", 3000); //send some data
    //sendCommandAndPrintResp("AT+NSOST=0,83.212.127.86,80,2,AB30,2", 3000);
    //sendCommandAndPrintResp("AT+NSOCL=0", 1000); //close socket
    
    return 0;
}

/*
* @param in: sessionId - value returned by create_socket()
*/
bool quectelArduinoClass::close_socket(unsigned int sessionId){

    sendCommandAndPrintResp("AT+NSOCL=0", 0, 1000); //close socket

    return true;
}

bool quectelArduinoClass::send_udp(unsigned int sessionId, unsigned int repeatNum){
    unsigned int i;

    sendCommandAndPrintResp("AT+CGATT?", 0, 300); //check connectivity

    for(i=0; i<repeatNum; i++)
    {
        sendCommandAndPrintResp("AT+NSOST=0,83.212.127.86,80,20,5175656374656C204E422D496F542044656D6F0D", 0, TYP_DELAY);
        sendCommandAndPrintResp("AT+NSOST=0,83.212.127.86,80,2,AB30", 0, TYP_DELAY);
        Serial.println("\n");
    }
    return true;
}

/*
************************************** UTILITES ****************************************************************
*
*/
//TODO - vhodni parametri
String quectelArduinoClass::constructHttpGet(void){

}

/*
* @brief Sends AT commands and checks if the response is OK
*/
bool quectelArduinoClass::send_handshake(){

    if(send_at_command("AT") == 0){
        return false;
    }

    //if(strcmp(get_at_response(1000).c_str(), "\nOK\n")){
    //   return true;
    //}
}

bool quectelArduinoClass::at_ping(String ip_addr, int timeout){

    String command = "AT+NPING="+ ip_addr + ",12,5000\r";
    Serial.println(command);
    send_at_command(command);
    //delay(timeout);
    char * resp = get_at_response(1000);
    Serial.println(crop_at_response());

    return true;
}

//do not use the hardware serial pins
bool quectelArduinoClass::init(int rxPin, int txPin, String nBand, String APN, String forceOperator){

    quectelSerial = new SoftwareSerial(rxPin, txPin);
    quectelSerial->begin(9600);
    quectelSerial->listen();

    String command;

    //test the serial conection
    sendCommandAndPrintResp("AT", 0, TYP_DELAY);

    //get the existing configuration
    sendCommandAndPrintResp("AT+NCONFIG?", 0, TYP_DELAY);
    sendCommandAndPrintResp("AT+CGDCONT?", 0, TYP_DELAY);

    //////////////////////////////initial configuration////////////////////////////////////////////
    sendCommandAndPrintResp("AT+CFUN=0", 5000, TYP_DELAY); //disable radio
    sendCommandAndPrintResp("AT+NCONFIG=\"AUTOCONNECT\",\"FALSE\"", 0, TYP_DELAY);
    sendCommandAndPrintResp("AT+NCONFIG=\"CR_0354_0338_SCRAMBLING\",\"TRUE\"", 5000, TYP_DELAY);
    sendCommandAndPrintResp("AT+NCONFIG=\"CR_0859_SI_AVOID\",\"TRUE\"", 0, TYP_DELAY);
    //sendCommandAndPrintResp("AT+NRB", 1000); //reboot the module
    //delay(15000);
    //sendCommandAndPrintResp("AT", 300);
    sendCommandAndPrintResp("AT+CEREG=2", 15000, 20000); //connect to iot core
    
    /////////////////////////////connect to network///////////////////////////////////////////////
    command = "AT+CGDCONT=1,\"IP\",\"" + APN + "\"";
    sendCommandAndPrintResp(command, 5000, 5000);
    //sendCommandAndPrintResp("AT+CGACT=1,1", 10000);

    sendCommandAndPrintResp("AT+CFUN=1", 10000, 20000); //enable radio

    command = "AT+COPS=1,2,\""+ forceOperator +"\"";
    sendCommandAndPrintResp(command, 5000, 10000);
    sendCommandAndPrintResp("AT+CSCON=1", 1000, TYP_DELAY);

    sendCommandAndPrintResp("AT+CEREG?", 0, TYP_DELAY);
    delay(10000);

    sendCommandAndPrintResp("AT+CGACT?", 1000, 5000);
    
    //sendCommandAndPrintResp("AT+CGACT=1,1", 10000);

    sendCommandAndPrintResp("AT+CSQ", 1000, 5000); //get radio signal quality
    sendCommandAndPrintResp("AT+NBAND?", 0, TYP_DELAY);//get the radio channel used
    sendCommandAndPrintResp("AT+CGPADDR=1", 0, TYP_DELAY);//show the device ip
    sendCommandAndPrintResp("AT+CEER", 0, TYP_DELAY); //show the error details

    /////////////////////////////try the connection///////////////////////////////////////////////
    at_ping("8.8.8.8", 1000);
    at_ping("83.212.127.86", 1000);
  
    return true;
}

quectelArduinoClass quectelArduino;


