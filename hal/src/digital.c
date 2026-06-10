/*********************************************************************************************************************
Copyright 2016-2025, Laboratorio de Microprocesadores
Facultad de Ciencias Exactas y Tecnología
Universidad Nacional de Tucuman
http://www.microprocesadores.unt.edu.ar/

Copyright 2016-2025, Esteban Volentini <evolentini@herrera.unt.edu.ar>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*************************************************************************************************/

/** @file digital.c
 ** @brief Implementación de la biblioteca para gestión de entradas y salidas digitales
 **/

/* === Headers files inclusions ================================================================ */

// #include "digital.h"
#include "digital.h"
#include "chip.h"
#include <stdlib.h>


/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

struct digital_output_s{
    uint8_t gpio;
    uint8_t bit;
};

struct digital_input_s{
    uint8_t gpio;
    uint8_t bit;
    bool inverted;
    bool last_state;
};
/* === Private function declarations =========================================================== */

/* === Private variable definitions ============================================================ */

/* === Public variable definition  ============================================================= */

/* === Private function definitions ============================================================ */

/* === Public function implementation ========================================================== */

digital_output_t DigitalOutputCreate(uint32_t gpio, uint8_t bit){
    digital_output_t self;
    self = malloc(sizeof(struct digital_output_s));
    if (self){
        self->gpio = gpio;
        self->bit = bit;
        DigitalOutputDeactivate(self);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->gpio, self->bit, true);
    }
    return self;
}

void DigitalOutputActivate(digital_output_t self){
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->gpio , self->bit , true);
}

void DigitalOutputDeactivate(digital_output_t self){
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->gpio, self->bit, false);
}

void DigitalOutputToggle(digital_output_t self){
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, self->gpio, self->bit);
}

digital_input_t DigitalInputCreate(uint32_t gpio, uint8_t bit, bool inverted){
    digital_input_t self;
    self = malloc(sizeof(struct digital_input_s));
    if (self){
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, gpio, bit, false);
        self->gpio = gpio;
        self->bit = bit;
        self->inverted = inverted;
        self->last_state = DigitalInputGetState(self);
    }
    return self;
}

bool DigitalInputGetState(digital_input_t self){
    return (Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, self->gpio, self->bit) == 0) ^ self->inverted;
}   

int DigitalInputHasChanged(digital_input_t self){
    int resultado=0;
    bool actual = DigitalInputGetState(self);
    if (actual && !self->last_state){
        resultado = ACTIVATE_EVENT;
    } else if (!actual && self->last_state){
        resultado = DEACTIVATE_EVENT;
    }
    self->last_state = actual;
    return resultado;
}

bool DigitalInputHasActivated(digital_input_t self){
    return DigitalInputHasChanged(self) == ACTIVATE_EVENT;
}

bool DigitalInputHasDeactivated(digital_input_t self){
    return DigitalInputHasChanged(self) == DEACTIVATE_EVENT;
}   
/* === End of documentation ==================================================================== */