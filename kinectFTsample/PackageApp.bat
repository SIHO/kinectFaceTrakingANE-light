@echo off
set PAUSE_ERRORS=1
call bat\SetupSDK.bat
call bat\SetupApplication.bat

set AIR_TARGET=
::set AIR_TARGET=-captive-runtime
::set OPTIONS=-tsa none
set OPTIONS=-target bundle
call bat\Packager.bat

pause