# FoxHuntDTMF
 Fox Hunt transmitter controller with DTMF remote commands   

 Bruce Ratoff, ko4xl@yahoo.com   

 * Designed to run on a small ATtiny85 board such as Digispark or similer.   
 * Requires tinyDTMF library (https://github.com/bratoff/tinyDTMF)   

 Complete documentation can be found in the source code.

DTMF command set:

  *   0 - disable transmission of Fox messages (just listen for further commands)
  *       Fox responds with "QRT"
  *   1 - enable Fox transmissions.  Current message (Lost or Found) will be sent immediately.
  *   4 - put fox in "found" state - sends back "F" - overrides the switch
  *   5 - status - sends back "L" for lost or "F" for found
  *   6 - put fox in "lost" state and enable transmission (like 7 followed by 1)
  *   7 - put fox in "lost" state - sends back "L" - overrides the switch
  *   9 - Send Fox Lost/Found message right now if transmission is enabled
  *   A - Read Vcc - sends back "BV xx" - xx is tenths of a volt
  *   B - blink LED continuously when not transmitting
  *   D - toggles Tx LED on and off with PTT - sends back "ON" or "OF"
  *   Any other key will be echoed back in morse, followed by '?'
  *   
  *   NOTE:  0 and 1 only affect the "lost" and "found" messages.  The command responses are always enabled.
