/*
 * @Author: your name
 * @Date: 2020-09-09 09:23:43
 * @LastEditTime: 2020-09-16 16:09:06
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /cruiserobot_ws/home/lsf/LSF/cruise_robot2/src/basic_data_type/include/basic_data_type/datatype.h
 */
#pragma once
struct SNetRecieve
{
    int length;
    char * content;
    int realNum;//the last value of nettransfer return 
};
enum EComponentType
{
   
    armA=0,
    armB=1,
    arm_operation=2,
    control_cabinet=3
};
enum EMotorState
{
    neutralPos=0,
    brake=1,
    walkForward=2,
    walkBackward=3
};
struct SRobotParams
{
   int armA_move_param;
   int armB_move_param;
   int armA_rotate_param;
   int armB_rotate_param;
   int armOp_move_param;
   int controlCabinet_move_param;
   int cam_anchor;
};

class CObstacle
{
private:
    CObstacle* nextObstacle;
    int    obstacleType;
    int    overcomingObstacleType;//翻转、旋转+行走
    double length;
    double nextObstacleDis;
    SRobotParams robotParams;
public:
    CObstacle(/* args */);
    ~CObstacle();
};


