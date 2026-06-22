/*********************************************************************************************************************
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

#ifndef RELOJ_H_
#define RELOJ_H_

/** @file reloj.h
 ** @brief definiciones de tipos para la gestión del tiempo y alarmas de un reloj en formato BCD
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdbool.h>
#include <stdint.h>
#include "bsp.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */
typedef uint8_t hora_t[6];

struct clock_s {
    hora_t hora;
    bool valida;
    unsigned int ticks_per_second;
    unsigned int tick_count;
    void (*callback)(struct clock_s *);
    hora_t alarma;
};

typedef struct clock_s * clock_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */
clock_t RelojCreate(unsigned int ticks_per_second, void (*callback)(clock_t));

bool RelojSetCurrentTime(clock_t reloj, hora_t nueva_hora);
bool RelojGetCurrentTime(clock_t reloj, hora_t current_time);

void reloj_configurar_ticks_por_segundo(int ticks);

void reloj_ajustar_hora(uint8_t nueva_hora[6]);

void reloj_tick(void);
void ClockTick(clock_t reloj);

void reloj_consultar_hora(uint8_t destino[6]);

bool reloj_hora_valida(void);

clock_t CreateClockWithTicksPerSecond(unsigned int ticks_per_second, void (*callback)(clock_t));
bool SetCurrentTime(clock_t reloj, hora_t nueva_hora);
bool GetCurrentTime(clock_t reloj, hora_t hora_actual);

clock_t ClockCreateWithTicks(unsigned int ticks_per_second, void (*callback)(clock_t));
bool IsCurrentTimeValid(clock_t reloj);
bool ReadCurrentTime(clock_t reloj, hora_t hora_actual);

clock_t ClockCreate(unsigned int ticks_per_second, void (*callback)(clock_t));
bool AlarmSetTime(clock_t reloj, hora_t nueva_alarma);
bool AlarmReadTime(clock_t reloj, hora_t alarma_actual);

bool SetAlarmTime(clock_t reloj, hora_t nueva_alarma);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}

#endif
#endif /* RELOJ_H_ */