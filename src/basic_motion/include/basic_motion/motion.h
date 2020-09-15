#pragma once
#include "basic_data_type/datatype.h"

//行走   前驱   后驱
//不同行走驱动方式走固定时间
//不同行走驱动方式走固定距离
//A B 臂移动到不同位置
//A B 臂转动到不同位置
//作业臂移动到不同位置
//控制柜移动到不同位置
//A B 臂压线机构工作位
class CRobot_Basic_Motion
{

public:
  CRobot_Basic_Motion(){};
  ~CRobot_Basic_Motion(){};

public:
  void armATravelFixedTime(EMotorState motorState,int speed, int sec);        
  void armATravelFixedDistance(EMotorState motorState,int speed ,double dis); 
  void armBTravelFixedTime(EMotorState motorState, int speed,int sec);
  void armBTravelFixedDistance(EMotorState motorState, int speed,double dis);

  void forwardDrivenTravelFixedTime(EMotorState motorState, int speed,int sec);
  void backwardDrivenTravelFixedTime(EMotorState motorState,int speed ,int sec);
  void dualDrivenTravelFixedTime(EMotorState motorState, int speed,int sec);

  void forwardDrivenTravelFixedDis(EMotorState motorState, int speed,double dis);
  void backwardDrivenTravelFixedDis(EMotorState motorState, int speed,double dis);
  void dualDrivenFixedDis(EMotorState motorState, int speed,double dis);

  void armARotateFixedAngle(EMotorState motorState,int speed,int angle);
  void armBRotateFixedAngle(EMotorState motorState,int speed,int angle);

  void armAMoveFixedPosition(EMotorState motorState,int speed,int position);
  void armBMoveFixedPosition(EMotorState motorState,int speed,int position);

  void opArmMoveFixedPosition(EMotorState motorState,int speed,int position);
  void controlCabinetMoveFixedPosition(EMotorState motorStatte,int speed,int position);
  
  void armALinePressMoveFixedPosition();
  void armBLinePressMoveFixedPosition();
};

class CRobot_Basic_State
{
  public:
  CRobot_Basic_State(){};
  ~CRobot_Basic_State(){};
};