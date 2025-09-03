/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       main.cpp
* Description           : 	    
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>

#include "HttpFlvClientDemo.h"

static void PrintUsage(char *i_strProcName);

/*****************************************************************************
-Fuction        : main
-Description    : main
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/01      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int main(int argc, char* argv[]) 
{
    int iRet = -1;
    
    if(argc < 2)
    {
        PrintUsage(argv[0]);
        return iRet;
    }
    HttpFlvClientDemo *pHttpFlvClientDemo = new HttpFlvClientDemo();
    if(argc == 2)
    {
        iRet=pHttpFlvClientDemo->Proc(argv[1]);//阻塞
        delete pHttpFlvClientDemo;
        return iRet;
    }
    iRet=pHttpFlvClientDemo->Proc(argv[1],atoi(argv[2]));//阻塞 第二个参数kB
    delete pHttpFlvClientDemo;
    return iRet;
}
/*****************************************************************************
-Fuction        : PrintUsage
-Description    : PrintUsage
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/01      V1.0.0              Yu Weifeng       Created
******************************************************************************/
static void PrintUsage(char *i_strProcName)
{
    printf("Usage: %s inputURL (this will Generate files in ./*.flv *.h264 or *.h265 *.aac or *.g711a)\r\n",i_strProcName);
    printf("eg: %s http://localhost:9214/file/H264AAC.flv/test.flv\r\n",i_strProcName);
    printf("or: %s inputURL outputSIZE(unit KB) \r\n",i_strProcName);
    printf("eg: %s http://localhost:9214/file/H265AAC.flv/test_enhanced.flv 1\r\n",i_strProcName);
}

