--[[
Title: 一个播放器控制器
Author(s): hyz
Date: 2023/04/19
use the lib:
------------------------------------------------------------
NPL.load("(gl)Mod/VideoPlayerPlugin/PlayerController.lua");
local PlayerController = commonlib.gettable("Mod.VideoPlayerPlugin.PlayerController")

------------------------------------------------------------
]]

local PlayerController = commonlib.inherit(nil, commonlib.gettable("Mod.VideoPlayerPlugin.PlayerController"))

NPL.load("(gl)Mod/VideoPlayerPlugin/VideoPlayerPlugin.lua");
local VideoPlayerPlugin = commonlib.gettable("Mod.VideoPlayerPlugin.VideoPlayerPlugin")

PlayerController._playerMap = PlayerController._playerMap or {}

local _id = 0;--播放器id
function PlayerController.Create(width,height)
    if width==nil or height==nil then
        return nil
    end
    _id = _id + 1
    local player = PlayerController:new({id = _id,width=width,height=height})
    PlayerController._playerMap[_id] = player
    return player
end

function PlayerController.GetPlayer(id)
    return PlayerController._playerMap[id]
end

function PlayerController:ctor()
    local params = {
        width = self.width,
        height = self.height,
    }
    VideoPlayerPlugin:_ActiveDll(self.id,"Create",params)
    self.state = {medias = {}}
    self:BindFrameToTexture("_VideoRT_"..self.id)
end

function PlayerController:_SetPlayerState(info)
    local _state = self.state
    if _state==nil then
        _state = {}
        self.state = _state
    end
    for k,v in pairs(info) do
        _state[k] = v
    end
    _state.medias = nil;
    if info.medias then
        _state.medias = {}
        _state.medias.playlist_mode = info.medias.playlist_mode
        for i=1,#info.medias do
            _state.medias[i] = {}
            for k,v in pairs(info.medias[i]) do
                _state.medias[i][k] = v
            end
        end
    end

    return _state
end

--video renderTexture
function PlayerController:GetVideoRT()
    return self._textureName
end

function PlayerController:BindFrameToTexture(textureName)
    self._textureName = textureName
    if self._textureAsset then
        self._textureAsset:Release()
    end
    self._textureAsset = ParaAsset.LoadTexture(self._textureName, self._textureName, 0); 
	self._textureAsset:SetSize(self.width,self.height);
	local attr = self._textureAsset:GetAttributeObject()
    attr:SetField("IsRGBA",true)
end

function PlayerController:CreateVideoUIObject(uiname,align,left,top,width,height)
    local uiobj = ParaUI.GetUIObject(uiname)
	uiobj = ParaUI.CreateUIObject("container", uiname,align,left,top,width,height);
	uiobj.background = self:GetVideoRT();
	return uiobj
end

--[[
    index = 0,--在播放列表的索引
	is_playing = false, --是否正在播放
	is_valid = false, --是否可用
	is_seekable = false, --是否可以快进
	is_completed = false, --是否播放完成
	position = 1651, --当前进度，单位毫秒
	duration = 329984, --总进度，单位毫秒
	volume = 0, --音量
	rate = 0, --
	is_playlist = false, --是否是操作播放列表引起的回调（如add、remove等）
	is_started = false, --是否开始播放了
    medias= --播放列表
    {
        {
        location="file:///D:/github/VideoPlayerPlugin/vlcTest.mp4",
        media_type="MediaType.file",
        resource="D:/github/VideoPlayerPlugin/vlcTest.mp4",
        start_time="",
        stop_time="" 
        },
        playlist_mode=0 
    }, 
]]
function PlayerController:GetState()
    return self.state
end

function PlayerController:SetLooping(bLoop)
    self.bLoop = bLoop;
end

function PlayerController:IsLooping()
    return self.bLoop;
end

--开始播放
function PlayerController:Play()
    VideoPlayerPlugin:_ActiveDll(self.id,"Play")
end

--暂停播放
function PlayerController:Pause()
    VideoPlayerPlugin:_ActiveDll(self.id,"Pause")
end

--恢复播放
function PlayerController:Resume()
    VideoPlayerPlugin:_ActiveDll(self.id,"Play")
end

--停止播放
function PlayerController:Stop()
    VideoPlayerPlugin:_ActiveDll(self.id,"Stop")
end

--停止播放并注销
function PlayerController:Dispose()
    VideoPlayerPlugin:_ActiveDll(self.id,"Dispose")
end

--跳转到某个进度
function PlayerController:Seek(position)
    VideoPlayerPlugin:_ActiveDll(self.id,"Seek",{
        position = position
    })
end

--设置音量
function PlayerController:SetVolume(volume)
    VideoPlayerPlugin:_ActiveDll(self.id,"SetVolume",{
        volume = volume
    })
end

--设置帧率
function PlayerController:SetRate(rate)
    VideoPlayerPlugin:_ActiveDll(self.id,"SetRate",{
        rate = rate
    })
end

--播放列表中的下一个
function PlayerController:Next()
    VideoPlayerPlugin:_ActiveDll(self.id,"Next")
end

--播放列表中的上一个
function PlayerController:Previous()
    VideoPlayerPlugin:_ActiveDll(self.id,"Previous")
end

--播放列表中指定的一个
function PlayerController:JumpToIndex(index)
    if index<0 or index>=#self.state.medias then
        return
    end
    VideoPlayerPlugin:_ActiveDll(self.id,"JumpToIndex",{
        index = index
    })
end

function PlayerController:_findIndex(urlOrPath)
    for i=1,#self.state.medias do
        local v = self.state.medias[i];
        if v.resource==urlOrPath then
            return i-1
        end
    end
    return nil
end

function PlayerController:PlayFile(filename)
    return self:AddFileToPlayList(filename,nil,true)
end

function PlayerController:PlayUrl(url)
    return self:AddUrlToPlayList(url,nil,true)
end

--往视频列表里添加一条本地视频
function PlayerController:AddFileToPlayList(filename,index,autoPlay)
    do
        local _index = self:_findIndex(filename)
        if _index then
            if autoPlay then
                self:JumpToIndex(_index)
                self:Play()
            end
            return
        end
    end
    local cmd = "Insert"
    if index==nil then
        cmd = "Add"
    end
    VideoPlayerPlugin:_ActiveDll(self.id,cmd,{
        type = "MediaType.file",
        resource = filename,
        index = index
    })

    if autoPlay then
        if index then
            self:JumpToIndex(index)
        end
        self:Play()
    end
    return index
end

--往视频列表里添加一条网络视频
function PlayerController:AddUrlToPlayList(url,index,autoPlay)
    do
        local _index = self:_findIndex(url)
        if _index then
            if autoPlay then
                self:JumpToIndex(_index)
                self:Play()
            end
            return _index
        end
    end
    local cmd = "Insert"
    if index==nil then
        cmd = "Add"
    end
    VideoPlayerPlugin:_ActiveDll(self.id,cmd,{
        type = "MediaType.network",
        resource = url,
        index = index
    })
    if autoPlay then
        if index then
            self:JumpToIndex(index)
        end
        self:Play()
    end
    return index
end

--从视频列表里移除一条
function PlayerController:RemoveFromPlayList(index)
    VideoPlayerPlugin:_ActiveDll(self.id,"Remove",{
        index = index
    })
end

--从视频列表改变一条的顺序
function PlayerController:MoveOrderFromPlayList(initial_index,final_index)
    VideoPlayerPlugin:_ActiveDll(self.id,"Move",{
        initial_index = initial_index,
        final_index = final_index,
    })
end

--对视频进行截屏
function PlayerController:TakeSnapshot(file_path,width,height,callback)
    local time = os.clock()
    self._captureCallbacks = self._captureCallbacks or {}
    self._captureCallbacks[time] = callback
    VideoPlayerPlugin:_ActiveDll(self.id,"TakeSnapshot",{
        file_path = file_path,
        width = width,
        height = height,
        isAsync = true,
        time = time
    })
end

--设置音轨
function PlayerController:SetAudioTrack(track)
    VideoPlayerPlugin:_ActiveDll(self.id,"SetAudioTrack",{
        track = track
    })
end

--获取视频的音轨
function PlayerController:GetAudioTrackCount(callback)
    self._audioTracksCallback = callback
    VideoPlayerPlugin:_ActiveDll(self.id,"GetAudioTrackCount")
end

--刷新帧数据到rendertexture
function PlayerController:_OnUpdateFrame(frame,width,height)
    -- local uiObj = ParaUI.GetUIObject(self._textureName);
    -- if uiObj then
    --     uiObj:SetField("TempVideoWidth",width)
    --     uiObj:SetField("TempVideoHeight",height)
    --     uiObj:SetField("TempVideoFramePointer",frame)
    --     uiObj:CallField("ApplyVideoFrame")
    -- else
    --     self:Stop()
    -- end
    if not self._textureAsset then
        return
    end
    local attr = self._textureAsset:GetAttributeObject()
    attr:SetField("TextureFramePointer",frame);
end

function PlayerController:OnActiveCallback(msg)
    local cmd = msg.cmd
    if cmd=="GetAudioTrackCount" then
        local count = msg.count;
        print("------GetAudioTrackCount",count)
        
        if self._audioTracksCallback then
            self._audioTracksCallback(count);
            self._audioTracksCallback = nil
        end
    elseif cmd=="TakeSnapshot" then
        local time = msg.time
        if self._captureCallbacks and self._captureCallbacks[time] then
            self._captureCallbacks[time]();
            self._captureCallbacks[time] = nil;
        end
    elseif cmd=="play_pallback" then
        local callbackType = msg.callbackType;
        local id = msg.id;
        if callbackType and id then
            msg.id = nil 
            msg.callbackType = nil
            local _state = self:_SetPlayerState(msg)
            if callbackType=="onPlayPauseStop" or callbackType=="onPosition" or callbackType=="onComplete"
                or callbackType=="onVolume" or callbackType=="onRate" or callbackType=="onOpen"
            then
                GameLogic.GetFilters():apply_filters('video_play_callback',callbackType,id,_state)
                if(self:IsLooping() and callbackType=="onComplete") then
                    self:Seek(0);
                end
            elseif callbackType=="onVideoDimensions" then
                GameLogic.GetFilters():apply_filters('video_play_callback',callbackType,id,_state,msg.width,msg.height)
            elseif callbackType=="onVideoFrame" then
                self:_OnUpdateFrame(msg.frame,msg.width,msg.height);
                -- GameLogic.GetFilters():apply_filters('video_play_callback',callbackType,id,_state,msg.frame,msg.width,msg.height)
            elseif callbackType=="onError" then
                print("-----------onError",msg.error)
                GameLogic.GetFilters():apply_filters('video_play_callback',callbackType,id,_state,msg.error)
            end

            if callbackType=="onPosition" then
                -- print("onPosition",msg.position,msg.duration)
                -- GameLogic.AddBBS(msg.id,"id:"..tostring(msg.id)..",pos:"..tostring(msg.position))
                -- print("---------onPosition","id:"..tostring(msg.id)..",pos:"..tostring(msg.position))
            end
        end
    end
end