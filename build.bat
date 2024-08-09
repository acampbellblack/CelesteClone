@echo off

set TIMESTAMP=%DATE:~-4%-%DATE:~3,2%-%DATE:~0,2%_%TIME:~0,2%-%TIME:~3,2%-%TIME:~6,2%

cl /Zi /Ithird_party /Fe:schnitzel.exe src/main.cpp user32.lib opengl32.lib gdi32.lib

del game_*
cl /Zi /LD /Fe:game_%TIMESTAMP%.dll src/game.cpp
move game_%TIMESTAMP%.dll game.dll

del *.obj