#include "N96FY.h"

int N96FY::counter = 0;
int N96FY::seconds = 0;
int N96FY::values[AVERAGE_PERIOD_IN_SECONDS];

N96FY::N96FY(bool useInternalPulldown)
: _useInternalPulldown(useInternalPulldown)
{
}

void N96FY::begin()
{
    if (_useInternalPulldown)
    {
        pinMode(INTERRUPT_PIN, INPUT_PULLUP); // We use the internal pullup resistor
    }
    else
    {
        pinMode(INTERRUPT_PIN, INPUT);
        digitalWrite(INTERRUPT_PIN, LOW); // We do NOT use the internal pullup resistor - instead you must use an external pulldown resistor
    }
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), ISR_sensor, CHANGE);   // With "CHANGE" we have 8 calls for one roundtrip
    
    startTimer();
}

void N96FY::startTimer()
{
    noInterrupts();
    timer0_isr_init();
    timer0_attachInterrupt(ISR_everySecond);
    timer0_write(ESP.getCycleCount() + INTERVAL_US);
    interrupts();
}

float N96FY::speedInKMH()
{
    return calcNumberOfClosingsInOneSecond() * KMH_Factor;
}

float N96FY::speedInMPH()
{
    return calcNumberOfClosingsInOneSecond() * MPH_Factor;
}

void N96FY::ISR_sensor()
{
    // We count the number of changes
    // With "CHANGE" (see begin()) we have 8 calls for one roundtrip
    N96FY::counter++;
}

void N96FY::ISR_everySecond()
{
    // We store the number of sensor changes within the last second in an array for the last n-seconds
    // so we are able to calculate the average changes within the last n-seconds
    N96FY::values[N96FY::seconds] = N96FY::counter; // Store number of changes from the last second
    N96FY::counter = 0; // Reset counter to start from 0 for next second
    N96FY::seconds++; // Point to next second (in our array)
    if (N96FY::seconds >= AVERAGE_PERIOD_IN_SECONDS)
    {
        N96FY::seconds = 0; // Start from 0 and override existing values
    }
    timer0_write(ESP.getCycleCount() + INTERVAL_US); // restart timer
}

float N96FY::calcNumberOfClosingsInOneSecond()
{
    float total = 0;
    for (int i = 0; i < AVERAGE_PERIOD_IN_SECONDS; i++)
    {
        total += N96FY::values[i];
    }
    if (total > 0) { // If there is something to calculate
        // Calculate the total number of turnarounds
        total = total / 6 ; // 1 turnaround means 8-CHANGES minus 2-Open/Closed singnals
        
        // Calculate the average for our measured period of time
        total = total / AVERAGE_PERIOD_IN_SECONDS;
    }
    return total;
}
