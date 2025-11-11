#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "TelegramBot.h"
#include "../lib/DEVICE/Device.h"

// Declaraciones externas de funciones y variables definidas en main.cpp
extern Device invernadero;
extern void useSensor();
extern void usePot();
extern void useOLED();

// ---------------- CONFIGURACIÓN ----------------
// aca van las contraseñas :: no pusheadas !


const unsigned long INTERVALO_TELEGRAM = 1000;

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long ultimaConsulta;

// Pines manejados por Telegram
#define PIN_LED_VERDE 23
#define PIN_LED_AZUL 2
#define PIN_POTENCIOMETRO 32
#define PIN_DHT22 33

// ---------------- INICIALIZACIÓN ----------------
void iniciarTelegram() {
  Serial.println("Conectando a WiFi...");
  WiFi.begin(SSID, PASS);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi conectado: " + WiFi.localIP().toString());
}

// pequeño wrapper para mantener compatibilidad si otros ficheros usan iniciarBot
void iniciarBot() {
  // actualmente la inicialización principal se realiza en iniciarTelegram
  ultimaConsulta = 0;
}

// ---------------- PROCESAMIENTO DE MENSAJES ----------------
void manejarComando(String chat_id, String text, float temp, float hum, float pot, bool led23, bool led2) {
  String rawText = text;
  text.toLowerCase();
  Serial.println(String("[Telegram] Mensaje recibido de ") + chat_id + ": '" + rawText + "' -> procesando '" + text + "'");

  if (text == "/start") {
    String msg = "👋 Bienvenido al *Bot del Invernadero*\n\n";
    msg += "Selecciona una opción:\n";
    
    // Crear teclado con botones
    String keyboardJson = R"(
    {
      "keyboard": [
        [{"text": "💡 LED Verde ON"}, {"text": "💡 LED Verde OFF"}],
        [{"text": "🔵 LED Azul ON"}, {"text": "🔵 LED Azul OFF"}],
        [{"text": "🌡 Sensor DHT22"}, {"text": "⚡ Potenciómetro"}],
        [{"text": "📊 Display LED"}, {"text": "📊 Display Pot"}],
        [{"text": "📊 Display Sensor"}, {"text": "🌍 Enviar a IoT"}]
      ],
      "one_time_keyboard": true,
      "resize_keyboard": true
    }
    )";
    
    bot.sendMessageWithReplyKeyboard(chat_id, msg, "Markdown", keyboardJson, true);
  }

  else if (text == "💡 led verde on" || text == "/led23on") {
    digitalWrite(PIN_LED_VERDE, HIGH);
    bot.sendMessage(chat_id, "💡 LED verde encendido");
  }

  else if (text == "💡 led verde off" || text == "/led23off") {
    digitalWrite(PIN_LED_VERDE, LOW);
    bot.sendMessage(chat_id, "💡 LED verde apagado");
  }

  else if (text == "🔵 led azul on" || text == "/led2on") {
    digitalWrite(PIN_LED_AZUL, HIGH);
    bot.sendMessage(chat_id, "🔵 LED azul encendido");
  }

  else if (text == "🔵 led azul off" || text == "/led2off") {
    digitalWrite(PIN_LED_AZUL, LOW);
    bot.sendMessage(chat_id, "🔵 LED azul apagado");
  }

  else if (text == "🌡 sensor dht22" || text == "/dht22") {
    bot.sendMessage(chat_id, "🌡 Temp: " + String(temp, 1) + "°C\n💧 Humedad: " + String(hum, 1) + "%");
  }

  else if (text == "⚡ potenciómetro" || text == "/pote") {
    bot.sendMessage(chat_id, "⚡ Voltaje: " + String(pot, 2) + " V");
  }

  else if (text == "🌍 enviar a iot" || text == "/platiot") {
    // Intento de enviar a ThingSpeak (requiere configurar THINGSPEAK_WRITE_API_KEY)
#ifndef THINGSPEAK_WRITE_API_KEY
    bot.sendMessage(chat_id, "⚠️ ThingSpeak no configurado. Defina THINGSPEAK_WRITE_API_KEY en el código.");
#else
    { // construir petición simple GET a ThingSpeak
      WiFiClient client;
      const char* host = "api.thingspeak.com";
      String url = "/update?api_key=" THINGSPEAK_WRITE_API_KEY;
      url += "&field1=" + String(temp, 2);
      url += "&field2=" + String(hum, 2);
      if (client.connect(host, 80)) {
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Connection: close\r\n\r\n");
        // leer respuesta mínima
        unsigned long timeout = millis();
        while (client.available() == 0) {
          if (millis() - timeout > 5000) break;
        }
        String line;
        while (client.available()) {
          line = client.readStringUntil('\n');
          // no hacemos nada con la respuesta ahora
        }
        client.stop();
        bot.sendMessage(chat_id, "🌍 Datos enviados a ThingSpeak correctamente");
      } else {
        bot.sendMessage(chat_id, "❌ Error conectando a ThingSpeak");
      }
    }
#endif
  }

  else if (text == "📊 display led" || text == "/displayled") {
    bot.sendMessage(chat_id,
      "💡 LED Verde: " + String(led23 ? "Encendido" : "Apagado") +
      "\n🔵 LED Azul: " + String(led2 ? "Encendido" : "Apagado"));
    // Mostrar en OLED
    useOLED();
  }
  else if (text == "📊 display pot" || text == "/displaypot") {
    bot.sendMessage(chat_id, "⚡ Potenciómetro: " + String(pot, 2) + " V");
    usePot();
  }
  else if (text == "📊 display sensor" || text == "/displaysensor") {
    bot.sendMessage(chat_id, "🌡 " + String(temp, 1) + "°C | 💧 " + String(hum, 1) + "%");
    useSensor();
  }
  else {
    // Si es un /display<algo> no reconocido, mostrar en pantalla
    if (text.startsWith("/display")) {
      invernadero.showDisplay("Comando /display no identificado");
      bot.sendMessage(chat_id, "❌ Comando /display no identificado. Ver en pantalla.");
    } else {
      bot.sendMessage(chat_id, "❌ Comando no reconocido");
    }
  }
}

// ---------------- ACTUALIZAR BOT ----------------
void actualizarTelegram(float temp, float hum, float pot, bool led23, bool led2) {
  if (millis() - ultimaConsulta > INTERVALO_TELEGRAM) {
    int numMensajes = bot.getUpdates(bot.last_message_received + 1);
    while (numMensajes) {
      for (int i = 0; i < numMensajes; i++) {
        String chat_id = bot.messages[i].chat_id;
        String text = bot.messages[i].text;
        manejarComando(chat_id, text, temp, hum, pot, led23, led2);
      }
      numMensajes = bot.getUpdates(bot.last_message_received + 1);
    }
    ultimaConsulta = millis();
  }
}
