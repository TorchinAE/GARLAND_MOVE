#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

const int LED_PIN = 0; 
const int PIR_PIN = 1;

volatile bool motionDetected = false;

// Обработчик прерывания от PIR
ISR(PCINT0_vect) {
    // Проверяем, что прерывание от PIR_PIN (PB1)
    if (digitalRead(PIR_PIN) == HIGH) {
        motionDetected = true;
    }
}

ISR(WDT_vect) {
    // Ничего не делаем — просто выходим из сна, если WDT сработал
    // Но в нашем случае WDT настроен на "interrupt", а не "reset"
}

void setup() {
    wdt_disable();
    pinMode(LED_PIN, OUTPUT);
    pinMode(PIR_PIN, INPUT);
    digitalWrite(LED_PIN, LOW);  // выключено по умолчанию
    // Настройка Pin Change Interrupt для PB1 (D1)
    GIMSK |= _BV(PCIE);   // Включить PCINT
    PCMSK |= _BV(PCINT1); // Прерывание по изменению на PB1 (бит 1)


    sei(); // Разрешить прерывания
}

void breatheOnce() {
    int time_up = random(5, 45);   // 5–15 мс на шаг вверх
    int time_down = random(2, 25);  // 2–8 мс на шаг вниз
    for (int i = 0; i <= 255; i++) {
        analogWrite(LED_PIN, i);
        delay(time_up);
    }
    for (int i = 255; i >= 0; i--) {
        analogWrite(LED_PIN, i);
        delay(time_down);
    }
}


void enterSleep() {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sei();
    sleep_cpu(); // Засыпаем
    sleep_disable();
}

void loop() {
    if (motionDetected) {
        motionDetected = false;
        cli();
        for (int cycle = 0; cycle < 14; cycle++) {
            breatheOnce();
            if (cycle < 13) {
                delay(500);
            }
        }
        analogWrite(LED_PIN, 0);
        sei();
    }
    enterSleep();
}