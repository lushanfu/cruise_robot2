#pragma once
#include<basic_data_type/datatype.h"
class autoControl
{
  public:
    autoControl();
    ~autoControl();
  private:
      bool isHaveObstacle=false;
      bool isExistObstacle=true;
      double mapNextObstacleDis;


  public:
    void getCurrentPos();
    void judgeRelativelyPos();
    bool beyondSafeThreshold();
    bool imgJudge();
    void otherSensor();
    void achieveObstacleAnchor();
    void autoTravel()
    {
        //获取当前机器人位置-异步
        getCurrentPos();//python scripts
        //判断当前机器人在地图中的位置
        judgeRelativelyPos();////python scripts
        //是否在设定阈值内有障碍物
        isHaveObstacle=beyondSafeThreshold();////python scripts

        if(!isHaveObstacle)
        {
            //地图范围内不存在障碍物，视觉再来判断一次
            isHaveObstacle=imgJudge();//c++   cvision module
            if(isHaveObstacle)
            {
                //估计距离、运动停止
                //将当前状态置为手动模式
                return;
            }
            else
            {
                //继续行走
            }
            return;
            
        }
        bool confirmObstacle=imgJudge();
        if(confirmObstacle && isHaveObstacle)
        {
            //估计距离、运动停止
            //越障动作
            //发送继续行走指令
            return;
        }
        else
        {
            //靠传感器观测，观测到、运动到合适位置，停止，即报给人工，转手动
            return;
        }
    }
};