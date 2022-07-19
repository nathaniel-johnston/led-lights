#include <Adafruit_NeoPixel.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <Wire.h>
#include <ArduinoJson.h>

#define PIN        13
#define NUMPIXELS 10

const char* ssid = "Godtier";
const char* pwd = "$up3rFa$t";
const char* broker = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

DynamicJsonDocument doc(2048);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  WiFi.begin(ssid, pwd);

  Serial.println("connecting");
  while(WiFi.status() != WL_CONNECTED);

  Serial.println("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(broker, 1883);
  client.setCallback(callback);

  pixels.begin();
  pixels.clear();
}

void callback(char* topic, byte* payload, unsigned int length) {
  DeserializationError error = deserializeJson(doc, (char*)payload);
  JsonArray red = doc["red"];
  JsonArray green = doc["green"];
  JsonArray blue = doc["blue"];

  for(int i = 0; i < red.size(); i++) {
    pixels.setPixelColor(i, pixels.Color(red[i], green[i], blue[i]));
  }
  pixels.show();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("medistation")) {
      Serial.println("connected");
      client.subscribe("binillights");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 second");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

void loop() {

  if(!client.connected()) {
    reconnect();
  }
  client.loop();
}
