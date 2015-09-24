/*
 * This example shows how to schedule some work at
 * regular intervals.
 *
 * This example is a bit more advanced. It uses the WDT as
 * an interrupt to wake up from deep sleep. During the wake
 * up it does the RTCTimer.update so that it can do scheduled
 * tasks. The RTC continues to run during the deep sleep so the
 * timing remains accurate. And at the same time the power
 * consumption is as low as possible.
 * 
 * Notice that the "epoch" of the RTC is used and the time units
 * are in seconds.  (The Timer library works with millis() so its
 * time units are milliseconds.
 */

// We need these includes to program the WDT
#include <avr/sleep.h>
#include <avr/wdt.h>

#include <Sodaq_DS3231.h>

#include <RTCTimer.h>

RTCTimer timer;           // Instantiate the timer

const int led = 6;

bool hz_flag;

void setup()
{
  pinMode(led, OUTPUT);

  // Do the work every 5 seconds
  timer.every(5, toggleLed);

  // Instruct the RTCTimer how to get the current timestamp
  timer.setNowCallback(getNow);

  setupWatchdog();

  interrupts();
}

void loop()
{
  if (hz_flag) {
    hz_flag = false;
    wdt_reset();
    WDTCSR |= _BV(WDIE);
  }

  timer.update();

  systemSleep();
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



//######### watchdog and system sleep #############
void systemSleep()
{
  ADCSRA &= ~_BV(ADEN);         // ADC disabled

  /*
  * Possible sleep modes are (see sleep.h):
  #define SLEEP_MODE_IDLE         (0)
  #define SLEEP_MODE_ADC          _BV(SM0)
  #define SLEEP_MODE_PWR_DOWN     _BV(SM1)
  #define SLEEP_MODE_PWR_SAVE     (_BV(SM0) | _BV(SM1))
  #define SLEEP_MODE_STANDBY      (_BV(SM1) | _BV(SM2))
  #define SLEEP_MODE_EXT_STANDBY  (_BV(SM0) | _BV(SM1) | _BV(SM2))
  */
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_mode();

  ADCSRA |= _BV(ADEN);          // ADC enabled
}


// The watchdog timer is used to make timed interrupts
#if 0
// Both WDE and WDIE are set!!
// Note from the doc: "Executing the corresponding interrupt
// vector will clear WDIE and WDIF automatically by hardware
// (the Watchdog goes to System Reset Mode)
#define my_wdt_enable(value)   \
__asm__ __volatile__ (  \
    "in __tmp_reg__,__SREG__" "\n\t"    \
    "cli" "\n\t"    \
    "wdr" "\n\t"    \
    "sts %0,%1" "\n\t"  \
    "out __SREG__,__tmp_reg__" "\n\t"   \
    "sts %0,%2" "\n\t" \
    : /* no outputs */  \
    : "M" (_SFR_MEM_ADDR(_WD_CONTROL_REG)), \
      "r" (_BV(_WD_CHANGE_BIT) | _BV(WDE)), \
      "r" ((uint8_t) (((value & 0x08) ? _WD_PS3_MASK : 0x00) | \
          _BV(WDE) | _BV(WDIE) | (value & 0x07)) ) \
    : "r0"  \
)
#else
// Only WDIE is set!!
#define my_wdt_enable(value)   \
__asm__ __volatile__ (  \
    "in __tmp_reg__,__SREG__" "\n\t"    \
    "cli" "\n\t"    \
    "wdr" "\n\t"    \
    "sts %0,%1" "\n\t"  \
    "out __SREG__,__tmp_reg__" "\n\t"   \
    "sts %0,%2" "\n\t" \
    : /* no outputs */  \
    : "M" (_SFR_MEM_ADDR(_WD_CONTROL_REG)), \
      "r" (_BV(_WD_CHANGE_BIT) | _BV(WDE)), \
      "r" ((uint8_t) (((value & 0x08) ? _WD_PS3_MASK : 0x00) | \
          _BV(WDIE) | (value & 0x07)) ) \
    : "r0"  \
)
#endif

void setupWatchdog()
{
  my_wdt_enable(WDTO_1S);
}

//################ interrupt ################
ISR(WDT_vect)
{
  hz_flag = true;
}
