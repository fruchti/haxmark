// -fruchti 2015

#ifndef MAIN_H_
#define MAIN_H_

#include "config.h"

typedef enum
{
    PowerOnWait,
    Idle,
    ClutchDelay,
    WaitingForPaperIn,
    ExitOnDelay,
    ManualPaperFeedOffDelay,
    PaperInOffDelay,
    ExitOffDelay
} State_t;

int main(void);
ISR(TIMER0_COMPA_vect);
ISR(INT0_vect);
ISR(INT1_vect);

#endif
