@echo off
setlocal
cd /d "%~dp0"

echo ============================================================
echo   Floweret Asset Background Remover
echo ============================================================

if "%~1"=="" (
    python "%~dp0remove_background.py"
) else (
    echo Processing: "%~1"
    python "%~dp0remove_background.py" "%~1" --crop
    echo.
    set /p EMBED="Would you like to embed this into the plugin header (FloweretLogoData.h)? [y/N]: "
    if /i "!EMBED!"=="y" (
        python "%~dp0remove_background.py" "%~1" --crop --embed
    )
)

echo.
pause
