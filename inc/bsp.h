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

#ifndef BSP_H_
#define BSP_H_

/** @file bsp.h
 ** @brief Abstracción de hardware para la placa EDU-CIAA-NXP
 **/

/* === Headers files inclusions ==================================================================================== */

#include "digital.h"
#include "screen.h"
#include <stdint.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */
typedef struct board_s {
    digital_output_t buzzer;
    digital_input_t f1;
    digital_input_t f2;
    digital_input_t f3;
    digital_input_t f4;
    digital_input_t accept;
    digital_input_t cancel;
    display_t display;
} const * board_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */
/**
 * @brief Inicializa el hardware de la placa y crea el descriptor de recursos
 *
 * @return puntero al descriptor de la placa
 */
board_t BoardCreate();

/**
 * @brief Configura el temporizador Systick del nucleo
 *
 * @param ticks Frecuencia de interrupcion del Systick en Hz
 */
void SysTick_Init(uint16_t ticks);
/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* BSP_H_*/