
#include "lcd.h"
#include "xgpio_l.h"

#define INIT_DELAY 100000 // usec delay timer during initialization, change if clock speed changes
#define INST_DELAY 10000 // usec delay timer between instructions
#define DATA_DELAY 10000 // usec delay timer between data

int kc_InitializeLCD(void) {

  // Set the GPIO bits for the LCD as outputs.
  XGpio_WriteReg(LCD_BASEADDR, LCD_TRISTATE_CONTROL_OFFSET, 0);

  //// RESET, needed for 4-bit mode only
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, 0xff);
  kc_Sleep(20);

  //send cmd 0x30
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, 0x61);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, 0x60);
  kc_Sleep(15);

  //send cmd 0x30
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, 0x61);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, 0x60);
  kc_Sleep(5);

  //send cmd 0x30
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, 0x61);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, 0x60);
  kc_Sleep(5);

  //send cmd 0x20 // set 4-bit mode
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, 0x21);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, 0x20);
  kc_Sleep(5);
  // note: BUSY flag will be valid only after above sequence

  // INITIALIZATION, common to 4bit and 8bit modes
  kc_FunctionSet(4, 2); // 0x28
  kc_CursorMode(0); // 0x0f
  kc_EntryMode(0); // 0x06
  kc_SetCursor(0, 0);

#if 1 // TEST PURPOSE ONLY
  kc_SendData('A');
  kc_SendData('B');
  kc_SendData('C');
  kc_SendData('D');
  kc_SendData('E');
  kc_SendData('F');
  kc_SendData('G');
  kc_SendData('H');
  kc_SendData('I');
  kc_SendData('J');
  kc_SendData('K');
  kc_SendData('L');
  kc_SendData('M');
  kc_SendData('N');
  kc_SendData('O');
  kc_SendData('P');

  kc_SetCursor(1, 0);
  kc_SendData('Q');
  kc_SendData('R');
  kc_SendData('S');
  kc_SendData('T');
  kc_SendData('U');
  kc_SendData('V');
  kc_SendData('W');
  kc_SendData('X');
  kc_SendData('Y');
  kc_SendData('Z');
  kc_SendData('0');
  kc_SendData('1');
  kc_SendData('2');
  kc_SendData('3');
  kc_SendData('4');
  kc_SendData('5');
#endif

  return 0;
}

int kc_SendCommand(int command) {

  int temp = 0;

  // STEP1: send D7-D4, rs=0,rw=0,en=1
  // get upper nibble from given command
  temp |= (command & 0x10) << 2; // place D4 in position 6
  temp |= (command & 0x20) << 0; // place D5 in position 5
  temp |= (command & 0x40) >> 2; // place D6 in position 4
  temp |= (command & 0x80) >> 4; // place D7 in position 3
  // en=1, rs=0, rw=0
  temp |= 0x1;
  //print("cmd.h1: 0x%x\r\n", temp);
  kc_Sleep(1);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, temp);
  // en=0, rs=0, rw=0
  temp &= ~(0x1);
  //print("cmd.h2: 0x%x\r\n", temp);
  kc_Sleep(1);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, temp);

  // STEP2: send D3-D0, rs=0,rw=0,en=0
  // get lower nibble from given command
  temp = 0;
  temp |= (command & 0x01) << 6; // place D0 in position 6
  temp |= (command & 0x02) << 4; // place D1 in position 5
  temp |= (command & 0x04) << 2; // place D2 in position 4
  temp |= (command & 0x08) >> 0; // place D3 in position 3
  // en=1, rs=0, rw=0
  temp |= 0x1;
  //print("cmd.l1: 0x%x\r\n", temp);
  kc_Sleep(1);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, temp);
  // en=0, rs=0, rw=0
  temp &= ~(0x1);
  //print("cmd.l2: 0x%x\r\n\r\n", temp);
  kc_Sleep(1);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, temp);

  // wait for busy / delay
  kc_Sleep(1);
  return 0;
}

int kc_SendData(int data) {

  int temp = 0;

  // STEP1: send D7-D4, rs=0,rw=0,en=1
  // get upper nibble from given command
  temp |= (data & 0x10) << 2; // place D4 in position 6
  temp |= (data & 0x20) << 0; // place D5 in position 5
  temp |= (data & 0x40) >> 2; // place D6 in position 4
  temp |= (data & 0x80) >> 4; // place D7 in position 3
  // en=1, rs=1, rw=0
  temp |= 0x3;
  //print("dat.h1: 0x%x\r\n", temp);
  kc_Sleep(1);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, temp);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, temp); // to introduce little delay
  // en=0, rs=1, rw=0
  temp &= ~(0x1);
  //print("dat.h2: 0x%x\r\n", temp);
  kc_Sleep(1);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, temp);

  // STEP2: send D3-D0, rs=0,rw=0,en=0
  // get lower nibble from given command
  temp = 0;
  temp |= (data & 0x01) << 6; // place D0 in position 6
  temp |= (data & 0x02) << 4; // place D1 in position 5
  temp |= (data & 0x04) << 2; // place D2 in position 4
  temp |= (data & 0x08) >> 0; // place D3 in position 3
  // en=1, rs=1, rw=0
  temp |= 0x3;
  //print("dat.l1: 0x%x\r\n", temp);
  kc_Sleep(1);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, temp);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, temp); // to introduce little delay
  // en=0, rs=1, rw=0
  temp &= ~(0x1);
  //print("dat.l2: 0x%x\r\n\r\n", temp);
  kc_Sleep(1);
  XGpio_WriteReg(LCD_BASEADDR, LCD_DATA_OFFSET, temp);

  // wait for busy / delay
  kc_Sleep(1);
  return 0;
}

int kc_FunctionSet(int bits, int lines) {
  int command = 0x28; // default: 4bits, 2lines
  if ((bits == 8) && (lines == 1)) command = 0x30;
  if ((bits == 8) && (lines == 2)) command = 0x38;
  if ((bits == 4) && (lines == 1)) command = 0x20;
  if ((bits == 4) && (lines == 2)) command = 0x28;
  kc_SendCommand(command);
  return 0;
}

int kc_EntryMode(int mode) {
  int command = 0x6; // no shift, auto increment
  kc_SendCommand(command);
  return 0;
}

int kc_CursorMode(int mode) {
  int command = 0xe; // default: cursor ON
  if (mode == 0) command = 0x0c; // display ON, cursor OFF
  if (mode == 1) command = 0x0e; // display ON, cursor ON
  if (mode == 2) command = 0x0f; // display ON, cursor BLINK
  kc_SendCommand(command);
  return 0;
}

int kc_DisplayMode(int mode) {
  int command = 0x8; // display OFF, cursor OFF (DDRAM content intact)
  if (mode == 0) command = 0x08; // display OFF, DDRAM content INTACT
  if (mode == 1) command = 0x01; // display OFF, DDRAM content CLEAR
  kc_SendCommand(command);
  return 0;
}

int kc_ShiftMode(int mode) {
  int command = 0x18; // default: shift left
  if (mode == 0) command = 0x18; // shift left
  if (mode == 1) command = 0x1c; // shift right
  if (mode == 2) command = 0x10; // move cursor left by 1 position
  if (mode == 3) command = 0x14; // move cursor right by 1 position
  kc_SendCommand(command);
  return 0;
}

void kc_SetCursor(int row, int col) {

  int command;

  if (row == 0) {
    command = 0x80;
  } else if (row == 1) {
    command = 0xc0;
  }

  kc_SendCommand(command + col);
}

int kc_PrintLine(const char* line) {

  char c = 0;

  for (int i = 0; i < 16; i++) {
    c = line[i];
    if (c == '\0') {
      xil_printf("\n");
      break;
    } else {
      xil_printf("%c", c);
      kc_SendData(c);
    }
  }

  return 0;
}

int kc_PrintTwoLines(const char* line1, const char* line2) {
  kc_SetCursor(0, 0);
  kc_PrintLine(line1);
  kc_SetCursor(1, 0);
  kc_PrintLine(line2);
  return 0;
}

void kc_Sleep(int delay) {
  for (volatile int i = 0; i < 2500*delay; i++) {}
  return;
}
