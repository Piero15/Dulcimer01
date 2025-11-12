#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>

// NeoPixel
#define NEOPIXEL_PIN 15
#define NUMPIXELS    1
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// MPU6050
Adafruit_MPU6050 mpu;

// Botón
#define BUTTON_PIN      2

// Joystick
#define JOY_BUTTON_PIN  16
#define JOY_X_PIN       36
#define JOY_Y_PIN       39

// MAC del esclavo ---- ESPCAM
uint8_t receptorAddress[] = {0x08, 0xD1, 0xF9, 0x99, 0x34, 0xA8}; //08:D1:F9:99:34:A8 // esp test 0xC0, 0x49, 0xEF, 0xE4, 0xAD, 0x78

String menu[] = {
  "wifi",
  "Pruevas Gimbal",
  "pageWeb",
};

struct Datos {
  int valor1;
  int valor2;
};

Datos datos;

int item = 0;
bool Menu = true;

void setup() {
  delay(1000);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error al iniciar pantalla OLED");
    while (true);
  }
  display.clearDisplay();

  // MPU6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 no encontrado");
    while (true);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  // Botón y joystick
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(JOY_BUTTON_PIN, INPUT_PULLUP);  // el joystick está normalmente alto

  // ESP NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, receptorAddress, 6);
  esp_now_add_peer(&peer);

  //Config I2C OLED
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  
  // NeoPixel
  pixels.begin();
  pixels.setBrightness(50);
  // pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  // pixels.show();

  delay(1000);
  
}

void loop() {

  display.clearDisplay();

  String direcction = joystick();

  if(digitalRead(JOY_BUTTON_PIN)== LOW){
    Menu = !Menu;
    while (digitalRead(JOY_BUTTON_PIN)== LOW) {
    }
  }

  if(Menu == true){
    int menuSize = sizeof(menu) / sizeof(menu[0]);

    if(direcction == "derecha"){
      item++;
      if (item >= menuSize) item = 0;
    };
    if(direcction == "izquierda"){
      item--;
      if (item < 0) item = menuSize - 1;
    };
    TextOled(0,0,2,String(menu[item]));
    TextOled(53, 56, 1, String(item + 1) + "/" + String(menuSize)); 
    display.display();
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
    delay(250);
  };
  

  //menu
  if(item == 0 && Menu == false){
    TextOled(0,0,2, "Este es el wifi");
    display.display();
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.show();
  };
  //////////////////////////////////////////////////////////////////////////////////////

  if (item == 1 && Menu == false) {

    datos.valor1 = analogRead(JOY_Y_PIN);
    datos.valor2 = analogRead(JOY_X_PIN);

    TextOled(0,0,2, "y:"+ String(datos.valor1));
    TextOled(0,17,2, "x:"+ String(datos.valor2));


    esp_now_send(receptorAddress, (uint8_t*)&datos, sizeof(datos));
    delay(10); // cada medio segundo
    display.display();
  };
  /////////////////////////////////////////////////////////////////////////////////////////

  if(item == 2 && Menu == false){
    TextOled(0,0,2, "Este es el pageWeb");
    display.display();
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.show();
  };
}

String joystick() {
  int joyY = analogRead(JOY_Y_PIN);
  int joyX = analogRead(JOY_X_PIN);

  if (joyY > 1700) return "izquierda";
  if (joyY < 250)  return "derecha";
  if (joyX > 1750) return "arriba";
  if (joyX < 250)  return "abajo";

  //return "nA";  // Retorna cadena vacía si está en el centro
}


void TextOled(int x, int y, int size, String text){
  // display.clearDisplay();
  display.setTextColor(WHITE); 
  display.setCursor(x, y);
  display.setTextSize(size);
  display.print(text);
  //display.display(); 
}



























