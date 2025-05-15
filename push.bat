@echo off
setlocal enabledelayedexpansion

set "source_folder=\release\bin\"
set "source_folder=%~dp0%source_folder%"
set "target_folder=/data/app/app2001/bin/"

for %%f in ("%source_folder%\*") do (
    adb push "%%f" "%target_folder%"
    if !errorlevel! equ 0 (
        echo Successfully pushed %%~nxf to %target_folder%
        adb shell chmod 777 "%target_folder%%%~nxf"
    ) else (
        echo Failed to push %%~nxf to %target_folder%
    )
)

endlocal
@REM pause