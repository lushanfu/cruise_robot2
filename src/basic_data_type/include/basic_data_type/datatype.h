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