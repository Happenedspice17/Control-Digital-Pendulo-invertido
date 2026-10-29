#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "pagina.h"

#define AP_SSID    "Pendulo-Invertido"
#define AP_PASS    "pendulo123"
#define AP_CANAL   6
#define PIN_RX2    16
#define PIN_TX2    17
#define BAUD_LINK  460800
#define PERIODO_MS 40
#define PIN_LED    2

WebServer http(80);
WebSocketsServer ws(81);

String lote, linea;
uint32_t tLote = 0, tDato = 0, tPedir = 0;

void onWs(uint8_t id, WStype_t tipo, uint8_t *dat, size_t n) {
  if (tipo == WStype_TEXT && n > 0 && n < 64) {
    Serial2.write(dat, n);
    Serial2.write('\n');
  } else if (tipo == WStype_CONNECTED) {
    Serial2.println("i");
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.setRxBufferSize(4096);
  Serial2.begin(BAUD_LINK, SERIAL_8N1, PIN_RX2, PIN_TX2);
  pinMode(PIN_LED, OUTPUT);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS, AP_CANAL, 0, 4);
  WiFi.setSleep(false);

  http.on("/", [] { http.send_P(200, "text/html", PAGINA); });
  http.onNotFound([] {
    http.sendHeader("Location", "/");
    http.send(302);
  });
  http.begin();
  ws.begin();
  ws.onEvent(onWs);

  lote.reserve(2048);
  linea.reserve(128);
  Serial.printf("AP %s  ->  http://%s\n", AP_SSID, WiFi.softAPIP().toString().c_str());
}

void loop() {
  http.handleClient();
  ws.loop();

  while (Serial2.available()) {
    char c = Serial2.read();
    if (c == '\r') continue;
    if (c != '\n') {
      if (linea.length() < 120) linea += c;
      continue;
    }
    if (linea.length()) {
      if (linea[0] == '#') {
        String m = "M" + linea;
        ws.broadcastTXT(m);
        Serial.println(linea);
      } else {
        lote += linea;
        lote += '\n';
        tDato = millis();
      }
    }
    linea = "";
  }

  uint32_t t = millis();
  if (lote.length() && t - tLote >= PERIODO_MS) {
    tLote = t;
    String m = "T" + lote;
    ws.broadcastTXT(m);
    lote = "";
  }
  if (lote.length() > 3000) lote = "";

  if (t - tDato > 1000 && t - tPedir > 1000) {
    tPedir = t;
    Serial2.println("t1");
  }
  digitalWrite(PIN_LED, ws.connectedClients() > 0);
}
