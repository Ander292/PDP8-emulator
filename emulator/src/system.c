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

// TODO: Replace with ReadFile on stdout
byte pollInput(unsigned long timeoutMS){
#if 0
    unsigned long elapsed = 0;
    unsigned long interval = timeoutMS;

    while(elapsed < timeoutMS){
        if(_kbhit()){
            byte c = _getch();
            if(c == 0) c = _getch();
            // Special cases
            if(c == '\r') c = '\n';
            return c;
        }
        Sleep(interval);
        elapsed += interval;
    }
    return 0;
#endif
    byte result = 0;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD feedback;

    unsigned long elapsed = 0;
    unsigned long interval = 1; // Granularity
    while(elapsed < timeoutMS){
        switch(GetFileType(hStdin)){
            case FILE_TYPE_CHAR:
                GetNumberOfConsoleInputEvents(hStdin, &feedback);
                break;
            case FILE_TYPE_PIPE:
                PeekNamedPipe(hStdin, NULL, 0, NULL, &feedback, NULL);
                break;
            default:
                printf("Fatal error : Invalid stdin handle!\n");
                break; 
        }
        Sleep(interval);
        elapsed += interval;
    }
    if(feedback != 0){
        ReadFile(hStdin, &result, 1, &feedback, NULL);
        if(feedback == 0) printf("Read 0 bytes!\n");
    }

    return result;
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
#include <sys/select.h>
struct termios original;

byte pollInput(unsigned long timeoutMS){
    fd_set readfds; 
    struct timeval timeout;

    while(1){
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        // Zero timeout makes the select call return immediately
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        int ready = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

        if(ready > 0 && FD_ISSET(STDIN_FILENO, &readfds)){
            char ch;
            read(STDIN_FILENO, &ch, 1);
        }else{
            sleep(1);
        }
    }
    return 0;
}
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