
#ifndef SRC_LCD_H_
#define SRC_LCD_H_

#include "platform.h"
#include "kc705_my_bist.h"

int kc_InitializeLCD(void);
int kc_SendCommand(int command);
int kc_SendData(int data);
void kc_SetCursor(int row, int col);
int kc_FunctionSet(int num_bits, int num_lines);
int kc_EntryMode(int mode);
int kc_CursorMode(int mode);
int kc_DisplayMode(int mode);
int kc_ShiftMode(int mode);
int kc_SetDDRAM(int address);
int kc_PrintLine(const char* line);
int kc_PrintTwoLines(const char* line1, const char* line2);
void kc_Sleep(int delay);

#endif
