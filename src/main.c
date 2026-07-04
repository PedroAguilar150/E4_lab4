/********************************
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
*********************************/

/** \brief Programa principal para la placa EDU-CIAA
 **
 **
 ** @{ */

/* === Headers files inclusions =============================================================== */

#ifndef EDU_CIAA_NXP
#error "This program can only be compiled for the EDU-CIAA-NXP board"
#endif

#include "bsp.h"
#include "digital.h"
#include "screen.h"
#include <stdbool.h>

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

static board_t board;

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */
int main(void) {
    // botones f1 a f4 aumentar el numero del segmento correspondiente
    // boton accept activa y desactiva el parpadeo de los puntos
    // boton cancel activa y desactiva el led de abajo(el que usamos en vez de un buzzer)
    uint8_t entrada[4] = {1, 2, 3, 4};

    bool parpadeo_puntos = false;
    bool puntos_encendidos = false;
    bool led_encendido = false;

    uint32_t contador = 0;

    board = BoardCreate();

    DisplayWriteBCD(board->display, entrada, sizeof(entrada));

    while (true) {

        if (DigitalInputHasActivated(board->accept)) {
            parpadeo_puntos = !parpadeo_puntos;

            if (!parpadeo_puntos && puntos_encendidos) {
                DisplayToggleDots(board->display, 0, 3);
                puntos_encendidos = false;
            }
        }

        if (DigitalInputHasActivated(board->cancel)) {
            led_encendido = !led_encendido;

            if (led_encendido) {
                DigitalOutputActivate(board->buzzer);
            } else {
                DigitalOutputDeactivate(board->buzzer);
            }
        }

        if (DigitalInputHasActivated(board->f1)) {
            entrada[3] = (entrada[3] + 1) % 10;
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }

        if (DigitalInputHasActivated(board->f2)) {
            entrada[2] = (entrada[2] + 1) % 10;
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }

        if (DigitalInputHasActivated(board->f3)) {
            entrada[1] = (entrada[1] + 1) % 10;
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }

        if (DigitalInputHasActivated(board->f4)) {
            entrada[0] = (entrada[0] + 1) % 10;
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }

        contador++;
        if (parpadeo_puntos && contador >= 20) {
            contador = 0;

            DisplayToggleDots(board->display, 0, 3);
            puntos_encendidos = !puntos_encendidos;
        }

        for (int index = 0; index < 50; index++) {
            for (int delay = 0; delay < 1000; delay++) {
                __asm("NOP");
            }
            DisplayRefresh(board->display);
        }
    }

    return 0;
}
/* === End of documentation ==================================================================== */
/** @} End of module definition for doxygen */