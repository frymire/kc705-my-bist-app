///* $Id: xaxiethernet_example_util.c,v 1.1.2.1 2011/08/29 09:03:21 anirudh Exp $ */
///******************************************************************************
//*
//* (c) Copyright 2010 Xilinx, Inc. All rights reserved.
//*
//* This file contains confidential and proprietary information of Xilinx, Inc.
//* and is protected under U.S. and international copyright and other
//* intellectual property laws.
//*
//* DISCLAIMER
//* This disclaimer is not a license and does not grant any rights to the
//* materials distributed herewith. Except as otherwise provided in a valid
//* license issued to you by Xilinx, and to the maximum extent permitted by
//* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
//* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
//* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
//* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
//* and (2) Xilinx shall not be liable (whether in contract or tort, including
//* negligence, or under any other theory of liability) for any loss or damage
//* of any kind or nature related to, arising under or in connection with these
//* materials, including for any direct, or any indirect, special, incidental,
//* or consequential loss or damage (including loss of data, profits, goodwill,
//* or any type of loss or damage suffered as a result of any action brought by
//* a third party) even if such damage or loss was reasonably foreseeable or
//* Xilinx had been advised of the possibility of the same.
//*
//* CRITICAL APPLICATIONS
//* Xilinx products are not designed or intended to be fail-safe, or for use in
//* any application requiring fail-safe performance, such as life-support or
//* safety devices or systems, Class III medical devices, nuclear facilities,
//* applications related to the deployment of airbags, or any other applications
//* that could lead to death, personal injury, or severe property or
//* environmental damage (individually and collectively, "Critical
//* Applications"). Customer assumes the sole risk and liability of any use of
//* Xilinx products in Critical Applications, subject only to applicable laws
//* and regulations governing limitations on product liability.
//*
//* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
//* AT ALL TIMES.
//*
//******************************************************************************/
///**
//*
//* @file xaxiethernet_example_util.c
//*
//* This file implements the utility functions for the Axi Ethernet example code.
//*
//* <pre>
//* MODIFICATION HISTORY:
//*
//* Ver   Who  Date     Changes
//* ----- ---- -------- -------------------------------------------------------
//* 1.00a asa  6/30/10 First release based on the ll temac driver
//* </pre>
//*
//******************************************************************************/
//
///***************************** Include Files *********************************/
//
//#include "xaxiethernet_example.h"
//
//
///************************** Variable Definitions ****************************/
//
///*
// * Local MAC address
// */
//char AxiEthernetMAC[6] = { 0x00, 0x0A, 0x35, 0x01, 0x02, 0x03 };
//
///******************************************************************************/
///**
//*
//* Set the MAC addresses in the frame.
//*
//* @param	FramePtr is the pointer to the frame.
//* @param	DestAddr is the Destination MAC address.
//*
//* @return	None.
//*
//* @note		None.
//*
//******************************************************************************/
//void AxiEthernetUtilFrameHdrFormatMAC(EthernetFrame *FramePtr, char *DestAddr)
//{
//	char *Frame = (char *) FramePtr;
//	char *SourceAddress = AxiEthernetMAC;
//	int Index;
//
//	/*
//	 * Destination address
//	 */
//	for (Index = 0; Index < XAE_MAC_ADDR_SIZE; Index++) {
//		*Frame++ = *DestAddr++;
//	}
//
//	/*
//	 * Source address
//	 */
//	for (Index = 0; Index < XAE_MAC_ADDR_SIZE; Index++) {
//		*Frame++ = *SourceAddress++;
//	}
//}
//
///******************************************************************************/
///**
//*
//* Set the frame type for the specified frame.
//*
//* @param	FramePtr is the pointer to the frame.
//* @param	FrameType is the Type to set in frame.
//*
//* @return	None.
//*
//* @note		None.
//*
//******************************************************************************/
//void AxiEthernetUtilFrameHdrFormatType(EthernetFrame *FramePtr, u16 FrameType)
//{
//	char *Frame = (char *) FramePtr;
//
//	/*
//	 * Increment to type field
//	 */
//	Frame = Frame + 12;
//
//	FrameType = Xil_Htons(FrameType);
//	/*
//	 * Set the type
//	 */
//	*(u16 *) Frame = FrameType;
//}
//
///******************************************************************************/
///**
//* This function places a pattern in the payload section of a frame. The pattern
//* is a  8 bit incrementing series of numbers starting with 0.
//* Once the pattern reaches 256, then the pattern changes to a 16 bit
//* incrementing pattern:
//* <pre>
//*   0, 1, 2, ... 254, 255, 00, 00, 00, 01, 00, 02, ...
//* </pre>
//*
//* @param	FramePtr is a pointer to the frame to change.
//* @param	PayloadSize is the number of bytes in the payload that will be
//*		set.
//*
//* @return	None.
//*
//* @note		None.
//*
//******************************************************************************/
//void AxiEthernetUtilFrameSetPayloadData(EthernetFrame *FramePtr,
//							int PayloadSize)
//{
//	unsigned BytesLeft = PayloadSize;
//	u8 *Frame;
//	u16 Counter = 0;
//
//	/*
//	 * Set the frame pointer to the start of the payload area
//	 */
//	Frame = (u8 *) FramePtr + XAE_HDR_SIZE;
//
//	/*
//	 * Insert 8 bit incrementing pattern
//	 */
//	while (BytesLeft && (Counter < 256)) {
//		*Frame++ = (u8) Counter++;
//		BytesLeft--;
//	}
//
//	/*
//	 * Switch to 16 bit incrementing pattern
//	 */
//	while (BytesLeft) {
//		*Frame++ = (u8) (Counter >> 8);	/* high */
//		BytesLeft--;
//
//		if (!BytesLeft)
//			break;
//
//		*Frame++ = (u8) Counter++;	/* low */
//		BytesLeft--;
//	}
//}
//
///******************************************************************************/
///**
//*
//* Set the frame VLAN info for the specified frame.
//*
//* @param	FramePtr is the pointer to the frame.
//* @param	VlanNumber is the VlanValue insertion position to set in frame.
//* @param	Vid  is the 4 bytes Vlan value (TPID, Priority, CFI, VID)
//*		to be set in frame.
//*
//* @return	None.
//*
//* @note		None.
//*
//******************************************************************************/
//void AxiEthernetUtilFrameHdrVlanFormatVid(EthernetFrame *FramePtr,
//						u32 VlanNumber,	u32 Vid)
//{
//	char *Frame = (char *) FramePtr;
//
//	/*
//	 * Increment to type field
//	 */
//	Frame = Frame + 12 + (VlanNumber * 4);
//
//	Vid = Xil_Htonl(Vid);
//
//	/*
//	 * Set the type
//	 */
//	*(u32 *) Frame = Vid;
//}
//
///******************************************************************************/
///**
//*
//* Set the frame type for the specified frame.
//*
//* @param	FramePtr is the pointer to the frame.
//* @param	FrameType is the Type to set in frame.
//* @param	VlanNumber is the VLAN friendly adjusted insertion position to
//*		set in frame.
//*
//* @return	None.
//*
//* @note		None.
//*
//******************************************************************************/
//void AxiEthernetUtilFrameHdrVlanFormatType(EthernetFrame *FramePtr,
//						u16 FrameType, u32 VlanNumber)
//{
//	char *Frame = (char *) FramePtr;
//
//	/*
//	 * Increment to type field
//	 */
//	Frame = Frame + 12 + (VlanNumber * 4);
//
//	FrameType = Xil_Htons(FrameType);
//
//	/*
//	 * Set the type
//	 */
//	*(u16 *) Frame = FrameType;
//}
//
///******************************************************************************/
///**
//* This function places a pattern in the payload section of a frame. The pattern
//* is a  8 bit incrementing series of numbers starting with 0.
//* Once the pattern reaches 256, then the pattern changes to a 16 bit
//* incrementing pattern:
//* <pre>
//*   0, 1, 2, ... 254, 255, 00, 00, 00, 01, 00, 02, ...
//* </pre>
//*
//* @param	FramePtr is a pointer to the frame to change.
//* @param	PayloadSize is the number of bytes in the payload that will be set.
//* @param	VlanNumber is the VLAN friendly adjusted insertion position to
//*		set in frame.
//*
//* @return	None.
//*
//* @note		None.
//*
//******************************************************************************/
//void AxiEthernetUtilFrameSetVlanPayloadData(EthernetFrame *FramePtr,
//					int PayloadSize, u32 VlanNumber)
//{
//	unsigned BytesLeft = PayloadSize;
//	u8 *Frame;
//	u16 Counter = 0;
//
//	/*
//	 * Set the frame pointer to the start of the payload area
//	 */
//	Frame = (u8 *) FramePtr + XAE_HDR_SIZE + (VlanNumber * 4);
//
//	/*
//	 * Insert 8 bit incrementing pattern
//	 */
//	while (BytesLeft && (Counter < 256)) {
//		*Frame++ = (u8) Counter++;
//		BytesLeft--;
//	}
//
//	/*
//	 * Switch to 16 bit incrementing pattern
//	 */
//	while (BytesLeft) {
//		*Frame++ = (u8) (Counter >> 8);	/* high */
//		BytesLeft--;
//
//		if (!BytesLeft)
//			break;
//
//		*Frame++ = (u8) Counter++;	/* low */
//		BytesLeft--;
//	}
//}
//
///******************************************************************************/
///**
//* This function verifies the frame data against a CheckFrame.
//*
//* Validation occurs by comparing the ActualFrame to the header of the
//* CheckFrame. If the headers match, then the payload of ActualFrame is
//* verified for the same pattern Util_FrameSetPayloadData() generates.
//*
//* @param	CheckFrame is a pointer to a frame containing the 14 byte header
//*		that should be present in the ActualFrame parameter.
//* @param	ActualFrame is a pointer to a frame to validate.
//*
//* @return	- XST_SUCCESS if successful.
//*		- XST_FAILURE in case of failure.
//*
//* @note		None.
//*
//******************************************************************************/
//int AxiEthernetUtilFrameVerify(EthernetFrame * CheckFrame,
//			 EthernetFrame * ActualFrame)
//{
//	unsigned char *CheckPtr = (unsigned char *) CheckFrame;
//	unsigned char *ActualPtr = (unsigned char *) ActualFrame;
//	u16 BytesLeft;
//	u16 Counter;
//	int Index;
//
//
//	/*
//	 * Compare the headers
//	 */
//	for (Index = 0; Index < XAE_HDR_SIZE; Index++) {
//		if (CheckPtr[Index] != ActualPtr[Index]) {
//			return XST_FAILURE;
//		}
//	}
//
//	Index = 0;
//
//	BytesLeft = *(u16 *) &ActualPtr[12];
//	BytesLeft = Xil_Ntohs(BytesLeft);
//	/*
//	 * Get the length of the payload, do not use VLAN TPID here.
//	 * TPID needs to be verified.
//	 */
//	while ((0x8100 == BytesLeft) || (0x88A8 == BytesLeft) ||
//	       (0x9100 == BytesLeft) || (0x9200 == BytesLeft)) {
//		Index++;
//		BytesLeft = *(u16 *) &ActualPtr[12+(4*Index)];
//		BytesLeft = Xil_Ntohs(BytesLeft);
//	}
//
//	/*
//	 * Validate the payload
//	 */
//	Counter = 0;
//	ActualPtr = &ActualPtr[14+(4*Index)];
//
//	/*
//	 * Check 8 bit incrementing pattern
//	 */
//	while (BytesLeft && (Counter < 256)) {
//		if (*ActualPtr++ != (u8) Counter++) {
//
//			return XST_FAILURE;
//		}
//		BytesLeft--;
//	}
//
//	/*
//	 * Check 16 bit incrementing pattern
//	 */
//	while (BytesLeft) {
//		if (*ActualPtr++ != (u8) (Counter >> 8)) {	/* high */
//			return XST_FAILURE;
//		}
//
//		BytesLeft--;
//
//		if (!BytesLeft)
//			break;
//
//		if (*ActualPtr++ != (u8) Counter++) {	/* low */
//			return XST_FAILURE;
//		}
//
//		BytesLeft--;
//	}
//
//	return XST_SUCCESS;
//}
//
///******************************************************************************/
///**
//* This function sets all bytes of a frame to 0.
//*
//* @param	FramePtr is a pointer to the frame itself.
//*
//* @return	None.
//*
//* @note		None.
//*
//******************************************************************************/
//void AxiEthernetUtilFrameMemClear(EthernetFrame * FramePtr)
//{
//	u32 *Data32Ptr = (u32 *) FramePtr;
//	u32 WordsLeft = sizeof(EthernetFrame) / sizeof(u32);
//
//	/*
//	 * Frame should be an integral number of words
//	 */
//	while (WordsLeft--) {
//		*Data32Ptr++ = 0;
//	}
//}
//
///******************************************************************************/
///**
//*
//* This function detects the PHY address by looking for successful MII status
//* register contents (PHY register 1). It looks for a PHY that supports
//* auto-negotiation and 10Mbps full-duplex and half-duplex.  So, this code
//* won't work for PHYs that don't support those features, but it's a bit more
//* general purpose than matching a specific PHY manufacturer ID.
//*
//* Note also that on some (older) Xilinx ML4xx boards, PHY address 0 does not
//* properly respond to this query.  But, since the default is 0 and asssuming
//* no other address responds, then it seems to work OK.
//*
//* @param	The Axi Ethernet driver instance
//*
//* @return	The address of the PHY (defaults to 0 if none detected)
//*
//* @note		None.
//*
//******************************************************************************/
///* Use MII register 1 (MII status register) to detect PHY */
//#define PHY_DETECT_REG  1
//
///* Mask used to verify certain PHY features (or register contents)
// * in the register above:
// *  0x1000: 10Mbps full duplex support
// *  0x0800: 10Mbps half duplex support
// *  0x0008: Auto-negotiation support
// */
//#define PHY_DETECT_MASK 0x1808
//
//u32 AxiEthernetDetectPHY(XAxiEthernet * AxiEthernetInstancePtr)
//{
//	u16 PhyReg;
//	int PhyAddr;
//
//	for (PhyAddr = 31; PhyAddr >= 0; PhyAddr--) {
//		XAxiEthernet_PhyRead(AxiEthernetInstancePtr, PhyAddr,
//				 		PHY_DETECT_REG, &PhyReg);
//
//		if ((PhyReg != 0xFFFF) &&
//		   ((PhyReg & PHY_DETECT_MASK) == PHY_DETECT_MASK)) {
//			/* Found a valid PHY address */
//			return PhyAddr;
//		}
//	}
//
//	return 0;		/* Default to zero */
//}
//
///******************************************************************************/
///**
//* Set PHY to loopback mode. This works with the marvell PHY common on ML40x
//* evaluation boards
//*
//* @param Speed is the loopback speed 10, 100, or 1000 Mbit
//*
//******************************************************************************/
//#define PHY_R0_RESET         0x8000
//#define PHY_R0_LOOPBACK      0x4000
//#define PHY_R0_ANEG_ENABLE   0x1000
//#define PHY_R0_DFT_SPD_MASK  0x2040
//#define PHY_R0_DFT_SPD_10    0x0000
//#define PHY_R0_DFT_SPD_100   0x2000
//#define PHY_R0_DFT_SPD_1000  0x0040
//
//
///******************************************************************************/
///**
//*
//* This function sets the PHY to loopback mode. This works with the marvell PHY
//* common on ML40x evaluation boards.
//*
//* @param	AxiEthernetInstancePtr is a pointer to the instance of the
//*		AxiEthernet component.
//* @param	Speed is the loopback speed 10, 100, or 1000 Mbit.
//*
//* @return	- XST_SUCCESS if successful.
//*		- XST_FAILURE, in case of failure..
//*
//* @note		None.
//*
//******************************************************************************/
//int AxiEthernetUtilEnterLoopback(XAxiEthernet *AxiEthernetInstancePtr,
//								int Speed)
//{
//	u16 PhyReg0;
//	signed int PhyAddr;
//
//	/* Detect the PHY address */
//	PhyAddr = AxiEthernetDetectPHY(AxiEthernetInstancePtr);
//
//	/* Clear the PHY of any existing bits by zeroing this out */
//	PhyReg0 = 0;
//
//	switch (Speed) {
//	case XAE_SPEED_10_MBPS:
//		PhyReg0 |= PHY_R0_DFT_SPD_10;
//		break;
//
//	case XAE_SPEED_100_MBPS:
//		PhyReg0 |= PHY_R0_DFT_SPD_100;
//		break;
//
//	case XAE_SPEED_1000_MBPS:
//		PhyReg0 |= PHY_R0_DFT_SPD_1000;
//		break;
//
//	default:
//		AxiEthernetUtilErrorTrap("Intg_LinkSpeed not 10, 100, or 1000 mbps");
//		return XST_FAILURE;
//	}
//
//	/* Set the speed and put the PHY in reset, then put the PHY in loopback */
//	AxiEthernetUtilPhyDelay(1);
//
//	XAxiEthernet_PhyWrite(AxiEthernetInstancePtr, PhyAddr, 0,
//					PhyReg0 | PHY_R0_RESET);
//	AxiEthernetUtilPhyDelay(AXIETHERNET_PHY_DELAY_SEC);
//	XAxiEthernet_PhyRead(AxiEthernetInstancePtr,PhyAddr, 0,&PhyReg0);
//	XAxiEthernet_PhyWrite(AxiEthernetInstancePtr, PhyAddr, 0,
//					PhyReg0 | PHY_R0_LOOPBACK);
//
//	AxiEthernetUtilPhyDelay(1);
//
//
//	return XST_SUCCESS;
//}
//
///******************************************************************************/
///**
//*
//* This function is called by example code when an error is detected. It
//* can be set as a breakpoint with a debugger or it can be used to print out the
//* given message if there is a UART or STDIO device.
//*
//* @param	Message is the text explaining the error
//*
//* @return	None
//*
//* @note		None
//*
//******************************************************************************/
//void AxiEthernetUtilErrorTrap(char *Message)
//{
//	static int Count = 0;
//
//	Count++;
//
//#ifdef STDOUT_BASEADDRESS
//	xil_printf("%s\r\n", Message);
//#endif
//}
//
///******************************************************************************/
///**
//*
//* For Microblaze we use an assembly loop that is roughly the same regardless of
//* optimization level, although caches and memory access time can make the delay
//* vary.  Just keep in mind that after resetting or updating the PHY modes,
//* the PHY typically needs time to recover.
//*
//* @return	None
//*
//* @note		None
//*
//******************************************************************************/
//void AxiEthernetUtilPhyDelay(unsigned int Seconds)
//{
//	static int WarningFlag = 0;
//
//	/* If MB caches are disabled or do not exist, this delay loop could
//	 * take minutes instead of seconds (e.g., 30x longer).  Print a warning
//	 * message for the user (once).  If only MB had a built-in timer!
//	 */
//	if (((mfmsr() & 0x20) == 0) && (!WarningFlag)) {
//		WarningFlag = 1;
//	}
//
//#define ITERS_PER_SEC   (XPAR_CPU_CORE_CLOCK_FREQ_HZ / 6)
//    asm volatile ("\n"
//			"1:               \n\t"
//			"addik r7, r0, %0 \n\t"
//			"2:               \n\t"
//			"addik r7, r7, -1 \n\t"
//			"bneid  r7, 2b    \n\t"
//			"or  r0, r0, r0   \n\t"
//			"bneid %1, 1b     \n\t"
//			"addik %1, %1, -1 \n\t"
//			:: "i"(ITERS_PER_SEC), "d" (Seconds));
//
//}
//
//
//
