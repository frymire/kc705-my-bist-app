
#include "kc705_my_bist.h"

int main() {

  xil_printf("\r\n********************************************************");
  xil_printf("\r\n********************************************************");
  xil_printf("\r\n**     Xilinx Kintex-7 FPGA KC705 Evaluation Kit      **");
  xil_printf("\r\n********************************************************");
  xil_printf("\r\n********************************************************\r\n");

  int choice;
  int exit_flag = 0;

  while (exit_flag != 1) {

    Xil_ICacheEnable();
    Xil_DCacheEnable();

    xil_printf("Choose a feature to test:\r\n");
    xil_printf("1: UART\r\n");
    xil_printf("2: LEDs\r\n");
    xil_printf("3: Buttons\r\n");
    xil_printf("4: Switches\r\n");
    xil_printf("5: Rotary\r\n");
    xil_printf("6: LCD\r\n");
    xil_printf("7: Timer (TODO: Clean this up.)\r\n");
    xil_printf("8: EEPROM via I2C\r\n");
    xil_printf("9: Flash via SPI\r\n");
    xil_printf("A: BRAM Initialization\r\n");
    xil_printf("B: BRAM\r\n");
    xil_printf("C: DDR\r\n");
    xil_printf("D: Custom VHDL\r\n");
    xil_printf("E: Non-Maskable Interrupts (TODO: Fix this.)\r\n");
    xil_printf("F: Nested Interrupts (TODO: Fix this.)\r\n");
    xil_printf("G: Ethernet\r\n");
    xil_printf("H: JTAG-AXI\r\n");
    xil_printf("I: VIO\r\n");
    xil_printf("J: Counter\r\n");
    xil_printf("K: Multiplier\r\n");
    xil_printf("L: Cross-Trigger\r\n");
    xil_printf("M: ROM\r\n");
    xil_printf("N: HLS\r\n");
    xil_printf("O: Finite State Machine\r\n");
    xil_printf("0: Exit\r\n");

    choice = inbyte();
    if (isalpha(choice)) { choice = toupper(choice); }
    xil_printf("%c\r\n", choice);

    switch (choice) {
    case '0':
      exit_flag = 1;
      break;
    case '1':
      hello_uart();
      break;
    case '2':
      hello_led();
      break;
    case '3':
      hello_button();
      break;
    case '4':
      hello_dip();
      break;
    case '5':
      hello_rotary();
      break;
    case '6':
      hello_lcd();
      break;
    case '7':
      hello_timer();
      break;
    case '8':
      hello_i2c();
      break;
    case '9':
      hello_spi();
      break;
    case 'A':
      hello_bram_init();
      break;
    case 'B':
      hello_bram();
      break;
    case 'C':
      hello_ddr();
      break;
    case 'D':
      hello_vhdl();
      break;
    case 'E':
      hello_nonmaskable_interrupt();
      break;
    case 'F':
      xil_printf("Not yet implemented.\r\n");
//      hello_nested_interrupt();
      break;
    case 'G':
      hello_ethernet_lite();
//      hello_temac();
      break;
    case 'H':
      xil_printf("No automated demo. From within Vivado, source Scripts/jtag_axi.tcl.\r\n");
      break;
    case 'I':
      hello_vio();
      break;
    case 'J':
      hello_counter();
      break;
    case 'K':
      hello_multiplier();
      break;
    case 'L':
      hello_cross_trigger();
      break;
    case 'M':
      hello_rom();
      break;
    case 'N':
      hello_hls();
      break;
    case 'O':
      hello_fsm();
      break;
    default:
      break;
    }

    if (exit_flag != 1) {
      xil_printf("\nPress any key to return to main menu.\r\n\r\n");
      inbyte();
    }
  }

  xil_printf("Goodbye!\r\n");
  return 0;
}
