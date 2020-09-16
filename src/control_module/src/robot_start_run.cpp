/*
 * @Author: LSF
 * @Date: 2020-09-16 09:24:46
 * @LastEditTime: 2020-09-16 16:56:26
 * @LastEditors: Please set LastEditors
 * @Description: logic of robot travel
 */
#include"ros/ros.h"
#include<iostream>
using namespace std;

int main(int argc,char ** argv)
{
    bool isAuto=0;
    bool isTakePhotos=0;
    bool  isComfirm;
    ros::init(argc, argv, "param_demo");
    ros::NodeHandle nh;
    //根据当前姿态，进行初始姿态校正
    //给人工确认，是否是合理的姿态，人工确认后，进行机器人运动模式。
    while (nh.getParam("isComfirm",isComfirm) && !isComfirm)
    {
    }
    
    while (true)
    {
        while (nh.getParam("isAuto",isAuto) && isAuto)//自动模式
        {
            //自动模式-开线程-实时准备线程挂起
            if(nh.getParam("isTakePhotos",isTakePhotos) && isTakePhotos)
            {
                //自动模式线程挂起，先执行拍照程序
                //拍照程序，进行多角度、全方位，照片拍摄。
                //判断当前驾驶模式，判断继续运行自动还是销毁线程。
                nh.setParam("isTakePhotos",0);
            }
            //判断当前驾驶模式，是否被改为手动，同时看自动线程是否结束
        }
        //手动模式     
    }
    
}