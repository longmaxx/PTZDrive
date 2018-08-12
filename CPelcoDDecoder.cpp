#include "CPelcoDDecoder.h"

void CPelcoDDecoder::begin(Stream* port, int address)
{
  this->port = port;
  this->address = address;
}
void CPelcoDDecoder:: parseData()
{
  while (port->available())
  {
    byte tmpByte = port->read();
    if (tmpByte == 0xff){
      bufCmdIndex = 0;
    }
    if (bufCmdIndex == 0)
    {
      if (tmpByte == 0xff)
      {
        bufCmd[0] = tmpByte;
        bufCmdIndex++;
      }
    }
    else
    {
      //bufCurIndex is > 0
      bufCmd[bufCmdIndex] = tmpByte;
      bufCmdIndex++;  
    }
    if (bufCmdIndex == 7){
      parseBuffer();
      bufCmdIndex = 0;
    }  
  }
}
void CPelcoDDecoder::parseBuffer()
{
  if (bufCmdIndex != CMD_ARR_LEN)//check data length
  {
    this->onErrorHandler(F("Bad command length"));
    return;
  } 
  else if (this->calcChecksum(bufCmd,bufCmdIndex)!= bufCmd[I_CHECKSUM])//checksum 
  {
    this->onErrorHandler("Checksum error" + (String)(this->calcChecksum(bufCmd,bufCmdIndex)));
    return;
  }
  else if ((bufCmd[I_CMD1] == 0) && (bufCmd[I_CMD2] == 0))// stop
  {
    this->onStopHandler();
    return;
  }
  //parseCommand1();
  parseCommand2();
}
void CPelcoDDecoder::parseCommand2()
{
  char cmdValue = this->bufCmd[I_CMD2];
  if ((cmdValue>>I_CMD2_RIGHT)&&0x01)
  {
    this->onMoveRightHandler(this->bufCmd[I_DATA1]); 
  }
  else if ((cmdValue>>I_CMD2_LEFT)&&0x01)
  {
    this->onMoveLeftHandler(this->bufCmd[I_DATA1]);
  }
  else if ((cmdValue>>I_CMD2_UP)&&0x01)
  {
    this->onMoveUpHandler(this->bufCmd[I_DATA2]);
  }
  else if ((cmdValue>>I_CMD2_DOWN)&&0x01)
  {
    this->onMoveDownHandler(this->bufCmd[I_DATA2]);
  }
}
unsigned char CPelcoDDecoder::calcChecksum(char* arrCmd, unsigned char len)
{
  unsigned char sum = 0;
  for (unsigned char i=1;i<len-1;i++)
  {
    sum+=arrCmd[i];
  }
  return sum;
}
void CPelcoDDecoder::setOnMoveLeftHandler(void( *handler(unsigned char)))
{
  this->onMoveLeft = handler;
}
void CPelcoDDecoder::setOnMoveRightHandler (void( *handler(unsigned char)))
{
  this->onMoveRight = handler;
}
void CPelcoDDecoder::setOnMoveUpHandler(void( *handler(unsigned char)))
{
  this->onMoveUp = handler;
}
void CPelcoDDecoder::setOnMoveDownHandler (void( *handler(unsigned char)))
{
  this->onMoveDown = handler;
}

void CPelcoDDecoder::setOnStopHandler(void* handler)
{
  this->onStop = handler;
}
void CPelcoDDecoder::setOnErrorHandler(void* handler)
{
  this->onErrorH = handler;
}

void CPelcoDDecoder::onMoveLeftHandler(unsigned char data)
{
  if (this->onMoveLeft ){
    this->onMoveLeft(data);
  }
}
void CPelcoDDecoder::onMoveRightHandler(unsigned char data)
{
  if (this->onMoveRight){
    this->onMoveRight(data);
  }
}
void CPelcoDDecoder::onMoveUpHandler(unsigned char data)
{
  if (this->onMoveUp ){
    this->onMoveUp(data);
  }
}
void CPelcoDDecoder::onMoveDownHandler(unsigned char data)
{
  if (this->onMoveDown){
    this->onMoveDown(data);
  }
}
void CPelcoDDecoder::onStopHandler()
{
  if (this->onStop){
    this->onStop();
  }
}
void CPelcoDDecoder::onErrorHandler(String errMsg)
{
  if (this->onErrorH)
  {
    this->onErrorH("Error:" + errMsg);
  }
}


