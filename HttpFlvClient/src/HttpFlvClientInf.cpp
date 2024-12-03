/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvClientInf.cpp
* Description           : 	    接口层，防止曝露内部文件
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFlvClientInf.h"
#include "HttpFlvClient.h"

/*****************************************************************************
-Fuction        : ~HttpFlvClientInf
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HttpFlvClientInf::HttpFlvClientInf()
{
    m_pHandle = NULL;
    m_pHandle = new HttpFlvClient();
}
/*****************************************************************************
-Fuction        : ~~HttpFlvClientInf
-Description    : ~~HttpFlvClientInf
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HttpFlvClientInf::~HttpFlvClientInf()
{
    if(NULL != m_pHandle)
    {
        HttpFlvClient *pHttpFlvClient = (HttpFlvClient *)m_pHandle;
        delete pHttpFlvClient;
    }  
}

/*****************************************************************************
-Fuction        : Proc
-Description    : Proc
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvClientInf::ParseHttpURL(const char * i_strHttpURL,string *o_strIP,int *o_iPort,string * o_strURL)
{
    HttpFlvClient *pHttpFlvClient = (HttpFlvClient *)m_pHandle;
    return pHttpFlvClient->ParseHttpURL(i_strHttpURL,o_strIP,o_iPort,o_strURL);
}

/*****************************************************************************
-Fuction        : Proc
-Description    : Proc
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvClientInf::GenerateGetFlvReq(const char * i_strFlvURL,char *o_strReq,int i_iReqMaxLen)
{
    HttpFlvClient *pHttpFlvClient = (HttpFlvClient *)m_pHandle;
    return pHttpFlvClient->GenerateGetFlvReq(i_strFlvURL,o_strReq,i_iReqMaxLen);
}


/*****************************************************************************
-Fuction        : ParseHttpMedia
-Description    : ParseHttpMedia
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvClientInf::ParseHttpMedia(char * i_strHttpMedia,int i_iMediaLen,char **o_ppMediaData,int *o_iDataLen)
{
    HttpFlvClient *pHttpFlvClient = (HttpFlvClient *)m_pHandle;
    return pHttpFlvClient->ParseHttpMedia(i_strHttpMedia,i_iMediaLen,o_ppMediaData,o_iDataLen);
}


/*****************************************************************************
-Fuction        : Proc
-Description    : Proc
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvClientInf::HandleHttpMedia(char * i_pcHttpMedia,int i_iMediaLen,void *o_ptVideoStream,void *o_ptAudioStream,void *o_ptMuxStream)
{
    HttpFlvClient *pHttpFlvClient = (HttpFlvClient *)m_pHandle;
    return pHttpFlvClient->HandleHttpMedia(i_pcHttpMedia,i_iMediaLen,(T_FlvClientStream *)o_ptVideoStream,(T_FlvClientStream *)o_ptAudioStream,(T_FlvClientStream *)o_ptMuxStream);
}

