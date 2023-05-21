// VideoPlayerPlugin.cpp: 定义应用程序的入口点。
////
//#ifndef _HAS_AUTO_PTR_ETC
//#define _HAS_AUTO_PTR_ETC 1
//#endif // _HAS_AUTO_PTR_ETC

#ifdef _HAS_STD_BYTE
#undef _HAS_STD_BYTE
#endif
#define _HAS_STD_BYTE 0

#include <iostream>
#include <functional>
#include <map>
#include <thread>
#include "VideoPlayerPlugin.h"
#include "api/api.h"
#include "stdafx.h"
#include "INPLRuntime.h"
#include "INPLRuntimeState.h"
#include "IParaEngineCore.h"
#include "IParaEngineApp.h"
#include "IAttributeFields.h"
#include "NPLInterface.hpp"
#include "VideoHelper.h"

#ifdef WIN32
#define CORE_EXPORT_DECL    __declspec(dllexport)
#else
#define CORE_EXPORT_DECL
#endif

// forware declare of exported functions. 
#ifdef __cplusplus
extern "C" {
#endif
	CORE_EXPORT_DECL const char* LibDescription();
	CORE_EXPORT_DECL int LibNumberClasses();
	CORE_EXPORT_DECL unsigned long LibVersion();
	CORE_EXPORT_DECL ParaEngine::ClassDescriptor* LibClassDesc(int i);
	CORE_EXPORT_DECL void LibInit();
	CORE_EXPORT_DECL void LibActivate(int nType, void* pVoid);
	CORE_EXPORT_DECL void LibInitParaEngine(ParaEngine::IParaEngineCore* pCoreInterface);
	CORE_EXPORT_DECL void DllForceTerm();
#ifdef __cplusplus
}   /* extern "C" */
#endif

bool __isTestCpp = true;

ParaEngine::IParaEngineCore* g_pCoreInterface = NULL;
ParaEngine::IParaEngineCore* GetCoreInterface()
{
	return g_pCoreInterface;
}

NPL::INPLRuntimeState* g_nplRuntimeState = nullptr;
NPL::INPLRuntimeState* GetMainNPLRuntimeState()
{
	return g_nplRuntimeState;
}

CORE_EXPORT_DECL void LibInitParaEngine(ParaEngine::IParaEngineCore* pCoreInterface)
{
	g_pCoreInterface = pCoreInterface;
	__isTestCpp = false;
}


HINSTANCE Instance = NULL;

using namespace ParaEngine;

ClassDescriptor* VideoPlayerPlugin_GetClassDesc();
typedef ClassDescriptor* (*GetClassDescMethod)();

GetClassDescMethod Plugins[] =
{
	VideoPlayerPlugin_GetClassDesc,
};

#define VideoPlayer_CLASS_ID Class_ID(0x20230326, 0x03262023)

class VideoPlayerPluginDesc :public ClassDescriptor
{
public:
	void* Create(bool loading = FALSE)
	{
		//return VideoPlayers::getInstance();
		return NULL;
	}

	const char* ClassName()
	{
		return "IVideoPlayer";
	}

	SClass_ID SuperClassID()
	{
		return OBJECT_MODIFIER_CLASS_ID;
	}

	Class_ID ClassID()
	{
		return VideoPlayer_CLASS_ID;
	}

	const char* Category()
	{
		return "Video Player Plugin";
	}

	const char* InternalName()
	{
		return "Video Player Plugin";
	}

	HINSTANCE HInstance()
	{
		extern HINSTANCE Instance;
		return Instance;
	}
};

ClassDescriptor* VideoPlayerPlugin_GetClassDesc()
{
	static VideoPlayerPluginDesc s_desc;
	return &s_desc;
}

CORE_EXPORT_DECL const char* LibDescription()
{
	return "ParaEngine VideoPlayerPlugin Ver 1.0.0";
}

CORE_EXPORT_DECL unsigned long LibVersion()
{
	return 1;
}

CORE_EXPORT_DECL int LibNumberClasses()
{
	return sizeof(Plugins) / sizeof(Plugins[0]);
}

CORE_EXPORT_DECL ClassDescriptor* LibClassDesc(int i)
{
	if (i < LibNumberClasses() && Plugins[i])
	{
		return Plugins[i]();
	}
	else
	{
		return NULL;
	}
}


CORE_EXPORT_DECL void LibInit()
{
	Init();
}

CORE_EXPORT_DECL void DllForceTerm()
{
	DisposeAllPlayer();
}

CORE_EXPORT_DECL void LibActivate(int nType, void* pVoid)
{
	if (nType == ParaEngine::PluginActType_STATE)
	{
		g_nplRuntimeState = (NPL::INPLRuntimeState*)pVoid;
		NPL::INPLRuntimeState* pState = (NPL::INPLRuntimeState*)pVoid;
		const char* sMsg = pState->GetCurrentMsg();
		int nMsgLength = pState->GetCurrentMsgLength();

		NPLInterface::NPLObjectProxy tabMsg = NPLInterface::NPLHelper::MsgStringToNPLTable(sMsg);
		std::string callback_file = tabMsg["callback_file"];
		std::string cmd = tabMsg["cmd"];
		int32_t id = static_cast<int>((double)tabMsg["id"]);

		NPLInterface::NPLObjectProxy ret;
		ret["cmd"] = cmd;
		if(id>0){
			ret["id"] = (double)id;
		}

		if (cmd == "OnFrameMove") {
			OnFrameMove();
			return;
		}
		else if (cmd == "Create") {
			SetCallbackFile(callback_file);
			int width = static_cast<int>((double)tabMsg["width"]);
			int height = static_cast<int>((double)tabMsg["height"]);
			
			::PlayerCreate(id, width, height, 0, NULL, GetCallbackObj());
		}
		else if(cmd == "CreateAndPlayAsync") {
			SetCallbackFile(callback_file);
			int width = static_cast<int>((double)tabMsg["width"]);
			int height = static_cast<int>((double)tabMsg["height"]);
			std::string type = tabMsg["type"];
			std::string resource = tabMsg["resource"];

			std::thread t([pState,callback_file,cmd,id, width, height,type,resource]() {
				::PlayerCreate(id, width, height, 0, NULL, GetCallbackObj());
				::PlayerAdd(id, type.c_str(), resource.c_str());
				::PlayerPlay(id);

				if(callback_file!=""){
					NPLInterface::NPLObjectProxy ret;
					ret["cmd"] = cmd;
					NPL_Activate(pState, callback_file, ret);
				}
			});
			t.detach();
			return;
		}
		else if (cmd == "Dispose") {
			::PlayerDispose(id);
		}
		else if (cmd == "Open") {
			SetCallbackFile(callback_file);
			bool auto_start = tabMsg["auto_start"];
			int source_size = static_cast<int>((double)tabMsg["source_size"]);
			NPLInterface::NPLObjectProxy sourceArr = tabMsg["source"];
			std::vector<std::string> args;
			for (int i = 1; i <= source_size*4; i+=1) {
				args.push_back((std::string)sourceArr[i]);
			}

			const char** source = new const char* [args.size() + 1];
			for (size_t i = 0; i < args.size(); i++) {
				source[i] = args[i].c_str();
			}
			
			std::thread t([pState,callback_file,cmd,id, auto_start, source, source_size]() {
				::PlayerOpen(id, auto_start, source, source_size);

				if(callback_file!=""){
					NPLInterface::NPLObjectProxy ret;
					ret["cmd"] = cmd;
					NPL_Activate(pState, callback_file, ret);
				}
			});
			t.detach();
			return;
		}
		else if (cmd == "Play") {
			::PlayerPlay(id);
		}
		else if (cmd == "Pause") {
			::PlayerPause(id);
		}
		else if (cmd == "Stop") {
			::PlayerStop(id);
		}
		else if (cmd == "Next") {
			::PlayerNext(id);
		}
		else if (cmd == "Previous") {
			::PlayerPrevious(id);
		}
		else if (cmd == "JumpToIndex") {
			int32_t index = static_cast<int>((double)tabMsg["index"]);
			::PlayerJumpToIndex(id,index);
		}
		else if (cmd == "Seek") {
			int32_t position = static_cast<int>((double)tabMsg["position"]);
			::PlayerSeek(id, position);
		}
		else if (cmd == "SetVolume") {
			float volume = static_cast<float>((double)tabMsg["volume"]);
			::PlayerSetVolume(id, volume);
		}
		else if (cmd == "SetRate") {
			float rate = static_cast<float>((double)tabMsg["rate"]);
			::PlayerSetRate(id, rate);
		}
		else if (cmd == "Add") {
			std::string type = tabMsg["type"];
			std::string resource = tabMsg["resource"];
			::PlayerAdd(id, type.c_str(), resource.c_str());
		}
		else if (cmd == "Remove") {
			int32_t index = static_cast<int>((double)tabMsg["index"]);
			::PlayerRemove(id, index);
		}
		else if (cmd == "Insert") {
			int32_t index = static_cast<int>((double)tabMsg["index"]);
			std::string type = tabMsg["type"];
			std::string resource = tabMsg["resource"];
			::PlayerInsert(id, index, type.c_str(),resource.c_str());
		}
		else if (cmd == "Move") {
			int32_t initial_index = static_cast<int>((double)tabMsg["initial_index"]);
			int32_t final_index = static_cast<int>((double)tabMsg["final_index"]);
			::PlayerMove(id, initial_index, final_index);
		}
		else if (cmd == "TakeSnapshot") {
			std::string file_path = tabMsg["file_path"];
			int width = static_cast<int>((double)tabMsg["width"]);
			int height = static_cast<int>((double)tabMsg["height"]);
			bool isAsync = tabMsg["isAsync"];
			int time = static_cast<int>((double)tabMsg["time"]);
			if(isAsync){
				std::thread t([pState,callback_file,cmd,id, width, height,file_path,time]() {
					::PlayerTakeSnapshot(id, file_path.c_str(), width, height);

					if(callback_file!=""){
						NPLInterface::NPLObjectProxy ret;
						ret["cmd"] = cmd;
						ret["time"] = (double)time;
						NPL_Activate(pState, callback_file, ret);
					}
				});
				t.detach();
				return;
			}
			
			::PlayerTakeSnapshot(id, file_path.c_str(), width, height);
		}
		else if (cmd == "SetAudioTrack") {
			int32_t track = static_cast<int>((double)tabMsg["track"]);
			::PlayerSetAudioTrack(id, track);
		}
		else if (cmd == "GetAudioTrackCount") {
			int32_t count = ::PlayerGetAudioTrackCount(id);
			ret["count"] = (float)count;
		}
		else if (cmd == "MediaParse") {
			int32_t timeout = static_cast<int>((double)tabMsg["timeout"]);
			std::string type = tabMsg["type"];
			std::string resource = tabMsg["resource"];
			const char ** charPtr = ::MediaParse(type.c_str(), resource.c_str(), timeout);
			std::vector<std::string> strArr(charPtr, charPtr + 24);
			for (size_t i = 0; i < strArr.size(); i++) {
				ret[i + 1] = strArr[i];
			}
			ret["url"] = resource;
		}
		else if (cmd == "DevicesAll") {
			auto list = ::DevicesAll();
			for (int i = 0; i < list->size; i++) {
				auto info = list->device_infos[i];
				NPLInterface::NPLObjectProxy obj;
				obj["id"] = info.id;
				obj["name"] = info.name;
				ret[i + 1] = obj;
			}
		}
		if(callback_file!=""){
			NPL_Activate(pState, callback_file, ret);
		}
	}
	//OUTPUT_LOG("gtyhuij c++ LibActivate");
}

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
#else
void __attribute__((constructor)) DllMain()
#endif
{
	// TODO: dll start up code here
#ifdef WIN32
	Instance = hinstDLL;				// Hang on to this DLL's instance handle.
	return (TRUE);
#endif
}


