// -fruchti 2015

#ifndef MAIN_H_
#define MAIN_H_

#include "config.h"

typedef enum
{
    PowerOnWait,                        // Power-on test state with all outputs
                                        // enabled

    Idle,                               // Waiting for print job

    AlignFeed,                          // Initial pulling in for proper align-
                                        // ment of the carrier

    ClutchDelay,                        // Delay before main carrier feed

    WaitingForFeed,                     // Waits for the end of the carrier
                                        // alignment mark

    WaitingForPaperIn,                  // Waits for the paper in mark on the
                                        // carrier

    ExitOnDelay,                        // Wait for print to be completed

    ManualPaperFeedOffDelay,            // Various delays
    PaperInOffDelay,                    // for disabling the
    ExitOffDelay                        // sensor signals again
} State_t;

int main(void);
ISR(TIMER0_COMPA_vect);
ISR(INT0_vect);
ISR(INT1_vect);

#endif
