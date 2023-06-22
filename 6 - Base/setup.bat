@echo off
set /p input= Input jdk folder path (e.g. "C:\Program Files\Java\jdk-17"): 
setx JDKPATH %input%
pause