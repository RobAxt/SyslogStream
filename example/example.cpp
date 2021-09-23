#include <Arduino.h>
#include <Homie.h>
#include "SyslogStream.hpp"

#define FW_NAME "Smart Weather Station"
#define FW_VERSION "0.0.0" 
#define SLEEP_TIME 60 * 1000000

void onHomieEvent(const HomieEvent & event);

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */


void setup() {
#if DEBUG_APP
  Serial.begin(SERIAL_SPEED,SERIAL_8N1,SERIAL_TX_ONLY);
  Serial << endl << endl;
  Homie.setLoggingPrinter(&SyslogStream);
  Homie.getLogger() << F("Build Date and Time: ") << __DATE__ << " & " << __TIME__ << endl;
#else
  Homie.disableLogging();
#endif
  Homie_setFirmware(FW_NAME, FW_VERSION); // The underscore is not a typo! See Magic bytes
  //Homie.setSetupFunction([](){});
  Homie.setLoopFunction([](){SyslogStream.loop();});
  //Homie.setBroadcastHandler([](const String& level, const String& value){});
  Homie.onEvent(onHomieEvent);
  //Homie.disableLedFeedback();
  Homie.disableResetTrigger();
  Homie.setup();
  SyslogStream.setup(Homie.getConfiguration().mqtt.server.host, Homie.getConfiguration().deviceId, "");
}


void loop() {
  Homie.loop();
}

void onHomieEvent(const HomieEvent & event) {
  switch (event.type) {
    case HomieEventType::MQTT_DISCONNECTED:
    case HomieEventType::WIFI_DISCONNECTED:
     // WiFi.disconnect();
      break;
    case HomieEventType::SENDING_STATISTICS:
      Homie.getLogger() << F("  • Heap: ") << ESP.getFreeHeap() << endl;
      Homie.getLogger() << F("  • Stack: ") << ESP.getFreeContStack() << endl;
      Homie.getLogger() << F("  • Last reset reason: ") << ESP.getResetInfo() << endl;
      break;
    case HomieEventType::OTA_STARTED:
      break;
    case HomieEventType::OTA_SUCCESSFUL:
    case HomieEventType::OTA_FAILED:
      break;
    case HomieEventType::READY_TO_SLEEP:
      Homie.getLogger() <<  F("  • Ready to sleep...") << endl;
      Homie.doDeepSleep(SLEEP_TIME);
      break;
    default:
      break;
  }
}