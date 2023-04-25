@echo off
rem build paracraft mod package

if exist VideoPlayerPlugin.zip ( del VideoPlayerPlugin.zip )

if exist release\Mod\VideoPlayerPlugin\VideoPlayerPlugin_d.dll ( del /f release\Mod\VideoPlayerPlugin\VideoPlayerPlugin_d.dll )

xcopy .\scripts\*.*  release\Mod\VideoPlayerPlugin\ /s /y /d

pushd release
call "..\7z.exe" a ..\VideoPlayerPlugin.zip Mod\
call "..\7z.exe" a ..\VideoPlayerPlugin.zip bin64\
popd

pause