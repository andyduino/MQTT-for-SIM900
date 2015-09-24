/*
 * This example shows how to synchronize the RTC with
 * the help of a GPRSbee and a time service.
 * 
 * Notice that the "epoch" of the RTC is used and the time units
 * are in seconds.  (The Timer library works with millis() so its
 * time units are milliseconds.
 */

//############ GPRS access details ################
#define APN ""    // Fill in your APN here

//############ time service ################
#define TIMEURL "http://time.sodaq.net/"

#include <GPRSbee.h>
#include <Sodaq_DS3231.h>

#include <RTCTimer.h>

RTCTimer timer;           // Instantiate the timer

void setup()
{
  // Do the check every 24 hours
  timer.every(24L * 60 * 60, syncRTCwithServer);

  // Instruct the RTCTimer how to get the current timestamp
  timer.setNowCallback(getNow);
}

void loop()
{
  timer.update();
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




/*
 * Extract a number from an ASCII string
 */
bool getUValue(const char *buffer, uint32_t * value)
{
  char *eptr;
  if (*buffer == '=') {
    ++buffer;
  }
  *value = strtoul(buffer, &eptr, 0);
  if (eptr != buffer) {
    return true;
  }
  return false;
}

//################ RTC ################
/*
 * Synchronize RTC with a time server
 */
void syncRTCwithServer(uint32_t now)
{
  char buffer[20];
  if (gprsbee.doHTTPGET(APN, TIMEURL, buffer, sizeof(buffer))) {
    uint32_t newTs;
    if (getUValue(buffer, &newTs)) {
      // Tweak the timestamp a little because doHTTPGET took a few seconds
      // to close the connection after getting the time from the server.
      // The number is just a rough guess.
      newTs += 3;

      uint32_t oldTs = getNow();
      int32_t diffTs = abs(newTs - oldTs);

      // Only change the RTC if the new value differs more than 3 seconds
      if (diffTs > 3) {
        timer.adjust(oldTs, newTs);     // Update the RTCTimer administration
        rtc.setEpoch(newTs);
      }
      return;
    }
  }

  // Sync failed.
  // ???? Retry in, say, 3 minutes
  //timer.every(5L * 60, syncRTCwithServer, 1);
}
