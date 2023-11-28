/*
 *  module to (re)connect to both WiFi and then MQTT broker
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <string.h>

#include "credentials.h"

#define WAITxSEC  (5000)

// init a secured WiFi client
//static WiFiClient espClient;
static WiFiClientSecure espClient;

// use WiFi client to init a MQTT client
PubSubClient mqttClient (espClient);

//https://arduino.stackexchange.com/questions/91039/coorect-syntax-for-setcacert-esp8266
static X509List cert (mqtt_cert);

void reconnect (bool fromsetup) 
{
  if (fromsetup)
  {
    WiFi.mode (WIFI_STA);
    // set root CA certificate for implicit checking
    //espClient.setCACert (mqtt_cert, sizeof (mqtt_cert) - 1);
    //https://arduino.stackexchange.com/questions/91039/coorect-syntax-for-setcacert-esp8266
    configTime (0, 0, "pool.ntp.org");      // get UTC time via NTP
    espClient.setTrustAnchors (&cert);      // add root certificate
    // set our MQTT broker
    mqttClient.setServer (mqtt_broker, mqtt_port);
  }

  // connecting to a WiFi network
  if (WiFi.status () != WL_CONNECTED)
  {
    do
    {
      delay (WAITxSEC);
      Serial.printf ("%sonnecting to WiFi %s...\n", (fromsetup ? "C" : "Rec"), wifi_ssid);
      WiFi.begin (wifi_ssid, wifi_password);
    } while (WiFi.status () != WL_CONNECTED);
    
    Serial.printf ("%sonnected to the WiFi network.\n", (fromsetup ? "C" : "Rec"));
    Serial.printf ("IP address: %s\n", WiFi.localIP().toString().c_str());
  }
  
  // connecting to MQTT broker
  while (!mqttClient.connected ())
  {
    char mqtt_clientid[mqtt_clen];

    sprintf (mqtt_clientid, "%s-%s", mqtt_prefix, WiFi.localIP().toString().c_str());
    Serial.printf ("Client %s %sconnects to the MQTT broker...\n", mqtt_clientid, fromsetup ? "" : "re");

    if (mqttClient.connect (mqtt_clientid, mqtt_username, mqtt_password))
    {
      Serial.printf ("%sonnected to MQTT broker.\n", (fromsetup ? "C" : "Rec"));
    }
    else
    {
      Serial.printf ("Failed to %sconnect to MQTT broker, rc = %d.\n", fromsetup ? "" : "re", mqttClient.state());
      Serial.println ("Retrying in a few seconds...");
      delay (WAITxSEC);
    }
  }

  // subscribe to our topics
  mqttClient.subscribe (mqtt_topic_t);
  mqttClient.subscribe (mqtt_topic_i);
  mqttClient.subscribe (mqtt_topic_s);
  Serial.println ("Subscribed to all our topics.");
}

// end of code file