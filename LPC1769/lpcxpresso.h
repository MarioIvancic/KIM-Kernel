/*  Copyright (c) 2012, Mario Ivančić
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// lpcxpresso.h

#ifndef __LPCXPRESSO_H__
#define __LPCXPRESSO_H__



// pin definitions

// PIO 0

#define __b0_00             BIT(0)  // 46, TXD3, SDA1
#define __b0_01             BIT(1)  // 47, RXD3, SCL1
#define __b0_02             BIT(2)  // 98, TXD0, used by bootloader
#define __b0_03             BIT(3)  // 99, RXD0, used by bootloader
#define __b0_04             BIT(4)  // 81,
#define __b0_05             BIT(5)  // 80,
#define __b0_06             BIT(6)  // 79, SSEL1
#define __b0_07             BIT(7)  // 78, SCK1
#define __b0_08             BIT(8)  // 77, MISO1
#define __b0_09             BIT(9)  // 76, MOSI1
#define __b0_10             BIT(10) // 48, TXD2, SDA2
#define __b0_11             BIT(11) // 49, RXD2, SCL2
#define reserved01          BIT(12) //    not implemented
#define reserved02          BIT(13) //    not implemented
#define reserved03          BIT(14) //    not implemented
#define __b0_15             BIT(15) // 62, TXD1, SCK0
#define __b0_16             BIT(16) // 63, RXD1, SSEL0
#define __b0_17             BIT(17) // 61, MISO0
#define __b0_18             BIT(18) // 60, MOSI0
#define __b0_19             BIT(19) // 59, SDA1
#define __b0_20             BIT(20) // 58, SCL1
#define __b0_21             BIT(21) // 57,
#define __b0_22             BIT(22) // 56,
#define __b0_23             BIT(23) // 9
#define __b0_24             BIT(24) // 8
#define __b0_25             BIT(25) // 7, TXD3
#define __b0_26             BIT(26) // 6, RXD3
#define __b0_27             BIT(27) // 25, SDA1
#define __b0_28             BIT(28) // 24, SCL1
#define __b0_29             BIT(29) // 29, USBDP, used by USB
#define __b0_30             BIT(30) // 30, USBDM, used by USB
#define reserved04          BIT(31) //    not implemented


#define TXD0_BIT            __b0_02 // 98, TXD0_BIT
#define RXD0_BIT            __b0_03 // 99, RXD0_BIT
#define USBDP               __b0_29 // 29, used by USB
#define USBDM               __b0_30 // 30, used by USB
#define LED                 __b0_22 // 56, LED, active high output

#define LED_ON()    (P0SET = LED)
#define LED_OFF()   (P0CLR = LED)


#define PIO0_ADC_BITS ( \
    0)

#define PIO0_DAC_BITS ( \
    0)

#define PIO0_OUTPUT_BITS ( \
    TXD0_BIT | \
    LED | \
    0)

#define PIO0_ONE_BITS ( \
    0 )

#define PIO0_REPEATER_BITS ( \
    0)

#define PIO0_PULLDOWN_BITS ( \
    0)

#define PIO0_PULLUP_BITS ( \
    RXD0_BIT | \
    0)

#define PIO0_OPENDRAIN_BITS ( \
    0)

// definicija stanja i smjera pinova za POWERDOWN stanje
#define PIO0_POWER_DOWN_OUTPUT_BITS ( \
    0 )

#define PIO0_POWER_DOWN_ONE_BITS ( \
    0 )


// ##############################################################################

// PIO 1

#define __b1_00             BIT(0)  // 95
#define __b1_01             BIT(1)  // 94
#define reserved10          BIT(2)  //    not implemented
#define reserved11          BIT(3)  //    not implemented
#define __b1_04             BIT(4)  // 93
#define reserved12          BIT(5)  //    not implemented
#define reserved13          BIT(6)  //    not implemented
#define reserved14          BIT(7)  //    not implemented
#define __b1_08             BIT(8)  // 92
#define __b1_09             BIT(9)  // 91
#define __b1_10             BIT(10) // 90
#define reserved15          BIT(11) //    not implemented
#define reserved16          BIT(12) //    not implemented
#define reserved17          BIT(13) //    not implemented
#define __b1_14             BIT(14) // 89
#define __b1_15             BIT(15) // 88,
#define __b1_16             BIT(16) // 87,
#define __b1_17             BIT(17) // 86,
#define __b1_18             BIT(18) // 32, usb-link-up LED, active low output
#define __b1_19             BIT(19) // 33,
#define __b1_20             BIT(20) // 34,
#define __b1_21             BIT(21) // 35,
#define __b1_22             BIT(22) // 36,
#define __b1_23             BIT(23) // 37,
#define __b1_24             BIT(24) // 38,
#define __b1_25             BIT(25) // 39,
#define __b1_26             BIT(26) // 40,
#define __b1_27             BIT(27) // 43,
#define __b1_28             BIT(28) // 44,
#define __b1_29             BIT(29) // 45,
#define __b1_30             BIT(30) // 21, usb power detect - active high input
#define __b1_31             BIT(15) // 20

#define usb_up_led          __b1_18 // 32, usb-link-up LED, active low output
#define usb_pwr             __b1_30 // 21, usb power detect - active high input


#define PIO1_ADC_BITS ( \
    0 )

#define PIO1_DAC_BITS ( \
    0 )

#define PIO1_OUTPUT_BITS ( \
    usb_up_led | \
    0 )

#define PIO1_ONE_BITS ( \
    usb_up_led | \
    0 )

#define PIO1_PULLUP_BITS ( \
    0)

#define PIO1_REPEATER_BITS ( \
    0)

#define PIO1_PULLDOWN_BITS ( \
    usb_pwr | \
    0)

#define PIO1_OPENDRAIN_BITS ( \
    0)

// definicija stanja i smjera pinova za POWERDOWN stanje
#define PIO1_POWER_DOWN_OUTPUT_BITS ( \
    0 )
#define PIO1_POWER_DOWN_ONE_BITS ( \
    0 )


// ##############################################################################

// PIO 2

#define __b2_00             BIT(0)  // 75,
#define __b2_01             BIT(1)  // 74,
#define __b2_02             BIT(2)  // 73,
#define __b2_03             BIT(3)  // 70,
#define __b2_04             BIT(4)  // 69,
#define __b2_05             BIT(5)  // 68,
#define __b2_06             BIT(6)  // 67,
#define __b2_07             BIT(7)  // 66,
#define __b2_08             BIT(8)  // 70,
#define __b2_09             BIT(9)  // 64, usb link/connect - active low output
#define __b2_10             BIT(10) // 53, Bootloader Select Line, active low input
#define __b2_11             BIT(11) // 52,
#define __b2_12             BIT(12) // 51,
#define __b2_13             BIT(13) // 50,

#define usb_con             __b1_09 // 64, usb link/connect - active low output
#define BSL                 __b1_10 // 53, Bootloader Select Line, active low input

#define PIO2_ADC_BITS ( \
    0 )

#define PIO2_DAC_BITS ( \
    0 )

#define PIO2_OUTPUT_BITS ( \
    usb_con | \
    0 )

#define PIO2_ONE_BITS ( \
    usb_con | \
    0 )

#define PIO2_PULLUP_BITS ( \
    0)

#define PIO2_REPEATER_BITS ( \
    0)

#define PIO2_PULLDOWN_BITS ( \
    0)

#define PIO2_OPENDRAIN_BITS ( \
    0)

// definicija stanja i smjera pinova za POWERDOWN stanje
#define PIO2_POWER_DOWN_OUTPUT_BITS ( \
    0 )
#define PIO2_POWER_DOWN_ONE_BITS ( \
    0 )


// ##############################################################################

// PIO 3

#define __b3_25             BIT(25) // 27
#define __b3_26             BIT(26) // 26

#define PIO3_ADC_BITS ( \
    0 )

#define PIO3_DAC_BITS ( \
    0 )

#define PIO3_OUTPUT_BITS ( \
    0 )

#define PIO3_ONE_BITS ( \
    0 )

#define PIO3_PULLUP_BITS ( \
    0)

#define PIO3_REPEATER_BITS ( \
    0)

#define PIO3_PULLDOWN_BITS ( \
    0)

#define PIO3_OPENDRAIN_BITS ( \
    0)

// definicija stanja i smjera pinova za POWERDOWN stanje
#define PIO3_POWER_DOWN_OUTPUT_BITS ( \
    0 )
#define PIO3_POWER_DOWN_ONE_BITS ( \
    0 )


// ##############################################################################

// PIO 4

#define __b4_28             BIT(28) // 82, TXD3
#define __b4_29             BIT(29) // 85, RXD3

#define PIO4_ADC_BITS ( \
    0 )

#define PIO4_DAC_BITS ( \
    0 )

#define PIO4_OUTPUT_BITS ( \
    0 )

#define PIO4_ONE_BITS ( \
    0 )

#define PIO4_PULLUP_BITS ( \
    0)

#define PIO4_REPEATER_BITS ( \
    0)

#define PIO4_PULLDOWN_BITS ( \
    0)

#define PIO4_OPENDRAIN_BITS ( \
    0)

// definicija stanja i smjera pinova za POWERDOWN stanje
#define PIO4_POWER_DOWN_OUTPUT_BITS ( \
    0 )
#define PIO4_POWER_DOWN_ONE_BITS ( \
    0 )

// ##############################################################################


#endif // __LPCXPRESSO_H__


