/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvClient.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FLV_CLIENT_H
#define HTTP_FLV_CLIENT_H

#include "HttpFlvClientCom.h"
#include "HttpFlvClientSession.h"
#include "HttpClient.h"


/*****************************************************************************
-Class          : HttpFlvClient
-Description    : HttpFlvClient
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HttpFlvClient : public HttpClient
{
public:
	HttpFlvClient();
	virtual ~HttpFlvClient();
    int ParseHttpURL(const char * i_strHttpURL,string *o_strIP,int *o_iPort,string * o_strURL);
    int GenerateGetFlvReq(const char * i_strFlvURL,char *o_strReq,int i_iReqMaxLen);//return ReqLen,<0 err
    int ParseHttpMedia(char * i_pcHttpMedia,int i_iMediaLen,char **o_ppMediaData,int *o_iDataLen);
    int HandleHttpMedia(char * i_pcHttpMedia,int i_iMediaLen,T_FlvClientStream *o_ptVideoStream,T_FlvClientStream *o_ptAudioStream,T_FlvClientStream *o_ptMuxStream);
private:
    int HttpFlvRegex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt);
    
    HttpFlvClientSession m_HttpFlvClientSession;
    T_MediaFrameInfo m_tFrameInfo;
    unsigned char * m_pbParseBuf;
    int m_iParseCurLen;
    unsigned char * m_pbMuxBuf;
};













#endif
