D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\PlayerSprites.o sprites\PlayerSprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\BulletSprites.o sprites\BulletSprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\EnemySprites.o sprites\EnemySprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\MultipleEnemySprites.o sprites\MultipleEnemySprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\WindowMap.o maps\WindowMap.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\main.o main.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-j -Wl-w -o build\main.gb build\main.o build\PlayerSprites.o build\BulletSprites.o build\EnemySprites.o build\MultipleEnemySprites.o build\WindowMap.o

del "%~dp0\build\*.sym" /f /q
del "%~dp0\build\*.lst" /f /q
del "%~dp0\build\*.asm" /f /q
del "%~dp0\build\*.adb" /f /q
del "%~dp0\build\*.ihx" /f /q
del "%~dp0\build\*.map" /f /q