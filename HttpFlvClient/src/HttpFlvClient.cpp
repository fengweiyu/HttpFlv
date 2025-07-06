/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvClient.cpp
* Description           : 	    Ϊ��Ӧ�Զ����ӣ�server��Ƴɳ�פ����
ע�⣬��Ա����Ҫ�����߳̾�������
* Created               :       2022.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFlvClient.h"
#include "HttpFlvClientCom.h"
#include <regex>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "cJSON.h"

using std::string;
using std::smatch;
using std::regex;

#define HTTP_FLV_MAX_MATCH_NUM       8
#define HTTP_FLV_BUF_MAX_LEN	(2*1024*1024) 


#define HTTP_FLV_UN_SUPPORT_FUN (-2)
#ifndef _WIN32
#include <regex.h> //C++ regexҪ��gcc 4.9���ϰ汾������linux������c��
/*****************************************************************************
-Fuction		: Regex
-Description	: ������ʽ
.��				ƥ�����\r\n��֮����κε����ַ�
*				ƥ��ǰ����ӱ��ʽ����Ρ����磬zo*��ƥ�䡰z����Ҳ��ƥ�䡰zo���Լ���zoo����*�ȼ���o{0,}
				����.*��ƥ��������洢�����������
(pattern)		ƥ��ģʽ��pattern����ȡ��һƥ�䡣����ȡ��ƥ����ԴӲ�����Matches���ϵõ�
[xyz]			�ַ����ϡ�ƥ��������������һ���ַ������磬��[abc]������ƥ�䡰plain���еġ�a����
+				ƥ��ǰ����ӱ��ʽһ�λ���(���ڵ���1�Σ������磬��zo+����ƥ�䡰zo���Լ���zoo����������ƥ�䡰z����+�ȼ���{1,}��
				//���������в���+��Ĭ����һ�Σ���ֻ��ƥ�䵽һ������6
				
[A-Za-z0-9] 	26����д��ĸ��26��Сд��ĸ��0��9����
[A-Za-z0-9+/=]	26����д��ĸ��26��Сд��ĸ0��9�����Լ�+/= �����ַ�


-Input			: i_strPattern ģʽ��,i_strBuf��ƥ���ַ���,
-Output 		: o_ptMatch �洢ƥ�䴮λ�õ�����,���ڴ洢ƥ�����ڴ�ƥ�䴮�е��±귶Χ
//����0��Ԫ�����������ʽƥ������λ��,�������������������ƥ��������ߵĵ�Ԫ���δ����������ʽƥ������λ��
-Return 		: -1 err,other cnt
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/11/01	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int CRegex(const char *i_strPattern,char *i_strBuf,regmatch_t *o_ptMatch,int i_iMatchMaxNum)
{
    char acErrBuf[256];
    int iRet=-1;
    regex_t tReg;    //����һ������ʵ��
    //const size_t dwMatch = 6;    //����ƥ�������������       //��ʾ������ƥ��


    //REG_ICASE ƥ����ĸʱ���Դ�Сд��
    iRet =regcomp(&tReg, i_strPattern, REG_EXTENDED);    //��������ģʽ��
    if(iRet != 0) 
    {
        regerror(iRet, &tReg, acErrBuf, sizeof(acErrBuf));
        HTTP_FLV_LOGE("Regex Error:\r\n");
        return -1;
    }
    
    iRet = regexec(&tReg, i_strBuf, i_iMatchMaxNum, o_ptMatch, 0); //ƥ����
    if (iRet == REG_NOMATCH)
    { //���ûƥ����
        HTTP_FLV_LOGE("Regex No Match!\r\n");
        iRet = 0;
    }
    else if (iRet == REG_NOERROR)
    { //���ƥ������
        HTTP_FLV_LOGD("Match\r\n");
        int i=0,j=0;
        for(i=0;i<i_iMatchMaxNum && o_ptMatch[i].rm_so != -1;i++)
        {
            for (j= o_ptMatch[i].rm_so; j < o_ptMatch[i].rm_eo;j++)
            { //�������ƥ�䷶Χ���ַ���
                //printf("%c", i_strBuf[j]);
            }
            //printf("\n");
        }
        iRet = i;
    }
    else
    {
        HTTP_FLV_LOGE("Regex Unknow err%d:\r\n",iRet);
        iRet = -1;
    }
    regfree(&tReg);  //�ͷ�������ʽ
    
    return iRet;
}
/*****************************************************************************
-Fuction		: HttpParseReqHeader
-Description	: Send
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int LinuxCRegex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt)
{
    int iRet = -1;
	string strBuf;
	regmatch_t atMatch[HTTP_FLV_MAX_MATCH_NUM];
	
    if(NULL == i_strPattern || NULL == i_strBuf || NULL == o_aMatch || i_iMatchMaxCnt <= 0 || i_iMatchMaxCnt <= 0)
    {
        HTTP_FLV_LOGE("LinuxCRegex NULL \r\n");
        return iRet;
    }
    strBuf.assign(i_strBuf);
    
    memset(atMatch,0,sizeof(atMatch));
    iRet= CRegex(i_strPattern,i_strBuf,atMatch,HTTP_FLV_MAX_MATCH_NUM);
    if(iRet <= 0 || i_iMatchMaxCnt < iRet)//ȥ������������-1
    {
        HTTP_FLV_LOGE("LinuxCRegex %d,%d, iRet <= 0 || i_iMatchMaxCnt < iRet err \r\n",i_iMatchMaxCnt,iRet);
        return iRet;
    }
    // �������ƥ�䵽���Ӵ�
    for (int i = 0; i < iRet; ++i) 
    {
        o_aMatch[i].assign(strBuf,atMatch[i].rm_so,atMatch[i].rm_eo-atMatch[i].rm_so);//0������
    }
    return iRet;
} 
#else
/*****************************************************************************
-Fuction		: HttpParseReqHeader
-Description	: Send
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int LinuxCRegex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt)
{
    return HTTP_FLV_UN_SUPPORT_FUN;
} 
#endif







/*****************************************************************************
-Fuction        : HttpFlvServer
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HttpFlvClient::HttpFlvClient()
{
    m_pbParseBuf = new unsigned char [HTTP_FLV_BUF_MAX_LEN];
    if(NULL == m_pbParseBuf)
    {
        HTTP_FLV_LOGE("NULL == m_tFileFrameInfo.pbFrameBuf err\r\n");
        return;
    } 
    m_iParseCurLen=0;
    memset(&m_tFrameInfo,0,sizeof(T_MediaFrameInfo));

    m_pbMuxBuf = new unsigned char [HTTP_FLV_BUF_MAX_LEN];
    if(NULL == m_pbMuxBuf)
    {
        HTTP_FLV_LOGE("NULL == m_tFileFrameInfo.pbMuxBuf err\r\n");
    } 
}
/*****************************************************************************
-Fuction        : ~HttpFlvServer
-Description    : ~HttpFlvServer
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HttpFlvClient::~HttpFlvClient()
{
    if(NULL != m_pbParseBuf)
    {
        delete [] m_pbParseBuf;
    } 
    if(NULL != m_pbMuxBuf)
    {
        delete [] m_pbMuxBuf;
    } 

}
/*****************************************************************************
-Fuction        : ParseHttpURL
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvClient::ParseHttpURL(const char * i_strHttpURL,string *o_strIP,int *o_iPort,string * o_strURL)
{
    int iRet = -1;
    string strHttpURL;
    
    if(NULL == i_strHttpURL || NULL == o_strIP || NULL == o_iPort || NULL == o_strURL)
    {
        HTTP_FLV_LOGE("HandleHttpReq NULL \r\n");
        return iRet;
    }

    strHttpURL.assign(i_strHttpURL);
    if(string::npos == strHttpURL.find(".flv"))//
    {
        HTTP_FLV_LOGE("strHttpURL.find flv err \r\n");
        return iRet;
    }
    
    auto dwHttpPos = strHttpURL.find("http://");
    if(string::npos == dwHttpPos)//
    {
        HTTP_FLV_LOGE("strHttpURL.find http://err \r\n");
        return iRet;
    }
    auto dwIpEndPos = strHttpURL.find(":",dwHttpPos+strlen("http://"));
    if(string::npos == dwIpEndPos)//
    {
        HTTP_FLV_LOGE("strHttpURL.find dwIpEndPos err \r\n");
        return iRet;
    }
    o_strIP->assign(strHttpURL.substr(dwHttpPos+strlen("http://"),dwIpEndPos-(dwHttpPos+strlen("http://"))).c_str());
    auto dwPortEndPos = strHttpURL.find("/",dwIpEndPos+strlen(":"));
    if(string::npos == dwPortEndPos)//
    {
        HTTP_FLV_LOGE("strHttpURL.find dwPortEndPos err \r\n");
        return iRet;
    }
    *o_iPort=atoi(strHttpURL.substr(dwIpEndPos+strlen(":"),dwPortEndPos-(dwIpEndPos+strlen(":"))).c_str());
    o_strURL->assign(strHttpURL.substr(dwPortEndPos).c_str());
    auto dwLocatePos = strHttpURL.find("/",dwPortEndPos+strlen("/"));
    if(string::npos == dwLocatePos)//
    {
        HTTP_FLV_LOGE("strHttpURL.find dwLocatePos err \r\n");
        return iRet;
    }
    auto dwLocateEndPos = strHttpURL.find("/",dwLocatePos+strlen("/"));
    if(string::npos == dwLocateEndPos)//
    {
        HTTP_FLV_LOGE("strHttpURL.find dwLocateEndPos err \r\n");
        return iRet;
    }
    HTTP_FLV_LOGD("strHttp %s:%d,%s \r\n",o_strIP->c_str(),*o_iPort,o_strURL->c_str());
    return 0;
}
/*****************************************************************************
-Fuction        : HandleReqGetM3U8
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvClient::GenerateGetFlvReq(const char * i_strFlvURL,char *o_strReq,int i_iReqMaxLen)
{
    int iRet = -1;
    char *pcM3U8 = NULL;
    int iM3U8Len = -1;
    
    if(NULL == i_strFlvURL || NULL == o_strReq|| i_iReqMaxLen <= 0)
    {
        HTTP_FLV_LOGE("GenerateGetM3U8Req NULL \r\n");
        return iRet;
    }
    iRet=HttpClient::CreateRequest("GET",i_strFlvURL);
    iRet=HttpClient::SetReqHeaderValue("User-Agent","ywfPalyer");
    iRet=HttpClient::SetReqHeaderValue("Accept","*/*");
    iRet=HttpClient::SetReqHeaderValue("Range","bytes=0-");
    iRet=HttpClient::SetReqHeaderValue("Connection","keep-alive");// close
    iRet=HttpClient::FormatReqToStream(NULL,0,o_strReq,i_iReqMaxLen);
    return iRet;
}

/*****************************************************************************
-Fuction        : HandleHttpM3U8
-Description    : Content-Type: video/mp2t
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvClient::ParseHttpMedia(char * i_pcHttpMedia,int i_iMediaLen,char **o_ppMediaData,int *o_iDataLen)
{
    int iRet = -1;
    T_HttpResPacket tHttpResPacket;
    
    if(NULL == i_pcHttpMedia || NULL == o_ppMediaData|| NULL == o_iDataLen|| i_iMediaLen <= 0)
    {
        HTTP_FLV_LOGE("HandleHttpMedia NULL \r\n");
        return iRet;
    }
    memset(&tHttpResPacket,0,sizeof(T_HttpResPacket));
    iRet=HttpClient::ParseResponse(i_pcHttpMedia,i_iMediaLen,&tHttpResPacket);
    if(iRet < 0)
    {
        HTTP_FLV_LOGE("HandleHttpMedia::ParseResponse err \r\n");
        return iRet;
    }
    if(tHttpResPacket.iStatusCode!=200)
    {
        HTTP_FLV_LOGE("HandleHttpMedia err, iStatusCode%d !=200,, strStatusMsg %s\r\n",tHttpResPacket.iStatusCode,tHttpResPacket.strStatusMsg);
        return iRet;
    }
    if(tHttpResPacket.iContentLength!=tHttpResPacket.iBodyCurLen)
    {
        HTTP_FLV_LOGW("tHttpResPacket.iContentLength%d!=tHttpResPacket.iBodyCurLen%d,i_iMediaLen%d\r\n",tHttpResPacket.iContentLength,tHttpResPacket.iBodyCurLen,i_iMediaLen);
    }
    *o_ppMediaData=tHttpResPacket.pcBody;
    *o_iDataLen=tHttpResPacket.iBodyCurLen;
    return 0;
}

/*****************************************************************************
-Fuction        : HandleHttpMedia
-Description    : Content-Type: video/mp2t
-Input          : 
-Output         : 
-Return         : �����Ѿ�����ĳ���
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvClient::HandleHttpMedia(char * i_pcHttpMedia,int i_iMediaLen,T_FlvClientStream *o_ptVideoStream,T_FlvClientStream *o_ptAudioStream,T_FlvClientStream *o_ptMuxStream)
{
    int iRet = -1;
    T_HttpResPacket tHttpResPacket;
    
    if(NULL == i_pcHttpMedia || i_iMediaLen <= 0)// || i_iMediaLen+m_iParseCurLen > HTTP_FLV_BUF_MAX_LEN)
    {
        HTTP_FLV_LOGE("HandleHttpMedia NULL %d %d\r\n",i_iMediaLen,m_iParseCurLen);
        return iRet;
    }
    //memcpy(m_pbParseBuf+m_iParseCurLen,i_pcHttpMedia,i_iMediaLen);
    //m_iParseCurLen+=i_iMediaLen;

    m_tFrameInfo.pbFrameBuf=(unsigned char *)i_pcHttpMedia;//�ⲿƫ�ƴ����ڲ�����Ҫ�ٴ���
    m_tFrameInfo.iFrameBufMaxLen = HTTP_FLV_BUF_MAX_LEN;
    m_tFrameInfo.iFrameBufLen = i_iMediaLen;
    m_tFrameInfo.iFrameLen=0;
    m_tFrameInfo.iFrameProcessedLen=0;
    m_tFrameInfo.eStreamType=STREAM_TYPE_FLV_STREAM;
    iRet = m_HttpFlvClientSession.GetFrame(&m_tFrameInfo);
    if(m_tFrameInfo.iFrameProcessedLen>0)
    {
        //memmove(m_pbParseBuf,m_pbParseBuf+m_tFrameInfo.iFrameProcessedLen,m_iParseCurLen-m_tFrameInfo.iFrameProcessedLen);
        //m_iParseCurLen = m_iParseCurLen-m_tFrameInfo.iFrameProcessedLen;
    }
    if(m_tFrameInfo.iFrameLen <= 0)
    {
        HTTP_FLV_LOGE("m_tFileFrameInfo.iFrameLen <= 0 ,%d %d %d\r\n",m_tFrameInfo.iFrameBufLen,m_iParseCurLen,m_tFrameInfo.iFrameProcessedLen);
        return m_tFrameInfo.iFrameProcessedLen>0?m_tFrameInfo.iFrameProcessedLen:-1;
    } 

    if(NULL != o_ptVideoStream && (m_tFrameInfo.eFrameType == MEDIA_FRAME_TYPE_VIDEO_I_FRAME ||
    m_tFrameInfo.eFrameType == MEDIA_FRAME_TYPE_VIDEO_P_FRAME || m_tFrameInfo.eFrameType == MEDIA_FRAME_TYPE_VIDEO_B_FRAME))
    {
        o_ptVideoStream->pStreamData=(char *)m_tFrameInfo.pbFrameStartPos;
        o_ptVideoStream->iStreamDataLen=m_tFrameInfo.iFrameLen;
        switch(m_tFrameInfo.eEncType)
        {
            case MEDIA_ENCODE_TYPE_H264:
            {
                snprintf(o_ptVideoStream->strStreamName,sizeof(o_ptVideoStream->strStreamName),"%s","h264");
                break;
            }
            case MEDIA_ENCODE_TYPE_H265:
            {
                snprintf(o_ptVideoStream->strStreamName,sizeof(o_ptVideoStream->strStreamName),"%s","h265");
                break;
            }
            default:
            {
                snprintf(o_ptVideoStream->strStreamName,sizeof(o_ptVideoStream->strStreamName),"%s","unknown");
                break;
            }
        }

    }
    if(NULL != o_ptAudioStream && m_tFrameInfo.eFrameType == MEDIA_FRAME_TYPE_AUDIO_FRAME)
    {
        o_ptAudioStream->pStreamData=(char *)m_tFrameInfo.pbFrameStartPos;
        o_ptAudioStream->iStreamDataLen=m_tFrameInfo.iFrameLen;
        switch(m_tFrameInfo.eEncType)
        {
            case MEDIA_ENCODE_TYPE_AAC:
            {
                snprintf(o_ptAudioStream->strStreamName,sizeof(o_ptAudioStream->strStreamName),"%s","aac");
                break;
            }
            case MEDIA_ENCODE_TYPE_G711A:
            {
                snprintf(o_ptAudioStream->strStreamName,sizeof(o_ptAudioStream->strStreamName),"%s","g711a");
                break;
            }
            default:
            {
                snprintf(o_ptAudioStream->strStreamName,sizeof(o_ptAudioStream->strStreamName),"%s","unknown");
                break;
            }
        }
    }
    if(NULL != o_ptMuxStream)
    {
        iRet = m_HttpFlvClientSession.FrameToContainer(&m_tFrameInfo,STREAM_TYPE_ENHANCED_FLV_STREAM,m_pbMuxBuf,HTTP_FLV_BUF_MAX_LEN);
        if(iRet < 0)
        {
            HTTP_FLV_LOGE("FrameToContainer err iWriteLen %d\r\n",iRet);
            return iRet;
        }
        if(iRet == 0)
        {
            HTTP_FLV_LOGE("FrameToContainer need more data %d\r\n",iRet);
            return m_tFrameInfo.iFrameProcessedLen;
        }
        o_ptMuxStream->pStreamData=(char *)m_pbMuxBuf;
        o_ptMuxStream->iStreamDataLen=iRet;
        snprintf(o_ptMuxStream->strStreamName,sizeof(o_ptMuxStream->strStreamName),"%s","flv");
    }
    return m_tFrameInfo.iFrameProcessedLen;
}

/*****************************************************************************
-Fuction        : HttpFlvRegex
-Description    : 
-Input          : 
-Output         : 
-Return         : -1 err,>0 cnt
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int HttpFlvClient::HttpFlvRegex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt)
{
    int iRet = -1;

    if(NULL == i_strPattern || NULL == i_strBuf || NULL == o_aMatch || i_iMatchMaxCnt <= 0)
    {
        HTTP_FLV_LOGE("HttpFlvRegex NULL \r\n");
        return iRet;
    }
    iRet = LinuxCRegex(i_strPattern,i_strBuf,o_aMatch,i_iMatchMaxCnt);
    if(HTTP_FLV_UN_SUPPORT_FUN!=iRet)
    {
        return iRet;
    }
    string strBuf(i_strBuf);
    regex Pattern(i_strPattern);//http://localhost:9212/file/H264AAC.flv/test.m3u8
    smatch Match;
    if (std::regex_search(strBuf,Match,Pattern)) 
    {
        if((int)Match.size()<=0 || i_iMatchMaxCnt < (int)Match.size())
        {
            HTTP_FLV_LOGE("HttpFlvRegex err i_iMatchMaxCnt %d<%d Match.size()-1 \r\n",i_iMatchMaxCnt,Match.size()-1);
            return iRet;
        }
        iRet = (int)Match.size();
        // �������ƥ�䵽���Ӵ�
        for (int i = 0; i < iRet; ++i) 
        {
            o_aMatch[i].assign( Match[i].str());
        }
    } 
    return iRet;
}


