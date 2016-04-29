@echo off
set Sel=3
Set Hex=1
Set Mcm=1
Set Conf=1
echo Enter COM port number:
set /p Com=

:start
cls
title OSD
color cf

echo Select what to do: 1-write hex 2-write mcm 3-read cfg 4-write cfg
set /p Sel=
if %Sel%==1 goto hex
if %Sel%==2 goto mcm
if %Sel%==3 goto rcfg
if %Sel%==4 goto wcfg
IF %Sel% LEQ 0 goto start
IF %Sel% GEQ 5 goto start
goto end

:hex
cls
title HEX
color fc
echo Select language: 1-ru 2-en
set /p Hex=
if %Hex%==1 goto hexru
if %Hex%==2 goto hexen
IF %Hex% LEQ 0 goto start
IF %Hex% GEQ 3 goto start
goto end

:hexru
Echo WRITE RUSSIAN HEX
minimosd_cfg writefw -P COM%Com% -fw MinimOSDx2_ru-0.6.0.hex
goto end

:hexen
Echo WRITE ENGLAND HEX
minimosd_cfg writefw -P COM%Com% -fw MinimOSDx2_en-0.6.0.hex
goto end

:mcm
cls
title MCM
color fc
echo Select language: 1-ru 2-en
set /p Mcm=
if %Mcm%==1 goto mcmru
if %Mcm%==2 goto mcmen
IF %Mcm% LEQ 0 goto start
IF %Mcm% GEQ 3 goto start
goto end

:mcmru
Echo WRITE RUSSIAN CHARSET
minimosd_cfg writefont -P COM%Com% -mcm MinimOSD_ru.mcm
goto end

:mcmen
Echo WRITE ENGLAND CHARSET
minimosd_cfg writefont -P COM%Com% -mcm MinimOSD_en.mcm
goto end

:rcfg
Echo READ CONFIG
minimosd_cfg read -P COM%Com% -cf config.osd
goto end


:wcfg
Echo WRITE CONFIG
minimosd_cfg write -P COM%Com% -cf config.osd

:end
echo Press any key

pause > nul
goto start
