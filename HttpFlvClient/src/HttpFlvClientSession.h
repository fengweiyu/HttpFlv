/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvClientSession.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FLV_CLIENT_SESSION_H
#define HTTP_FLV_CLIENT_SESSION_H

#include <thread>
#include <mutex>
#include <string>
#include <list>
#include <map>

#include "MediaHandle.h"

using std::map;
using std::string;
using std::list;
using std::mutex;
using std::thread;

/*****************************************************************************
-Class          : HttpFlvClientSession
-Description    : HttpFlvClientSession
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2022/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HttpFlvClientSession
{
public:
	HttpFlvClientSession();
	virtual ~HttpFlvClientSession();
	int GetFrame(T_MediaFrameInfo *m_ptFileFrameInfo);
	int FrameToContainer(T_MediaFrameInfo *m_ptFileFrameInfo,E_StreamType i_eStreamType,unsigned char * o_pbBuf, unsigned int i_dwMaxBufLen);
private:


    MediaHandle m_oMediaHandle;
};













#endif
