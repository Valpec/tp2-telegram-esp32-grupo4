#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

const char* SSID = "";
const char* PASS = "";
const char* BOT_TOKEN  = "";
const unsigned long tiempo = 1000; //tiempo medio entre mensajes de escaneo

const int PIN_RELAY = 26;

//Token de Telegram BOT se obtenienen desde Botfather en telegram
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long tiempoAnterior; //última vez que se realizó el análisis de mensajes

String chat_id;
const char * ID_Chat = "tu_id_chat";//ID_Chat se obtiene de telegram
void mensajesNuevos(int numerosMensajes)
{
  for (int i = 0; i < numerosMensajes; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    if (text == "start")
    {
      String welcome = "Bot de control de RELAY.\n";
      welcome += "start - Ver los comandos disponibles" "\n";
      welcome += "led23on - Prender led verde de la placa de desarrollo" "\n";
      welcome += "led23off - Apagar led verde de la placa de desarrollo" "\n";
      welcome += "led2on - Prender led azul de la placa de desarrollo" "\n";
      welcome += "led2off - Apagar led azul de la placa de desarrollo" "\n";
      welcome += "dth22 - Informe de valores de humedad y temperatura del sensor" "\n";
      welcome += "pote - Informe de valor de voltaje según lectura del potenciometro" "\n";
      welcome += "platiot - Enviar valores de humedad y temperatura a ThingSpeak" "\n";
      welcome += "displayled - Estado actual del led" "\n";
      welcome += "displaypot - Estado actual del potenciometro" "\n";
      welcome += "displaysensor - Estado actual del sensor";
      bot.sendMessage(chat_id, welcome, "");
    }
 
    if (text == "led23on")
    {
      
      bot.sendMessage(chat_id, "RELAY apagado!", "");
    }

    if (text == "led23off")
    {
      digitalWrite(PIN_RELAY, LOW); // 
      bot.sendMessage(chat_id, "RELAY apagado!", "");
    }

    if (text == "led2on")
    {
      digitalWrite(PIN_RELAY, LOW); // 
      bot.sendMessage(chat_id, "RELAY apagado!", "");
    }

    if (text == "led2off")
    {
      digitalWrite(PIN_RELAY, LOW); // 
      bot.sendMessage(chat_id, "RELAY apagado!", "");
    }

    if (text == "dth22")
    {
      digitalWrite(PIN_RELAY, LOW); // 
      bot.sendMessage(chat_id, "RELAY apagado!", "");
    }
    if (text == "pote")
    {
      digitalWrite(PIN_RELAY, LOW); // 
      bot.sendMessage(chat_id, "RELAY apagado!", "");
    }
    if (text == "platiot")
    {
      digitalWrite(PIN_RELAY, LOW); // 
      bot.sendMessage(chat_id, "RELAY apagado!", "");
    }
    if (text == "displayled")
    {
      digitalWrite(PIN_RELAY, LOW); // 
      bot.sendMessage(chat_id, "RELAY apagado!", "");
    }
    if (text == "displaypot")
    {
      digitalWrite(PIN_RELAY, LOW); // 
      bot.sendMessage(chat_id, "RELAY apagado!", "");
    }
    if (text == "displaysensor")
    {
      digitalWrite(PIN_RELAY, LOW); // 
      bot.sendMessage(chat_id, "RELAY apagado!", "");
    }

  }
}
 
void setup()
{
  Serial.begin(9600);
  pinMode(PIN_RELAY, OUTPUT); //inicializar  pin 12 digital como salida.
  // Intenta conectarse a la red wifi
  Serial.print("Conectando a la red ");
  WiFi.begin(SSID, PASS);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); //Agregar certificado raíz para api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nConectado a la red wifi. Dirección IP: ");
  Serial.println(WiFi.localIP());
}
 
void loop()
{
  //Verifica si hay datos nuevos en telegram cada 1 segundo
  if (millis() - tiempoAnterior > tiempo)
  {
    int numerosMensajes = bot.getUpdates(bot.last_message_received + 1);
 
    while (numerosMensajes)
    {
      Serial.println("Comando recibido");
      mensajesNuevos(numerosMensajes);
      numerosMensajes = bot.getUpdates(bot.last_message_received + 1);
    }
 
    tiempoAnterior = millis();
  }
  
}