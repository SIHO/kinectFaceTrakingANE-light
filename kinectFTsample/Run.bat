@echo off
set PAUSE_ERRORS=1
call bat\SetupSDK.bat
call bat\SetupApplication.bat

echo.
echo Starting AIR Debug Launcher...
echo.

::adl "%APP_XML%" "%APP_DIR%"
adl -runtime "%FLEX_SDK%\runtimes\air\win" -profile extendedDesktop -extdir extdir/debug "%APP_XML%" "%APP_DIR%"
if errorlevel 1 goto error
goto end

:error
pause

:end