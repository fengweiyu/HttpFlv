/*
 * Copyright (C) 2020-2025 Hanson Yu All Rights Reserved.
 *
 * @author yu weifeng 
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import playerImp from './playerImp.js';


class player 
{
    constructor(type,url,needSoftDecode,needDownload,needMark,markText) 
    {
        this.m_PlayerImp = new playerImp();
        this.m_URL = url;
        this.m_SrcName = type;
        this.m_SrcType = ".pri";
        this.m_DstType = ".mp4";
        if(needSoftDecode)
            this.m_DstType = ".OriginalData";
        this.m_NeedDownload = needDownload;
        this.m_NeedMark = needMark;
        this.m_MarkText = markText;
    }
    getInitedFlag() 
    {//wasm初始化之后才可以进行转换播放操作
		return this.m_PlayerImp.getInitedFlag();
    }
    /**
     * 绑定媒体元素
     * @param {HTMLMediaElement} mediaElement - 用于播放的媒体元素（如 <video>）
     */
    attachMediaElement(mediaElement,canvasElement) 
    {               
        this.m_PlayerImp.attachMediaElement(mediaElement,canvasElement,this.m_MarkText,this.m_NeedMark,this.m_NeedDownload);
    }
    setTransCodec(iVideoEnable,strDstVideoEncodec,iAudioEnable,strDstAudioEncodec,strDstAudioSample) 
	{
		this.m_PlayerImp.setTransCodec(iVideoEnable,strDstVideoEncodec,iAudioEnable,strDstAudioEncodec,strDstAudioSample);  
	}
    play() 
    {
        if(".flv"==this.m_URL.substring(this.m_URL.length - 4))
        {
            this.m_SrcType = ".flv";
        }
        else if(".dat"==this.m_URL.substring(this.m_URL.length - 4)||".pri"==this.m_URL.substring(this.m_URL.length - 4))
        {
            this.m_SrcType = ".pri";
        }
        else
        {
            console.error('unsupport url data : ', this.m_URL); 
            return;            
        }

        this.m_PlayerImp.play();
        
        if("http"==this.m_URL.substring(0,4) || "https"==this.m_URL.substring(0,5))
        {
            this.HandleHttpReq(this.m_URL);
        }
        else if("ws"==this.m_URL.substring(0,2) || "wss"==this.m_URL.substring(0,3))
        {
            this.HandleWsURL(this.m_URL);
        }
        else
        {
            console.error('unsupport url : ', this.m_URL);  
        }
    }
    
    
    detachMediaElement() 
    {
        //this.m_PlayerImp.detachMediaElement()
    }
    destroy() 
    {
        this.m_PlayerImp.destroy();
    }
    async HandleHttpReq(url) 
    {  
        try 
        {  
            const response = await fetch(url);  
            if (!response.ok) 
            {  
                throw new Error(`HTTP error! status: ${response.status}`);  
            }  

            // 创建一个读取流  
            const reader = response.body.getReader();  
            let receivedLength = 0; // 收到的总字节数  
            while (true) 
            {  
                const { done, value } = await reader.read();  
                if (done) 
                {  
                    console.log('读取完成:', receivedLength);  
                    this.m_PlayerImp.close();
                    break; // 读取完成  
                }  
                receivedLength += value.byteLength; // 更新总字节数  
                this.m_PlayerImp.process(value,this.m_SrcName,this.m_SrcType,this.m_DstType); 
            }  
        } 
        catch (error) 
        {  
            console.error('获取 FLV 数据时出错:', error);  
        }  
    }
    HandleHttpURL(url) 
    {  
        const xhr = new XMLHttpRequest();  
        // 初始化请求  
        xhr.open('POST', url, true);  
    
        // 设置响应类型为 'text' 或 'arraybuffer'（取决于数据格式）  
        xhr.responseType = 'arraybuffer'; // 可以根据实际数据类型选择  

        xhr.onload = function () 
        {  
            if (xhr.status >= 200 && xhr.status < 300) 
            {  
                console.log('Response received:', xhr.response);  
            } 
            else 
            {  
                console.error('Request failed with status:', xhr.status);  
            }  
        };  

        // 处理进度更新  
        xhr.onprogress = this.HandleHttpData.bind(this);  

        xhr.onerror = function () 
        {  
            console.error('Request error');  
        };  
  
    } 
    HandleHttpData(event) 
    {
        if (event.lengthComputable) 
        {  
            const loaded = event.loaded;  
            const total = event.total;  
            console.log(`Loaded ${loaded} of ${total} bytes`);  
        } 
        else 
        {  
            // 数据流信息（未计算总字节时）  
            console.log(`Loaded ${event.loaded} bytes`);  
        }  

        // 这里可以处理实时接收到的数据  
        const chunk = xhr.response; // 或者 xhr.response（根据 responseType）  
        // 输入数据块进行转换
        this.m_PlayerImp.process(chunk,this.m_SrcName,this.m_SrcType,this.m_DstType); 
    }
    HandleWsURL(url) 
    {  
        // 创建 WebSocket 连接  
        const socket = new WebSocket(url);  

        // 连接成功时的回调  
        socket.onopen = function () 
        {  
            console.log('WebSocket connection opened.');  
            // 你可以在这里发送初始消息  
            // socket.send('Hello Server!');  
        };  

        //接收到消息时的回调  
        socket.onmessage = this.HandleWsBlob.bind(this);

        // 连接关闭时的回调  
        socket.onclose = this.HandleWsClose.bind(this);

        // 处理错误的回调  
        socket.onerror = function (error) 
        {  
            console.error('WebSocket Error:', error);  
        }; 
  
    } 
    HandleWsClose(event) 
    {
        console.log('WebSocket closed with code:', event.code);  
        if (event.reason) 
        {  
            console.log('WebSocket closed with Reason:', event.reason);  
        } 
        this.m_PlayerImp.close();
    }
    HandleWsBlob(event) 
    {
        const blob = event.data; // 获取接收到的 Blob 数据  
        // 使用 FileReader 将 Blob 转换为 ArrayBuffer  
        const reader = new FileReader();   
        reader.onload = this.HandleWsData.bind(this);
        reader.readAsArrayBuffer(blob); // 开始转换  
    }
    HandleWsData(event) 
    {
        // 这里可以处理实时接收到的数据  
        const arrayBuffer = event.target.result; // 获取 ArrayBuffer   
        // 输入数据块进行转换 
        this.m_PlayerImp.process(arrayBuffer,this.m_SrcName,this.m_SrcType,this.m_DstType); 
    }
}







// factory method
function createPlayer(mediaDataSource) 
{
    let mds = mediaDataSource;
    if (mds == null || typeof mds !== 'object') {
        console.log('MediaDataSource must be an javascript object!');
    }

    if (!mds.hasOwnProperty('url')) {
        console.log('MediaDataSource must has url field to indicate video file url!');
    }

    return new player(mds.type,mds.url,mds.needSoftDecode,mds.needDownload,mds.needMark,mds.markText);
}

// interfaces
let playerjs = {};

playerjs.createPlayer = createPlayer;

export default playerjs;