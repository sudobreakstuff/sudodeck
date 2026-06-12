@echo off
title SudoDeck Installer
echo ==================================
echo   SudoDeck - Windows Setup
echo ==================================
echo.

:: Check Python
python --version >nul 2>&1
if errorlevel 1 (
    echo [!] Python not found. Install from https://python.org
    echo     Check "Add Python to PATH" during install.
    pause
    exit /b 1
)
echo [*] Python found

:: Create app directory
set APPDIR=%APPDATA%\SudoDeck
mkdir "%APPDIR%" 2>nul

:: Copy daemon
copy /Y "%~dp0sudodeckd.py" "%APPDIR%\sudodeckd.py" >nul
echo [*] Daemon copied

:: Create startup shortcut
set STARTUP=%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup
echo Set ws = WScript.CreateObject("WScript.Shell") > "%TEMP%\sudodeck_shortcut.vbs"
echo ws.CurrentDirectory = "%APPDIR%" >> "%TEMP%\sudodeck_shortcut.vbs"
echo ws.Run "pythonw sudodeckd.py", 0, False >> "%TEMP%\sudodeck_shortcut.vbs"
cscript //nologo "%TEMP%\sudodeck_shortcut.vbs"
del "%TEMP%\sudodeck_shortcut.vbs"

:: Start daemon
start /B pythonw "%APPDIR%\sudodeckd.py"
timeout /t 2 /nobreak >nul

:: Verify
curl -s http://localhost:8092/ping >nul 2>&1
if errorlevel 1 (
    echo [!] Daemon failed to start. Try running manually:
    echo     python "%APPDIR%\sudodeckd.py"
) else (
    echo [*] Daemon running on port 8092
)

echo.
echo Done. Open https://sudobreakstuff.github.io/sudodeck in Chrome/Edge
echo Connect your CYD and tap the screen.
echo.
echo The daemon auto-starts on login.
pause
