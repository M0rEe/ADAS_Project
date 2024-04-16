/*
 * @ file:   ultrasonic.c
 * @ brief:  Ultrasonic Program file.c
 * @ Author: Lenovo
 * @ Date:   Apr 6, 2024
 * @ Version 1.0
 */

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "ultrasonic.h"
#include "tm4c123gh6pm.h"

/* Timer0A initialization function */
/* Initialize Timer0A in input-edge time mode with up-count mode */

void TIMER0A_voidCaptureInit(void)
{
    SYSCTL_RCGCTIMER_R |= 1;     /* enable clock to Timer Block 0 */
    SYSCTL_RCGCGPIO_R |= 2;      /* enable clock to PORTB */
    GPIO_PORTB_DIR_R &= ~0x40;        /* make PB6 an input pin */
    GPIO_PORTB_DEN_R |= 0x40;         /* make PB6 as digital pin */
    GPIO_PORTB_AFSEL_R |= 0x40;       /* use PB6 alternate function */
    GPIO_PORTB_PCTL_R &= ~0x0F000000;  /* configure PB6 for T0CCP0 */
    GPIO_PORTB_PCTL_R |= 0x07000000;
    /* PB2 as a digital output signal to provide trigger signal */
    SYSCTL_RCGCGPIO_R |= 1;      /* enable clock to PORTA */
    GPIO_PORTA_DIR_R |=(1<<3);         /* set PB2 as a digial output pin */
    GPIO_PORTA_DEN_R |=(1<<3);         /* make PB2 as digital pin */
    TIMER0_CTL_R &= ~1;          /* disable timer0A during setup */
    TIMER0_CFG_R = 4;            /* 16-bit timer mode */
    TIMER0_TAMR_R = 0x17;        /* up-count, edge-time, capture mode */
    TIMER0_CTL_R |=0x0C;        /* capture the rising edge */
    TIMER0_CTL_R |= (1<<0);           /* enable timer0A */
}

/* Timer2A initialization function */
/* Initialize Timer2A in input-edge time mode with up-count mode */
void TIMER2A_voidCaptureInit(void)
{
    SYSCTL_RCGCTIMER_R |= 0x04;    /* enable clock to Timer Block 2 */
    SYSCTL_RCGCGPIO_R |= 0x02;     /* enable clock to PORTB */
    GPIO_PORTB_DIR_R &= ~0x10;     /* make PB4 an input pin */
    GPIO_PORTB_DEN_R |= 0x10;      /* make PB4 as digital pin */
    GPIO_PORTB_AFSEL_R |= 0x10;    /* use PB4 alternate function */
    GPIO_PORTB_PCTL_R &= ~0x000F0000;  /* configure PB4 for T2CCP0 */
    GPIO_PORTB_PCTL_R |= 0x00070000;

    /* PA2 as a digital output signal to provide trigger signal */
    SYSCTL_RCGCGPIO_R |= 1;      /* enable clock to PORTA */
    GPIO_PORTA_DIR_R |=(1<<3);         /* set PA2 as a digial output pin */
    GPIO_PORTA_DEN_R |=(1<<3);         /* make PA2 as digital pin */

    TIMER2_CTL_R &= ~1;           /* disable Timer 2A during setup */
    TIMER2_CFG_R = 4;             /* 16-bit timer mode */
    TIMER2_TAMR_R = 0x17;         /* up-count, edge-time, capture mode */
    TIMER2_CTL_R |= 0x0C;         /* capture the rising edge */
    TIMER2_CTL_R |= (1 << 0);     /* enable Timer 2A */
}

/* This function captures consecutive rising and falling edges of a periodic signal */
/* from Timer Block 2 Timer A and returns the time difference (the period of the signal). */
u32 Ultrasonic2_u32GetDistance(void)
{
    u32 lastEdge, thisEdge, distance;;

    /* Given 10us trigger pulse pinA3*/
    GPIO_PORTA_DATA_R &= ~(1<<3); /* make trigger  pin high */
    Delay_MicroSecond(10); /*10 seconds delay */
    GPIO_PORTA_DATA_R |= (1<<3); /* make trigger  pin high */
    Delay_MicroSecond(10); /*10 seconds delay */
    GPIO_PORTA_DATA_R &= ~(1<<3); /* make trigger  pin low */

    while(1)
    {
        TIMER2_ICR_R= 4;            /* clear timer0A capture flag */
        while((TIMER2_RIS_R & 4) == 0) ;    /* wait till captured */
        if(GPIO_PORTB_DATA_R & (1<<4)) /*check if rising edge occurs */
        {
            lastEdge = TIMER2_TAR_R;     /* save the timestamp */
            /* detect falling edge */
            TIMER2_ICR_R = 4;            /* clear timer0A capture flag */
            while((TIMER2_RIS_R & 4) == 0) ;    /* wait till captured */
            thisEdge = TIMER2_TAR_R;     /* save the timestamp */
            distance = ((thisEdge - lastEdge) * 10625)/10000000;
            return distance;
        }
    }

}



/* This function captures consecutive rising and falling edges of a periodic signal */
/* from Timer Block 0 Timer A and returns the time difference (the period of the signal). */
u32 Ultrasonic1_u32GetDistance(void)
{
    u32 lastEdge, thisEdge,distance;

    /* Given 10us trigger pulse pinA3*/
    GPIO_PORTA_DATA_R &= ~(1<<3); /* make trigger  pin high */
    Delay_MicroSecond(10); /*10 seconds delay */
    GPIO_PORTA_DATA_R |= (1<<3); /* make trigger  pin high */
    Delay_MicroSecond(10); /*10 seconds delay */
    GPIO_PORTA_DATA_R &= ~(1<<3); /* make trigger  pin low */

    while(1)
    {
        TIMER0_ICR_R= 4;            /* clear timer0A capture flag */
        while((TIMER0_RIS_R & 4) == 0) ;    /* wait till captured */
        if(GPIO_PORTB_DATA_R & (1<<6)) /*check if rising edge occurs */
        {
            lastEdge = TIMER0_TAR_R;     /* save the timestamp */
            /* detect falling edge */
            TIMER0_ICR_R = 4;            /* clear timer0A capture flag */
            while((TIMER0_RIS_R & 4) == 0) ;    /* wait till captured */
            thisEdge = TIMER0_TAR_R;     /* save the timestamp */
            /* take pulse duration measurement */
            distance = ((thisEdge - lastEdge) * 10625)/10000000;
            return distance; /* return the time difference */
        }
    }

}

/* Create one microsecond second delay using Timer block 1 and sub timer A */

void Delay_MicroSecond(u32 time)
{
    u32 i;
    SYSCTL_RCGCTIMER_R |= 2;     /* enable clock to Timer Block 1 */
    TIMER1_CTL_R = 0;            /* disable Timer before initialization */
    TIMER1_CFG_R = 0x04;         /* 16-bit option */
    TIMER1_TAMR_R = 0x02;        /* periodic mode and down-counter */
    TIMER1_TAILR_R = 16 - 1;  /* TimerA interval load value reg */
    TIMER1_ICR_R = 0x1;          /* clear the TimerA timeout flag */
    TIMER1_CTL_R |= 0x01;        /* enable Timer A after initialization */

    for(i = 0; i < time; i++)
    {
        while ((TIMER1_RIS_R & 0x1) == 0) ;      /* wait for TimerA timeout flag */
        TIMER1_ICR_R = 0x1;      /* clear the TimerA timeout flag */
    }
}


void Delay(u32 counter)
{
    u32 i = 0;

    for(i=0; i< counter*1000; i++);
}

/* This function is called by the startup assembly code to perform system specific initialization tasks. */
