/*
 * This example shows how to schedule some work at
 * regular intervals.
 * 
 * Notice that the "epoch" of the RTC is used and the time units
 * are in seconds.  (The Timer library works with millis() so its
 * time units are milliseconds.
 */

#include <Sodaq_DS3231.h>

#include <RTCTimer.h>

RTCTimer timer;           // Instantiate the timer

const int led = 6;

void setup()
{
  pinMode(led, OUTPUT);

  // Do the work every 5 seconds
  timer.every(5, toggleLed);

  // Instruct the RTCTimer how to get the current timestamp
  timer.setNowCallback(getNow);
}

void loop()
{
  timer.update();
}



void toggleLed(uint32_t ts)
{
  static bool on;

  on = !on;
  digitalWrite(led, on);
}

/*
 * Return the current timestamp
 *
 * This is a general purpose wrapper function to get the current timestamp.
 * It can also be used for timer.setNowCallback
 */
uint32_t getNow()
{
  return rtc.now().getEpoch();
}
