# cei_-_Change_Executable_Icon


Change Executable Icon for command line:
  - Based on https://github.com/fatrolls/IconChanger
  - Only the 1st image of the .ico file is added to the .exe
  - Tested on Windows 10 Pro, build 19041.685, locale en-US


Compiled with MSYS2/MinGW-w64:

  $ gcc -o cei cei.c -s -O3 -Wl,--gc-sections -fno-asynchronous-unwind-tables -nostartfiles --entry=cei


Usage:

  > cei PATH\TO\ICO PATH\TO\EXE [PATH\TO\NEWEXEWITHICO]
  - If [PATH\TO\NEWEXEWITHICO] is ommitted, cei.exe edits the original .exe located at PATH\TO\EXE
  - If [PATH\TO\NEWEXEWITHICO] is passed, cei.exe makes a copy of the original .exe to PATH\TO\EXEWITHICO, and edits this copy


Notes:

  - !! Some executables wont work anymore after changing their icon.. !!
  - ..So make a backup beforehand and/or pass [PATH\TO\NEWEXEWITHICO] as a 3rd argument

To view the source code in your browser with original tabbing (4 white-spaces instead of browsers default 8), add '/?ts=4' to url.  
Works on Firefox/Chrome based browser, except for edge..  
https://github.com/0x546F6D/cei_-_Change_Executable_Icon/blob/main/cei.c/?ts=4
