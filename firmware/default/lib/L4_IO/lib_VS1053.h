#ifndef LIB_VS1053_H
#define LIB_VS1053_H

#define LIB_VS1053_SC_MULT_MASK0 0x0000	// XTALI
#define LIB_VS1053_SC_MULT_MASK1 0x2000 // XTALI x 2.0
#define LIB_VS1053_SC_MULT_MASK2 0x4000 // XTALI x 2.5
#define LIB_VS1053_SC_MULT_MASK3 0x6000 // XTALI x 3.0
#define LIB_VS1053_SC_MULT_MASK4 0x8000 // XTALI x 3.5
#define LIB_VS1053_SC_MULT_MASK5 0xA000 // XTALI x 4.0
#define LIB_VS1053_SC_MULT_MASK6 0xC000 // XTALI x 4.5

#define LIB_VS1053_SC_ADD_MASK0 0x0000	// No modification is allowed
#define LIB_VS1053_SC_ADD_MASK1 0x8000	// XTALI x 1.0
#define LIB_VS1053_SC_ADD_MASK2 0x1000	// XTALI x 1.5
#define LIB_VS1053_SC_ADD_MASK3 0x1800  // XTALI x 2.0

/**
 * @brief Serial control interface (SCI) registers embedded in the VS1053.
 */
typedef enum
{
	LIB_VS1053_REG_ADDR_SCI_0 = 0x0,	// Mode Control
	LIB_VS1053_REG_ADDR_SCI_1 = 0x1, 	// Status of VS1053B
	LIB_VS1053_REG_ADDR_SCI_2 = 0x2,	// Built-in bass/treble control
	LIB_VS1053_REG_ADDR_SCI_3 = 0x3,    // Clock freq + multiplier
	LIB_VS1053_REG_ADDR_SCI_4 = 0x4,	// Decode time in seconds 
	LIB_VS1053_REG_ADDR_SCI_5 = 0x5, 	// Misc. audio data
	LIB_VS1053_REG_ADDR_SCI_6 = 0x6,	// RAM write/read
	LIB_VS1053_REG_ADDR_SCI_7 = 0x7,	// Base address for RAM write/read
	LIB_VS1053_REG_ADDR_SCI_8 = 0x8,	// Stream header data 0
	LIB_VS1053_REG_ADDR_SCI_9 = 0x9,	// Stream header data 1
	LIB_VS1053_REG_ADDR_SCI_10 = 0xA,	// Start address of application	
	LIB_VS1053_REG_ADDR_SCI_11 = 0x0B,	// Volume Control	
	LIB_VS1053_REG_ADDR_SCI_12 = 0xC,	// Application control register 0
	LIB_VS1053_REG_ADDR_SCI_13 = 0xD,	// Application control register 1
	LIB_VS1053_REG_ADDR_SCI_14 = 0xE,	// Application control register 2
	LIB_VS1053_REG_ADDR_SCI_15 = 0xF,	// Application control register 3

} LIB_VS1053_SCI_regAddr_E;

/**
 * @brief Mode bits in the mode control register (LIB_VS1053_REG_ADDR_SCI_0).
 */
typedef enum 
{
	LIB_VS1053_SCI_0_SM_DIFF 			= 0x0, 	// Differential
	LIB_VS1053_SCI_0_SM_LAYER12 		= 0x1,	// Allow MPEG layers I & II
	LIB_VS1053_SCI_0_SM_RESET 			= 0x2,	// Soft reset
	LIB_VS1053_SCI_0_SM_CANCEL 			= 0x3,	// Cancel decoding current file	
	LIB_VS1053_SCI_0_SM_EARSPEAKER_LO 	= 0x4,	// EarSpeaker low setting
	LIB_VS1053_SCI_0_SM_TESTS 			= 0x5,	// Allow SDI tests
	LIB_VS1053_SCI_0_SM_STREAM 			= 0x6,	// Stream mode
	LIB_VS1053_SCI_0_SM_EARSPEAKER_HI 	= 0x7,	// EarSpeaker high setting
	LIB_VS1053_SCI_0_SM_DACT 			= 0x8,	// DCLK active edge
	LIB_VS1053_SCI_0_SM_SDIORD 			= 0x9,	// SDI bit order
	LIB_VS1053_SCI_0_SM_SDISHARE 		= 0xA,	// Share SPI chip select
	LIB_VS1053_SCI_0_SM_SDINEW 			= 0xB,	// VS1002 native SPI modes
	LIB_VS1053_SCI_0_SM_ADPCM 			= 0xC,	// PCM/ADPCM recording active
	LIB_VS1053_SCI_0_SM_RESERVED 		= 0xD,	// -
	LIB_VS1053_SCI_0_SM_LINE1 			= 0xE,	// MIC / LINE1 selector
	LIB_VS1053_SCI_0_SM_CLK_RANGE 		= 0xF,	// Input clock range

} LIB_VS1053_SCI_0_mode_E;

/**
 * @brief Status bits in the status control register (LIB_VS1053_REG_ADDR_SCI_1).
 */
typedef struct 
{
	uint8_t LIB_VS1053_SCI_1_SS_REFERENCE_SEL 	: 1; 	// Reference voltage slection, '0' = 1.23V, '1' = 1.65V
	uint8_t LIB_VS1053_SCI_1_SS_AD_CLOCK 		: 1;	// AD clock select, '0' = 6 MHz, '1' = 3 MHz
	uint8_t LIB_VS1053_SCI_1_SS_APDOWN1 		: 1;	// Analog internal powerdown
	uint8_t LIB_VS1053_SCI_1_SS_APDOWN2 		: 1;	// Analog driver powerdown	
	uint8_t LIB_VS1053_SCI_1_SS_VERSION		 	: 4;	// Version
	uint8_t LIB_VS1053_SCI_1_SS_RESERVED 		: 2;	// RESERVED
	uint8_t LIB_VS1053_SCI_1_SS_VCM_DISABLE 	: 1;	// GBUF overload detection '1' = disable
	uint8_t LIB_VS1053_SCI_1_SS_VCM_OVERLOAD	: 1;	// GBUF overload indicator '1' = overload
	uint8_t LIB_VS1053_SCI_1_SS_SWING_BIT12 	: 3;	// Set swing to +0dB, +0.5dB, .., or +3.5dB
	uint8_t LIB_VS1053_SCI_1_SS_DO_NOT_JUMP 	: 1; 	// Header in decode, do not fast forward/rewind

} LIB_VS1053_SCI_1_status_S;

/**
 * @brief Bass/treble bits in the base/treble control register (LIB_VS1053_REG_ADDR_SCI_2).
 */
typedef struct 
{
	uint8_t LIB_VS1053_SCI_2_SB_FREQLIMIT	: 4; 	// Bass lower limit frequency in 10 Hz steps (2..15)	
	uint8_t LIB_VS1053_SCI_2_SB_AMPLITUDE 	: 4;	// Bass enhancement in 1dB steps (0..15, 0 = off)
	uint8_t LIB_VS1053_SCI_2_ST_FREQLIMIT	: 4;	// Treble lower limit frequency in 1000 Hz steps (1..15)
	uint8_t LIB_VS1053_SCI_2_ST_AMPLITUDE	: 4;	// Treble control in 1.5 dB steps (-8..7, 0 = off)

} LIB_VS1053_SCI_2_bass_treble_S;

/**
 * @brief Clock/frequency bits in the CLOCKF control register (LIB_VS1053_REG_ADDR_SCI_3).
 */
typedef struct 
{
	uint8_t LIB_VS1053_SCI_3_SC_FREQ : 11;	// Clock frequency
	uint8_t LIB_VS1053_SCI_3_SC_ADD	 : 2;	// Allowed multiplier addition
	uint8_t LIB_VS1053_SCI_3_SC_MULT : 3;	// Clock multiplier 

} LIB_VS1053_SCI_3_clock_S;

/**
 * @brief Stream header data 0 bits in the HDAT0 control register (LIB_VS1053_REG_ADDR_SCI_8).
 */
typedef struct 
{
	uint8_t LIB_VS1053_SCI_8_HDAT0_EMPHASIS		: 2; // See pg. 45
	uint8_t LIB_VS1053_SCI_8_HDAT0_ORIGINAL		: 1; // See pg. 45
	uint8_t LIB_VS1053_SCI_8_HDAT0_COPYRIGHT	: 1; // See pg. 45
	uint8_t LIB_VS1053_SCI_8_HDAT0_EXTENSION	: 2; // See pg. 45	 
	uint8_t LIB_VS1053_SCI_8_HDAT0_MODE			: 2; // See pg. 45	
	uint8_t LIB_VS1053_SCI_8_HDAT0_PRIVATE_BIT	: 1; // See pg. 45	
	uint8_t LIB_VS1053_SCI_8_HDAT0_PAD_BIT		: 1; // See pg. 45	
	uint8_t LIB_VS1053_SCI_8_HDAT0_SAMPLE_RATE	: 2; // See pg. 45	
	uint8_t LIB_VS1053_SCI_8_HDAT0_BIT_RATE		: 4; // See pg. 45	

	
} LIB_VS1053_SCI_8_hdat0_S;

/**
 * @brief Stream header data 1 bits in the HDAT1 control register (LIB_VS1053_REG_ADDR_SCI_9).
 */
typedef struct 
{
	uint8_t LIB_VS1053_SCI_8_HDAT1_PROTECT_BIT	: 1;  // See pg. 45
	uint8_t LIB_VS1053_SCI_8_HDAT1_LAYER		: 2;  // See pg. 45
	uint8_t LIB_VS1053_SCI_8_HDAT1_ID			: 2;  // See pg. 45
	uint8_t LIB_VS1053_SCI_8_HDAT1_SYNCWORD		: 11; // See pg. 45	 
	
} LIB_VS1053_SCI_8_hdat1_S;

#endif 