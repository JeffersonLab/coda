#ifndef BANDTriggerBoardRegs_H
#define BANDTriggerBoardRegs_H


/* Board Supports VME A32/A24 D32 Accesses (BLT32 only in address range 0x0000-0x0FFC */

#define CLOCK_PERIOD_NS					5
#define	MAX_PRESCALE					1023
#define MAX_DELAY_LONG					1023
#define MAX_DELAY						31
#define MAX_STMULT						24
#define MAX_PERSIST_LONG				255
#define MAX_PERSIST						7

#define BOARDID_A395A 					0x00	// 32CH IN LVDS/ECL INTERFACE
#define BOARDID_A395B 					0x01	// 32CH OUT LVDS INTERFACE
#define BOARDID_A395C 					0x02	// 32CH OUT ECL INTERFACE
#define BOARDID_A395D					0x03	// 8CH I/O SELECT NIM/TTL INTER


/* if only one board, set both addresses to the same value !!! */

#define BAND_BOARD_ADDRESS_1			0x11580000


#define BAND_FW_REVISION				0x1000
#define BAND_CFG_SECTOR					0x1004
#define BAND_TRIG0_SCALER				0x1008

#define BAND_REVISION				    0x2000
#define BAND_ENABLE_SCALERS				0x2004
#define BAND_REF_SCALER				    0x2008

/*scope*/
#define BAND_TRIG_STATUS				0x3000
#define BAND_TRIG_VALUE7				0x3004
#define BAND_TRIG_VALUE6			   	0x3008
#define BAND_TRIG_VALUE5			   	0x300C
#define BAND_TRIG_VALUE4			   	0x3010
#define BAND_TRIG_VALUE3			   	0x3014
#define BAND_TRIG_VALUE2			   	0x3018
#define BAND_TRIG_VALUE1			   	0x301C
#define BAND_TRIG_VALUE0			   	0x3020
#define BAND_TRIG_INGORE7			   	0x3024
#define BAND_TRIG_INGORE6			   	0x3028
#define BAND_TRIG_INGORE5			   	0x302C
#define BAND_TRIG_INGORE4			   	0x3030
#define BAND_TRIG_INGORE3			   	0x3034
#define BAND_TRIG_INGORE2			   	0x3038
#define BAND_TRIG_INGORE1			   	0x303C
#define BAND_TRIG_INGORE0			   	0x3040
#define BAND_TRIG_BUFFER			   	0x3044


/************************************************/
/************** BEGIN SCALER REGISTERS **********/
/************************************************/
/* Notes:
   1) Scalers are all 32bits, BIG-ENDIAN.
   2) BAND_REF_SCALER is a reference scaler which contains gate time of all scalers (in 25ns ticks)
   3) Set TS_ENABLE_SCALERS to '1' to enable scalers. Set to '0' to stop scalers for readout.
      Setting back to '1' will clear all scalers and allow them to count again.
   4) Scalers are capable of counting at 100MHz, which is about 43sec before overflowing at this high rate
*/


#define BAND_U_DELAY_BASE               0x1200
#define BAND_V_DELAY_BASE               0x1400
#define BAND_W_DELAY_BASE               0x1600

/*
#define BAND_U_SCALER_BASE              0x1800
#define BAND_V_SCALER_BASE              0x1A00
#define BAND_W_SCALER_BASE              0x1C00
*/

#define BAND_D1_SCALER_BASE              0x1800
#define BAND_D2_SCALER_BASE              0x1840
#define BAND_D3_SCALER_BASE              0x1880
#define BAND_D4_SCALER_BASE              0x18c0
#define BAND_D5_SCALER_BASE              0x1900
#define BAND_D6_SCALER_BASE              0x1940
#define BAND_D7_SCALER_BASE              0x1980
#define BAND_D8_SCALER_BASE              0x19c0


/************************************************/
/******** BEGIN SECTOR TRIGGER REGISTERS ********/
/************************************************/
/* Notes:
	bits (17:16) ECPCC config
		"00" => ECP and CC
		"01" => CC
		"10" => ECP
		"11" => 0
*/

/****************/
/* DSC2 scalers */

#define BAND_DSC2_ADDRESS_SLOT2			0x00100000
#define BAND_DSC2_ADDRESS_SLOT3			0x00180000
#define BAND_DSC2_ADDRESS_SLOT4			0x00200000
#define BAND_DSC2_ADDRESS_SLOT5			0x00280000
#define BAND_DSC2_ADDRESS_SLOT6			0x00300000
#define BAND_DSC2_ADDRESS_SLOT7			0x00380000
#define BAND_DSC2_ADDRESS_SLOT8			0x00400000
#define BAND_DSC2_ADDRESS_SLOT9			0x00480000
#define BAND_DSC2_ADDRESS_SLOT10		0x00500000
#define BAND_DSC2_ADDRESS_SLOT11		0x00580000
#define BAND_DSC2_ADDRESS_SLOT12		0x00600000
#define BAND_DSC2_ADDRESS_SLOT13		0x00680000
#define BAND_DSC2_ADDRESS_SLOT14		0x00700000
#define BAND_DSC2_ADDRESS_SLOT15		0x00780000
#define BAND_DSC2_ADDRESS_SLOT16		0x00800000
#define BAND_DSC2_ADDRESS_SLOT17		0x00880000
#define BAND_DSC2_ADDRESS_SLOT18		0x00900000
#define BAND_DSC2_ADDRESS_SLOT19		0x00980000
#define BAND_DSC2_ADDRESS_SLOT20		0x00A00000


#define BAND_DSC2_SCALER_LATCH      0x009C
#define BAND_DSC2_SCALER_GATE       0x00BC
#define BAND_DSC2_SCALER_BASE       0x0100
#define BAND_DSC2_SCALER_REF        0x0204

#endif
