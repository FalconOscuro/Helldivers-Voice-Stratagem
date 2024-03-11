@echo off

set env-file=.\.venv\

if exist %env-file% goto run

echo No virtual environment found, configuring...
py -m venv %env-file%
echo Virtual envrionment setup in '%env-file%

:run
call %env-file%Scripts\activate.bat
echo Virtual environment activated

echo Checking dependencies...
pip install -r .\requirements.txt

echo Running HDVS...
python .\hdvs\main.py

pause