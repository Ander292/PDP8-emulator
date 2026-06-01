#include "emulator.h"
#include "terminal.h"

#include <stdio.h>

#define WINDOWS

#if defined WINDOWS
#include <windows.h>
DWORD oldMode;

consoleInfo getConsoleInfo(){
    consoleInfo result = {0};
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO si = {0};
    GetConsoleScreenBufferInfo(hStdout, &si);

    result.height = si.dwMaximumWindowSize.Y;
    result.width = si.dwMaximumWindowSize.X;

    return result;
}

void moveCursorPos(int x, int y){
    printf(ESC_SEQ"%d;%dH", y, x);
    fflush(stdout);
}

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

void sleep(unsigned long miliseconds){
    Sleep((DWORD)miliseconds);
}

// Always blocks for now
int pollInput(int block){
    int result = 0;
    DWORD feedback = 0;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    ReadConsoleA(hStdin, &result, 1, &feedback, NULL);

    return result;
}

#elif defined LINUX
#include <unistd.h>
#include <sys/ioctl.h>

consoleInfo getConsoleInfo(){

}
#endif