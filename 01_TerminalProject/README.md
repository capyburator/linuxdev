## Description
This is a directory for the first task `01_TerminalProject` of the course [Linux application development](http://uneex.org/LecturesCMC/LinuxApplicationDevelopment2024).

## The task
Implement file shower by means of `ncurses`:
* Program `Show.c` gets path to regular file in command line argument;
* Program creates a window where the file is displayed. The first line contains file name and number of lines.
* Program must support following keys:
    - space and down arrow: file scrolls one line down;
    - up arrow: file scrolls one line up;
    - page up: file scrolls one page up;
    - page down: file scrolls one page down;
    - left arrow: file scrolls one column left;
    - right arrow: file scrolls one column right;
    - escape: quit the program.
* It is OK to map whole file in memory;
* File encoding: ASCII;
* Executable name: `Show`.

## Build
To build the program run `make all`.

![](../demo/01_TerminalProject_demo.gif)
