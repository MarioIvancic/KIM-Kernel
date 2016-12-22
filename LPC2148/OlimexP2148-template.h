// Definicije pinova i signala za Olimex P2148 razvojnu plocu

#ifndef __OLIMEXP2148_H__
#define __OLIMEXP2148_H__

//#include "types.h"
#include "lpc21xx.h"


// PIO 0
// UART 0
#define TXD0                BIT(0) // 19, used by UART0
#define RXD0                BIT(1) // 21, used by UART0

// I2C 0
#define SCL0			    BIT(2) // 22, used by I2C0
#define SDA0		        BIT(3) // 26, used by I2C0

// SPI 0
#define SCK0				BIT(4 ) // 27, used by SPI0
#define MISO0				BIT(5 ) // 29, used by SPI0
#define MOSI0				BIT(6 ) // 30, used by SPI0
#define SSEL0				BIT(7 ) // 31, used by SPI0

// UART 1
#define TXD1                BIT(8 ) // 33, used by UART1
#define RXD1                BIT(9 ) // 34, used by UART1

#define LED1				BIT(10) // 35,
#define LED2				BIT(11) // 37,
#define P012				BIT(12) // 38,
#define P013				BIT(13) // 39,
#define BSL                 BIT(14) // 41, BSL switch - active low input
#define BTN1				BIT(15) // 45,
#define BTN2				BIT(16) // 46,

// SPI 1
#define SCK1				bit(17) // 47, used by SPI1
#define MISO1				bit(18) // 53, used by SPI1
#define MOSI1				bit(19) // 54, used by SPI1
#define SSEL1				BIT(20) // 55, used by SPI1

#define P021				BIT(21) // 1,
#define P022				BIT(22) // 2,
#define VBUS	            BIT(23) // 58, usb power detect - active high input
#define __RESERVED01		BIT(24) // ovi pinovi ne postoje
#define AOUT				BIT(25) // 9,
#define __RESERVED02		BIT(26) // ovi pinovi ne postoje
#define __RESERVED03		BIT(27) // ovi pinovi ne postoje
#define P028				BIT(28) // 13,
#define P029				BIT(29) // 14,
#define AD03                BIT(30) // 15, AD03 analog input - active low input
#define CONNECT				BIT(31) // 17, active low output only

#define LED3 CONNECT
#define USB_LED LED3
#define LINK_LED LED3

#define BUZZ1 P012 // 38,
#define BUZZ2 P013 // 39,


// PIO 1
#define P116				BIT(16) // 16,
#define P117				BIT(17) // 12,
#define P118				BIT(18) // 8,
#define P119				BIT(19) // 4,
#define P120				BIT(20) // 48,
#define P121				BIT(21) // 44,
#define P122				BIT(22) // 40,
#define P123				BIT(23) // 36,
#define P124				BIT(24) // 32,
#define P125				BIT(25) // 28,
#define RTCK				BIT(26) // 24, used for JTAG
#define TDO					BIT(27) // 64, used for JTAG
#define TDI					BIT(28) // 60, used for JTAG
#define TCK					BIT(29) // 56, used for JTAG
#define TMS					BIT(30) // 52, used for JTAG
#define TRST				BIT(31) // 20, used for JTAG

#define MMC_WP P124 // 32, mmc wp switch - active low input
#define MMC_CP P125 // 28, mmc cp switch - active low input



// #### PIO 0 ####
#define PIO0_ADC_BITS ( \
	0)

#define PIO0_DAC_BITS ( \
	0)

#define PIO0_OUTPUT_BITS ( \
	0)

#define PIO0_ONE_BITS ( \
	0)


// definicija stanja i smjera pinova za POWERDOWN stanje
#define PIO0_POWER_DOWN_OUTPUT_BITS ( \
	0xffffffff )
#define PIO0_POWER_DOWN_ONE_BITS ( \
	0 )



// #### PIO 1 ####
#define PIO1_ADC_BITS ( \
	0)

#define PIO1_DAC_BITS ( \
	0)

#define PIO1_OUTPUT_BITS ( \
	0)

#define PIO1_ONE_BITS ( \
    0 )


// definicija stanja i smjera pinova za POWERDOWN stanje
#define PIO1_POWER_DOWN_OUTPUT_BITS ( \
	0 )
#define PIO1_POWER_DOWN_ONE_BITS ( \
	0 )



#endif // __OLIMEXP2148_H__
