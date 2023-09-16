
#ifndef KC705_MY_BIST_H
#define KC705_MY_BIST_H

#include "xparameters.h"
#include "xil_printf.h"
#include "xintc.h"
#include "xil_cache.h"
#include "xstatus.h"

#include "lcd.h"

const int k_alu_base_address = XPAR_ALU_S_AXI_CRTL_BUS_BASEADDR;

#define BRAM_BASE_ADDR XPAR_BRAM_CTRL_S_AXI_BASEADDR
#define BRAM_BANK_WIDTH 1
#define BRAM_ROW_WIDTH 13
#define BRAM_COL_WIDTH 10
#define BRAM_MEM_WIDTH 32

//#define DDR_BASE_ADDR XPAR_MIG7SERIES_0_BASEADDR
#define DDR_HIGH_ADDR XPAR_MIG7SERIES_0_HIGHADDR
#define DDR_BANK_WIDTH XPAR_MIG7SERIES_0_DDR_BANK_WIDTH
#define DDR_ROW_WIDTH XPAR_MIG7SERIES_0_DDR_ROW_WIDTH
#define DDR_COL_WIDTH XPAR_MIG7SERIES_0_DDR_COL_WIDTH
#define DDR_MEM_WIDTH XPAR_MIG7SERIES_0_DDR_DQ_WIDTH

#define DIP_DEVICE XPAR_DIPS_LCD_DEVICE_ID
#define DIP_CHANNEL 1 // the GPIO channel that the DIP switches are on, either 1 or 2

#define IIC_DEVICE_ID XPAR_IIC_0_DEVICE_ID
#define IIC_INTR_ID XPAR_INTC_0_IIC_0_VEC_ID

#define INTC_DEVICE_ID XPAR_INTC_0_DEVICE_ID

#define LCD_BASEADDR XPAR_DIPS_LCD_BASEADDR
#define LCD_DATA_OFFSET 0x0008
#define LCD_TRISTATE_CONTROL_OFFSET 0x000C

#define LED_GPIO_DEVICE XPAR_LEDS_BUTTONS_DEVICE_ID
const int k_led_channel = 1; // the GPIO channel that the LEDs are on, either 1 or 2

#define MUX_BASEADDR XPAR_MUX_S00_AXI_BASEADDR

#define ROTARY_BASEADDR XPAR_ROTARY_VIO_BASEADDR

#define TMRCTR_DEVICE_ID XPAR_TMRCTR_0_DEVICE_ID
#define TMRCTR_INTERRUPT_ID XPAR_INTC_0_TMRCTR_0_INTERRUPT_VEC_ID

#define UART_BAUDRATE 115200
#define UARTLITE_DEVICE_ID XPAR_UARTLITE_0_DEVICE_ID
#define UARTLITE_INT_IRQ_ID XPAR_INTC_0_UARTLITE_0_VEC_ID

#define VIO_DEVICE XPAR_ROTARY_VIO_DEVICE_ID
#define VIO_CHANNEL 2 // the GPIO channel that the VIO inputs are on, either 1 or 2

int hello_uart();
int hello_led();
int hello_button();
int hello_dip();
int hello_rotary();
int hello_lcd();
int hello_timer();
int hello_i2c();
int hello_spi();
//int FlashProtectionExample();
int hello_bram_init();
int hello_bram();
int hello_ddr();
int hello_vhdl();
int hello_nonmaskable_interrupt();
int hello_nested_interrupt();
int hello_ethernet_lite();
int hello_temac();
int hello_vio();
int hello_counter();
int hello_multiplier();
int hello_cross_trigger();
int hello_rom();
int hello_hls();
int hello_fsm();

#endif // KC705_MY_BIST_H
