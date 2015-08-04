IF "%~3"=="" EXIT
SETLOCAL
CD /D %3
IF EXIST .\functions.cpp.cfg EXIT
CD /D %2

copy .\base_functions.cpp %3
copy .\base_functions.h %3
copy .\config.cfg %3
copy .\functions.cpp.cfg %3
copy .\api\windows\lib\fmod.dll %3
copy .\api\windows\lib\fmod64.dll %3

IF "%~5"=="dllcopy" (
    IF "%~4"=="release" (
        for /F %%A in ('dir /b "%~1\*.dll"') do (
            IF EXIST "%~1\%%~nAd.dll" copy "%~1\%%~nA.dll" %3
        )
    ) ELSE (
        for /F %%A in ('dir /b "%~1\*d.dll"') do copy %~1\%%A %3
    )
)

md %3\translations
copy .\translations\*.qm %3\translations

md %3\examples
copy .\examples\*.* %3\examples
ENDLOCAL
