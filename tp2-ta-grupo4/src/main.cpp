#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "Device.h"
#include "TelegramBot.h" // 🔹 MODIFICADO: incluye el header del nuevo archivo

// ======= PINES =======
#define PIN_LED_VENTILADOR 23
#define PIN_LED_RIEGO 2
#define PIN_POTENCIOMETRO 32
#define PIN_SENSOR_DHT 33

// ======= OBJETO PRINCIPAL =======
Device invernadero(128, 64, -1, PIN_SENSOR_DHT, DHT22);

// ======= VARIABLES =======
bool mostrarSensorActual = 0;
bool mostrarPotActual = 0;
bool mostrarOLEDActual = 0;

// ======= PROTOTIPOS =======
void configurarEntradasSalidas();

// ======= SETUP =======
void setup() {
  Serial.begin(9600);
  configurarEntradasSalidas();

  // Inicializa WiFi y Telegram
  iniciarTelegram();

  invernadero.begin();
  Serial.println("🌱 Invernadero iniciado");
  invernadero.showDisplay("Bienvenido! \n\n ^_^ \n\nEsperando instrucciones");

}

// ======= LOOP =======
void loop() {
  // Lectura de sensores
  float temp = invernadero.readTemp();
  float hum = invernadero.readHum();
  float pot = analogRead(PIN_POTENCIOMETRO) * (3.3 / 4095.0);

  bool led23 = digitalRead(PIN_LED_VENTILADOR) == HIGH;
  bool led2 = digitalRead(PIN_LED_RIEGO) == HIGH;
  actualizarTelegram(temp, hum, pot, led23, led2);

  // Mostrar sensor
  if(mostrarSensorActual){
    invernadero.showDisplay("Temperatura: " + String(temp, 1) + " C\nHumedad: " + String(hum, 1) + " %");
  } else if(mostrarPotActual){
    invernadero.showDisplay("Potenciometro:\n" + String(pot, 2) + " V");
  } else if(mostrarOLEDActual){
    invernadero.showDisplay("LED Verde: " + String(led23 ? "Encendido" : "Apagado") + "\nLED Azul: " + String(led2 ? "Encendido" : "Apagado"));
  }

  delay(100);
}

// ======= FUNCIONES AUXILIARES =======
void configurarEntradasSalidas() {
  pinMode(PIN_LED_VENTILADOR, OUTPUT);
  pinMode(PIN_LED_RIEGO, OUTPUT);
  pinMode(PIN_POTENCIOMETRO, INPUT);
}

void useSensor(){
  mostrarSensorActual = 1;
  mostrarPotActual = 0;
  mostrarOLEDActual = 0;
}

void usePot(){
  mostrarSensorActual = 0;
  mostrarPotActual = 1;
  mostrarOLEDActual = 0;
}

void useOLED(){
  mostrarPotActual = 0;
  mostrarSensorActual = 0;
  mostrarOLEDActual = 1;
}