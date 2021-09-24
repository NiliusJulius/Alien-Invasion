# Alien Invasion

## Why?

The reason for making this game is that I had been learning how to program for the gameboy for a little while, and I wanted to challenge myself to see if I could push the hardware a little bit.

One of the limitations is that you can only have 40 sprites (8x8 or 8x16 pixels) on screen at any time. 
One game which I thought about would have an issue with this is Space Invaders (which this game is obviously heavily influenced by), because you already want around 40 enemies on screen, otherwise it would be a bit too easy/boring.

My approach to handling enough enemies is by using 8x16 sprites, where you will have two enemies on top of each other (each using for example 8x7 pixels, to leave a 1 pixel gap between them).
An extra benefit of this approach is that it is also possible to make larger enemies, which would be destroyed in 2 hits.

Some extra changes I added are the possibility to add enemies of different sizes, for example you could add a thin enemy of maybe only 2 pixels wide.
The same holds for bullets, so they can also have varying sizes.
All of this comes at the cost of a lot of cpu usage, but figuring out how to keep it within the limits has been part of the challenge.

All of this hopefully makes for some engaging and interesting additions to an classic game format.

## GB COMPO 2021 Game Jam

Right when I started the project I heard about the announcement of the gbcompo21 game jam, so I decided to also enter this game.
Info can be found here:

https://gbdev.io/gbcompo21/

## Usage

Use [GBDK 2020 4.0.3](https://github.com/gbdk-2020/gbdk-2020/releases/tag/4.0.3 "GBDK 2020 4.0.3")

Update make.bat and run it to build the rom.

Commands to check disk usage:
```bash

..\romusage.exe build\main.map -g -e:STACK:DEFF:100
..\romusage.exe build\main.map -g -e:SHADOW_OAM:C000:A0
..\romusage.exe build\main.map -g -e:STACK:DEFF:100 -e:SHADOW_OAM:C000:A0

..\romusage.exe build\main.noi -a
..\romusage.exe build\main.noi

..\romusage.exe build\main.map -G -E
..\romusage.exe build\main.map -g
```

## License
[License](../master/LICENSE)