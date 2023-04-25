@echo off 
pushd "%~dp0..\..\" 
rem call "D:\work\para_client\ParaEngineClient.exe" single="false" mc="true" noupdate="true" dev="%~dp0" mod="VideoPlayerPlugin" isDevEnv="true"  
rem call "D:\work\NPLRuntime\Client\build_nodll\ParaEngineClient\Release\ParaEngineClient.exe" single="false" mc="true" noupdate="true" dev="%~dp0" mod="VideoPlayerPlugin" isDevEnv="true"  
call "D:\github\NPLRuntime\Client\bin\win32\ParaEngineClientApp\Debug\ParaEngineClient_d.exe" single="false" mc="true" noupdate="true" dev="%~dp0/release/" mod="VideoPlayerPlugin" isDevEnv="true"  

popd 
