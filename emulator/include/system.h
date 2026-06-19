#ifndef SYSTEM_H
#define SYSTEM_H

#define ESC_SEQ "\x1b["
#define ESC(c) ESC_SEQ c

#define ESC_HIDE_CURSOR ESC("?25l")
#define ESC_SHOW_CURSOR ESC("?25h")

#define ESC_CLEAR_SCREEN ESC("2J")
#define ESC_CLEAR_LINE ESC("2K")

#define MOVE_TO_AUX_BUFFER  ESC("?1049h")
#define MOVE_TO_MAIN_BUFFER ESC("?1049l")

#define INVERTED_TEXT_COLOR ESC("7m")
#define RESET_TEXT_ATTRIBUTES ESC("m")

#define ESC_RESET_CURSOR_POS ESC("H")

typedef struct consoleInfo{
    int height;
    int width;
} consoleInfo;

void moveCursorPos(int x, int y);
int instrToStr(char *outBuffer, word memoryWord);
void sleepF(unsigned long miliseconds);
void enterAux();
void leaveAux();

#endif