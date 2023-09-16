
#include "kc705_my_bist.h"
#include "xsimple_alu.h"

// See this video: https://www.youtube.com/watch?v=Hf73NBjEB7k&list=PLo7bVbJhQ6qxRyrF7CSW3wXcbmmFl9z7-&index=2

int RunALU(XSimple_alu alu, int a, int b, int op);
int RunALU2(XSimple_alu alu, int a, int b, int op);
void bm_IP_Start();
unsigned int bm_IP_IsDone();

volatile unsigned int* regCrtl = (unsigned int*) (k_alu_base_address + 0x00);
volatile unsigned int* regA = (unsigned int*) (k_alu_base_address + 0x10);
volatile unsigned int* regB = (unsigned int*) (k_alu_base_address + 0x18);
volatile unsigned int* regOp = (unsigned int*) (k_alu_base_address + 0x20);
volatile unsigned int* regC = (unsigned int*) (k_alu_base_address + 0x28);

int hello_hls(void) {

  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**     KC705 - High Level Synthesis Test              **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");
  xil_printf("\nTesting the ALU core generated using HLS...\n");

  XSimple_alu_Config* config = XSimple_alu_LookupConfig(XPAR_ALU_DEVICE_ID);
  if (!config) {
    xil_printf("ALU not found.\n");
    return XST_FAILURE;
  }

  XSimple_alu alu;
  if (XSimple_alu_CfgInitialize(&alu, config) != XST_SUCCESS) {
    xil_printf("ALU initialization failed.\n");
    return XST_FAILURE;
  }

  int a, b, c;

  a = 2;
  b = 3;
  c = RunALU(alu, a, b, 0);
  xil_printf("%d + %d = %d\n", a, b, c);

  a = 7;
  b = 5;
  c = RunALU(alu, a, b, 1);
  xil_printf("%d - %d = %d\n", a, b, c);

  a = 10;
  b = 2;
  c = RunALU(alu, a, b, 2);
  xil_printf("%d * %d = %d\n", a, b, c);

  a = 50;
  b = 5;
  c = RunALU(alu, a, b, 3);
  xil_printf("%d / %d = %d\n", a, b, c);

  xil_printf("\nDo it again using direct register access...\n", a, b, c);

  a = 2;
  b = 3;
  c = RunALU2(alu, a, b, 0);
  xil_printf("%d + %d = %d\n", a, b, c);

  a = 7;
  b = 5;
  c = RunALU2(alu, a, b, 1);
  xil_printf("%d - %d = %d\n", a, b, c);

  a = 10;
  b = 2;
  c = RunALU2(alu, a, b, 2);
  xil_printf("%d * %d = %d\n", a, b, c);

  a = 50;
  b = 5;
  c = RunALU2(alu, a, b, 3);
  xil_printf("%d / %d = %d\n", a, b, c);

  xil_printf("\nDone.\n");
  return 0;
}

int RunALU(XSimple_alu alu, int a, int b, int op) {
  XSimple_alu_Set_a(&alu, a);
  XSimple_alu_Set_b(&alu, b);
  XSimple_alu_Set_op(&alu, op);
  XSimple_alu_Start(&alu);
  while(!XSimple_alu_IsDone(&alu)) {}
  return (int) XSimple_alu_Get_c(&alu);
}

int RunALU2(XSimple_alu alu, int a, int b, int op) {
  *regA = a;
  *regB = b;
  *regOp = op;
  bm_IP_Start();
  while (!bm_IP_IsDone()) {}
  return (int) *regC;
}

void bm_IP_Start() {
  unsigned int data = (*regCrtl & 0x80);
  *regCrtl = (data | 0x01);
}

unsigned int bm_IP_IsDone() {
  unsigned int data = *regCrtl;
  return ((data >> 1) & 0x1);
}
