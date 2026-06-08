#include "emulator.h"
#include "system.h"

#include <stdio.h>

#define WINDOWS

void moveCursorPos(int x, int y){
    printf(ESC_SEQ"%d;%dH", y, x);
    fflush(stdout);
}

#if defined WINDOWS
#include <windows.h>
#include <conio.h>
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

void sleepF(unsigned long miliseconds){
    Sleep(miliseconds);
}

// TODO: Replace with ReadInput
byte pollInput(unsigned long timeoutMS){
    unsigned long elapsed = 0;
    unsigned long interval = 50;

    while(elapsed < timeoutMS){
        if(_kbhit()){
            byte c = _getch();
            if(c == 0) c = _getch();
            //printf("\npollInput reports %d:%c\n", c, c);
            return c;
        }
        Sleep(interval);
        elapsed += interval;
    }
    return 0;
}

/*
    Creates the thread and closes its handle as its not needed.
*/
// void threadCreate(void (*fPtr)(void), void *arg){
//     HANDLE hThread = CreateThread(
//         NULL, 0, fPtr,
//         arg, 0, NULL
//     );
//     if(hThread) CloseHandle(hThread);
// }

#elif defined LINUX
#include <termios.h>
#include <unistd.h>
struct termios original;

void enterRawMode(){
    tcgetattr(STDIN_FILENO, &original);
    struct termios raw = original;

    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON);
    //raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    puts(MOVE_TO_AUX_BUFFER);
    puts(ESC_HIDE_CURSOR);
}

void leaveRawMode(){
    puts(MOVE_TO_MAIN_BUFFER);
    puts(ESC_SHOW_CURSOR);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
}

void sleepF(unsigned long miliseconds){
    usleep(miliseconds);
}

#endif