$(function () {
    $('#flv-play').click(function () {
        console.log("textarea:", $('#flv-inText').val());
        startVideo($('#flv-inText').val());
    })
})

var flvPlayer = null;
var v = document.getElementById('flv-video');

function startVideo(url){
    flvPlayer = flvjs.createPlayer({
        type: 'flv',
        isLive: true,
        url: url
    });
    flvPlayer.attachMediaElement(v);
    flvPlayer.load();
    flvPlayer.play();
    flvPlayer.on("error", playError);
    flvPlayer.on("statistics_info",playInfo);
}
function stopVideo() 
{
    if (flvPlayer) 
    {
        flvPlayer.pause();// 暂停播放数据流
        flvPlayer.unload();// 取消数据流加载
        flvPlayer.detachMediaElement();// 将播放实例从节点中取出
        flvPlayer.destroy(); // 销毁播放实例
        flvPlayer = null;
    }
}
var decFrameNum=0;
function playInfo(res) 
{//当前播放信息，获取当前的网速
    decFrameNum=res.decodedFrames;
    if (this.lastDecodedFrame == 0) 
    {//this.lastDecodedFrame 是回调对象内定义的，如果回调对象内没有定义，则被自动定义
        this.lastDecodedFrame = res.decodedFrames;
        return;
    }
    if (this.lastDecodedFrame != res.decodedFrames) 
    {
        this.lastDecodedFrame = res.decodedFrames;
    } 
    else 
    {
        //console.log("lastDecodedFrame:"+this.lastDecodedFrame+"decodedFrames:"+res.decodedFrames);
        this.lastDecodedFrame = 0;
        if (this.player) 
        {
            console.log("player : reload");
            this.player.pause();
            this.player.unload();
            this.player.detachMediaElement();
            this.player.destroy();
            this.player= null;
            this.createPlayer(v, this.url);
        }
    }
}
function playError(errorType, errorDetail, errorInfo) 
{
    console.log("errorType:", errorType);
    console.log("errorDetail:", errorDetail);
    console.log("errorInfo:", errorInfo);
    //视频出错后销毁重新创建 'error' flvjs.Events.ERROR "statistics_info" flvjs.Events.STATISTICS_INFO
    if (this.player) 
    {
        console.log("playError : reload");
        this.player.pause();
        this.player.unload();
        this.player.detachMediaElement();
        this.player.destroy();
        this.player= null;
        this.createPlayer(v, this.url);
    }
}
var lastDecFrameNum=0;
var int1 = self.setInterval("clock()", 5000);
function clock() 
{
    if (v.buffered.length > 0) 
    {
        let end = v.buffered.end(0);//获取当前buffered值 videoElement
        let diff = end - v.currentTime;//获取buffered与currentTime的差值 videoElement
        if (diff >= 1.0) 
        {//如果差值大于等于1.0 手动跳帧 这里可根据自身需求来定 //
            //console.log("diff:"+diff+"currentTime:"+v.currentTime+"end:"+v.buffered.end(0));
            if(diff >= 10)
            {
                v.currentTime = v.buffered.end(0)-0.2;//手动跳帧,卡顿 (比自己当前时间前0.2s)
                lastDecFrameNum-=1;//重新回到页面，由于暂停恢复到启动，会触发定时器10s但是实际并不是流断了，所以此时不重连
            }
            else
            {
                v.playbackRate +=0.1;//采用加速方式追帧 //网页要放置后台，否则会暂停，flvPlayer.currentTime不走，差值很大，这个倍数不够
            }
        }
        else
        {
            v.playbackRate =1.0;
        }
    }
}
var frameCheck = self.setInterval("CheckFrame()", 20000);
function CheckFrame() 
{
    if (decFrameNum > lastDecFrameNum)
    {
        lastDecFrameNum = decFrameNum;
    }
    else
    {
        const currentDate = new Date();  
        const year = currentDate.getFullYear();  
        const month = String(currentDate.getMonth() + 1).padStart(2, '0'); // 月份从0开始  
        const day = String(currentDate.getDate()).padStart(2, '0');  
        const hours = String(currentDate.getHours()).padStart(2, '0');  
        const minutes = String(currentDate.getMinutes()).padStart(2, '0');  
        const seconds = String(currentDate.getSeconds()).padStart(2, '0');  

        const formattedDate = `${year}-${month}-${day} ${hours}:${minutes}:${seconds}`; 
        console.log(formattedDate+",20s无流，重连");
        stopVideo();
        startVideo($('#flv-inText').val());
        lastDecFrameNum=0;
        decFrameNum=0;
    }
}

