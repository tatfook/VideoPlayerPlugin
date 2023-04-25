#include "VideoHelper.h"
#include <mutex>
#include <functional>
#include "api/api.h"
#include "stdafx.h"
#include "INPLRuntime.h"
#include "INPLRuntimeState.h"
#include "IParaEngineCore.h"
#include "IParaEngineApp.h"
#include "IAttributeFields.h"
#include "NPLInterface.hpp"


std::mutex _performMutex;
std::vector<std::function<void()>> _functionsToPerform;
::EventCallbackStruct _callbackObj;

::EventCallbackStruct& GetCallbackObj() {
	return _callbackObj;
}

void OnFrameMove()
{
	if (!_functionsToPerform.empty()) {
		_performMutex.lock();
		auto temp = std::move(_functionsToPerform);
		_performMutex.unlock();

		for (const auto& function : temp) {
			function();
		}
	}
}

void performFunctionInMainThread(std::function<void()> function)
{
	std::lock_guard<std::mutex> lock(_performMutex);
	_functionsToPerform.push_back(std::move(function));
}

std::string _callbackFile;
void SetCallbackFile(std::string file) {
	_callbackFile = file;
}

std::string GetCallbackFile() {
	return _callbackFile;
}

std::map<int, NPLInterface::NPLObjectProxy*> _stateMap;
std::map<int, NPLInterface::NPLObjectProxy*> _playlistMap;

NPLInterface::NPLObjectProxy& CreateCallbackProxy(int32_t id) {
	if (_stateMap.find(id) == _stateMap.end()) {
		NPLInterface::NPLObjectProxy* p = new NPLInterface::NPLObjectProxy();
		_stateMap.insert(std::make_pair(id, p));
	}
	NPLInterface::NPLObjectProxy& data = *_stateMap[id];
	data["id"] = (double)id;
	data["cmd"] = "play_pallback";
	return data;
}

NPLInterface::NPLObjectProxy& CreateCallbackProxy(int32_t id, ::PlayerState* state) {
	NPLInterface::NPLObjectProxy& data = CreateCallbackProxy(id);

	data["index"] = (double)state->index();
	data["is_playing"] = state->is_playing();
	data["is_valid"] = state->is_valid();
	data["is_seekable"] = state->is_seekable();
	data["is_completed"] = state->is_completed();
	data["position"] = (double)state->position();
	data["duration"] = (double)state->duration();
	data["volume"] = (double)state->volume();
	data["rate"] = (double)state->rate();
	data["is_playlist"] = state->is_playlist();
	data["is_started"] = state->is_started();

	Playlist* list = state->medias();
	int addr = (int)list;
	if (_playlistMap.find(addr) == _playlistMap.end()) {
		NPLInterface::NPLObjectProxy* p = new NPLInterface::NPLObjectProxy();
		_playlistMap.insert(std::make_pair(addr, p));
	}
	NPLInterface::NPLObjectProxy& medias = *_playlistMap[addr];
	medias["playlist_mode"] = (double)(int)list->playlist_mode();
	for (size_t i = 0; i < list->medias().size(); i++) {
		auto ptr = list->medias()[i].get();
		auto& _media = medias[i + 1];
		if (_media.GetType() == NPLInterface::NPLObjectBase::NPLObjectType_Nil) {
			NPLInterface::NPLObjectProxy* p = new NPLInterface::NPLObjectProxy();
			_media = *p;
			medias[i + 1] = _media;
		}
		_media["media_type"] = ptr->media_type();
		_media["resource"] = ptr->resource();
		_media["location"] = ptr->location();
		_media["start_time"] = ptr->start_time();
		_media["stop_time"] = ptr->stop_time();

		if (ptr->metas().size() > 0) {
			auto& _meta = _media["metas"];
			if (_meta.GetType() == NPLInterface::NPLObjectBase::NPLObjectType_Nil) {
				NPLInterface::NPLObjectProxy* p = new NPLInterface::NPLObjectProxy();
				_meta = *p;
				_media["metas"] = _meta;
				_meta["xx"] = "";
			}
			for (auto iter = ptr->metas().begin(); iter != ptr->metas().end(); iter++) {
				_meta[iter->first] = iter->second;
			}
		}
		else {
			auto& _meta = _media["metas"];
			if (_meta.GetType() != NPLInterface::NPLObjectBase::NPLObjectType_Nil) {
				_meta.MakeNil();
			}
		}

	}
	for (int i = list->medias().size(); true; i++) {
		auto& _media = medias[i + 1];
		if (_media.GetType() == NPLInterface::NPLObjectBase::NPLObjectType_Nil) {
			break;
		}
		auto& _meta = _media["metas"];
		delete& _meta;
		delete& _media;
		medias[i + 1].MakeNil();
	}
	data["medias"] = medias;

	return data;
}

void DisposePlayerState(int32_t id) {
	if (_stateMap.find(id) == _stateMap.end()) {
		return;
	}
	delete _stateMap[id];
	_stateMap.erase(id);
}

void Init() {
	_callbackObj.onPlayPauseStop = [](int32_t id, ::PlayerState* state) {
		auto& data = CreateCallbackProxy(id, state);
		data["callbackType"] = "onPlayPauseStop";
		NPL_Activate(GetMainNPLRuntimeState(), GetCallbackFile(), data);
	};
	_callbackObj.onPosition = [](int32_t id, ::PlayerState* state) {
		auto& data = CreateCallbackProxy(id, state);
		data["callbackType"] = "onPosition";
		NPL_Activate(GetMainNPLRuntimeState(), GetCallbackFile(), data);
	};
	_callbackObj.onComplete = [](int32_t id, ::PlayerState* state) {
		auto& data = CreateCallbackProxy(id, state);
		data["callbackType"] = "onComplete";
		NPL_Activate(GetMainNPLRuntimeState(), GetCallbackFile(), data);
	};
	_callbackObj.onVolume = [](int32_t id, ::PlayerState* state) {
		auto& data = CreateCallbackProxy(id, state);
		data["callbackType"] = "onVolume";
		NPL_Activate(GetMainNPLRuntimeState(), GetCallbackFile(), data);
	};
	_callbackObj.onRate = [](int32_t id, ::PlayerState* state) {
		auto& data = CreateCallbackProxy(id, state);
		data["callbackType"] = "onRate";
		NPL_Activate(GetMainNPLRuntimeState(), GetCallbackFile(), data);
	};
	_callbackObj.onOpen = [](int32_t id, ::PlayerState* state) {
		auto& data = CreateCallbackProxy(id, state);
		data["callbackType"] = "onOpen";
		NPL_Activate(GetMainNPLRuntimeState(), GetCallbackFile(), data);
	};

	_callbackObj.onVideoDimensions = [](int32_t id, int32_t video_width, int32_t video_height) {
		auto& data = CreateCallbackProxy(id);
		data["callbackType"] = "onVideoDimensions";
		data["width"] = (double)video_width;
		data["height"] = (double)video_height;
		NPL_Activate(GetMainNPLRuntimeState(), GetCallbackFile(), data);
	};

	_callbackObj.onVideoFrame = [](int32_t id, uint8_t* frame, int32_t width, int32_t height) {
		auto& data = CreateCallbackProxy(id);
		data["callbackType"] = "onVideoFrame";
		data["frame"] = (double)(int)frame;
		data["width"] = (double)width;
		data["height"] = (double)height;
		NPL_Activate(GetMainNPLRuntimeState(), GetCallbackFile(), data);
	};

	_callbackObj.onError = [](int32_t id, const char* error) {
		auto& data = CreateCallbackProxy(id);
		data["callbackType"] = "onError";
		data["error"] = (std::string)error;
		NPL_Activate(GetMainNPLRuntimeState(), GetCallbackFile(), data);
	};
}

void NPL_Activate(NPL::INPLRuntimeState* pState, std::string activateFile, NPLInterface::NPLObjectProxy& data) {

	if (!pState || activateFile.empty())
	{
		return;
	}
	std::string data_string;
	NPLInterface::NPLHelper::NPLTableToString("msg", data, data_string);
	pState->activate(activateFile.c_str(), data_string.c_str(), data_string.length());

}

void* GetUIObject(const char* uiname) {
	auto pParaEngine = GetCoreInterface()->GetAppInterface()->GetAttributeObject();
	auto pGUI = pParaEngine->GetChildAttributeObject("GUI");
	auto cls = pGUI->GetAttributeClass();
	auto field = cls->GetField("TempUINameStr");
	if (field == nullptr) {
		return NULL;
	}
	field->Set(pGUI, uiname);

	void* voidPtr = NULL;
	field = cls->GetField("GetUIObjectByTempNameStr");
	if (field == nullptr) {
		return NULL;
	}
	field->Get(pGUI, &voidPtr);
	//CGUIBase* uiObj = static_cast<CGUIBase*>(voidPtr);
	return voidPtr;
}

void SetVideoFrame(void* voidPtr, uint8_t* frames, int32_t width, int32_t height) {
	ParaEngine::IAttributeFields* attr = static_cast<ParaEngine::IAttributeFields*>(voidPtr);
	auto cls = attr->GetAttributeClass();
	auto field = cls->GetField("TempVideoWidth");
	if (field) {
		field->Set(voidPtr, width);
	}
	field = cls->GetField("TempVideoHeight");
	if (field) {
		field->Set(voidPtr, height);
	}
	field = cls->GetField("TempVideoFramePointer");
	if (field) {
		field->Set(voidPtr, frames);
	}
	field = cls->GetField("ApplyVideoFrame");
	if (field) {
		field->Call(voidPtr);
	}
}

std::string formatStr(const char* format, ...)
{
#define CC_MAX_STRING_LENGTH (1024*100)

	std::string ret;

	va_list ap;
	va_start(ap, format);

	char* buf = (char*)malloc(CC_MAX_STRING_LENGTH);
	if (buf != nullptr)
	{
		vsnprintf(buf, CC_MAX_STRING_LENGTH, format, ap);
		ret = buf;
		free(buf);
	}
	va_end(ap);

	return ret;
}