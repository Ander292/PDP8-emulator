#include "emulator.h"
#include "terminal.h"

#include <stdio.h>

#define WINDOWS

void moveCursorPos(int x, int y){
    printf(ESC_SEQ"%d;%dH", y, x);
    fflush(stdout);
}

#if defined WINDOWS
#include <windows.h>
DWORD oldMode;

void enterRawMode(){
    DWORD mode;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &mode);
    oldMode = mode;
    mode = mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_INSERT_MODE) | ENABLE_VIRTUAL_TERMINAL_INPUT | ENABLE_PROCESSED_OUTPUT;
    SetConsoleMode(hStdin, mode);
    puts(MOVE_TO_AUX_BUFFER);
    puts(ESC_HIDE_CURSOR);
}

void leaveRawMode(){
    puts(MOVE_TO_MAIN_BUFFER);
    puts(ESC_SHOW_CURSOR);
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), oldMode);
}

#elif defined LINUX
#include <termios.h>
#include <unistd.h>
void enterRawMode(){

}

void leaveRawMode(){
    
}

#endif