print("start test lua")


print("end test lua")

local aa = tf.VideoPlayer.test(100,2)
print(aa,"aaaa")

local infos = tf.VideoPlayer.MediaParse("MediaType.file","D:/github/VideoPlayerPlugin/vlcTest.mp4",10,{})
for i,v in pairs(infos) do 
	print("---info",i,v)
end

local list = tf.VideoPlayer.DevicesAll({})
for k=1,#list do 
	local v = list[k]
	print("k,v",k,"id:"..v.id,"name:"..v.name)
end

tf.VideoPlayer.PlayerCreate(1,1280,720,3,{"1","22","ccc"})
--tf.VideoPlayer.PlayerAdd(1, "MediaType.file", "D:/github/VideoPlayerPlugin/vlcTest.mp4");
--tf.VideoPlayer.PlayerPlay(1)
tf.VideoPlayer.PlayerOpen(1,true,{"MediaType.file","D:/github/VideoPlayerPlugin/vlcTest.mp4","0","100"});

tf.VideoPlayer.setCallback_onPosition(function ( id,state )
	-- print("state.position","lua","position:",state.position,"duration:",state.duration,"is_playing:",state.is_playing,"is_completed:",state.is_completed)
end)

__onVideoCallback = function(callbackType,id)
	if tf then
		if tf.__stateMap then
			
			local state = tf.__stateMap[id]
			if state then
				print("position:",state.position,"duration:",state.duration,"is_playing:",state.is_playing,"is_completed:",state.is_completed)
			else
				print("state is nil")
			end
		else
			print("tf.__stateMap is nil")
		end
	else
		print("tf is nil")
	end
end
