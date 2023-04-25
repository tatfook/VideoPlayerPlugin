### 功能：
在NPLRuntime中播放视频

### 主要用法：
``` lua
NPL.load("(gl)Mod/VideoPlayerPlugin/VideoPlayerPlugin.lua");
local VideoPlayerPlugin = commonlib.gettable("Mod.VideoPlayerPlugin.VideoPlayerPlugin")

--创建一个播放器
local player = VideoPlayerPlugin:CreatePlayer(640,360)

--添加一个本地视频到播放列表，并播放
player:PlayFile("D:/github/VideoPlayerPlugin/vlcTest.mp4")

-- --添加一个网络视频并播放
-- player:PlayUrl("https://media.w3.org/2010/05/sintel/trailer.mp4")

--显示Texture
local videoFrame = player:CreateVideoUIObject("my_video", "_lt", 100, 100, 640, 360)
videoFrame.zorder = -1;
videoFrame:AttachToRoot();

```

### 主要使用的第三方库：
#### vlc: https://github.com/videolan/vlc
#### libvlcpp: https://github.com/videolan/libvlcpp
#### dart_vlc_core: https://github.com/alexmercerind/dart_vlc/tree/master/core