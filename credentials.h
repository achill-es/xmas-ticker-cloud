#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#include <Arduino.h>
#include <PubSubClient.h>

// WiFi
static const char wifi_ssid[]     = "<your-ssid>";                    // enter your WiFi name here
static const char wifi_password[] = "<your-pwd>";                     // enter your WiFi password here

// MQTT Broker
static const char mqtt_broker[]   = "<your-broker>";                  // broker address
static const char mqtt_username[] = "<your-username>";                // username for authentication
static const char mqtt_password[] = "<your-user-pwd>";                // password for authentication

static const char mqtt_topic_t[]  = "MatrixDisplay2/text";            // define topic for text
static const char mqtt_topic_i[]  = "MatrixDisplay2/intensity";       // define topic for intensity
static const char mqtt_topic_s[]  = "MatrixDisplay2/scrollwait";      // define topic for scrollwait

// TCP port of MQTT cloud broker with TLS
static const uint16_t mqtt_port   = 8883;

// prefix part for the name (clientID) of the MQTT client
static const char mqtt_prefix[]   = "mqtt_scrolling";

// max length of MQTT clientID
static const uint8_t mqtt_clen    = 250;

static const char mqtt_cert[]     = \
"-----BEGIN CERTIFICATE-----\n" \
"=\n" \
"-----END CERTIFICATE-----";

// global data structure
extern PubSubClient mqttClient;

// exported function
extern void reconnect (bool);

#endif //CREDENTIALS_H
