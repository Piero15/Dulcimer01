#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>

struct Datos {
  int valor1;
  int valor2;
};

Datos datosRecibidos;

Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(115200);

  servo1.attach(12);
  servo2.attach(13);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }

  esp_now_register_recv_cb([](const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    if (len == sizeof(Datos)) {
      memcpy(&datosRecibidos, data, sizeof(Datos));
      Serial.printf("Recibido: valor1=%d, valor2=%d\n", datosRecibidos.valor1, datosRecibidos.valor2);
    }
  });
}

void loop() {
  int val1 = map(datosRecibidos.valor1, 0, 1850, 0, 180);
  int val2 = map(datosRecibidos.valor2, 0, 1850, 160, 10);

  servo1.write(val1);
  servo2.write(val2);

  delay (20);
}
