#define PE_CORE_EXPORTING

#ifndef _HAS_AUTO_PTR_ETC
#define _HAS_AUTO_PTR_ETC 1
#endif // _HAS_AUTO_PTR_ETC

#include <iostream>
#include <Windows.h>
#include <thread>
#include "VideoPlayerPlugin.h"
#include "PluginAPI.h"
#include "IParaEngineCore.h"
using namespace std;

#ifdef WIN32
#define CLIB_DEFHANDLE	((void *)-1)
#endif

void* GetProcAddress(void* Lib, const char* Fnname)
{
#ifdef WIN32 // Microsoft compiler
	if (Lib == CLIB_DEFHANDLE)
	{
		void* p = NULL;
		/* Default libraries. */
		enum {
			CLIB_HANDLE_EXE, // ParaEngineClient.exe
			CLIB_HANDLE_DLL, // ParaEngineClient(_d).dll
			CLIB_HANDLE_MAX,
		};
		static void* clib_def_handle[CLIB_HANDLE_MAX] = { 0,0 };

		for (int i = 0; i < CLIB_HANDLE_MAX; i++)
		{
			HINSTANCE h = (HINSTANCE)clib_def_handle[i];
			if (!(void*)h) {  /* Resolve default library handles (once). */
				switch (i) {
				case CLIB_HANDLE_EXE:
					GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, NULL, &h);
					break;
				case CLIB_HANDLE_DLL:
					GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (const char*)clib_def_handle, &h);
					break;
				}
				if (!h) continue;
				clib_def_handle[i] = (void*)h;
			}
			p = (void*)::GetProcAddress(h, Fnname);
			if (p)
				break;
		}
		return p;
	}
	else
		return (void*)::GetProcAddress((HINSTANCE)Lib, Fnname);
#else
	return dlsym(Lib, Fnname);
#endif
}

bool FreeLibrary(void* hDLL)
{
	if (hDLL == CLIB_DEFHANDLE)
		return true;
#ifdef WIN32 // Microsoft compiler
	return !!::FreeLibrary((HINSTANCE)hDLL);
#else
	return dlclose(hDLL);
#endif
}

class ParaEngine::ClassDescriptor;
class ParaEngine::IParaEngineCore;

/** "LibDescription": must be implemented in a plug-in. */
typedef const char* (*lpFnLibDescription)();
/** "LibVersion": must be implemented in a plug-in. */
typedef unsigned long (*lpFnLibVersion)();
/** "LibNumberClasses": must be implemented in a plug-in. */
typedef int (*lpFnLibNumberClasses)();
/** "LibClassDesc": must be implemented in a plug-in. */
typedef ParaEngine::ClassDescriptor* (*lpFnLibClassDesc)(int i);
/** "LibInit": this is optional in a plug-in */
typedef void (*lpFnLibInit)();
/** "LibInitParaEngine": this is optional in a plug-in */
typedef void(*lpFnLibInitParaEngine)(ParaEngine::IParaEngineCore* pIParaEngineCore);
/** "LibActivate": this is optional in a plug-in */
typedef int (*lpFnLibActivate)(int nType, void* pVoid);

typedef void (STDCALL* pfnEnsureInit)(void);
typedef void (STDCALL* pfnForceTerm)(void);

void* checkLoad() {
	
#ifdef _DEBUG
	std::string dll_name = "VideoPlayerPlugin.dll";
#else
	std::string dll_name = "VideoPlayerPlugin.dll";
#endif
	void* pDll = ::LoadLibrary(dll_name.c_str());
	return pDll;
}
//
//ParaEngine::IVideoPlayers* getVideoPlayer(void* m_hDLL) {
//
//	ParaEngine::IVideoPlayers* m_pMovieCodec = nullptr;
//	vector<ParaEngine::ClassDescriptor*> m_listClassDesc;
//
//	lpFnLibDescription pLibDescription = (lpFnLibDescription)GetProcAddress(m_hDLL, "LibDescription");
//	lpFnLibVersion pLibVersion = (lpFnLibVersion)GetProcAddress(m_hDLL, "LibVersion");
//
//	int nClassNum = 0;
//	lpFnLibNumberClasses pLibNumberClasses = (lpFnLibNumberClasses)GetProcAddress(m_hDLL, "LibNumberClasses");
//	if (pLibNumberClasses != 0)
//	{
//		// call the function
//		nClassNum = pLibNumberClasses();
//		//OUTPUT_LOG("lib classes count %d \r\n", nClassNum);
//		m_listClassDesc.reserve(nClassNum);
//	}
//	lpFnLibClassDesc pLibClassDesc = (lpFnLibClassDesc)GetProcAddress(m_hDLL, "LibClassDesc");
//	if (pLibDescription != 0)
//	{
//		// call the function
//		for (int i = 0; i < nClassNum; ++i)
//		{
//			ParaEngine::ClassDescriptor* pClassDesc = pLibClassDesc(i);
//			if (pClassDesc != 0)
//			{
//				m_listClassDesc.push_back(pClassDesc);
//			}
//			else
//			{
//				int error = 1;
//			}
//		}
//	}
//
//	lpFnLibInit pLibInit = (lpFnLibInit)GetProcAddress(m_hDLL, "LibInit");
//	if (pLibDescription != 0)
//	{
//		// call the function
//		pLibInit();
//	}
//
//	lpFnLibInitParaEngine pLibInitParaEngine = (lpFnLibInitParaEngine)GetProcAddress(m_hDLL, "LibInitParaEngine");
//	if (pLibInitParaEngine != 0)
//	{
//		// call the function
//		pLibInitParaEngine(NULL);
//	}
//
//	lpFnLibActivate m_pFuncActivate = (lpFnLibActivate)GetProcAddress(m_hDLL, "LibActivate");
//	if (pLibDescription != 0)
//	{
//	}
//
//
//	for (int i = 0; i < nClassNum; ++i)
//	{
//		ParaEngine::ClassDescriptor* pClassDesc = m_listClassDesc[i];
//		int j = 0;
//		if (pClassDesc)
//		{
//			m_pMovieCodec = (ParaEngine::IVideoPlayers*)pClassDesc->Create();
//			//ParaEngine::IVideoPlayer*  m_pMovieCodec = (ParaEngine::IVideoPlayer*)pClassDesc->Create();
//		}
//	}
//
//	return m_pMovieCodec;
//}

int main()
{
	void * m_hDLL = checkLoad();
	
	if (m_hDLL ==nullptr) {
		std::cout << "load dll failed" << endl;
	}

	//setMainLuaState(NULL);
	char basedir[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH, basedir);
	std::string workDir = std::string(basedir) + "/../../";
	//workDir = workDir + "../";

	//testExecuteLuaFile(workDir+"TestCpp/test.lua");

	//m_pMovieCodec->setCallback_onPosition([](int32_t id, ParaEngine::PlayerState* state) {
	//	//std::cout << "setCallback_onPosition: " << state->position() << std::endl;
	//});
	//m_pMovieCodec->PlayerCreate(0, 1280, 720, 0, NULL);
	//m_pMovieCodec->setCallback_onVideoFrame(0, [](int32_t id, uint8_t* frame, int32_t width, int32_t height) {
	//	for (int i = 0; i < width; i++) {
	//		for (int j = 0; j < height; j++) {
	//			int index = j * width + i;
	//			//第j行第i列
	//			int r = frame[index + 0];
	//			int g = frame[index + 1];
	//			int b = frame[index + 2];
	//			int a = frame[index + 3];

	//			if (index == width * height * 0.5) {
	//				//std::cout << "r:" << r << ", g:" << g << ", b:" << b << ", a:" << a << endl;
	//			}
	//		}
	//	}
	//	int i = 0;
	//});
	/*std::thread t([m_pMovieCodec, workDir](){
		
		Sleep(5);
		std::string videoPaht = workDir + "vlcTest.mp4";
		m_pMovieCodec->PlayerAdd(0, "MediaType.file", videoPaht.c_str());
		std::cout << "aaaaa" << endl;
		m_pMovieCodec->PlayerPlay(0);
	});
	t.detach();*/

	/*std::thread t2([m_pMovieCodec]() {
		Sleep(10 * 1000);
		m_pMovieCodec->PlayerPause(0);
		Sleep(5 * 1000);
		m_pMovieCodec->PlayerPlay(0);
	});*/
	

	while (true)
	{
		Sleep(1000);
		//std::cout << "main sleep" << endl;
	}
	return 0;
}

