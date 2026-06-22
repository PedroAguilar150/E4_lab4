/************************************************************************************************
Copyright (c) 2026, Pedro Aguilar
Facultad de Ciencias Exactas y Tecnología, Universidad Nacional de Tucumán

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*************************************************************************************************/

/** @file reloj.c
 * @brief Implementación de la lógica interna para la gestión del tiempo y alarmas de un reloj en formato BCD
 **/

/* === Headers files inclusions ================================================================ */

#include "reloj.h"
#include <string.h>
#include <stdlib.h>
/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */
static uint8_t hora[6] = {0};
static int ticks_por_segundo = 1;
static int contador_ticks = 0;
static bool hora_es_valida = false;

/* === Private function declarations =========================================================== */

/* === Private variable definitions ============================================================ */

/* === Public variable definition  ============================================================= */

/* === Private function definitions ============================================================ */

/* === Public function implementation ========================================================== */
clock_t RelojCreate(unsigned int ticks_per_second, void (*callback)(clock_t)) {
    clock_t nuevo = malloc(sizeof(struct clock_s));
    if (!nuevo)
        return NULL;
    memset(nuevo->hora, 0, sizeof(hora_t));
    nuevo->valida = false;
    nuevo->ticks_per_second = ticks_per_second;
    nuevo->tick_count = 0;
    nuevo->callback = callback;
    return nuevo;
}

bool RelojSetCurrentTime(clock_t reloj, hora_t nueva_hora) {
    if (!reloj)
        return false;
    memcpy(reloj->hora, nueva_hora, sizeof(hora_t));
    reloj->valida = true; // <-- esta línea es clave
    return true;
}

bool RelojGetCurrentTime(clock_t reloj, hora_t current_time) {
    if (!reloj)
        return false;
    memcpy(current_time, reloj->hora, sizeof(hora_t));
    return reloj->valida; // devuelve true si la hora fue marcada como válida
}

void reloj_configurar_ticks_por_segundo(int ticks) {
    ticks_por_segundo = ticks;
}

void reloj_ajustar_hora(uint8_t nueva_hora[6]) {
    for (int i = 0; i < 6; i++) {
        hora[i] = nueva_hora[i];
    }
    hora_es_valida = true;
}

void reloj_tick(void) {
    if (!hora_es_valida)
        return;

    contador_ticks++;
    if (contador_ticks >= ticks_por_segundo) {
        contador_ticks = 0;
        // incrementar segundos en BCD
        hora[5]++; // unidad de segundos
        if (hora[5] >= 10) {
            hora[5] = 0;
            hora[4]++;          // decena de segundos
            if (hora[4] >= 6) { // 60 segundos
                hora[4] = 0;
                hora[3]++; // unidad de minutos
                if (hora[3] >= 10) {
                    hora[3] = 0;
                    hora[2]++;          // decena de minutos
                    if (hora[2] >= 6) { // 60 minutos
                        hora[2] = 0;
                        hora[1]++; // unidad de horas
                        if (hora[1] >= 10) {
                            hora[1] = 0;
                            hora[0]++;                          // decena de horas
                            if (hora[0] >= 3 && hora[1] >= 4) { // 24 horas
                                hora[0] = 0;
                                hora[1] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

void reloj_consultar_hora(uint8_t destino[6]) {
    for (int i = 0; i < 6; i++) {
        destino[i] = hora[i];
    }
}

bool reloj_hora_valida(void) {
    return hora_es_valida;
}
void ClockTick(clock_t reloj) {
    if (!reloj)
        return;

    reloj->tick_count++;

    if (reloj->tick_count >= reloj->ticks_per_second) {
        reloj->tick_count = 0;

        // Avanzar segundos
        reloj->hora[5]++;
        if (reloj->hora[5] >= 60) {
            reloj->hora[5] = 0;
            reloj->hora[4]++; // minutos
            if (reloj->hora[4] >= 60) {
                reloj->hora[4] = 0;
                reloj->hora[3]++; // horas
                if (reloj->hora[3] >= 24) {
                    reloj->hora[3] = 0;
                }
            }
        }

        // Si hay callback, llamarlo
        if (reloj->callback) {
            reloj->callback(reloj);
        }
    }
}

clock_t CreateClockWithTicksPerSecond(unsigned int ticks_per_second, void (*callback)(clock_t)) {
    return RelojCreate(ticks_per_second, callback);
}

bool SetCurrentTime(clock_t reloj, hora_t nueva_hora) {
    return RelojSetCurrentTime(reloj, nueva_hora);
}

bool GetCurrentTime(clock_t reloj, hora_t hora_actual) {
    return RelojGetCurrentTime(reloj, hora_actual);
}

clock_t ClockCreateWithTicks(unsigned int ticks_per_second, void (*callback)(clock_t)) {
    return RelojCreate(ticks_per_second, callback);
}

bool IsCurrentTimeValid(clock_t reloj) {
    hora_t dummy;
    return RelojGetCurrentTime(reloj, dummy);
}

bool ReadCurrentTime(clock_t reloj, hora_t hora_actual) {
    return RelojGetCurrentTime(reloj, hora_actual);
}

clock_t ClockCreate(unsigned int ticks_per_second, void (*callback)(clock_t)) {
    return RelojCreate(ticks_per_second, callback);
}

bool AlarmSetTime(clock_t reloj, hora_t nueva_alarma) {
    if (reloj == NULL)
        return false;
    memcpy(reloj->alarma, nueva_alarma, sizeof(hora_t));
    return true;
}

bool AlarmReadTime(clock_t reloj, hora_t alarma_actual) {
    if (reloj == NULL)
        return false;
    memcpy(alarma_actual, reloj->alarma, sizeof(hora_t));
    return true;
}

bool SetAlarmTime(clock_t reloj, hora_t nueva_alarma) {
    if (reloj == NULL)
        return false;
    memcpy(reloj->alarma, nueva_alarma, sizeof(hora_t));
    return true;
}

void AlarmEnable(clock_t reloj, bool estado) {
    if (reloj != NULL) {
        reloj->alarma_habilitada = estado;
    }
}

bool IsAlarmActive(clock_t reloj) {
    if (reloj == NULL)
        return false;
    return reloj->alarma_habilitada;
}

void SetAlarmEnabled(clock_t reloj, bool estado) {
    if (reloj != NULL) {
        reloj->alarma_habilitada = estado;
    }
}

bool IsAlarmEnabled(clock_t reloj) {
    if (reloj == NULL)
        return false;
    return reloj->alarma_habilitada;
}
bool AjustarHora(clock_t reloj, uint8_t * hora) {
    if (reloj == NULL)
        return false;
    for (int i = 0; i < 6; i++) {
        reloj->hora[i] = hora[i];
    }
    reloj->valida = true;
    return true;
}

bool ConfigurarAlarma(clock_t reloj, uint8_t * hora) {
    if (reloj == NULL)
        return false;
    for (int i = 0; i < 6; i++) {
        reloj->alarma[i] = hora[i];
    }
    return true;
}

void ActivarAlarma(clock_t reloj, bool estado) {
    if (reloj != NULL) {
        reloj->alarma_habilitada = estado;
    }
}

static bool HoraIgual(hora_t h1, hora_t h2) {
    for (int i = 0; i < 6; i++) {
        if (h1[i] != h2[i])
            return false;
    }
    return true;
}

void TickClock(clock_t reloj) {
    if (reloj == NULL)
        return;

    // Verificar alarma
    if (reloj->alarma_habilitada && HoraIgual(reloj->hora, reloj->alarma)) {
        if (reloj->callback != NULL) {
            reloj->callback(reloj);
        }
    }
}

/* === End of documentation ==================================================================== */