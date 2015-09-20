// -fruchti 2015

#include "main.h"

volatile uint32_t MillisecondCounter = 0;
volatile uint16_t HallSensorTimer = T_HALL_L;
volatile State_t State = PowerOnWait;
volatile uint8_t HallSensorBuffer = 0;

int main(void)
{
    //// IN- AND OUTPUTS ////

    D_LED_RD |= (1 << P_LED_RD);
    D_LED_YE |= (1 << P_LED_YE);
    D_LED_GN |= (1 << P_LED_GN);
    O_LED_RD |= (1 << P_LED_RD);
    O_LED_YE |= (1 << P_LED_YE);
    O_LED_GN |= (1 << P_LED_GN);
    O_NPPINS |= (1 << P_NPPINS);
    O_PPINSO |= (1 << P_PPINSO);
    D_PPINSO |= (1 << P_PPINSO);
    O_MNPFSO |= (1 << P_MNPFSO);
    D_MNPFSO |= (1 << P_MNPFSO);
    O_EXITSO |= (1 << P_EXITSO);
    D_EXITSO |= (1 << P_EXITSO);
    D_CLUTCH |= (1 << P_CLUTCH);
    O_CLUTCH |= (1 << P_CLUTCH);


    //// TIMER 0 ////

    // Enable CTC mode
    TCCR0A = (1 << WGM01);
    // Select clock F_CPU/64 (125kHz)
    TCCR0B = (1 << CS01) | (1 << CS00);
    // 1kHz interrupt frequency
    OCR0A = 124;

    // Enable OC0A interrupt
    TIMSK = (1 << OCIE0A);


    //// EXTERNAL INTERRUPTS ////

    // Trigger on the falling edge for operator panel input
    MCUCR |= (1 << ISC01);
    // Enable external interrupt 0
    GIMSK |= (1 << INT0);

    // Trigger on both edges for hall sensor input
    MCUCR |= (1 << ISC10);
    // Enable external interrupt 1
    GIMSK |= (1 << INT1);

    sei();

    MillisecondCounter = 0;
    State = PowerOnWait;

    while(1)
    {
        switch(State)
        {
            case PowerOnWait:
                // Does nothing.
                // Waits for the initialization of the printer to complete
                if(MillisecondCounter >= T_POWRON)
                {
                    State = Idle;
                }
                break;
            case Idle:
                // Activate clutch and reset all sensor output just in case
                // something has gone mucus
                O_PPINSO |= (1 << P_PPINSO);
                O_MNPFSO |= (1 << P_MNPFSO);
                O_EXITSO |= (1 << P_EXITSO);
                O_LED_RD |= (1 << P_LED_RD);
                O_CLUTCH &= ~(1 << P_CLUTCH);
                O_LED_GN &= ~(1 << P_LED_GN);

                // Check if motor is running and new paper in sensor is
                // blocked by carrier
                if(HallSensorTimer < T_HALL_L && !(I_NPPINS & (1 << P_NPPINS)))
                {
                    // Block carrier
                    O_CLUTCH |= (1 << P_CLUTCH);
                    O_LED_GN |= (1 << P_LED_GN);
                    O_LED_RD &= ~(1 << P_LED_RD);
                    State = ClutchDelay;
                    MillisecondCounter = 0;
                }
                break;
            case ClutchDelay:
                if(MillisecondCounter >= T_CLUTCH)
                {
                    // Deactivate clutch to allow carrier to get pulled in
                    O_CLUTCH &= ~(1 << P_CLUTCH);

                    // Enable manual paper feed sensor
                    O_MNPFSO &= ~(1 << P_MNPFSO);

                    State = WaitingForPaperIn;
                }
                break;
            case WaitingForPaperIn:
                // Check if paper in sensor is triggered
                if(I_NPPINS & (1 << P_NPPINS))
                {
                    // Enable paper in sensor output
                    O_PPINSO &= ~(1 << P_PPINSO);
                    O_LED_RD |= (1 << P_LED_RD);

                    MillisecondCounter = 0;
                    State = ExitOnDelay;
                }
                break;
            case ExitOnDelay:
                if(MillisecondCounter >= T_EXIT_H)
                {
                    // Activate exit sensor output
                    O_EXITSO &= ~(1 << P_EXITSO);

                    MillisecondCounter = 0;
                    State = ManualPaperFeedOffDelay;
                }
                break;
            case ManualPaperFeedOffDelay:
                if(MillisecondCounter >= T_MNPF_L)
                {
                    // Deactivate manual paper feed sensor output
                    O_MNPFSO |= (1 << P_MNPFSO);

                    MillisecondCounter = 0;
                    State = PaperInOffDelay;
                }
                break;
            case PaperInOffDelay:
                if(MillisecondCounter >= T_PPIN_L)
                {
                    // Deactivate paper in sensor output
                    O_PPINSO |= (1 << P_PPINSO);

                    MillisecondCounter = 0;
                    State = ExitOffDelay;
                }
            case ExitOffDelay:
                if(MillisecondCounter >= T_EXIT_L)
                {
                    // Deactivate exit sensor output
                    O_EXITSO |= (1 << P_EXITSO);

                    MillisecondCounter = 0;
                    State = Idle;
                }
                break;
        }
    }
}

ISR(TIMER0_COMPA_vect)
{
    MillisecondCounter++;
    if(MillisecondCounter == 0)
    {
        MillisecondCounter = UINT32_MAX;
    }

    if(State != PowerOnWait)
    {
        HallSensorTimer++;
        if(HallSensorTimer == 0)
        {
            HallSensorTimer = UINT16_MAX;
        }
        if(HallSensorTimer < T_HALL_L)
        {
            O_LED_YE |= (1 << P_LED_YE);
        }
        else
        {
            O_LED_YE &= ~(1 << P_LED_YE);
        }
    }
}

// External interrupt. Triggered by control panel
ISR(INT0_vect)
{
    // Just reset everything
    if(State == WaitingForPaperIn || State == ClutchDelay)
    //if(State != PowerOnWait)
    {
        State = Idle;
    }
}

// External interrupt. Triggered by hall sensor
ISR(INT1_vect)
{
    HallSensorTimer = 0;
}
