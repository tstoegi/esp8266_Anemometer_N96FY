#ifndef N96FY_h
#define N96FY_h

#include <Arduino.h>

/* Arduino N96FY Anemometer Library
 * For anemometers based on reed switch technic - usually with 2 wires (for GND,VCC)
 * See: https://blogskyeweather.wordpress.com/2016/10/07/anemometer-improvement/
 *      https://hackaday.io/project/12397-modular-weather-station/log/40644-analog-sensors-ultraviolet-light-wind-and-rain
 *      
 * For this reed swith based sensor I found the following information:
 *    "A wind speed of 1.492 MPH (2,4 km/h) causes the switch to close once per second."
 * I assume "close" means turnaround.
 * 
 * We count the number of turnarounds within a dedicated period (e.g. 10 seconds) and 
 * just multiply with with the speed factor (e.g.KMH_Factor 2.401)
 * 
 * The implementaion is using 2  interrupts, so you can just use one single instance of this class.
 * - A timer interrupt for a dedicated call every second
 * - A hardware interrupt to track the changes on the signal (reed switch open/closing)
 * 
 * The current implementation is ESP8266 ONLY!
 *    Connect one cable to GND and the other to D2(
 * 
 * TODO Add support for other MCUs
 * 
 * (C) 2019 Tobias Stöger (@tstoegi)
 * 
*/

#define KMH_Factor    2.401 //One turnaround in Km/h
#define MPH_Factor    1.492 //One turnaround in MPH

#define AVERAGE_PERIOD_IN_SECONDS    10 // Calculate the average speed within the last n-Seconds

// Setup the ESP internal timer to 1 second
#define INTERVAL_US   80000000L // 80MHz == 1sec

#define INTERRUPT_PIN D2

class N96FY
{
  public:
    N96FY ();
    void begin();
    float speedInKMH();
    float speedInMPH();

  private:
    void startTimer();
    float calcNumberOfClosingsInOneSecond(void);

    static void ISR_sensor();
    static void ISR_everySecond();

    static int counter;
    static int values[AVERAGE_PERIOD_IN_SECONDS];
    static int seconds;
};



#endif
