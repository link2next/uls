@echo off
REM
REM  make_SampleLex : make SampleLex.java from sample.ulc
REM  Written by Stanley Hong <link2next@gmail.com>, Aug 2022.
REM

setlocal

set ULC_FILE=%1

if "%ULC_FILE%" == "" set ULC_FILE=.\sample.ulc

if not exist "%ULC_FILE%" (
	echo %ULC_FILE%: Not Exist!
	goto end_0
)

set ULC2CLASS_FPATH=
for /F "tokens=*" %%a in ('where ulc2class') do set ULC2CLASS_FPATH=%%a

if "%ULC2CLASS_FPATH%" == ""  (
	echo ulc2class: Not Found!
	goto end_0
)

echo making the java-file SampleLex.java from %ULC_FILE% ...
ulc2class -o ..\src\uls\tests\Sample1Lex.java -ljava -n uls.tests.Sample1Lex %ULC_FILE%

:end_0
pause
endlocal
