#include "CPelcoDDecoder.h"
#include <SoftwareSerial.h>
#include <Servo.h>

Servo SrvH;// Servo for horizontal axis
Servo SrvV;// Servo for vertical axis
CPelcoDDecoder PD;
//SoftwareSerial SWSerial(10,11);// debug serial port
#define DbgPort Serial
#define CMDPort Serial
#define PIN_SERVO_H (12)
#define PIN_SERVO_V (9)
#define SERVO_MOVE_DELAY_MAX (300)
#define PIN_SERVO_POWER (13)

long timerOvfValue  = 0;
int moveSpeedForTimer = SERVO_MOVE_DELAY_MAX;
boolean flag_moveTimerEnabled = false;
boolean flag_MoveTimerOverflow = true;

#define MOVE_LEFT (-1)
#define MOVE_NONE (0)
#define MOVE_RIGHT (1)
#define MOVE_UP (2)
#define MOVE_DOWN (3)
signed int moveState = MOVE_NONE;
int moveHStep = 3;
int moveVStep = 2;

void logMessage( String errMsg);
void doMoveRight(unsigned char speedVal);
void doMoveLeft(unsigned char speedVal);
void doMoveUp(unsigned char speedVal);
void doMoveDown(unsigned char speedVal);
void doStop();
int calcMoveSpeed(unsigned char data);

void setup() {
  // put your setup code here, to run once:
  CMDPort.begin(2400);
//  DbgPort.begin(9600);
  // set Pelco Handlers
  PD.setOnErrorHandler(*logMessage);
  PD.setOnMoveRightHandler(*doMoveRight);
  PD.setOnMoveLeftHandler(*doMoveLeft);
  PD.setOnMoveUpHandler(*doMoveUp);
  PD.setOnMoveDownHandler(*doMoveDown);
  PD.setOnStopHandler(*doStop);
  PD.begin(&CMDPort, 0x01);

  SrvH.attach(PIN_SERVO_H);
  SrvV.attach(PIN_SERVO_V);
  test();
  DbgPort.println("Hello");
}

void loop()
{
  PD.parseData(); 
  if (flag_MoveTimerOverflow)
  {
    flag_MoveTimerOverflow = false;
    doMoveHandler();
  }
  if (flag_moveTimerEnabled)
  {
    moveTimerCheck(); 
  }
}

void doMoveHandler()
{
  switch (moveState)
  {
    case (MOVE_NONE):
      stopMoving();
      return;
    case (MOVE_LEFT):
      if (SrvH.read()<180){
        SrvH.write(SrvH.read() + moveHStep);      
      }
      else
      {
        stopMoving();
      }  
      break;
    case (MOVE_RIGHT):
      if (SrvH.read()>0){
        SrvH.write(SrvH.read() - moveHStep);      
      }
      else
      {
        stopMoving();
      }
      break;
    case (MOVE_UP):
      if (SrvV.read()>0){
        SrvV.write(SrvV.read() + moveVStep);      
      }
      else
      {
        stopMoving();
      }
      break;
    case (MOVE_DOWN):
      if (SrvV.read()>0){
        SrvV.write(SrvV.read() - moveVStep);      
      }
      else
      {
        stopMoving();
      }
      break;        
  }
  logMessage(((String)SrvH.read()+"/"+(String)SrvV.read()));
  moveTimerReset();
}

void moveTimerCheck()
{
  if (millis()> timerOvfValue){
    flag_MoveTimerOverflow = true;
  }
}
void moveTimerReset()
{
  //flag_moveTimerEnabled = true;
  timerOvfValue = millis() + moveSpeedForTimer;
}

void moveTimerStop()
{
  flag_moveTimerEnabled = false;
  //logMessage("STOP timer");
}

void stopMoving()
{
  moveTimerStop();
  disableServoPower();
}

void startMoving()
{
  flag_moveTimerEnabled = true;
  moveTimerReset();
  enableServoPower();
}

void disableServoPower()
{
  digitalWrite(PIN_SERVO_POWER,LOW);
}

void enableServoPower()
{
  digitalWrite(PIN_SERVO_POWER,HIGH);
}

void logMessage(String errMsg)
{
  DbgPort.println(errMsg);
}

void doMoveRight(unsigned char speedVal)
{
  logMessage(F("Event: Move right"));
  moveState = MOVE_RIGHT;
  moveSpeedForTimer = calcMoveSpeed(speedVal);
  startMoving();
}

void doMoveLeft(unsigned char speedVal)
{
  logMessage(F("Event: Move left"));
  moveState = MOVE_LEFT;
  moveSpeedForTimer = calcMoveSpeed(speedVal);
  startMoving();
}

void doMoveUp(unsigned char speedVal)
{
  logMessage(F("Event: Move up"));
  moveState = MOVE_UP;
  moveSpeedForTimer = calcMoveSpeed(speedVal);
  startMoving();
}

void doMoveDown(unsigned char speedVal)
{
  logMessage(F("Event: Move down"));
  moveState = MOVE_DOWN;
  moveSpeedForTimer = calcMoveSpeed(speedVal);
  startMoving();
}

int calcMoveSpeed(unsigned char data)
{
  return SERVO_MOVE_DELAY_MAX - ((int)data*4);
}

void doStop()
{
  logMessage(F("Event: Stop"));
  moveState = MOVE_NONE;
  flag_MoveTimerOverflow = true;
}

//==============test===================
void test()
{
  char cmd_right[] = {0xff,// sync
                    0x01,//addrr
                    0x00,//cmd1
                    2,//cmd2
                    10,//data1
                    0,//data2
                    13// sum
                   };

char cmd_left[] = {0xff,// sync
                    0x01,//addrr
                    0x00,//cmd1
                    4,//cmd2
                    2,//data1
                    0,//data2
                    7// sum
                   };
char cmd_stop[] = {0xff,// sync
                    0x01,//addrr
                    0x00,//cmd1
                    0,//cmd2
                    0,//data1
                    0,//data2
                    1// sum
                   };                   
  //PD.parseCommand(cmd_right, 7);
  //PD.parseCommand(cmd_left,  7);
//  PD.parseCommand(cmd_stop,  7);
}

