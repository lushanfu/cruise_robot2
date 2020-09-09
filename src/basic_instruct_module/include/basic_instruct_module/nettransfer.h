#pragma once
#include"basic_data_type/datatype.h"

class CNetTransfer
{
private:
    /* data */
public:
    CNetTransfer(/* args */);
    ~CNetTransfer();
public:
    SNetRecieve netTransfer(const char* iP,int port,char arrMsg[],int n=8);
};


