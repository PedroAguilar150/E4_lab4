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
clock_t RelojCreate(unsigned int ticks_per_second, void * alarm_handler) {
    return NULL;
}

bool RelojGetCurrentTime(clock_t clock, hora_t current_time) {
    memset(current_time, 0, sizeof(hora_t));
    return false;
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

/* === End of documentation ==================================================================== */