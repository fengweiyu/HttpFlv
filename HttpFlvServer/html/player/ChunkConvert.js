 

self.Module ={
	onRuntimeInitialized : async function () {
		console.log('Wasm module initialized');
        // 写入自定义字体文件 
        const fontResponse = await fetch('msyh.ttf');  
        const fontArrayBuffer = await fontResponse.arrayBuffer();
        Module.FS.writeFile('msyh.ttf', new Uint8Array(fontArrayBuffer)); 
	}
	
}

var oConvert = null;

class convert {
    constructor() {
        this.m_MediaSource = new MediaSource();
        this.m_MediaElement = null;
        this.m_SrcName = null;
        this.m_SrcType = null;
        this.m_DstName = null;
        this.m_SrcBuffer = null;
        this.m_SourceBuffer = null;
        this.m_IsConverting = false;
        this.m_IsPaused = false;
        this.m_Queue = []; // 数据队列，用于存储尚未处理的数据块
        this.m_CurrentOffset = 0;
        oConvert = this;
		
		this.m_AccumulatedBuffer = new Uint8Array(0); // 累计缓冲区
		this.m_AccumulatedSize = 0; // 累计大小
        this.m_DownBuffer = new Uint8Array(0); // 累计缓冲区
        this.m_BufferQueue = []; // 数据队列，用于存储转换后的帧数据与帧信息
    }

    /**
     * 绑定媒体元素
     * @param {HTMLMediaElement} mediaElement - 用于播放的媒体元素（如 <video>）
     */
    attachMediaElement(mediaElement) {
        this.m_MediaElement = mediaElement;
        this.m_MediaElement.src = URL.createObjectURL(this.m_MediaSource);
        //this.m_MediaSource.addEventListener('sourceopen', this.MediaSourceOpen.bind(this));
    }

    /**
     * 解绑媒体元素
     */
    detachMediaElement() {
        if (this.m_MediaElement) {
            this.m_MediaElement.src = '';
            this.m_MediaElement = null;
        }
    }

    /**
     * 处理 MediaSource 打开事件
     */
    MediaSourceOpen() {
        //this.m_SourceBuffer = this.m_MediaSource.addSourceBuffer('video/mp4; codecs="avc1.42E01E, mp4a.40.2"');
        //this.m_SourceBuffer.addEventListener('updateend', this.handleUpdateEnd.bind(this));
        //this.processQueue(); // 开始处理队列中的数据
    }

    /**
     * 处理 SourceBuffer 更新结束事件
     */
    handleUpdateEnd() {
        //console.log("handleUpdateEnd",this.m_BufferQueue.length);
        /*if (this.m_Queue.length > 0 && !this.m_IsPaused) {
            //this.processQueue(); // 继续处理队列中的数据
        }*/
        if (this.m_BufferQueue.length > 0) 
        {
            const dequeuedObject = this.m_BufferQueue.shift(); // 从队列中取出一个数据块
            try {
                this.m_SourceBuffer.appendBuffer(dequeuedObject.data);
            } catch (error) {
                console.error('Failed to append buffer:', error);
                //this.m_Queue.unshift({ data, srcType, dstType }); // 如果失败，将数据块重新放回队列
            }
        }

    }

    /**
     * 外部接口：输入数据块
     * @param {ArrayBuffer} dataChunk - 数据块
     * @param {string} srcType - 源文件类型（如 ".webm"）
     * @param {string} dstType - 目标文件类型（如 ".mp4"）
     */
    inputDataChunk(dataChunk, srcType, dstType) {
		// 如果累计大小达到 1MB，触发下载操作
		const uint8Chunk = new Uint8Array(dataChunk);
		this.m_AccumulatedBuffer = this.concatBuffers(this.m_AccumulatedBuffer, uint8Chunk);
		this.m_AccumulatedSize += uint8Chunk.length;
		// console.log("AAAAAAAAAA",this.m_AccumulatedSize,1 * 1024 * 1024);
		if (this.m_AccumulatedSize >= 256 * 1024) {
		    //DownloadMedia(this.m_AccumulatedBuffer,'test.pri');
		 	this.m_AccumulatedBuffer = new Uint8Array(0); // 清空累计缓冲区
		 	this.m_AccumulatedSize = 0; // 重置累计大小
		}
		// console.clear();//清除日志
		
        if (!dataChunk || !srcType || !dstType) {
            console.error('Invalid input data');
            return;
        }
		
        // 将数据块加入队列
        this.m_Queue.push({ data: dataChunk, srcType, dstType });

        // 如果没有在转换中，则开始处理队列
        if (!this.m_IsConverting && !this.m_IsPaused) {
            this.processQueue();
        }
    }
	
	/**
	     * 合并两个 Uint8Array
	     * @param {Uint8Array} buffer1 - 第一个缓冲区
	     * @param {Uint8Array} buffer2 - 第二个缓冲区
	     * @returns {Uint8Array} - 合并后的缓冲区
	     */
	concatBuffers(buffer1, buffer2) {
		const result = new Uint8Array(buffer1.length + buffer2.length);
		result.set(buffer1, 0);
		result.set(buffer2, buffer1.length);
		return result;
	}
	GetMediaDstEnc()
	{
		var bufDstMaxLen=10;
		let bufDstVideoEnc = Module._malloc(bufDstMaxLen);
		let bufDstAudioEnc = Module._malloc(bufDstMaxLen);
		var ret = Module._GetEncodeType(bufDstVideoEnc,bufDstMaxLen,bufDstAudioEnc,bufDstMaxLen);
		let VideoArray = new Uint8Array(bufDstMaxLen);
		VideoArray.set(new Uint8Array(Module.HEAPU8.buffer, bufDstVideoEnc, 6)); // 将C中的数组拷贝到JS数组中
		let AudioArray = new Int8Array(bufDstMaxLen);
		AudioArray.set(new Uint8Array(Module.HEAP8.buffer, bufDstAudioEnc,6)); // 将C中的数组拷贝到JS数组中
		const stringVideo = String.fromCharCode.apply(null, VideoArray);  
		const stringAudio = String.fromCharCode.apply(null, AudioArray);  
		Module._free(bufDstVideoEnc);
		Module._free(bufDstAudioEnc);
		return [stringVideo,stringAudio];
	}
    /**
     * 处理队列中的数据块
     */
    processQueue() {
        if (this.m_IsConverting || this.m_IsPaused || this.m_Queue.length === 0) {
            return;
        }

        this.m_IsConverting = true;

        const { data, srcType, dstType } = this.m_Queue.shift(); // 从队列中取出一个数据块
        this.convert(data, srcType, dstType);
        var convertedChunk = null; 
        if (this.m_BufferQueue.length > 1) 
        {
            const dequeuedObject = this.m_BufferQueue.shift(); // 从队列中取出一个数据块
            convertedChunk=dequeuedObject.data;
        }
        if (convertedChunk) {
            if(!this.m_SourceBuffer)
            {
                var VideoCodec=null;
                var AudioCodec=null;
                let mMedia = this.GetMediaDstEnc();
                let v = mMedia[0];
                let a = mMedia[1];
                if (v.includes("h264"))//video/webm; codecs="avc1.64001E"：用于 WebM 容器中的 H.264 视频（通常则使用 VP8 或 VP9）。
                {//video/mp4; codecs="avc1.64001E"：用于 MP4 容器中的 H.264 视频。
                    VideoCodec="avc1.42E01E";//avc1.42E01E
                }//video/mp2t; codecs="avc1.42E01E"：用于 MPEG-TS 容器中的 H.264 视频
                else if (v.includes("h265"))
                {//video/mp4; codecs="hevc"：用于 MPEG-4 容器格式中的 H.265 视频。
                    VideoCodec="hvc1.1.6.L93.B0";
                }//用于传输流（如 TS 或 MPEG-TS）编码中的 H.265 视频。
                if (a.includes("aac"))
                {
                    AudioCodec="mp4a.40.2";//audio/mp4; codecs="mp4a.40.2"：这是一般用于 MP4 容器中的 AAC 音频格式。
                }//audio/aac：用于裸 AAC 数据。
                else if(a.includes("g711a"))
                {
                    AudioCodec="alaw";//audio/mp4; codecs="law"：表示使用 G.711 A-law 编码的音频数据。
                }//audio/g711：用于直接表示 G.711 编码的数据。
                if(null != VideoCodec && null !=AudioCodec)
                {
                    this.m_SourceBuffer = this.m_MediaSource.addSourceBuffer('video/mp4; codecs="'+VideoCodec+','+AudioCodec+'"');
                    this.m_MediaElement.style.display = 'block'; // 显示视频 = 'none'; // 隐藏视频  
                    this.m_SourceBuffer.addEventListener('updateend', this.handleUpdateEnd.bind(this));
                }
                else if(null != VideoCodec)
                {
                    this.m_SourceBuffer = this.m_MediaSource.addSourceBuffer('video/mp4; codecs="'+VideoCodec+'"');
                    this.m_MediaElement.style.display = 'block'; // 显示视频 = 'none'; // 隐藏视频  
                }
                else if(null != AudioCodec)
                {
                    this.m_SourceBuffer = this.m_MediaSource.addSourceBuffer('video/mp4; codecs="'+AudioCodec+'"');
                    this.m_MediaElement.style.display = 'block'; // 显示视频 = 'none'; // 隐藏视频  
                }
                else
                {
                    console.log('GetMediaDstEnc err v '+VideoCodec+' a '+AudioCodec);    
                }
            }
            
            if( this.m_DownBuffer==null)
                this.m_DownBuffer=convertedChunk;
            else
            {
                //this.m_DownBuffer=this.appendTypedArray(this.m_DownBuffer,convertedChunk);
                if(this.m_DownBuffer.length>=4*512*1024)
                {
                    //DownloadMedia(this.m_DownBuffer,"666666.mp4");
                    //this.m_DownBuffer=null;
                }
            }
            try {
                this.m_SourceBuffer.appendBuffer(convertedChunk);
            } catch (error) {
                console.error('Failed to append buffer:', error);
                //this.m_Queue.unshift({ data, srcType, dstType }); // 如果失败，将数据块重新放回队列
            }
            /*try {
                this.m_SourceBuffer.appendBuffer(convertedChunk);
            } catch (error) {
                console.error('Failed to append buffer:', error);
                //this.m_Queue.unshift({ data, srcType, dstType }); // 如果失败，将数据块重新放回队列
            }*/

            //Module._Clean();
            //this.convert(data, srcType, dstType);
        } else {
            //console.error('Failed to convert chunk');
        }

        this.m_IsConverting = false;

        // 如果队列中还有数据，继续处理
        if (this.m_Queue.length > 0 && !this.m_IsPaused) {
            this.processQueue();
        }
    }
    appendTypedArray(a, b) 
    {  
        // 创建一个新的 TypedArray，长度为 a 和 b 的长度之和  
        const result = new (a.constructor)(a.length + b.length);  
        
        // 将 a 的内容复制到新数组中  
        result.set(a, 0);  
        
        // 将 b 的内容复制到新数组中，从 a 的末尾开始  
        result.set(b, a.length);  
        
        return result;  
    } 
    /**
     * 转换数据块
     * @param {ArrayBuffer} srcBuffer - 源数据
     * @param {string} srcType - 源文件类型
     * @param {string} dstType - 目标文件类型
     * @returns {Uint8Array|null} - 转换后的数据块
     */
    convert(srcBuffer, srcType, dstType) {
        if (!srcBuffer || srcBuffer.byteLength === 0) {
            console.error('Invalid source buffer');
            return;
        }

        const arrDstType = intArrayFromString(dstType).concat(0);
        const bufDstType = Module._malloc(arrDstType.length);
        Module.HEAPU8.set(arrDstType, bufDstType);

        const arrSrcType = intArrayFromString(srcType).concat(0);
        const bufSrcType = Module._malloc(arrSrcType.length);
        Module.HEAPU8.set(arrSrcType, bufSrcType);

        const uint8View = new Uint8Array(srcBuffer);
        const bufSrcFrame = Module._malloc(uint8View.byteLength);
        Module.HEAPU8.set(uint8View, bufSrcFrame);

        Module._InputData(bufSrcFrame, uint8View.byteLength, bufSrcType, bufDstType);

        const bufDstMaxLen = uint8View.byteLength + (10 * 1024 * 1024);
        const bufDstFrame = Module._malloc(bufDstMaxLen);
        const bufDstInfoMaxLen = 80;
        const bufDstInfo = Module._malloc(bufDstInfoMaxLen);
        let ret = 0;
        let len = 0;

        do {
            ret = Module._GetData(bufDstFrame, bufDstMaxLen,bufDstInfo,bufDstInfoMaxLen);
            if(ret <= 0)
            {
                break;                   
            }
            len = ret;
            const convertedChunk = new Uint8Array(len);
            convertedChunk.set(new Uint8Array(Module.HEAPU8.buffer, bufDstFrame, len));
            const convertedInfo = new Int32Array(bufDstInfoMaxLen);
            convertedInfo.set(new Int32Array(Module.HEAP32.buffer, bufDstInfo, bufDstInfoMaxLen));
            const haveKeyFrame = convertedInfo[0]; // 第一个元素表示是否有i帧，第二个元素是开始时间，第三个元素是持续时间
            const startTime = convertedInfo[1]; // 
            const durationTime = convertedInfo[2]; //
            const videoCnt = convertedInfo[3]; // 
            const audioCnt = convertedInfo[4]; //
            var absTimeHigh = BigInt(convertedInfo[5]); // Convert to BigInt  
            var absTimeLow = BigInt(convertedInfo[6]);  // Convert to BigInt  
            const absTimeS = convertedInfo[7]; //
            // Combine the two values  
            const absTime = (absTimeHigh << BigInt(32)) | absTimeLow; // Use | for combined value  
            // 创建 Date 对象  
            const absTimeNumber =absTimeS*1000;//Number(absTime);
            const date = new Date(absTimeNumber);  //absTime.toString()
            // 格式化时间  
            const formattedDate = date.toLocaleString('zh-CN', {  
                year: 'numeric',  
                month: '2-digit',  
                day: '2-digit',  
                hour: '2-digit',  
                minute: '2-digit',  
                second: '2-digit',  
                //fractionalSecondDigits: 3, // 显示毫秒  
                hour12: false, // 24小时制
            });  
            this.m_BufferQueue.push({ data: convertedChunk, haveKeyFrame,startTime,durationTime});
            console.log('convertedChunk haveKeyFrame '+haveKeyFrame+' startTime '+startTime+' durationTime '+durationTime+' videoCnt '+videoCnt+' audioCnt '+audioCnt+' absTime '+formattedDate); 
        } while (1);


        Module._free(bufDstType);
        Module._free(bufSrcType);
        Module._free(bufSrcFrame);
        Module._free(bufDstFrame);
        Module._free(bufDstInfo);
    }

    /**
     * 暂停转换
     */
    pause() {
        this.m_IsPaused = true;
    }

    /**
     * 恢复转换
     */
    resume() {
        this.m_IsPaused = false;
        this.processQueue(); // 恢复时继续处理队列
    }

    /**
     * 清空队列并停止转换
     */
    stop() {
        this.m_Queue = [];
        this.m_IsPaused = true;
        this.m_IsConverting = false;
        Module._Clean();
    }

    /**
     * 销毁实例
     */
    destroy() {
        this.stop();
        this.detachMediaElement();
        this.m_MediaSource = null;
        this.m_SourceBuffer = null;
    }
}
