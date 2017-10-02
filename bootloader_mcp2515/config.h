
// ----------------------------------------------------------------------------
// Bootloader Settings

#define	BOOTLOADER_TYPE		0

#define	BOOT_LED			B,5  // B,5 Nano

//#define	BOOT_INIT
//#define	BOOT_LED_SET_OUTPUT
//#define	BOOT_LED_ON
//#define	BOOT_LED_OFF
//#define	BOOT_LED_TOGGLE

// ----------------------------------------------------------------------------
// CAN Settings

#define	MCP2515_CS			B,2  // B,2 Nano  // B,4 Hap-CU
#define	MCP2515_INT			D,2

#define MCP2515_BITRATE 	125
#define MCP2515_CLOCK 		8

