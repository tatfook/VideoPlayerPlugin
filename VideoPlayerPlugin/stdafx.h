#pragma once

#ifdef WIN32
#	ifndef _CRT_SECURE_NO_WARNINGS
#		define _CRT_SECURE_NO_WARNINGS
#	endif
#endif

#include <functional>
#include "PluginAPI.h"

/**
* Optional NPL includes, just in case you want to use some core functions see GetCoreInterface()
*/
#include "IParaEngineCore.h"
#include "IParaEngineApp.h"
#include "ParaEngineClientConfig.win32.h"
#include "INPLRuntimeState.h"

extern ParaEngine::IParaEngineCore* GetCoreInterface();

extern NPL::INPLRuntimeState* GetMainNPLRuntimeState();

#ifndef OUTPUT_LOG
#define OUTPUT_LOG	GetCoreInterface()->GetAppInterface()->WriteToLog
//#define OUTPUT_LOG	printf
#endif

#pragma warning( disable : 4819)