/*
 *    oorail-ddc-module-template
 *    version 1.0.2
 *    
 *    Copyright (c) 2020 IdeaPier LLC, All Rights Reserved
 * 
 */

/*
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *   
 */

/*
 *    For more information about our projects visit:
 *    
 *      https://oorail.co.uk/tech/
 *      https://oorail.co.uk/digital-dc/
 *      
 *    You can find helpful videos on our YouTube channel:
 *   
 *      https://youtube.com/oorail
 *      
 */

/*
 *    This code provides a starting point template for new modules.
 *    It is also the place where we test new common features before porting
 *    those changes into existing modules.
 *    
 *    Currently this module provides:
 *    
 *      Basic useful output via serial port
 *      ESP32 Chip ID
 *      WiFi support
 *      MDNS support
 *      Async Web Server
 *      Standard API endpoints
 *      HTTP-OTA Updates
 *      Supports ESP32 (ESP8266 is in progress)
 *      Basic ESP32 Multi-Core Support
 *      
 */

/*
 * The project, version and multicast DNS hostname should be changed to reflect the purpose of your module.
 * You should also increment the OORAIL_FW_BUILD version with each build.
 * 
 */

#define OORAIL_PROJECT "oorail-project"
#define OORAIL_MDNS_HOSTNAME "oorail-template"
#define OORAIL_VERSION "1.0.2"

#define OORAIL_COPYRIGHT "Copyright (c) 2020 IdeaPier LLC (oorail.co.uk)"
#define OORAIL_LICENSE "GPLv3"

/*
 * The OORAIL_FW_BUILD defines the BUILD version for the firmware.
 * This is used for over the air (OTA) updating once the code is 
 * installed on an ESP32/ESP8266 module.
 * 
 * The syntax for this is YYYYMMDDBB where:
 * 
 *  YYYY is the year (eg. 2020)
 *  MM is the month (eg. 02 for February)
 *  DD is the day of the month (eg. 17)
 *  BB is the build of the day, which is a counter that is incremented
 *  
 * This will ultimately be auto-generated by controller
 * 
 */
#define OORAIL_FW_BUILD "2020021700"

#if !(defined(ESP32) || defined(ESP8266))
 #error "Unsupported hardware platform - please use ESP32 or ESP8266"
#endif

/*
 * Platform identifier, platform specific features, pin definitions
 */

#if defined(ESP32)
#define OORAIL_PLATFORM "ESP32"
#define OORAIL_SERIAL_BAUD 115200

/*
 * FEATURE FLAGS:
 * 
 * OORAIL_OTA_UPDATE                Enable OTA Updates via HTTP (typically to controller)
 * OORAIL_ESP32_MULTICORE           Enable Multi-Core Processing Support
 * 
 * OORAIL_FEATURE_ASYNC_WEB         Asynchronous Web (HTTP) Server
 * OORAIL_FEATURE_LIST_API          Lists API Endpoints available via the serial monitor
 * OORAIL_FEATURE_UPTIME            Handle Uptime
 * 
 */
#define OORAIL_FEATURE_ASYNC_WEB 1
#define OORAIL_OTA_UPDATE 1
#define OORAIL_FEATURE_UPTIME 1
/*
 * #define OORAIL_ESP32_MULTICORE 1
 * #define OORAIL_FEATURE_LIST_API 1
 */
#endif

#if defined(ESP8266)
#define OORAIL_PLATFORM "ESP8266"
#define OORAIL_SERIAL_BAUD 115200
#endif

#include <strings.h>
#if defined(ESP32)
 #include <WiFi.h>
 #include <WiFiClient.h>
 #include <ESPmDNS.h>
//#error "ESP32 support is disabled for this module"
#else 
 #include <ESP8266WiFi.h>
//#error "ESP8266 support is disabled for this module"
#endif

#if defined(OORAIL_FEATURE_ASYNC_WEB)
#include <ESPAsyncWebServer.h>
#endif

#if defined(OORAIL_OTA_UPDATE)
#if defined(ESP32)
#include <Arduino.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#endif
#endif

#if defined(OORAIL_FEATURE_ASYNC_WEB)
#define OORAIL_HTTP_PORT 80

AsyncWebServer server(OORAIL_HTTP_PORT);
#endif

/* Constants */
#if defined(OORAIL_OTA_UPDATE)
#if defined(ESP32)
const char* oorail_ota_url = "http://oorail-controller.local/ota/firmware/oorail-project/esp32/";
#endif
#endif

/* Global Variables */
#if defined(ESP32)
uint64_t chipid;
#if defined(OORAIL_ESP32_MULTICORE)
TaskHandle_t core_zero;
#endif
#endif

#if defined(OORAIL_OTA_UPDATE)
WiFiClient ota_client;
#endif

/* WiFi credentials */
const char* ssid = "REPLACE-ME-WITH-YOUR-WIFI-SSID";
const char* password = "REPLACE-ME-WITH-YOUR-WIFI-PASSWORD";

void setup() {
  Serial.begin (OORAIL_SERIAL_BAUD);
  oorail_banner();
#if defined(ESP32)
#if defined(OORAIL_ESP32_MULTICORE)
  oorail_multicore_init();
#endif
#endif
  oorail_wifi();
#if defined(OORAIL_FEATURE_ASYNC_WEB)
  oorail_web_init();
#endif
}

#if defined(OORAIL_FEATURE_ASYNC_WEB)
void oorail_web_init_endpoints() {
/*
 * server.on("/endpoint/path/", HTTP_METHOD, function); 
 * 
 * #if defined(OORAIL_FEATURE_LIST_API)
 * Serial.printf(" * API Endpoint: /endpoint/path/ \r\n");
 * #endif
 * 
 */
  return;
}
#endif

/*
 * The loop function is continously executed on:
 * 
 *  ESP32             Core 1 (Dual-Core Platform)
 *  ESP8266           Core 0 (Single Core Platform)
 *  
 */
void loop() {

#if !defined(OORAIL_ESP32_MULTICORE)
/*
 * When ESP32 multicore mode is disabled, we trigger core_zero_loop() at the bottom
 * of the main loop function. This is done to help keep the code simplified. 
 * 
 * You may need to alter this behavior for your specific application.
 * 
 */
  core_zero_loop();
#endif 

}

/*
 * The core_zero_loop() function is continously executed on:
 * 
 * ESP32              Core 0 (Dual-Core Platform)
 * ESP8266            Core 0 (Single Core Platform)
 * 
 * Note: For ESP32 if MULTICORE is disabled, this runs on Core 1
 *       When ESP32_MULTICORE is disabled, we don't need to fake the infinite loop.
 *       You should also disable semaphore code when not using ESP32_MULTICORE as
 *       you don't need to pass data between cores.
 *       
 * Tip:  Remember that both processor cores can access GLOBAL variables.
 *       Make sure you put some kind of global variable lock on structures.
 * 
 */
void core_zero_loop() {
#if defined(OORAIL_ESP32_MULTICORE)
  /* Arduino IDE does this for us in loop for core 1 but we need to fake an infinite loop for core 0 */
  for (;;) {
#endif

/*
 * ESP32: This is where you put code to execute on the other core
 *        Remember that with Arduino IDE the ESP32 support runs other stuff on this core
 *        So try not to overload it. If you disable multicore support this stuff gets executed
 *        at the end of loop() unless you comment out the call to this function in loop().
 *        
 */

     
#if defined(OORAIL_ESP32_MULTICORE)
  } /* This closes out the faked infinite loop eg. for (;;) { at the top of this function */
#endif

}

#if defined(ESP32)
#if defined(OORAIL_ESP32_MULTICORE)
/*
 * This function initializes a new task called corezero which invokes core_zero_loop
 * 
 */
void oorail_multicore_init() {
  xTaskCreatePinnedToCore(
      core_zero_loop,                                   /* Function to implement the task */
      "corezero",                                       /* Name of the task */
      10000,                                            /* Stack size in words */
      NULL,                                             /* Task input parameter */
      0,                                                /* Priority of the task */
      &core_zero,                                       /* Task handle. */
      0);                                               /* Core where the task should run */
}
#endif
#endif

#if defined(OORAIL_FEATURE_ASYNC_WEB)
void oorail_web_init() {

/*
 * These are standard endpoints for all our modules.
 * 
 * /       - web root provides basic info
 * /debug  - provides advanced debugging information
 * /hc     - health check on module
 * /info   - provides module information
 * /ping   - basic ping check for testing if the API is up
 * /status - provides advanced status information on the module
 * 
 * /ota/update - Check for updates and perform OTA update
 */
#if defined(OORAIL_FEATURE_LIST_API)
  Serial.printf("\r\n * Registering API Endpoints: \r\n");
#endif
  
  server.on("/", HTTP_GET, oorail_web_root);
  server.on("/debug", HTTP_GET, oorail_web_debug);
  server.on("/hc", HTTP_GET, oorail_web_hc);
  server.on("/info", HTTP_GET, oorail_web_info);
  server.on("/ping", HTTP_GET, oorail_web_ping);
  server.on("/status", HTTP_GET, oorail_web_status);
#if defined(OORAIL_OTA_UPDATE)
  server.on("/ota/update", HTTP_GET, oorail_web_ota_update);
#endif

#if defined(OORAIL_FEATURE_LIST_API)
  Serial.printf(" * API Endpoint: / \r\n");
  Serial.printf(" * API Endpoint: /debug \r\n");
  Serial.printf(" * API Endpoint: /hc \r\n");
  Serial.printf(" * API Endpoint: /info \r\n");
  Serial.printf(" * API Endpoint: /ping \r\n");
  Serial.printf(" * API Endpoint: /status \r\n");
#if defined(OORAIL_OTA_UPDATE)
  Serial.printf(" * API Endpoint: /ota/update \r\n");
#endif
#endif

  oorail_web_init_endpoints();

  server.onNotFound(oorail_web_notFound);
  server.begin();  
}

void oorail_web_notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found\r\n");
}

void oorail_web_root(AsyncWebServerRequest *request) {
  char message[32];
  String web_message;

  sprintf(message,"Hello from Core %d\r\n", xPortGetCoreID());
  web_message = message;
    
  request->send(200, "text/plain", web_message);
}

void oorail_web_debug(AsyncWebServerRequest *request) {
  char response[64];

  /*
   * Modify this to include specific debug information for the module
   */
  sprintf(response,"\r\nModule Debug Information: \r\n");
  request->send(200, "text/plain", response);
}

void oorail_web_hc(AsyncWebServerRequest *request) {
  /*
   * Modify this to run module specific checks and return a result code
   * 
   * This should return 200 OK if things are OK
   * This should return a custom 5xx error if things are NOT OK
   * 
   * This is used by the controller, so make sure your controller health check code
   * for this module matches what you put below.
   * 
   */
  request->send(200, "text/plain", "I am OK\r\n");
}

void oorail_web_info(AsyncWebServerRequest *request) {
  char response[1024];
#if defined(OORAIL_FEATURE_UPTIME)
  unsigned long uptime = millis();
  unsigned long upseconds = 0;
  unsigned long upminutes = 0;
  unsigned long uphours = 0;
  unsigned long updays = 0;
#endif
  IPAddress oorail_ip = WiFi.localIP();
  IPAddress oorail_subnet = WiFi.subnetMask();
  IPAddress oorail_gw = WiFi.gatewayIP();

#if defined(OORAIL_FEATURE_UPTIME)
  upseconds = uptime/1000;
  upminutes = upseconds/60; 
  uphours = upminutes/60;
  updays = uphours/24; 
  upseconds -= (upminutes*60);
  upminutes -= (uphours*60); 
  uphours -= (updays*24); 
#endif

#if defined(ESP32)
#if defined(OORAIL_FEATURE_UPTIME)
  sprintf(response,"\r\n%s, version %s [%s-%04X%08X] (Build: %s)\r\n\r\nLicensed under %s\r\n\r\nModule IP: %d.%d.%d.%d (%d.%d.%d.%d)\r\nModule GW: %d.%d.%d.%d\r\n\r\nUptime is %d %s, %d %s, %d %s and %d %s\r\n\r\n",
    OORAIL_PROJECT, OORAIL_VERSION, OORAIL_PLATFORM, (uint16_t)(chipid>>32), (uint32_t)chipid, OORAIL_FW_BUILD, OORAIL_LICENSE,
    oorail_ip[0], oorail_ip[1], oorail_ip[2], oorail_ip[3], oorail_subnet[0], oorail_subnet[1], oorail_subnet[2], oorail_subnet[3],
    oorail_gw[0], oorail_gw[1], oorail_gw[2], oorail_gw[3], updays,(updays > 1) ? "days" : (updays != 0) ? "day" : "days", uphours, (uphours > 1) ? "hours" : (uphours != 0) ? "hour" : "hours", 
    upminutes, (upminutes > 1) ? "minutes" : (upminutes != 0) ? "minute" : "minutes", upseconds, (upseconds > 1) ? "seconds" : (upseconds != 0) ? "second" : "seconds");
#endif
#endif
#if !defined(OORAIL_FEATURE_UPTIME)
#if defined(ESP32)
  sprintf(response,"\r\n%s, version %s [%s-%04X%08X] (Build: %s)\r\n\r\nLicensed under %s\r\n\r\nModule IP: %d.%d.%d.%d (%d.%d.%d.%d)\r\nModule GW: %d.%d.%d.%d\r\n\r\n",
    OORAIL_PROJECT, OORAIL_VERSION, OORAIL_PLATFORM, (uint16_t)(chipid>>32), (uint32_t)chipid, OORAIL_FW_BUILD, OORAIL_LICENSE,
    oorail_ip[0], oorail_ip[1], oorail_ip[2], oorail_ip[3], oorail_subnet[0], oorail_subnet[1], oorail_subnet[2], oorail_subnet[3],
    oorail_gw[0], oorail_gw[1], oorail_gw[2], oorail_gw[3]);
#endif
#if !defined(ESP32)
  sprintf(response,"\r\n%s, version %s [%s] (Build: %s)\r\n\r\nLicensed under %s\r\n\r\n", 
    OORAIL_PROJECT, OORAIL_VERSION, OORAIL_PLATFORM, OORAIL_FW_BUILD, OORAIL_LICENSE);
#endif
#endif
  request->send(200, "text/plain", response);
}

void oorail_web_ping(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "OK\r\n");
}

void oorail_web_status(AsyncWebServerRequest *request) {
  char response[1024];
  /*
   * This end point should dump everything displayed in /info
   * It should also provide additional information on the current state
   * 
   */
#if defined(ESP32)
  IPAddress oorail_ip = WiFi.localIP();
  IPAddress oorail_subnet = WiFi.subnetMask();
  IPAddress oorail_gw = WiFi.gatewayIP();

  sprintf(response,"\r\n%s, version %s [%s-%04X%08X] (Build: %s)\r\n\r\nLicensed under %s\r\n\r\nModule IP: %d.%d.%d.%d (%d.%d.%d.%d)\r\nModule GW: %d.%d.%d.%d\r\n\r\n",
    OORAIL_PROJECT, OORAIL_VERSION, OORAIL_PLATFORM, (uint16_t)(chipid>>32), (uint32_t)chipid, OORAIL_FW_BUILD, OORAIL_LICENSE,
    oorail_ip[0], oorail_ip[1], oorail_ip[2], oorail_ip[3], oorail_subnet[0], oorail_subnet[1], oorail_subnet[2], oorail_subnet[3],
    oorail_gw[0], oorail_gw[1], oorail_gw[2], oorail_gw[3]);
#else
  sprintf(response,"\r\n%s, version %s [%s] (Build: %s)\r\n\r\nLicensed under %s\r\n\r\n", 
    OORAIL_PROJECT, OORAIL_VERSION, OORAIL_PLATFORM, OORAIL_FW_BUILD, OORAIL_LICENSE);
#endif
  request->send(200, "text/plain", response);
}

#if defined(OORAIL_OTA_UPDATE)
void oorail_web_ota_update(AsyncWebServerRequest *request) {
  HTTPClient httpClient;
  int httpCode = 0;
  String firmwareURL = String(oorail_ota_url);

  firmwareURL.concat("latest.bin");
  httpClient.begin( ota_client, firmwareURL );
  httpCode = httpClient.GET();

  if (httpCode == 200) {
    request->send(200, "text/plain", "\r\nOTA Check for Update has been queued\r\n");
  } else {
    request->send(200, "text/plain", "\r\nOTA URL Not Found\r\n");
  }
}
#endif

#endif

void oorail_wifi() {
#if defined(ESP32)
  Serial.print(" * Setting up WiFi on core ");
  Serial.println(xPortGetCoreID());
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print(" * Connecting to WiFi [");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".*");
  }

  Serial.println("]");
  Serial.println("");
  Serial.print(" * Connected to ");
  Serial.println(ssid);
  Serial.print(" * IP address  : ");
  Serial.println(WiFi.localIP());
  Serial.print(" * Subnet Mask : ");
  Serial.println(WiFi.subnetMask());
  Serial.print(" * Gateway IP  : ");
  Serial.println(WiFi.gatewayIP());
  
  if (MDNS.begin(OORAIL_MDNS_HOSTNAME)) {
    Serial.printf("\r\n * MDNS registered as %s.local\r\n", OORAIL_MDNS_HOSTNAME);
    Serial.printf(" * MDNS responder started\r\n");
  }
#endif
}
 
void oorail_banner() {
#if defined(ESP32)
  chipid = ESP.getEfuseMac();
#endif

  Serial.printf("\r\n\r\n%s, version %s [%s] (Build: %s)\r\n", OORAIL_PROJECT, OORAIL_VERSION, OORAIL_PLATFORM, OORAIL_FW_BUILD);
  Serial.printf("%s\r\n", OORAIL_COPYRIGHT);
  Serial.printf("For additional information visit: \r\n https://oorail.co.uk/tech/ \r\n\r\n");
  Serial.printf("License: %s\r\n\r\n", OORAIL_LICENSE);

#if defined(ESP32)
  Serial.printf(" * ESP32 Device ID: %04X",(uint16_t)(chipid>>32));
  Serial.printf("%08X\r\n",(uint32_t)chipid);
  Serial.print(" * ESP32 is using processor core ");
  Serial.println(xPortGetCoreID());
#endif
#if defined(OORAIL_FEATURE_ASYNC_WEB)
  Serial.printf(" * Active Feature: Web Server (HTTP)\r\n");
#endif 
#if defined(OORAIL_OTA_UPDATE)
  Serial.printf(" * Active Feature: OTA HTTP Update\r\n");
  Serial.printf(" * OTA Update URL is %s\r\n", oorail_ota_url);
#endif
#if defined(OORAIL_FEATURE_LIST_API)
  Serial.printf(" * Active Feature: List API Endpoints on Serial Monitor\r\n");
#endif
#if defined(ESP32)
#if defined(OORAIL_ESP32_MULTICORE)
  Serial.printf(" * Active Feature: ESP32 Multicore Support Enabled\r\n");
#endif
#endif
  Serial.printf("\r\n");
}
