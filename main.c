/*
 * File:   main.c
 * Author: kanderson
 *
 * Created on January 20, 2021, 2:11 PM
 */

#include <xc.h>
#include <stdbool.h>

// PIC16F627A Configuration Bit Settings
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSC oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital input, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = OFF         // Low-Voltage Programming Enable bit (RB4/PGM pin has PGM function, low-voltage programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#include "system.h"        /* System funct/params, like osc/peripheral config */

unsigned char momentary = 0;
unsigned char rb5_changed = 0;

void initialize_hw() {
    CMCON = 0x07;

    // PORT A
    PORTA = 0x00;
    TRISA = 0x00;

    // PORT B
    PORTB = 0x00;
    TRISB = 0xFF;
    OPTION_REGbits.nRBPU = 1;

    // Interrupts
    INTCONbits.RBIF = 0; // ensure port b flag is not already set
    INTCONbits.RBIE = 1; // enable port b state change interrupts
    OPTION_REGbits.INTEDG = 1; //Interrupt on rising edge
    INTCONbits.GIE = 1; // enable general interrupts
    INTCONbits.PEIE = 1; // enable peripheral interrupts    
}

void __interrupt() interrupt_handler(void) {
    if (INTCONbits.RBIF) {
        if (PORTBbits.RB5 && !rb5_changed) {
            momentary = !momentary;
            rb5_changed = 1;
            __delay_ms(50);
        } else if (!PORTBbits.RB5) {
            rb5_changed = 0;
        }
        INTCONbits.RBIF = 0;
    }
}

void main(void) {
    initialize_hw();

    unsigned char PORTAshadow = 0x00;

    while (1) {
        if (momentary == 0) {
            bitset(PORTAshadow, 1);
            bitset(PORTAshadow, 2);
        } else {
            bitclr(PORTAshadow, 1);
            bitclr(PORTAshadow, 2);
        }

        if (PORTBbits.RB4) {
            bitset(PORTAshadow, 0);
        } else if (!PORTBbits.RB4) {
            bitclr(PORTAshadow, 0);
        }

        PORTA = PORTAshadow;
    };

    return;
}
