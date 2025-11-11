#ifndef TELEGRAMBOT_H
#define TELEGRAMBOT_H

#include <Arduino.h>

// Inicializa la comunicación con Telegram
void iniciarTelegram();

// Verifica mensajes nuevos y ejecuta las acciones correspondientes
void actualizarTelegram(float temp, float hum, float pot, bool led23, bool led2);

// Funciones de display (definidas en main.cpp, declaradas aquí para uso en TelegramBot.cpp)
void useSensor();
void usePot();
void useOLED();

#endif
