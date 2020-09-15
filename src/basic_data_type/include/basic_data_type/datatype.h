/*
 * @Author: your name
 * @Date: 2020-09-09 09:23:43
 * @LastEditTime: 2020-09-15 11:14:13
 * @LastEditors: your name
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