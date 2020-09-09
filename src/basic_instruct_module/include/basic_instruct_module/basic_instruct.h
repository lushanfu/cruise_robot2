#include "basic_data_type/datatype.h"
#include "basic_instruct_module/nettransfer.h"
class CParseData //used to parse basic instruct and send to network
{
    public:
            CParseData();
    virtual ~CParseData();
    public:
          /******************control part******************/
          
          void armTravelMotor(EComponentType conponentType,EMotorState motorState,int speed);//A B  两个行进轮控制
          void armMoveMotor(EComponentType conponentType,EMotorState motorState,int speed=0,int step=0,int stopThreshold=0);//各个组件在丝杆上滑动控制
          void linePressMechanismMotor(EComponentType conponentType,int step,int stopThreshold);
          void assistLinePressMechanismMotor(EComponentType conponentType,EMotorState motorState,int step,int stopThreshold);
          void armRotateMotor(EComponentType conponentType,EMotorState motorState,int degree,int stopThreshold);
          
          /******************sensor value read part******************/
};