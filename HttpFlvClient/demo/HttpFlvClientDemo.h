/*****************************************************************************
* Copyright (C) 2023-2028 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvClientDemo.h
* Description           : 	
* Created               :       2023.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FLV_CLIENT_DEMO_H
#define HTTP_FLV_CLIENT_DEMO_H

#include <mutex>
#include <string>
#include <list>
#include <map>
#include "HttpFlvClientIO.h"

using std::map;
using std::string;
using std::list;
using std::mutex;

/*****************************************************************************
-Class			: HttpFlvClientDemo
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2019/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class HttpFlvClientDemo
{
public:
	HttpFlvClientDemo();
	virtual ~HttpFlvClientDemo();
    int Proc(const char * i_strHttpURL,const char *i_strFlag=NULL);
    
private:
    HttpFlvClientIO m_HttpFlvClientIO;
};

#endif
