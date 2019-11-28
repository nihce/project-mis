
#ifndef quectel_arduino_lib_h
#define quectel_arduino_lib_h

//includes
#include <SoftwareSerial.h>
#include <string.h>
#include <Arduino.h>

//macros
#define TYP_DELAY   300

//declarations
class quectelArduinoClass {
    private:
        SoftwareSerial * quectelSerial;
        char atResponseBuffer[128];
        unsigned int udpStringSize;
        //GET /upload.php?
        String getRequestStart = "474554202f75706c6f61642e7068703f";
        // HTTP/1.1\r\nHost: 83.212.127.86\r\nAccept-Encoding: gzip,deflate\r\nAccept: */*\r\n\r\n
        String getRequestEnd = "20485454502f312e315c725c6e486f73743a2038332e3231322e3132372e38365c725c6e4163636570742d456e636f64696e673a20677a69702c6465666c6174655c725c6e4163636570743a202a2f2a5c725c6e5c725c6e";

    public:
        int send_at_command(String command);
        char * get_at_response(unsigned int timeout);
        bool send_handshake(void);
        bool init(int rxPin, int txPin, String nBand, String APN, String forceOperator);
        char * crop_at_response(void); 
        void sendCommandAndPrintResp(String command, unsigned int delayTime, unsigned int timeout);
        bool at_ping(String ip_addr, int timeout);
        unsigned int create_socket(String port);
        bool send_udp(unsigned int sessionId, unsigned int repeatNum);
        bool close_socket(unsigned int sessionId);
        String constructHttpGet(void);
};

#endif
