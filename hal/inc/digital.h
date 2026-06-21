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

#ifndef DIGITAL_H_
#define DIGITAL_H_

/** @file digital.h
 ** @brief Declaracion de las funciones para gestion de entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include <stdbool.h>
/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define ACTIVATE_EVENT   1
#define DEACTIVATE_EVENT -1

/* === Public data type declarations =============================================================================== */

/**
 * @brief  Puntero opaco al discritor de una salida digital
 */
typedef struct digital_output_s * digital_output_t;

/**
 * @brief Puntero opaco al discritor de una entrada digital
 *
 */
typedef struct digital_input_s * digital_input_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea y configura una salida digital en el puerto y terminal indicados
 *
 * @param puerto
 * @param terminal
 * @return digital_output_t
 */

digital_output_t DigitalOutputCreate(uint32_t gpio, uint8_t bit);

/**
 * @brief Activa la salida digital
 * @param salida Puntero a la salida digital
 */
void DigitalOutputActivate(digital_output_t output);

/**
 * @brief Desactiva la salida digital
 * @param salida Puntero a la salida digital
 */
void DigitalOutputDeactivate(digital_output_t output);

/**
 * @brief Alterna el estado de la salida digital
 * @param salida Puntero a la salida digital
 */
void DigitalOutputToggle(digital_output_t output);

/**
 * @brief Crea y configura una entrada digital en el puerto y terminal indicados
 *
 * @param gpio
 * @param bit
 * @param inverted
 * @return digital_input_t
 */
digital_input_t DigitalInputCreate(uint32_t gpio, uint8_t bit, bool inverted);

/**
 * @brief Lee el estalo logico actual de la entrada digital
 *
 * @param input
 * @return true
 * @return false
 */
bool DigitalInputGetState(digital_input_t input);

/**
 * @brief Detecta si la entrada digital ha cambiado su estado logico desde la ultima lectura
 *
 * @param input
 * @return int
 */

int DigitalInputHasChanged(digital_input_t input);

/**
 * @brief Detecta si la entrada digital ha cambiado su estado logico desde la ultima lectura y se encuentra en estado de
 * activacion
 *
 * @param self
 * @return true
 * @return false
 */
bool DigitalInputHasActivated(digital_input_t self);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DIGITAL_H_ */