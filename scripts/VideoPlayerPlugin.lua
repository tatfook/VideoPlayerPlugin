--[[
Title: entry of videoPlugin
Author(s): hyz
Date: 2023/04/19
use the lib:
------------------------------------------------------------

NPL.load("(gl)Mod/VideoPlayerPlugin/VideoPlayerPlugin.lua");
local VideoPlayerPlugin = commonlib.gettable("Mod.VideoPlayerPlugin.VideoPlayerPlugin")

--创建一个播放器
local player = VideoPlayerPlugin:CreatePlayer(640,360)
--player:SetLooping(true)

--添加一个本地视频到播放列表，并播放
--player:PlayFile("D:/github/VideoPlayerPlugin/vlcTest.mp4")

-- 添加一个网络视频并播放
player:PlayUrl("https://media.w3.org/2010/05/sintel/trailer.mp4")

--显示Texture
local uiobj = ParaUI.CreateUIObject("container", "MyVideo", "_lt", 100, 100, 640, 360);
echo("virtual video texture file name is "..player:GetVideoRT())
uiobj.background = player:GetVideoRT();
uiobj:AttachToRoot();
------------------------------------------------------------
]]

local VideoPlayerPlugin = commonlib.inherit(commonlib.gettable("System.Core.ToolBase"), commonlib.gettable("Mod.VideoPlayerPlugin.VideoPlayerPlugin"))

NPL.load("(gl)Mod/VideoPlayerPlugin/PlayerController.lua");
local PlayerController = commonlib.gettable("Mod.VideoPlayerPlugin.PlayerController")

local callback_file = "(gl)Mod/VideoPlayerPlugin/VideoPlayerPlugin.lua"

function VideoPlayerPlugin:Init()
    if not self._isInited then
        self._isInited = true;

        self._parseCallbacks = {} --解析视频回调
        self._deviceAllCallbacks = {} --获取设备信息回调
        local OnFrameMove = function(timer)
            NPL.activate("Mod/VideoPlayerPlugin/VideoPlayerPlugin.dll",{cmd="OnFrameMove"})
        end
        
        if _VideoPlayerPluginTimer~=nil then
            _VideoPlayerPluginTimer:Change()
        end
        _VideoPlayerPluginTimer = commonlib.Timer:new({callbackFunc = OnFrameMove})
        _VideoPlayerPluginTimer:Change(1, 0);
        return;
    end 
end

--创建一个播放器
function VideoPlayerPlugin:CreatePlayer(width,height)
    return PlayerController.Create(width,height);
end

function VideoPlayerPlugin:GetPlayer(id)
    return PlayerController.GetPlayer(id);
end

function VideoPlayerPlugin:_ActiveDll(id,cmd,params)
    params = params or {}
    
    params.cmd = cmd
    params.id = id
    params.callback_file = callback_file
    
    NPL.activate("Mod/VideoPlayerPlugin/VideoPlayerPlugin.dll",params)
end

--解析本地视频
function VideoPlayerPlugin:MediaParseFile(filename,callback,timeout)
    self._parseCallbacks[filename] = callback
    local params = {
        timeout = timeout or 10,
        type = "MediaType.file",
        resource = filename,
    }
    self:_ActiveDll(nil,"MediaParse",params)
end

--解析网络视频
function VideoPlayerPlugin:MediaParseUrl(url,callback,timeout)
    self._parseCallbacks[url] = callback
    local params = {
        timeout = timeout or 10,
        type = "MediaType.network",
        resource = url,
    }
    self:_ActiveDll(nil,"MediaParse",params)
end


--获取所有播放设备
function VideoPlayerPlugin:DevicesAll(callback)
    table.insert(self._deviceAllCallbacks,callback);

    self:_ActiveDll(nil,"DevicesAll")
end

VideoPlayerPlugin:Init()
VideoPlayerPlugin:InitSingleton()

local function activate()
    if msg then
        local cmd = msg.cmd
        if cmd==nil then
            return
        end
        local id = msg.id
        local player = PlayerController.GetPlayer(id)
        if player then
            player:OnActiveCallback(msg)
        else
            if cmd=="DevicesAll" then
                local arr = msg;
                
                for k,v in pairs(VideoPlayerPlugin._deviceAllCallbacks) do
                    v(arr);
                    VideoPlayerPlugin._deviceAllCallbacks[k] = nil
                end
            elseif cmd=="MediaParse" then
                local arr = msg;
                
                if VideoPlayerPlugin._parseCallbacks[msg.url] then
                    VideoPlayerPlugin._parseCallbacks[msg.url](arr);
                    VideoPlayerPlugin._parseCallbacks[msg.url] = nil
                end
            end
        end
        
        msg = nil
    end
end
NPL.this(activate)