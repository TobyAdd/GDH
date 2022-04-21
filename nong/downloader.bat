@echo off

cd .gdh
cd nong

cls

set arg1=%1
set arg2=%2
set arg3=%3

if exist "%localappdata%\%arg3%\tempfile.mp3" (
echo Deleting "tempfile.mp3"
del /S /Q "%localappdata%\%arg3%\tempfile.mp3"
)

echo Executing: youtube-dl --extract-audio --audio-quality 320k --audio-format mp3 --output %localappdata%\%arg3%\tempfile.mp3 https://youtu.be/%arg2%

youtube-dl --extract-audio --audio-quality 320k --audio-format mp3 --output %localappdata%\%arg3%\tempfile.mp3 https://youtu.be/%arg2%

if exist "%localappdata%\%arg3%\tempfile.mp3" (
echo Executing: ffmpeg -y -i "%localappdata%\%arg3%\tempfile.mp3" -b:a 320k -preset ultrafast "%localappdata%\%arg3%\%arg1%.mp3"
ffmpeg -y -i "%localappdata%\%arg3%\tempfile.mp3" -b:a 320k -preset ultrafast "%localappdata%\%arg3%\%arg1%.mp3"
) else (
echo Cant find "tempfile.mp3", cancelling...
pause
)

if exist "%localappdata%\%arg3%\%arg1%.mp3" (
    echo Done!
) else (
    echo Cant find "%localappdata%\%arg3%\%arg1%.mp3"! Maybe something went wrong
)