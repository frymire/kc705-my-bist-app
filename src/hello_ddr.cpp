
/*
 * Performs the following memory tests:
 *  - all zeroes
 *  - all ones
 *  - stuck-together row/column
 *  - maximum band address/row/column noise
 *  - Data = !Address
 */

#include "kc705_my_bist.h"
#include "xil_printf.h"
#include "xil_cache.h"

// TODO: Move this to the top-level header file. Test 3 is written incorrectly, so we have to
// trick it into thinking the DDR_BASE_ADDR is higher than it is, rather than use the offset.
//#define DDR_BASE_ADDR XPAR_MIG7SERIES_0_BASEADDR
#define DDR_BASE_ADDR	(XPAR_MIG7SERIES_0_BASEADDR + 0x00080000)

/*
 * Specify an offset from the memory base address from which to run the memory tests, to avoid overwriting
 * data on the stack.
 *
 * Default = 0x00080000 (512 KB). Generally, set it to a power of 2 that is larger than the size of this
 * program. Also, it should be set to a value less than half the size of the DDR memory for the
 * 'Maximum Bank Address/Row/Column noise' test (DDR_TEST_3) and to a value less than the size of the
 * memory for remaining tests.
 */
#define DDR_MEM_OFFSET 0x00000000
//#define DDR_MEM_OFFSET 0x00080000

// Define the DDR tests to run.
#define DDR_TEST_0 0x0001 // write all zeros and verify
#define DDR_TEST_1 0x0002 // write all ones and verify
#define DDR_TEST_2 0x0004 // test for stuck together row/col bits
#define DDR_TEST_3 0x0008 // maximum BA/ROW/COL noise
#define DDR_TEST_4 0x0010 // data = !address
#define DDR_TEST (DDR_TEST_0 | DDR_TEST_1 | DDR_TEST_2 | DDR_TEST_3 | DDR_TEST_4)

// Define the cache options to include.
#define DDR_CACHE_TEST_0	0x0001 /* ICache: ON, DCache: ON */
#define DDR_CACHE_TEST_1	0x0002 /* ICache: OFF, DCache: OFF */
#define DDR_CACHE_TEST_2	0x0004 /* ICache: ON, DCache: OFF */
#define DDR_CACHE_TEST_3	0x0008 /* ICache: OFF, DCache: ON */
#define DDR_CACHE_TEST (DDR_CACHE_TEST_0 | DDR_CACHE_TEST_1)
//#define DDR_CACHE_TEST (DDR_CACHE_TEST_0 | DDR_CACHE_TEST_1 | DDR_CACHE_TEST_2 | DDR_CACHE_TEST_3)

// Specifies how many passes of the test to run, -1 == infinite.
#define DDR_NUM_ITERATIONS 1

// Macros to read and write words directly to memory.
#define WR_WORD(ADDR, DATA)	(*(volatile u32*)(ADDR) = (DATA))
#define RD_WORD(ADDR, DATA)	((DATA) = *(volatile u32*)(ADDR))

static int RunSelectedTests(u32 BaseAddr, u32 HighAddr);
static u32 V6_DdrxMemTest(u32 BaseAddr, u32 HighAddr);

// The read buffer is defined as global to avoid having a large buffer on the stack.
#if (DDR_TEST & DDR_TEST_3)
u32 DataBuffer[32];
#endif

int hello_ddr(void) {

	xil_printf("\n********************************************************");
	xil_printf("\n********************************************************");
	xil_printf("\n**     KC705 - AXI 7-Series DDRx Test                 **");
	xil_printf("\n********************************************************");
	xil_printf("\n********************************************************\n");

  int* p_ddr = (int*) DDR_BASE_ADDR;

  // The first access to the DDR is painfully slow. Just to show that the subsequent
  // accesses are fast, do one access just to wake it up.
  xil_printf("\nWakeup!\n");
  int dummy_value = p_ddr[0];
  xil_printf("Done (read %d).\n", dummy_value);

  xil_printf("\nWriting...\n");
  for (int i = 0; i < 10; i++) { *(p_ddr + i) = i*i; } // pointer arithmetic notation
  xil_printf("Done writing.\n");

  xil_printf("Reading...\n");
  for (int i = 9; i >= 0; i--) { xil_printf("%d ", p_ddr[i]); } // array notation
  xil_printf("\nDone reading.\n");

	u32 total_errors;
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_HIGH_ADDR); // 1 GB (original)
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x20000000); // 512 MB
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x10000000); // 256 MB
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x8000000); // 128 MB
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x1000000); // 16 MB
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x800000); // 8 MB
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x400000); // 4 MB
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x200000); // 2 MB
	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x100000); // 1 MB
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x40000); // 256 KB
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x10000); // 64 KB
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x8000); // 32 KB
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x2000); // 8 KB
//	total_errors = RunSelectedTests(DDR_BASE_ADDR, DDR_BASE_ADDR + 0x1000); // 4 KB

	if (total_errors) {
		xil_printf("\n### Finished with errors ###\n");
	} else {
		xil_printf("\n### Finished successfully ###\n");
	}
	return total_errors;
}


// Returns the numbers of errors detected after running all selected memory tests.
static int RunSelectedTests(u32 base_address, u32 high_address) {

	xil_printf("\nTesting address range 0x%08X-0x%08X.\n", base_address + DDR_MEM_OFFSET, high_address);

	u32 num_test_errors = 0;
	u32 total_errors = 0;

	// Run each enabled cache option.

#if (DDR_CACHE_TEST & DDR_CACHE_TEST_0)
	Xil_ICacheEnable();
	Xil_DCacheEnable();
	//Xil_DCacheInvalidate();
	xil_printf("\nICache: On, DCache: On\n");
	num_test_errors = V6_DdrxMemTest(base_address, high_address);
	xil_printf("\n\tNumber of errors in this test = %d\n", num_test_errors);
	total_errors += num_test_errors;
#endif

#if (DDR_CACHE_TEST & DDR_CACHE_TEST_1)
	Xil_ICacheDisable();
	Xil_DCacheDisable();
	xil_printf("\nICache: Off, DCache: Off\n");
	num_test_errors = V6_DdrxMemTest(base_address, high_address);
	xil_printf("\n\tNumber of errors in this test = %d\n", num_test_errors);
	total_errors += num_test_errors;
#endif

#if (DDR_CACHE_TEST & DDR_CACHE_TEST_2)
	Xil_ICacheEnable();
	Xil_DCacheDisable();
	xil_printf("\nICache:  On, DCache: Off\n");
	num_test_errors = V6_DdrxMemTest(base_address, high_address);
	xil_printf("\n\tNumber of errors in this test = %d\n", num_test_errors);
	total_errors += num_test_errors;
#endif

#if (DDR_CACHE_TEST & DDR_CACHE_TEST_3)
	Xil_ICacheDisable();
	Xil_DCacheEnable();
	xil_printf("\nICache: Off, DCache: On\n");
	num_test_errors = V6_DdrxMemTest(base_address, high_address);
	xil_printf("\n\tNumber of errors in this test = %d\n", num_test_errors);
	total_errors += num_test_errors;
#endif

	xil_printf("\nMemory test ");
	if (total_errors == 0) {
		xil_printf("PASSED.\n");
	} else {
		xil_printf("FAILED.\n");
	}
		
	xil_printf("Total number of errors for all tests = %d\n", total_errors);
	return total_errors;
}

// Returns the numbers of errors detected after running selected memory tests for a given cache option setting.
static u32 V6_DdrxMemTest(u32 base_address, u32 high_address) {

	u32 address;
	u32 start_address;
	u32 memory_size;
	u32 range;
	u32 offset;
	u32 num_errors = 0;
	u32 total_errors = 0;
	u32 data;
	u32 WriteAdr;
	u32 i;
	u32 j;

	offset = DDR_MEM_OFFSET;
	start_address = base_address + offset;
	memory_size = high_address - base_address + 1;
	range = memory_size - offset;


#if (DDR_TEST & DDR_TEST_0) // Write all zeros and verify.

	xil_printf("\n\tTEST0: Write all memory to 0x00000000 and check.\n");

	if (offset > memory_size) {
		xil_printf("\tMemory offset can't be greater than the size of the memory.\n");
		return ++num_errors;
	}

	xil_printf("\t\tWriting...\n");
	for (address = start_address; address < start_address + range; address += 4) {
		WR_WORD(address, 0x00000000);
	}

	xil_printf("\t\tReading...\n");
	for (address = start_address; address < start_address + range; address += 4) {
		RD_WORD(address, data);
		if (data != 0x00000000) {
			num_errors++;
			xil_printf("\t\tTEST0 - ERROR #%d: Address = 0x%08x, Expected 0x00000000, Received 0x%08x\n", num_errors, address, data);
		}
	}

	xil_printf("\t\tTest Complete. Status = ");
	if (num_errors) {
		xil_printf("FAILURE\n");
	} else {
		xil_printf("SUCCESS\n");
	}

	total_errors += num_errors;

#endif /* DDR_TEST & DDR_TEST_0 */


#if (DDR_TEST & DDR_TEST_1) // Write all ones and verify.

	xil_printf("\n\tTEST1: Write all memory to 0xFFFFFFFF and check\n\t\tWriting...\n");

	if (offset > memory_size) {
		xil_printf("\tMemory Offset can't be greater than the size of the memory\n");
		return ++num_errors;
	}

	// Set memory range to 0xFFFFFFFF.
	for (address = start_address; address < start_address + range; address += 4) {
		WR_WORD(address, 0xFFFFFFFF);
	}

	xil_printf("\t\tReading...\n");

	for (address = start_address; address < start_address + range; address += 4) {
		RD_WORD(address, data);

		if (data != 0xFFFFFFFF) {
			num_errors++;
			xil_printf("\t\tTEST1 - ERROR #%d: Address = 0x%08x, Data Expected was 0xFFFFFFFF, Data Received was 0x%08x\n",
					num_errors, address, data);
		}
	}

	xil_printf("\t\tTest Complete. Status = ");
	if (num_errors) {
		xil_printf("FAILURE\n");
	} else {
		xil_printf("SUCCESS\n");
	}

	total_errors += num_errors;

#endif /* DDR_TEST & DDR_TEST_1 */


#if (DDR_TEST & DDR_TEST_2) // Test for stuck together row/col bits.

	xil_printf("\n\tTEST2: Testing for stuck together bank/row/col bits\n");

	if (offset > memory_size) {
		xil_printf("\tMemory Offset can't be greater than the size of the memory\n");
		return ++num_errors;
	}

	// Clear all memory.
	xil_printf("\t\tClearing memory to zeros...\n");
	for (address = start_address; address < start_address + range; address += 4) {
		WR_WORD(address, 0x00000000);
	}

	xil_printf("\t\tWriting and Reading...\n");

	for (i = 1, WriteAdr = start_address;
			 WriteAdr < start_address + range;
			 ++i, WriteAdr = start_address | (u32) (1 << i) ) {

		// Write out the value we'll be checking for later.
		WR_WORD(WriteAdr, 0xFFFFFFFF);

		for (j = 1, address = start_address; address < start_address + range;
				++j, address = start_address | (u32) (1 << j)) {

			// Read back a word of data.
			RD_WORD(address, data);

			// Check to make sure it is zero.
			if ((data != 0) && (WriteAdr != address)) {
				num_errors++;
				xil_printf("\t\tTEST2 - ERROR #%d:, Address = 0x%08x, Data Expected was 0x00000000, Data Received was 0x%08x\n",
						num_errors, address, data);
			}
		}

		// Clean out the old value before pointer update.
		WR_WORD(WriteAdr, 0x00000000);
	}

	xil_printf("\t\tTest Complete. Status = ");
	if (num_errors) {
		xil_printf("FAILURE\n");
	} else {
		xil_printf("SUCCESS\n");
	}

	total_errors += num_errors;
#endif /* DDR_TEST & DDR_TEST_2 */

	/*
	 * DDR_TEST_3 - MAX BA/ROW/COL NOISE.
	 */
#if (DDR_TEST & DDR_TEST_3)

	xil_printf("\n\tTEST3: Testing for maximum ba/row/col noise\n");
	xil_printf("\t\tThis test performs 16 word writes followed by 16 word reads\n");
	xil_printf("\t\tEach 64 bytes inverts the ba/row/col address\n\t\tInitializing Memory to 0xA5A5A5A5...\n");

	if (offset > (memory_size / 2)) {
		xil_printf("\tMemory Offset can't be greater than half the size of the memory\n");
		return ++num_errors;
	}


	// Calculate bit masks for Column, Row and Bank addresses. We are assuming the memory
	// is a power of 2 and even alignment.

	u16 MemWidth = DDR_MEM_WIDTH;
	u16 StartBit = (MemWidth == 8 ? 0 : MemWidth == 16 ? 1 : MemWidth == 32 ? 2 : MemWidth == 64 ? 3 : 4);
	u16 ColAddrMask = (((1 << DDR_COL_WIDTH) - 1) << (StartBit)) & (memory_size - 1);
	u16 RowAddrMask = (((1 << DDR_ROW_WIDTH) - 1) << (StartBit + DDR_COL_WIDTH)) & (memory_size - 1);
	u16 BankAddrMask = (((1 << DDR_BANK_WIDTH) - 1) << (StartBit + DDR_COL_WIDTH + DDR_ROW_WIDTH)) & (memory_size - 1);

	// Set the range of memory to 0xA5A5A5A5.
	for (address = start_address; address < start_address + range; address += 4) {
		WR_WORD(address, 0xA5A5A5A5);
	}

	// Setup initial values.
	WriteAdr = start_address;
	address = WriteAdr;

	xil_printf("\t\tWriting and Reading...\n");

	// Ping-pong and meet in middle.
	u32 BankAddr, Row, Col;
	for (i = 0; i < ((range - offset) / 128); i++) {
		BankAddr = WriteAdr & BankAddrMask;
		Row = WriteAdr & RowAddrMask;
		Col = WriteAdr & ColAddrMask & 0xFFFFFFFC;

		// Write out 64 bytes.
		for (j = 0; j < 8; j++) {
			WR_WORD(WriteAdr, 0xFFFFFFFF);
			WR_WORD(WriteAdr + 4, 0x00000000);
			WriteAdr += 8;
		}

		WriteAdr = address;

		// Read back the 64 bytes.
		for (j = 0; j < 16; j++) {
			RD_WORD(WriteAdr, DataBuffer[j]);
			WriteAdr += 4;
		}

		WriteAdr = address;

		// Compare the results to the expected values now....
		for (j = 0; j < 16; j += 2) {
			if (DataBuffer[j] != 0xFFFFFFFF) {
				num_errors++;
				xil_printf("\t\tTEST3.1 - ERROR #%d: Address = 0x%08x, Data Expected was 0xFFFFFFFF, Data Received was 0x%08x\n",
						num_errors, (address + 4 * j), DataBuffer[j]);
			}

			if (DataBuffer[j + 1] != 0x00000000) {
				num_errors++;
				xil_printf("\t\tTEST3.2 - ERROR #%d: Address = 0x%08x, Data Expected was 0x00000000, Data Received was 0x%08x\n",
						num_errors, address + (4 * j) + 4, DataBuffer[j + 1]);
			}
		}

		// Calculate the inverse address for DDR SDRAM.
		BankAddr = ~BankAddr & BankAddrMask;
		Row = ~Row & RowAddrMask;
		Col = ~Col & ColAddrMask & 0xFFFFFFFC;
		WriteAdr = (BankAddr | Row | Col) - 60 + base_address;
		address = WriteAdr;

		// Write out 64 bytes.
		for (j = 0; j < 8; j++) {
			WR_WORD(WriteAdr, 0xFFFFFFFF);
			WR_WORD(WriteAdr + 4, 0x00000000);
			WriteAdr += 8;
		}

		WriteAdr = address;

		// Read back the 64 bytes.
		for (j = 16; j < 32; j++) {
			RD_WORD(WriteAdr, DataBuffer[j]);
			WriteAdr += 4;
		}

		// Lets compare the results to the expected values now....
		for (j = 16; j < 32; j += 2) {
			if (DataBuffer[j] != 0xFFFFFFFF) {
				num_errors++;
				xil_printf("\t\tTEST3.3 - ERROR #%d: Address = 0x%08x, Data Expected was 0xFFFFFFFF, Data Received was 0x%08x\n",
						num_errors, (address + 4 * (j - 16)), DataBuffer[j]);
			}
			if (DataBuffer[j + 1] != 0x00000000) {
				num_errors++;
				xil_printf("\t\tTEST3.4 - ERROR #%d: Address = 0x%08x, Data Expected was 0xFFFFFFFF, Data Received was 0x%08x\n",
						num_errors, (address + (4 * (j - 16)) + 4), DataBuffer[j + 1]);
			}
		}

		// Calculate the "normal" address for the next cycles.
		BankAddr = ~BankAddr & BankAddrMask;
		Row = ~Row & RowAddrMask;
		Col = ~Col & ColAddrMask & 0xFFFFFFFC;

		// Increase by one 64 byte quantum.
		WriteAdr = (BankAddr | Row | Col) + 64 + base_address;
	}

	xil_printf("\t\tTest Complete. Status = ");
	if (num_errors) {
		xil_printf("FAILURE\n");
	} else {
		xil_printf("SUCCESS\n");
	}

	total_errors += num_errors;

#endif /* DDR_TEST & DDR_TEST_3 */


#if (DDR_TEST & DDR_TEST_4) // DATA = !ADDR test

	xil_printf("\n\tTEST4: Testing for Inverse Data at Address\n");

	if (offset > memory_size) {
		xil_printf("\n\tMemory Offset can't be greater than the size of the memory");
		return ++num_errors;
	}

	xil_printf("\t\tWriting...\n");
	for (address = start_address; address < start_address + range; address += 4) {
		WR_WORD(address, ~address);
	}

	xil_printf("\t\tReading...\n");
	for (address = start_address; address < start_address + range; address += 4) {
		RD_WORD(address, data);

		if (data != ~address) {
			num_errors++;
			xil_printf("\t\t\tTEST4 - ERROR #%d: Address = 0x%08x, Data Expected was 0x%08x, Data Received was 0x%08x\n",
					num_errors, address, ~address, data);
		}
	}

	xil_printf("\t\tTest Complete. Status = ");
	if (num_errors) {
		xil_printf("FAILURE\n");
	} else {
		xil_printf("SUCCESS\n");
	}

	total_errors += num_errors;

#endif /* DDR_TEST & DDR_TEST_4 */

	return total_errors;
}
