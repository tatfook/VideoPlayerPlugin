#pragma once
#ifdef _HAS_STD_BYTE
#undef _HAS_STD_BYTE
#endif
#define _HAS_STD_BYTE 0

#include <functional>
#include "api/api.h"
#include "NPLInterface.hpp"
#include "INPLRuntime.h"
#include "INPLRuntimeState.h"

::EventCallbackStruct& GetCallbackObj();

void OnFrameMove();

void performFunctionInMainThread(std::function<void()> function);

void SetCallbackFile(std::string file);

std::string GetCallbackFile();

NPLInterface::NPLObjectProxy& CreateCallbackProxy(int32_t id);

NPLInterface::NPLObjectProxy& CreateCallbackProxy(int32_t id, ::PlayerState* state);

void DisposePlayerState(int32_t id);

void DisposeAllPlayer();

void Init();

void NPL_Activate(NPL::INPLRuntimeState* pState, std::string activateFile, NPLInterface::NPLObjectProxy& data);

void* GetUIObject(const char* uiname);

void SetVideoFrame(void* voidPtr, uint8_t* frames, int32_t width, int32_t height);

std::string formatStr(const char* format, ...);