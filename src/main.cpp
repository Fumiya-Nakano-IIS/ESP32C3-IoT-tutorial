#include <Arduino.h>
#include <ESP32Servo.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <esp_wifi.h>

// ==== CONFIG ====
const char* ssid = "ESP32C3-LED";
const char* password = "12345678";
const int ledPin = 2;
const int servoPin = 4;
const char* configPath = "/config.txt";

// ==== GLOBALS ====
WebServer server(80);
Servo myServo;
int servoOnAngle = 90;
int servoOffAngle = 0;
bool timerEnabled = false;
unsigned long timerInterval = 1000;

hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile bool toggleState = false;

// ==== UTILITIES ====
void saveConfig() {
  File f = SPIFFS.open(configPath, "w");
  if (!f) return;
  f.printf("%d,%d,%lu\n", servoOnAngle, servoOffAngle, timerInterval);
  f.close();
}

void loadConfig() {
  File f = SPIFFS.open(configPath, "r");
  if (!f) return;
  String line = f.readStringUntil('\n');
  f.close();
  int on, off;
  unsigned long interval;
  if (sscanf(line.c_str(), "%d,%d,%lu", &on, &off, &interval) == 3) {
    servoOnAngle = on;
    servoOffAngle = off;
    timerInterval = interval;
  }
}

// ==== TIMER ISR ====
void IRAM_ATTR onTimer() {
  if (!timerEnabled) return;
  portENTER_CRITICAL_ISR(&timerMux);
  toggleState = !toggleState;
  if (toggleState) {
    digitalWrite(ledPin, LOW);
    myServo.write(servoOnAngle);
  } else {
    digitalWrite(ledPin, HIGH);
    myServo.write(servoOffAngle);
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}

// ==== HTTP HANDLERS ====
void handleRoot() {
  File f = SPIFFS.open("/index.html", "r");
  if (!f) {
    server.send(500, "text/plain", "Failed to open");
    return;
  }
  server.streamFile(f, "text/html");
  f.close();
}

void handlePins() {
  String json = "{\"ledPin\":" + String(ledPin) +
                ",\"servoPin\":" + String(servoPin) + "}";
  server.send(200, "application/json", json);
}

void handleStatus() {
  String json = "{";
  json += "\"on\":" + String(servoOnAngle) + ",";
  json += "\"off\":" + String(servoOffAngle) + ",";
  json += "\"timerEnabled\":" + String(timerEnabled ? 1 : 0) + ",";
  json += "\"timerInterval\":" + String(timerInterval);
  json += "}";
  server.send(200, "application/json", json);
}

void handleOn() {
  digitalWrite(ledPin, LOW);
  myServo.write(servoOnAngle);
  server.send(200, "text/plain", String(servoOnAngle));
}

void handleOff() {
  digitalWrite(ledPin, HIGH);
  myServo.write(servoOffAngle);
  server.send(200, "text/plain", String(servoOffAngle));
}

void handleSet() {
  bool changed = false;
  if (server.hasArg("on")) {
    servoOnAngle = server.arg("on").toInt();
    changed = true;
  }
  if (server.hasArg("off")) {
    servoOffAngle = server.arg("off").toInt();
    changed = true;
  }
  if (changed) saveConfig();
  server.send(200, "text/plain",
              "OnAngle:" + String(servoOnAngle) +
                  ", OffAngle:" + String(servoOffAngle));
}

void handleTimer() {
  bool changed = false;
  if (server.hasArg("enable")) timerEnabled = server.arg("enable").toInt();
  if (server.hasArg("interval")) {
    timerInterval = server.arg("interval").toInt();
    timerAlarmWrite(timer, timerInterval * 1000, true);  // microseconds
    changed = true;
  }
  if (changed) saveConfig();
  server.send(200, "text/plain",
              "Timer Enabled:" + String(timerEnabled) +
                  ", Interval:" + String(timerInterval));
}

// ==== SETUP & LOOP ====
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  SPIFFS.begin(true);
  loadConfig();
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 500, 2400);
  myServo.write(servoOffAngle);
  WiFi.softAP(ssid, password);
  esp_wifi_set_max_tx_power(20 * 4);

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/set", handleSet);
  server.on("/timer", handleTimer);
  server.on("/pins", handlePins);
  server.on("/status", handleStatus);
  server.serveStatic("/", SPIFFS, "/");
  server.begin();

  timer = timerBegin(0, 80, true);  // 80 prescaler: 1us per tick (80MHz/80)
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, timerInterval * 1000, true);  // microseconds
  timerAlarmEnable(timer);
}

void loop() { server.handleClient(); }
