Run make.bat to build the rom.

Commands to check disk usage:

..\romusage.exe build\main.map -g -e:STACK:DEFF:100
..\romusage.exe build\main.map -g -e:SHADOW_OAM:C000:A0
..\romusage.exe build\main.map -g -e:STACK:DEFF:100 -e:SHADOW_OAM:C000:A0

..\romusage.exe build\main.noi -a
..\romusage.exe build\main.noi

..\romusage.exe build\main.map -G -E
..\romusage.exe build\main.map -g