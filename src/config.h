// -fruchti 2015

#ifndef CONFIG_H_
#define CONFIG_H_

#define F_CPU           8000000

#include <avr/io.h>
#include <avr/interrupt.h>

// All delay times are measured in milliseconds
#define T_HALL_L        6               // Hall sensor timeout. After this time
                                        // is elapsed without a pin change, the
                                        // motor is considered stopped
#define T_POWRON        5000            // Delay to allow printer to start up

#define T_CLUTCH        3406            // Delay between starting print job and
                                        // pulling in carrier/activating manual
                                        // paper feed sensor
#define T_EXIT_H        841             // Delay between activation of paper in
                                        // and activation of exit sensor
#define T_MNPF_L        213             // Delay before deactivation of manual
                                        // paper feed sensor after exit sensor
                                        // is activated
#define T_PPIN_L        311             // Delay before deactivation of paper
                                        // in sensor
#define T_EXIT_L        849             // Delay before deactivation of exit
                                        // sensor

#define D_LED_RD        DDRD            // Red status LED
#define O_LED_RD        PORTD
#define P_LED_RD        PD6

#define D_LED_YE        DDRD            // Yellow status LED
#define O_LED_YE        PORTD
#define P_LED_YE        PD5

#define D_LED_GN        DDRD            // Green status LED
#define O_LED_GN        PORTD
#define P_LED_GN        PD4

#define I_NPPINS        PINB            // New paper in sensor
#define O_NPPINS        PORTB
#define P_NPPINS        PB0

#define D_PPINSO        DDRB            // Paper in sensor output
#define O_PPINSO        PORTB
#define P_PPINSO        PB1

#define D_MNPFSO        DDRB            // Manual paper feed sensor output
#define O_MNPFSO        PORTB
#define P_MNPFSO        PB2

#define D_EXITSO        DDRB            // Exit sensor output
#define O_EXITSO        PORTB
#define P_EXITSO        PB3

#define D_CLUTCH        DDRB            // Clutch control pin
#define O_CLUTCH        PORTB
#define P_CLUTCH        PB4

#define I_OPANEL        PIND            // Operator panel
#define P_OPANEL        PD2             // CAUTION: Do not change (INT0 is used
                                        // for monitoring this input)!

#define I_HALL_1        PIND            // Hall sensor from motor
#define P_HALL_1        PD3

#endif
