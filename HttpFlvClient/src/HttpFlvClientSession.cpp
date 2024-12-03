/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvClientSession.cpp
* Description           : 	
* Created               :       2022.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFlvClientSession.h"
#include "HttpFlvClientCom.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <functional>


/*****************************************************************************
-Fuction        : HttpFlvClientSession
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HttpFlvClientSession::HttpFlvClientSession()
{

}

/*****************************************************************************
-Fuction        : ~HttpFlvClientSession
-Description    : ~HttpFlvClientSession
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HttpFlvClientSession::~HttpFlvClientSession()
{

}


/*****************************************************************************
-Fuction        : FrameToContainer
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvClientSession::GetFrame(T_MediaFrameInfo *m_ptFileFrameInfo)
{
    int iRet = -1;
    
    if(NULL == m_ptFileFrameInfo)
    {
        HTTP_FLV_LOGE("GetFrame NULL \r\n");
        return iRet;
    }
    m_ptFileFrameInfo->iFrameLen = 0;
    m_oMediaHandle.GetFrame(m_ptFileFrameInfo);
    if(m_ptFileFrameInfo->iFrameLen <= 0)
    {
        HTTP_FLV_LOGE("m_tFileFrameInfo.iFrameLen <= 0 %d\r\n",m_ptFileFrameInfo->iFrameLen);
        return iRet;
    } 
    return 0;
}
/*****************************************************************************
-Fuction        : FrameToContainer
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvClientSession::FrameToContainer(T_MediaFrameInfo *m_ptFileFrameInfo,E_StreamType i_eStreamType,unsigned char * o_pbBuf, unsigned int i_dwMaxBufLen)
{
    int iRet =-1;
    int iHeaderLen=0;
    
    iRet = m_oMediaHandle.FrameToContainer(m_ptFileFrameInfo,i_eStreamType,o_pbBuf,i_dwMaxBufLen,&iHeaderLen);
    return iRet;
}

