D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\player_sprites.o sprites\player_sprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\bullet_sprites.o sprites\bullet_sprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\explosion_sprites.o sprites\explosion_sprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\multiple_enemy_sprites.o sprites\multiple_enemy_sprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\window_map.o maps\window_map.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\music.o music.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\sound.o sound.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\main.o main.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-j -Wl-w -o build\main.gb build\main.o build\player_sprites.o build\bullet_sprites.o ^
build\explosion_sprites.o build\multiple_enemy_sprites.o build\window_map.o build\music.o build\sound.o

del "%~dp0\build\*.sym" "%~dp0\build\*.lst" "%~dp0\build\*.asm" "%~dp0\build\*.adb" "%~dp0\build\*.ihx" /f /q