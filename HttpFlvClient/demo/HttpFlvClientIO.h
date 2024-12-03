/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvClientIO.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FLV_CLIENT_IO_H
#define HTTP_FLV_CLIENT_IO_H

#include "TcpSocket.h"
#include "HttpFlvClientInf.h"
#include "HttpFlvClientCom.h"
#include <thread>
#include <mutex>

using std::thread;
using std::mutex;

/*****************************************************************************
-Class			: HttpFlvClientIO
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2019/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class HttpFlvClientIO : public TcpClient
{
public:
	HttpFlvClientIO();
	virtual ~HttpFlvClientIO();
    int Proc(const char * i_strHttpURL,const char *i_strOutPath=NULL,int i_iOutSize=-1);
    int GetProcFlag();
private:
    int SaveFile(const char * strFileName,char *i_pData,int i_iLen);
    int CreateSaveDir(const char * i_strLocation,string *o_strSaveDir);
    int GetFileName(string * i_strRoot,string *o_strFileName);

    HttpFlvClientInf * m_pHttpFlvClient;
	int m_iClientSocketFd;

	int m_iHttpFlvClientIOFlag;
};

#endif
