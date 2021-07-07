D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\PlayerSprites.o sprites\PlayerSprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\BulletSprites.o sprites\BulletSprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\Explosion.o sprites\Explosion.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\MultipleEnemySprites.o sprites\MultipleEnemySprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\WindowMap.o maps\WindowMap.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\music.o music.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\sound.o sound.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\main.o main.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-j -Wl-w -o build\main.gb build\main.o build\PlayerSprites.o build\BulletSprites.o build\Explosion.o build\MultipleEnemySprites.o build\WindowMap.o build\music.o build\sound.o

del "%~dp0\build\*.sym" "%~dp0\build\*.lst" "%~dp0\build\*.asm" "%~dp0\build\*.adb" "%~dp0\build\*.ihx" /f /q