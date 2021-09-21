D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\player_sprites.o sprites\player_sprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\bullet_sprites.o sprites\bullet_sprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\explosion_sprites.o sprites\explosion_sprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\multiple_enemy_sprites.o sprites\multiple_enemy_sprites.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\multiple_enemy_sprites_sec.o sprites\multiple_enemy_sprites_sec.c

D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\intro_map.o maps\intro_map.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\game_over_map.o maps\game_over_map.c

D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\enemy_groups.o levels\enemy_groups.c

D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\music.o sound\music.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\sound.o sound\sound.c

D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\game.o states\game.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\intro.o states\intro.c
D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\game_over.o states\game_over.c

D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\globals.o globals.c

D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-w -c -o build\main.o main.c

D:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wf--nolospre -Wl-y -Wl-j -Wl-w -o build\main.gb build\main.o build\player_sprites.o build\bullet_sprites.o ^
build\explosion_sprites.o build\multiple_enemy_sprites.o build\multiple_enemy_sprites_sec.o build\music.o build\sound.o build\enemy_groups.o build\game.o build\globals.o build\intro.o ^
build\intro_map.o build\game_over.o build\game_over_map.o

del "%~dp0\build\*.sym" "%~dp0\build\*.lst" "%~dp0\build\*.asm" "%~dp0\build\*.adb" "%~dp0\build\*.ihx" /f /q