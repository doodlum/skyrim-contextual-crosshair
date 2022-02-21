@echo off

RMDIR dist /S /Q

cmake -S . --preset=pre-ae --check-stamp-file "build\pre-ae\CMakeFiles\generate.stamp"
if %ERRORLEVEL% NEQ 0 exit 1
cmake --build build/pre-ae --config Release
if %ERRORLEVEL% NEQ 0 exit 1

cmake -S . --preset=post-ae --check-stamp-file "build\post-ae\CMakeFiles\generate.stamp"
if %ERRORLEVEL% NEQ 0 exit 1
cmake --build build/post-ae --config Release
if %ERRORLEVEL% NEQ 0 exit 1

xcopy "build\pre-ae\release\*.dll" "dist\pre-ae\SKSE\Plugins\" /I /Y
xcopy "build\post-ae\release\*.dll" "dist\post-ae\SKSE\Plugins\" /I /Y

xcopy "package" "dist" /I /Y /E

pause