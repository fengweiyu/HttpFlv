/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvClientInf.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FLV_CLIENT_INF_H
#define HTTP_FLV_CLIENT_INF_H

#include <string>

using std::string;


/*****************************************************************************
-Class          : HttpFlvClientInf
-Description    : HttpFlvClientInf
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HttpFlvClientInf
{
public:
	HttpFlvClientInf();
	virtual ~HttpFlvClientInf();
    int ParseHttpURL(const char * i_strHttpURL,string *o_strIP,int *o_iPort,string * o_strURL);
    int GenerateGetFlvReq(const char * i_strFlvURL,char *o_strReq,int i_iReqMaxLen);//return ReqLen,<0 err
    int ParseHttpMedia(char * i_pcHttpMedia,int i_iMediaLen,char **o_ppMediaData,int *o_iDataLen);
    int HandleHttpMedia(char * i_pcHttpMedia,int i_iMediaLen,void *o_ptVideoStream,void *o_ptAudioStream,void *o_ptMuxStream);
private:
    void * m_pHandle;
};










#endif
