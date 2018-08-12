#ifndef CPELCODDECODER_H
#define CPELCODDECODER_H
#include <arduino.h>


#define I_SYNC (0)
#define I_ADDRESS (1)
#define I_CMD1 (2)
#define I_CMD2 (3)
#define I_DATA1 (4)
#define I_DATA2 (5)


#define I_CHECKSUM (6)

#define I_CMD2_RIGHT (1)
#define I_CMD2_LEFT (2)
#define I_CMD2_UP (3)
#define I_CMD2_DOWN (4)
#define I_CMD2_ZOOM_TELE (5)
#define I_CMD2_ZOOM_WIDE (6)
#define I_CMD2_FOCUS_FAR (7)



#define CMD_ARR_LEN (7)


class CPelcoDDecoder
{
  public: void begin(Stream* port, int address);//2400 baud
  public: void parseData();  
 
  //public: void setOnMoveUpHandler(void* handler);
  //public: void setOnMoveDownHandler(void* handler);
  public: void setOnMoveLeftHandler(void( *handler(unsigned char)));
  public: void setOnMoveRightHandler(void( *handler(unsigned char)));
  public: void setOnMoveUpHandler(void( *handler(unsigned char)));
  public: void setOnMoveDownHandler(void( *handler(unsigned char)));
  public: void setOnStopHandler(void* handler);
  public: void setOnErrorHandler(void* handler);

  protected: void parseBuffer();
  protected: void onMoveUpHandler(unsigned char data);
  protected: void onMoveDownHandler(unsigned char data);
  protected: void onMoveLeftHandler(unsigned char data);
  protected: void onMoveRightHandler(unsigned char data);
  protected: void onStopHandler();
  protected: void onErrorHandler(String errMsg);
  protected: unsigned char CPelcoDDecoder::calcChecksum(char* arrCmd, unsigned char len);
  //protected: void parseCommand1();
  protected: void parseCommand2();
  
  protected: void (*onMoveUp)(unsigned char data), (*onMoveDown)(unsigned char data), (*onMoveLeft)(unsigned char data), (*onMoveRight)(unsigned char data), (*onStop)(), (*onErrorH)(String);
  protected: byte bufCmd[7];
  protected: byte bufCmdIndex = 0;
  protected: char address;  
  protected: Stream* port;
};

#endif
