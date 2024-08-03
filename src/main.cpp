#include <Arduino.h>
#include "SoftwareSerial.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
const int ledPin = 2; // GPIO2 is typically the onboard LED pin on ESP8266

const char *ssid = "Bismillah";
const char *password = "1234567890";
// const char *ssid = "KARAN";
// const char *password = "12345679";

WiFiClient client;

SoftwareSerial fromUno(13, 15); // RX, TX

void setup()
{
  Serial.begin(9600);
  fromUno.begin(9600);

  // Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Turn off LED initially

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(ledPin, LOW); // Turn on LED to indicate Wi-Fi connected
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(ledPin, LOW); // Turn on LED to indicate Wi-Fi connected
    String data = "";
    while (fromUno.available() > 0)
    {
      data += char(fromUno.read());
      // delay(1);
    }
    // Serial.println(data);

    // Ensure the data string is properly terminated
    data.trim();

    // Split the data string by commas
    int commaIndex1 = data.indexOf(',');
    String light = data.substring(0, commaIndex1);
    int commaIndex2 = data.indexOf(',', commaIndex1 + 1);
    String pln = data.substring(commaIndex1 + 1, commaIndex2);
    int commaIndex3 = data.indexOf(',', commaIndex2 + 1);
    String value1 = data.substring(commaIndex2 + 1, commaIndex3);
    int commaIndex4 = data.indexOf(',', commaIndex3 + 1);
    int commaIndex5 = data.indexOf(',', commaIndex4 + 1);
    String value2 = data.substring(commaIndex3 + 1, commaIndex4);
    String value3 = data.substring(commaIndex4 + 1, commaIndex5);
    String value4 = data.substring(commaIndex5 + 1);

    // Ensure no extra data is included
    value4 = value4.substring(0, value4.indexOf('\n') >= 0 ? value4.indexOf('\n') : value4.length());

    // Construct the JSON payload
    String jsonSend = "{\"light\":\"" + light + "\",\"pln\":\"" + pln + "\",\"value1\":\"" + value1 + "\",\"value2\":\"" + value2 + "\",\"value3\":\"" + value3 + "\",\"no\":\"" + value4 + "\"}";

    Serial.println(jsonSend);

    HTTPClient http;
    http.begin(client, "http://traffic-light2.kicap-karan.com/");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonSend);
    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    delay(1000);
  }
  else
  {
    Serial.println("WiFi is disconnected, attempting to reconnect");
    digitalWrite(ledPin, HIGH);
    WiFi.begin(ssid, password);

    // Wait for connection
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 20)
    { // wait for 10 seconds max
      delay(500);
      Serial.print(".");
      timeout++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("");
      Serial.println("Reconnected to WiFi");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      digitalWrite(ledPin, LOW); // Turn on LED to indicate Wi-Fi connected
    }
    else
    {
      Serial.println("");
      Serial.println("Failed to reconnect to WiFi");
    }
  }
}
