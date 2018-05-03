#ifndef __BUTTON_H
#define __BUTTON_H

#include "msp430f1611.h"
#include "stdint.h"

typedef void(*btnCallBackFunc)(void);

btnCallBackFunc btnCb;

void btn_setCallbacks(btnCallBackFunc func)
{
    btnCb = func;
    P2DIR &= ~0x80; // input direction
    P2OUT |= 0x80; // put pin high as pushing button brings low
    P2IES |= 0x80; // interrup when transition is high-to-low
    P2IE |= 0x80; // enable interrupts
}

#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void) {
    P2IFG &= ~0x80; // clear the interrupt flag
    btnCb();
}

#endif
