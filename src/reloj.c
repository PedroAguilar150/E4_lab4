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
struct clock_s {
    hora_t hora;
    hora_t alarma;
    hora_t alarma_pospuesta;
    bool alarma_pospuesta_activa;
    bool valida;
    bool alarma_habilitada;
    unsigned int ticks_per_second;
    unsigned int tick_count;
    void (*callback)(clock_t);
};

/* === Private function declarations =========================================================== */
static bool HoraIgual(hora_t h1, hora_t h2);

/* === Private variable definitions ============================================================ */

/* === Public variable definition  ============================================================= */

/* === Private function definitions ============================================================ */
static bool HoraIgual(hora_t h1, hora_t h2) {
    for (int i = 0; i < 6; i++) {
        if (h1[i] != h2[i])
            return false;
    }
    return true;
}

/* === Public function implementation ========================================================== */

clock_t RelojCreate(unsigned int ticks_per_second, void (*callback)(clock_t)) {
    clock_t nuevo = malloc(sizeof(struct clock_s));
    if (!nuevo)
        return NULL;
    memset(nuevo->hora, 0, sizeof(hora_t));
    memset(nuevo->alarma, 0, sizeof(hora_t));
    memset(nuevo->alarma_pospuesta, 0, sizeof(hora_t));
    nuevo->alarma_pospuesta_activa = false;
    nuevo->valida = false;
    nuevo->alarma_habilitada = false;
    nuevo->ticks_per_second = ticks_per_second;
    nuevo->tick_count = 0;
    nuevo->callback = callback;
    return nuevo;
}

bool RelojSetCurrentTime(clock_t reloj, hora_t nueva_hora) {
    if (!reloj)
        return false;
    memcpy(reloj->hora, nueva_hora, sizeof(hora_t));
    reloj->valida = true;
    return true;
}

bool RelojGetCurrentTime(clock_t reloj, hora_t current_time) {
    if (!reloj)
        return false;
    memcpy(current_time, reloj->hora, sizeof(hora_t));
    return reloj->valida; // devuelve true si la hora fue marcada como válida
}

void ClockTick(clock_t reloj) {
    if (!reloj || !reloj->valida)
        return;

    reloj->tick_count++;

    if (reloj->tick_count >= reloj->ticks_per_second) {
        reloj->tick_count = 0;

        // Incrementar unidad de segundos en BCD
        reloj->hora[5]++;
        if (reloj->hora[5] >= 10) {
            reloj->hora[5] = 0;
            reloj->hora[4]++; // decena de segundos

            if (reloj->hora[4] >= 6) {
                reloj->hora[4] = 0;
                reloj->hora[3]++; // unidad de minutos

                if (reloj->hora[3] >= 10) {
                    reloj->hora[3] = 0;
                    reloj->hora[2]++; // decena de minutos

                    if (reloj->hora[2] >= 6) {
                        reloj->hora[2] = 0;
                        reloj->hora[1]++; // unidad de horas

                        if (reloj->hora[1] >= 10) {
                            reloj->hora[1] = 0;
                            reloj->hora[0]++; // decena de horas

                            if (reloj->hora[0] >= 2 && reloj->hora[1] >= 4) { // Corrección para 24 hs reales
                                reloj->hora[0] = 0;
                                reloj->hora[1] = 0;
                            }
                        }
                    }
                }
            }
        }

        if (reloj->alarma_habilitada && HoraIgual(reloj->hora, reloj->alarma)) {
            if (reloj->callback != NULL) {
                reloj->callback(reloj);
            }
        }

        if (reloj->alarma_pospuesta_activa && HoraIgual(reloj->hora, reloj->alarma_pospuesta)) {
            reloj->alarma_pospuesta_activa = false; // lo apagamos para el proximo dia
            if (reloj->callback != NULL) {
                reloj->callback(reloj);
            }
        }
    }
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

void AlarmEnable(clock_t reloj, bool estado) {
    if (reloj != NULL) {
        reloj->alarma_habilitada = estado;
    }
}
bool IsAlarmEnabled(clock_t reloj) {
    if (reloj != NULL) {
        return reloj->alarma_habilitada;
    }
    return false; // por seguridad, si el reloj es NULL, consideramos que la alarma no está habilitada
}

bool IsAlarmActive(clock_t reloj) {
    if (reloj == NULL)
        return false;
    return reloj->alarma_habilitada;
}

bool PostponeAlarm(clock_t reloj, unsigned int minutos) {
    if (reloj == NULL)
        return false;

    // tomamos como base la hora actual del reloj, no la hora de la alarma
    int horas = reloj->alarma[0] * 10 + reloj->alarma[1];
    int mins = reloj->alarma[2] * 10 + reloj->alarma[3];
    int segs = reloj->alarma[4] * 10 + reloj->alarma[5];

    int total = horas * 60 + mins + minutos;
    total %= (24 * 60);

    horas = total / 60;
    mins = total % 60;

    // guardamos como hora pospuesta en la variable temporal
    reloj->alarma_pospuesta[0] = horas / 10;
    reloj->alarma_pospuesta[1] = horas % 10;
    reloj->alarma_pospuesta[2] = mins / 10;
    reloj->alarma_pospuesta[3] = mins % 10;
    reloj->alarma_pospuesta[4] = segs / 10;
    reloj->alarma_pospuesta[5] = segs % 10;

    reloj->alarma_pospuesta_activa = true; // activamos el snooze

    return true;
}

bool GetAlarmTime(clock_t reloj, hora_t hora) {
    if (reloj == NULL)
        return false;
    memcpy(hora, reloj->alarma, sizeof(hora_t));
    return true;
}

/* === End of documentation ==================================================================== */