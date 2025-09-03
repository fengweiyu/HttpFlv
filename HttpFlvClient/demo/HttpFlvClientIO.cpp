/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvClientIO.c
* Description           : 	
* Created               :       2023.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFlvClientIO.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <thread>
#include <stdio.h>  
#include <sys/types.h>  
#include <errno.h>  
#include <string.h>

using std::thread;

#define HTTP_FLV_IO_SEND_MAX_LEN (10240)
#define HTTP_FLV_IO_RECV_MAX_LEN (500*1024) //(1*1024*1024)

typedef enum HttpFlvClientState
{
    HTTP_FLV_CLIENT_INIT=0,
    HTTP_FLV_CLIENT_GET_FLV,
    HTTP_FLV_CLIENT_HANDLE_FLV,
    HTTP_FLV_CLIENT_EXIT,
}E_HttpFlvClientState;


/*****************************************************************************
-Fuction		: HttpFlvClientIO
-Description	: HttpFlvClientIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HttpFlvClientIO :: HttpFlvClientIO()
{
    m_pHttpFlvClient=new HttpFlvClientInf();
    m_iClientSocketFd=-1;
    m_iHttpFlvClientIOFlag = 0;
}

/*****************************************************************************
-Fuction		: ~HttpFlvClientIO
-Description	: ~HttpFlvClientIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HttpFlvClientIO :: ~HttpFlvClientIO()
{
    if(NULL!= m_pHttpFlvClient)
    {
        HTTP_FLV_LOGW("~HttpFlvClientIO start exit\r\n");
        delete m_pHttpFlvClient;
        m_pHttpFlvClient = NULL;
    }
    HTTP_FLV_LOGW("~~HttpFlvClientIO exit\r\n");
}

/*****************************************************************************
-Fuction		: Proc
-Description	: 阻塞
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HttpFlvClientIO :: Proc(const char * i_strHttpURL,int i_iOutSize)
{
    int iRet=-1;
    char *pcRecvBuf=NULL;
    char *pcSendBuf=NULL;
    int iRecvLen=-1;
    timeval tTimeValue;
    string strDomain("");
    string strIP("");
    int iPort=0;
    string strURL("");
    E_HttpFlvClientState eHttpFlvClientState=HTTP_FLV_CLIENT_INIT;
    milliseconds timeFlvMS(10000);// 表示10000毫秒
    milliseconds timeMediaMS(500);// 表示500毫秒，每次都超时500ms，可以减小这个值从而缩短处理(等待)时间
    int iOffset=0,iTryTime=10;//第一次10s
    int iFileLen=0,iProcessedLen=0;
    char *pcFileBuf=NULL;
    string strFileDir("");
    string strFileName("");
    char strFilePath[128]={0,};
    int iTrySendTime=3;
    int iTryRecvTime=15;
    int iSendLen=0;
    int iOutSize=0;
    T_FlvClientStream tVideoStream;
    T_FlvClientStream tAudioStream;
    T_FlvClientStream tMuxStream;



    if(NULL == i_strHttpURL)
    {
        HTTP_FLV_LOGE("NULL == i_strHttpURL || NULL == i_strOutPath || i_iOutSize <1 err\r\n");
        return -1;
    }
    if(i_iOutSize>0)
    {
        iOutSize=i_iOutSize*1024;
    }
    else
    {
        iOutSize=HTTP_FLV_IO_RECV_MAX_LEN;
    }
    pcRecvBuf = new char[iOutSize];
    if(NULL == pcRecvBuf)
    {
        HTTP_FLV_LOGE("HttpFlvClientIO NULL == pcRecvBuf err\r\n");
        return -1;
    }
    pcSendBuf = new char[HTTP_FLV_IO_SEND_MAX_LEN];
    if(NULL == pcSendBuf)
    {
        HTTP_FLV_LOGE("HttpFlvClientIO NULL == pcSendBuf err\r\n");
        delete[] pcRecvBuf;
        return -1;
    }





    m_iHttpFlvClientIOFlag = 1;
    HTTP_FLV_LOGW("HttpFlvClientIO start Proc\r\n");

    while(m_iHttpFlvClientIOFlag)
    {
        switch(eHttpFlvClientState)
        {
            case HTTP_FLV_CLIENT_INIT:
            {
                iRet=m_pHttpFlvClient->ParseHttpURL(i_strHttpURL,&strDomain,&iPort,&strURL);
                if(iRet < 0)
                {
                    HTTP_FLV_LOGE("m_pHttpFlvClient->ParseHttpURL err exit %s\r\n",i_strHttpURL);
                    break;
                }
                iRet=TcpClient::ResolveDomain(&strDomain,&strIP);
                if(iRet < 0)
                {
                    HTTP_FLV_LOGE("TcpClient::ResolveDomain err exit %s,%s\r\n",strDomain.c_str(),strIP.c_str());
                    break;
                }
                iRet=CreateSaveDir(strURL.c_str()+1,&strFileDir);
                if(iRet < 0)
                {
                    HTTP_FLV_LOGE("m_pHttpFlvClient->CreateSaveDir err exit %s\r\n",i_strHttpURL);
                    break;
                }
                iRet=GetFileName(&strURL,&strFileName);
                if(iRet < 0)
                {
                    HTTP_FLV_LOGE("m_pHttpFlvClient->GetFileName err exit %s\r\n",i_strHttpURL);
                    break;
                }
                eHttpFlvClientState=HTTP_FLV_CLIENT_GET_FLV;
                break;
            }
            case HTTP_FLV_CLIENT_GET_FLV:
            {
                memset(pcSendBuf,0,HTTP_FLV_IO_SEND_MAX_LEN);
                iSendLen=m_pHttpFlvClient->GenerateGetFlvReq(strURL.c_str(),pcSendBuf,HTTP_FLV_IO_SEND_MAX_LEN);
                if(iSendLen < 0)
                {
                    HTTP_FLV_LOGE("m_pHttpFlvClient->GenerateGetM3U8Req err exit %s\r\n",i_strHttpURL);
                    break;
                }

                iTrySendTime=3;
                do
                {
                    iRet=TcpClient::Init(&strIP,(unsigned short)iPort);
                    if(iRet < 0)
                    {
                        HTTP_FLV_LOGE("TcpClient::Init err exit %s,%d\r\n",strIP.c_str(),iPort);
                        break;
                    }
                    m_iClientSocketFd=TcpClient::GetClientSocket();
                    iRet=TcpClient::Send(pcSendBuf,iSendLen);
                    if(iRet < 0)
                    {
                        HTTP_FLV_LOGE("TcpClient::Send err exit %s,%d\r\n",strIP.c_str(),iPort);
                        if(iTrySendTime > 0)
                        {
                            SleepMs(1000);
                            iTrySendTime--;
                            TcpClient::Close();
                            m_iClientSocketFd=-1;
                            continue;
                        }
                        break;
                    }
                    do
                    {
                        iRecvLen = 0;
                        memset(pcRecvBuf,0,iOutSize);
                        iRet=TcpClient::Recv(pcRecvBuf,&iRecvLen,iOutSize,m_iClientSocketFd,&timeFlvMS);//不知道协议大小，内部必然超时才退出
                        if(iRet < 0)
                        {
                            HTTP_FLV_LOGE("TcpClient::Recv err exit %d\r\n",eHttpFlvClientState);
                            break;
                        }
                        if(iRecvLen<=0)
                        {//超时
                            HTTP_FLV_LOGE("TcpClient::Recv iRecvLen err exit %d\r\n",eHttpFlvClientState);
                            if(iTryRecvTime > 0)
                            {//第一次登陆比较久
                                iTryRecvTime--;
                                continue;
                            }
                            iRet=-1;//退出程序
                            break;
                        }
                        break;
                    }while(1);
                    if(iRet < 0)
                    {//Recv
                        HTTP_FLV_LOGE("TcpClient::Recv Recv err exit %d\r\n",eHttpFlvClientState);
                        if(iTryTime > 0)
                        {
                            SleepMs(iTryTime*1000);
                            iTryTime=0;
                            TcpClient::Close();
                            m_iClientSocketFd=-1;
                            continue;
                        }
                        break;
                    }
                    break;
                }while(1);
                if(iRet < 0)
                {
                    HTTP_FLV_LOGE("HTTP_FLV_CLIENT_GET_M3U8 eHttpFlvClientState err exit %s,%d\r\n",strIP.c_str(),eHttpFlvClientState);
                    break;
                }

                iOffset=0;
                iTryTime=0;
                iRet = m_pHttpFlvClient->ParseHttpMedia(pcRecvBuf,iRecvLen,&pcFileBuf,&iFileLen);
                if(iRet < 0)
                {
                    HTTP_FLV_LOGE("m_pHttpFlvClient->ParseHttpMedia err exit %s,%s\r\n",strIP.c_str(),pcRecvBuf);
                    break;
                }
                snprintf(strFilePath,sizeof(strFilePath),"%s/%s.%s",strFileDir.c_str(),strFileName.substr(0,31).c_str(),"flv.flv");
                this->SaveFile((const char *)strFilePath,pcFileBuf,iFileLen);
                eHttpFlvClientState=HTTP_FLV_CLIENT_HANDLE_FLV;
                break;
            }
            case HTTP_FLV_CLIENT_HANDLE_FLV:
            {
                iProcessedLen=0;
                while(iFileLen-iProcessedLen > 0)
                {
                    memset(&tVideoStream,0,sizeof(T_FlvClientStream));
                    memset(&tAudioStream,0,sizeof(T_FlvClientStream));
                    memset(&tMuxStream,0,sizeof(T_FlvClientStream));
                    iRet = m_pHttpFlvClient->HandleHttpMedia(pcFileBuf+iProcessedLen,iFileLen-iProcessedLen,&tVideoStream,&tAudioStream,&tMuxStream);
                    if(iRet < 0)
                    {
                        HTTP_FLV_LOGE("HandleHttpMedia over exit %d,%d\r\n",iFileLen,iProcessedLen);
                        break;
                    }
                    iProcessedLen += iRet;
                    if(strlen(tVideoStream.strStreamName) > 0 && tVideoStream.pStreamData != 0 && tVideoStream.iStreamDataLen > 0)
                    {
                        snprintf(strFilePath,sizeof(strFilePath),"%s/%s.%s",strFileDir.c_str(),strFileName.substr(0,31).c_str(),tVideoStream.strStreamName);
                        this->SaveFile((const char *)strFilePath,tVideoStream.pStreamData,tVideoStream.iStreamDataLen);
                    } 
                    if(strlen(tAudioStream.strStreamName) > 0 && tAudioStream.pStreamData != 0 && tAudioStream.iStreamDataLen > 0)
                    {
                        snprintf(strFilePath,sizeof(strFilePath),"%s/%s.%s",strFileDir.c_str(),strFileName.substr(0,31).c_str(),tAudioStream.strStreamName);
                        this->SaveFile((const char *)strFilePath,tAudioStream.pStreamData,tAudioStream.iStreamDataLen);
                    } 
                    if(strlen(tMuxStream.strStreamName) > 0 && tMuxStream.pStreamData != 0 && tMuxStream.iStreamDataLen > 0)
                    {
                        snprintf(strFilePath,sizeof(strFilePath),"%s/%s.%s",strFileDir.c_str(),strFileName.substr(0,31).c_str(),tMuxStream.strStreamName);
                        this->SaveFile((const char *)strFilePath,tMuxStream.pStreamData,tMuxStream.iStreamDataLen);
                    } 
                }
                if(i_iOutSize<=0)
                {//表示取完流
                    memmove(pcRecvBuf,pcFileBuf+iProcessedLen,iFileLen-iProcessedLen);
                    iFileLen = iFileLen-iProcessedLen;
                    iRecvLen = 0;
                    iRet=TcpClient::Recv(pcRecvBuf+iFileLen,&iRecvLen,iOutSize-iFileLen,m_iClientSocketFd,&timeMediaMS);
                    if(iRet < 0)
                    {
                        HTTP_FLV_LOGE("TcpClient::Recv err exit %d\r\n",eHttpFlvClientState);
                    }
                    if(iRecvLen<=0)
                    {
                        HTTP_FLV_LOGE("TcpClient::Recv iRecvLen err %d\r\n",iTryTime);
                    }
                    else
                    {
                        snprintf(strFilePath,sizeof(strFilePath),"%s/%s.%s",strFileDir.c_str(),strFileName.substr(0,31).c_str(),"flv.flv");
                        this->SaveFile((const char *)strFilePath,pcRecvBuf+iFileLen,iRecvLen);
                    }
                    pcFileBuf=pcRecvBuf;
                    iFileLen+=iRecvLen;
                }
                else
                {
                    iFileLen=0;
                    eHttpFlvClientState=HTTP_FLV_CLIENT_EXIT;
                }
                break;
            }
            default :
            {
                break;
            }
        }        
        if(iRet < 0)
        {
            HTTP_FLV_LOGE("eHttpFlvClientState err exit %s,%d\r\n",strIP.c_str(),eHttpFlvClientState);
            break;
        }
        if(HTTP_FLV_CLIENT_EXIT == eHttpFlvClientState)
        {
            HTTP_FLV_LOGE("eHttpFlvClientState exit %s,%d\r\n",strIP.c_str(),eHttpFlvClientState);
            break;
        }
    }
    
    if(m_iClientSocketFd>=0)
    {
        TcpClient::Close();
        HTTP_FLV_LOGW("TcpClient::Close m_iClientSocketFd Exit%d\r\n",m_iClientSocketFd);
    }
    if(NULL != pcSendBuf)
    {
        delete[] pcSendBuf;
    }
    if(NULL != pcRecvBuf)
    {
        delete[] pcRecvBuf;
    }
    m_iHttpFlvClientIOFlag=0;
    return 0;
}

/*****************************************************************************
-Fuction		: GetProcFlag
-Description	: HlsServerIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HttpFlvClientIO :: CreateSaveDir(const char * i_strRoot,string *o_strSaveDir)
{
    int iRet = -1;
    char strLocation[512];

    if(NULL == i_strRoot || NULL == o_strSaveDir)
    {
        HTTP_FLV_LOGE("CreateSaveDir NULL \r\n");
        return iRet;
    }
    memset(strLocation,0,sizeof(strLocation));
    memcpy(strLocation,i_strRoot,strlen(i_strRoot));
    //iRet = DeleteDir((const char *)strLocation);//无法删除非空目录
    char *sep = strchr(strLocation, '/');
    if (sep != NULL) 
    {  
        *sep = '\0'; 
        RemoveDir((const char *)strLocation);
        SleepMs(300);
        if (MakeDir(strLocation) == -1) 
        {  
            if (errno == EEXIST) 
            {  
            } 
            else if (errno == ENOENT) 
            {  
                return iRet;
            } 
        } 
        *sep = '/';  
        char *sep2 = strchr(sep+1, '/');
        *sep2 = '\0';  
        if (MakeDir(strLocation) == -1) 
        {  
            if (errno == EEXIST) 
            {  
            } 
            else if (errno == ENOENT) 
            {  
                return iRet;
            } 
        } 
        o_strSaveDir->assign(strLocation);
        *sep2 = '/';  
    }

    return 0;
}

/*****************************************************************************
-Fuction		: GetFileName
-Description	: /file/H265AAC.flv/test_enhanced.flv
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HttpFlvClientIO :: GetFileName(string * i_strRoot,string *o_strFileName)
{
    int iRet = -1;
    char strFileName[512];


    auto iFormatPos = i_strRoot->rfind("/");
    if(string::npos == iFormatPos)
    {
        HTTP_FLV_LOGE("GetFileName no flv err \r\n");
        return iRet;
    }
    
    auto iFormatNamePos = i_strRoot->rfind(".");
    if(string::npos != iFormatNamePos)
    {
        o_strFileName->assign(i_strRoot->substr(iFormatPos+1,iFormatNamePos-iFormatPos-1).c_str());
        iRet = 0;
    }
    else
    {
        HTTP_FLV_LOGE("GetFileName err \r\n");
    }

    
    auto iFormatName = i_strRoot->substr(iFormatPos).find(".flv");
    if(string::npos == iFormatName)
    {
        HTTP_FLV_LOGE("GetFileName err no flv \r\n");
        return iRet;
    }
    return iRet;
}

/*****************************************************************************
-Fuction		: SaveFile
-Description	: /file/H265AAC.flv/test_enhanced.flv
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HttpFlvClientIO :: SaveFile(const char * strFileName,char *i_pData,int i_iLen)
{
    int iRet = -1;
    FILE  *pMediaFile=NULL;

    if(NULL == strFileName || NULL == i_pData || i_iLen <= 0)
    {
        HTTP_FLV_LOGE("SaveFile NULL \r\n");
        return iRet;
    }
    pMediaFile = fopen(strFileName,"ab");//wb
    if(NULL == pMediaFile)
    {
        HTTP_FLV_LOGE("fopen %s err\r\n",strFileName);
        return iRet;
    } 
    iRet = fwrite(i_pData, 1,i_iLen, pMediaFile);
    if(iRet != i_iLen)
    {
        HTTP_FLV_LOGE("fwrite err %d iWriteLen%d\r\n",iRet,i_iLen);
        return iRet;
    }
    if(NULL != pMediaFile)
    {
        fclose(pMediaFile);//fseek(m_pMediaFile,0,SEEK_SET); 
    } 
    return 0;
}

/*****************************************************************************
-Fuction		: GetProcFlag
-Description	: HttpFlvServerIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HttpFlvClientIO :: GetProcFlag()
{
    return m_iHttpFlvClientIOFlag;//多线程竞争注意优化
}

/*****************************************************************************
-Fuction        : RemoveDir
-Description    : 
-Input          : 
-Output         : 
-Return         : len
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int HttpFlvClientIO::RemoveDir(const char *i_strPath) 
{
    int iRet = -1;
    iRet = ExecuteProgram("rm",i_strPath,"-rf");//只有用户点击删除配置才删配置目录
    return iRet;
}

/*****************************************************************************
-Fuction        : ExecuteProgram
-Description    : 
-Input          : 
-Output         : 
-Return         : len
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int HttpFlvClientIO::ExecuteProgram(const char* szExeAPPPath, const char*szExeParam1,const char*szExeParam2)
{
#ifndef _WIN32
	pid_t pid = fork(); // 创建子进程
	if (pid < 0)
	{
		HTTP_FLV_LOGE("fork err\r\n");
		return -1; // 返回-1表示失败
	}

	if (pid == 0)// 子进程
	{//fork() 成功，返回值在父进程中是子进程的 PID，在子进程中是0
        
        prctl(PR_SET_PDEATHSIG,SIGKILL);/*父进程退出时，会收到SIGKILL信号*/

		char* const argv[] = { (char *)szExeAPPPath, (char*)szExeParam1, (char*)szExeParam2, NULL};
		// 使用execlp执行程序，注意：成功后不会返回
		if (execvp(argv[0], argv) == -1)
		{
			HTTP_FLV_LOGE("execvp err\r\n");
			exit(EXIT_FAILURE); // execvp失败时退出子进程
		}
        // execvp 只有在失败时返回  
        HTTP_FLV_LOGW("execvp exit");  
        exit(1); 
	}
	// 父进程  
	return pid; // 父进程中返回子进程PID
#else
    char strParam[128];
    snprintf(strParam,sizeof(strParam),"%s %s",szExeParam1,szExeParam2);
    // 要执行的程序路径  
    LPCSTR applicationName = szExeAPPPath;  

    // 传递给新进程的参数  
    LPCSTR commandLineArguments = strParam;//"arg1 arg2 arg3";  // 可以传递多个参数  

    // 创建进程所需的结构体  
    STARTUPINFOA startupInfo;  
    PROCESS_INFORMATION processInfo;  

    // 初始化结构体  
    ZeroMemory(&startupInfo, sizeof(startupInfo));  
    startupInfo.cb = sizeof(startupInfo);  
    ZeroMemory(&processInfo, sizeof(processInfo));  

    // 创建进程  
    BOOL success = CreateProcessA(  
        applicationName,           // 应用程序名称  
        (LPSTR)commandLineArguments, // 命令行参数  
        NULL,                      // 进程句柄不可继承   
        NULL,                      // 线程句柄不可继承   
        FALSE,                    // 不继承句柄   
        0,                         // 无特殊标志  
        NULL,                      // 使用当前环境  
        NULL,                      // 使用当前目录  
        &startupInfo,             // 指向 STARTUPINFO 的指针  
        &processInfo               // 指向 PROCESS_INFORMATION 的指针  
    );  
    if (success) 
    {  
        HTTP_FLV_LOGE("CreateProcessA success\r\n");
        // 等待进程完成  
        WaitForSingleObject(processInfo.hProcess, INFINITE);  
        return reinterpret_cast<ULONG_PTR>(processInfo.hProcess); 
    } 
    HTTP_FLV_LOGE("CreateProcessA fail,warn\r\n");
    return -1;
#endif
}


/*****************************************************************************
-Fuction        : ExecuteProgram
-Description    : 
-Input          : 
-Output         : 
-Return         : len
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int HttpFlvClientIO::ExecuteProgram(const char* szExeAPPPath, const char*szExeParam, int nWindowType)
{
#ifndef _WIN32
	pid_t pid = fork(); // 创建子进程
	if (pid < 0)
	{
		HTTP_FLV_LOGE("fork err\r\n");
		return -1; // 返回-1表示失败
	}

	if (pid == 0)// 子进程
	{//fork() 成功，返回值在父进程中是子进程的 PID，在子进程中是0
	
        prctl(PR_SET_PDEATHSIG,SIGKILL);/*父进程退出时，会收到SIGKILL信号*/

		char* const argv[] = { (char *)szExeAPPPath, (char*)szExeParam, NULL };
		// 使用execlp执行程序，注意：成功后不会返回
		if (execvp(argv[0], argv) == -1)
		{
			HTTP_FLV_LOGE("execvp err\r\n");
			exit(EXIT_FAILURE); // execvp失败时退出子进程
		}
        // execvp 只有在失败时返回  
        HTTP_FLV_LOGW("execvp exit");  
        exit(1); 
	}
	// 父进程  
	return pid; // 父进程中返回子进程PID
#else
    // 要执行的程序路径  
    LPCSTR applicationName = szExeAPPPath;  

    // 传递给新进程的参数  
    LPCSTR commandLineArguments = szExeParam;//"arg1 arg2 arg3";  // 可以传递多个参数  

    // 创建进程所需的结构体  
    STARTUPINFOA startupInfo;  
    PROCESS_INFORMATION processInfo;  

    // 初始化结构体  
    ZeroMemory(&startupInfo, sizeof(startupInfo));  
    startupInfo.cb = sizeof(startupInfo);  
    ZeroMemory(&processInfo, sizeof(processInfo));  

    // 创建进程  
    BOOL success = CreateProcessA(  
        applicationName,           // 应用程序名称  
        (LPSTR)commandLineArguments, // 命令行参数  
        NULL,                      // 进程句柄不可继承   
        NULL,                      // 线程句柄不可继承   
        FALSE,                    // 不继承句柄   
        0,                         // 无特殊标志  
        NULL,                      // 使用当前环境  
        NULL,                      // 使用当前目录  
        &startupInfo,             // 指向 STARTUPINFO 的指针  
        &processInfo               // 指向 PROCESS_INFORMATION 的指针  
    );  
    if (success) 
    {  
        HTTP_FLV_LOGE("CreateProcessA success\r\n");
        // 等待进程完成  
        WaitForSingleObject(processInfo.hProcess, INFINITE);  
        return reinterpret_cast<ULONG_PTR>(processInfo.hProcess); 
    } 
    HTTP_FLV_LOGE("CreateProcessA fail,warn\r\n");
    return -1;
#endif
}

