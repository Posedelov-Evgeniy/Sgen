@echo off
IF "%~2"=="" EXIT
IF EXIST %2\functions.cpp.cfg EXIT
SETLOCAL
copy .\base_functions.cpp %2
copy .\base_functions.h %2
copy .\config.cfg %2
copy .\functions.cpp.cfg %2
copy .\api\windows\fmodex.dll %2

IF "%~4"=="dllcopy" (
    IF "%~3"=="release" (
        for /F %%A in ('dir /b "%~1\*.dll"') do (
            IF EXIST "%~1\%%~nAd.dll" copy "%~1\%%~nA.dll" %2
        )
    ) ELSE (
        for /F %%A in ('dir /b "%~1\*d.dll"') do copy %~1\%%A %2
    )
)
ENDLOCAL
