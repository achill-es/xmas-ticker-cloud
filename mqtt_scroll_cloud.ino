// https://www.smarthome-tricks.de/software-iobroker/esp8266-max7219-mqtt-matrixdisplay-fuer-iobroker/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

#include "credentials.h"

// mapping of pins:

//DOT Matrix:       ESP8266 NodeMCU:
//VCC               5V (VIN when using USB)
//GND               GND
//DIN               D7 (GPIO13)
//CS                D8 
//CLK               D5 (GPIO14)

#define WAITxSEC (5000)

const byte pinCS = D8;                          // attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI)

const byte numberOfHorizontalDisplays = 4;      // number of modules horizontally
const byte numberOfVerticalDisplays = 1;        // number of modules vertically

Max72xxPanel matrix = Max72xxPanel (pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

unsigned int scrollwait = 25;                   // default delay in ms for scroll speed
byte intensity = 1;                             // default brightness of display (0 up to 15)

const byte spacer = 1;                          // width of a single space
const byte width = 5 + spacer;                  // character width

String MatrixText = "...";                      // default ticker text

void MQTTCallback (const char topic[], byte* payload, unsigned int length)
{
  String PayloadString = "";

  for (byte i = 0; i < length; i++) 
  {
    PayloadString = PayloadString + (char) payload[i];
  }

  Serial.print ("New message arrived in topic: ");
  Serial.println (topic);

  Serial.print ("Payload = ");
  Serial.println (PayloadString);

  if (strcmp (topic, mqtt_topic_t) == 0) 
  {
    if (MatrixText != PayloadString)
    {
      MatrixText = PayloadString;
      Serial.println ("Set new text.");
    }
  }

  if (strcmp (topic, mqtt_topic_i) == 0) 
  {
    if (intensity != PayloadString.toInt ())
    {
      intensity = PayloadString.toInt ();
      matrix.setIntensity (intensity);
      Serial.println ("Set new intensity.");
    }
  }

  if (strcmp (topic, mqtt_topic_s) == 0) 
  {
    if (scrollwait != PayloadString.toInt ())
    {
      scrollwait = PayloadString.toInt ();
      Serial.println ("Set new scrollwait.");
    }
  }
}

void setup ()
{
  matrix.fillScreen (LOW);
  matrix.shutdown (true);
  
  Serial.begin (115200);
  Serial.println ("Boot display...");

  for (byte matrixIndex = 0 ; matrixIndex < numberOfHorizontalDisplays ; matrixIndex++)
  {
    matrix.setRotation (matrixIndex, 1);        // set rotation for DOT matrix display
  }
  matrix.setIntensity (intensity);    // use a value between 0 and 15 for brightness

  // connect to WiFi and MQTT for the first time
  reconnect (true);
  
  // register callback
  mqttClient.setCallback (MQTTCallback);
}

void printMatrix ()
{
  matrix.shutdown (false);

  // https://github.com/markruys/arduino-Max72xxPanel/blob/master/examples/Ticker/Ticker.pde
  for (int i = 0 ; i < width * MatrixText.length() + matrix.width() - 1 - spacer; i++)
  {
    matrix.fillScreen (LOW);

    int letter = i / width;
    int x = (matrix.width () - 1) - i % width;
    int y = (matrix.height () - 8) / 2;   // center the text vertically

    while ((x + width - spacer >= 0) && (letter >= 0))
    {
      if (letter < MatrixText.length ())
      {
        matrix.drawChar (x, y, MatrixText[letter], HIGH, LOW, 1);
      }
      letter--;
      x -= width;
    }

    matrix.write ();      // send bitmap to display
    delay (scrollwait);
  }
}

void loop ()
{
  if (!mqttClient.connected ())
  {
    // make sure we get reconnected when connection to MQTT broker is lost
    reconnect (false);

    // register callback function
    mqttClient.setCallback (MQTTCallback);
  }

  mqttClient.loop ();
  delay (500);

  printMatrix ();
}

// end of code file