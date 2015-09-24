This is an Arduino library to easily perform scheduled tasks.  The timing
is done by looking at an RTC and an update function that must be called
from the Arduino main loop.

This library was inspired by Simon Monk's Timer.

The reason for this library is that it uses an RTC (e.g. DS3231) to supply
a clock. It will remain functional when the MCU goes to "deep sleep".

The biggest difference with Timer is that RTCTimer uses the units that RTC
can give, which is most likely seconds (not milliseconds).
