#include <Arduino.h>
#ifdef USE_WIFI
#include <WiFi.h>
#else
#include <Ethernet.h>
#endif
#include <HttpClientEx.h>

#ifndef USE_WIFI
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

#define RESET_P	17				// Tie the W5500 reset pin to ESP32 GPIO17 pin.
#define CS_P 16

static void WizReset() {
    pinMode(RESET_P, OUTPUT);
    digitalWrite(RESET_P, HIGH);
    delay(250);
    digitalWrite(RESET_P, LOW);
    delay(50);
    digitalWrite(RESET_P, HIGH);
    delay(350);
}
#endif

void setup()
{
  unsigned long t0;
  Serial.begin(115200);

#ifdef USE_WIFI
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin("ssid", "******"); // Fill here your SSID and Password for your WiFi network
  t0 = millis();
  // Wait 30 seconds for WiFi network to connect
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 30000);
  if (WiFi.status() != WL_CONNECTED)
  {
    log_e("Could not connect to WiFi network!");
    return;
  }

  log_d("WiFi Connected!");
  log_d("RSSI: %ddb, BSSID: %s", WiFi.RSSI(), WiFi.BSSIDstr().c_str());

  WiFiClient client;
#else // USE_WIFI
  // start the Ethernet connection:
  Ethernet.init(CS_P);
  WizReset();
  log_d("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0)
  {
    log_e("Failed to configure Ethernet using DHCP!");
    return;
  } 
  else
  {
    log_d("DHCP assigned IP: %s\n", Ethernet.localIP().toString().c_str());
  }

  EthernetClient client;
#endif // USE_WIFI
  HttpClientEx http(client);
  String url = "http://google.com";
  Serial.printf("URL: %s\n", url.c_str());
  if (!http.begin(url))
  {
    Serial.printf("http.begin(\"%s\") failed!\n", url.c_str());
    return;
  }

  int code = http.get();
  if (code != 0)
  {
    Serial.printf("http.get() failed with code: %d\n", code);
    return;
  }

  t0 = millis();
  // Wait up to 10 seconds for the response
  while (!http.available() && millis() - t0 < 10000);

  // http.responseStatusCode() must be called before calling http.collectHeaders()
  Serial.printf("Response code: %d\n", http.responseStatusCode());
  
  HttpClientEx::Headers headers[] = { { "Expires" }, { "Location" } };

  http.collectHeaders(headers, sizeof(headers)/sizeof(*headers));
  Serial.printf("Header Expires: %s\n", headers[0].value.c_str());
  Serial.printf("Header Location: %s\n", headers[1].value.c_str());
  Serial.println("--------------------------------------- Body Start");

  while (http.available())
  {
    Serial.print((char)http.read());
  }

  Serial.println("Body End -----------------------------------------");
}

void loop()
{
  delay(2000);
}