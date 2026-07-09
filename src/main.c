#include "bsp.h"
#include "digital.h"
#include "screen.h"
#include "reloj.h"
#include <stdbool.h>
#include <string.h>

#define TICKS_POR_SEGUNDO 500
#define HOLD_TIME         (3 * TICKS_POR_SEGUNDO)

typedef enum {
    ESTADO_CONFIG_INICIAL,
    ESTADO_NORMAL,
    ESTADO_SET_MIN,     // setear minutos
    ESTADO_SET_HR,      // setear horas
    ESTADO_SET_ALM_MIN, // setear minutos de alarma
    ESTADO_SET_ALM_HR   // setear horas de alarma
} estado_t;

static board_t mi_placa;
static clock_t reloj;
static estado_t estado = ESTADO_CONFIG_INICIAL;

static uint32_t tick_global = 0;
static uint32_t tick_f1 = 0;
static uint32_t tick_f2 = 0;

static int horas_aux = 0;
static int minutos_aux = 0;
static int minuto_prev = -1;

static bool punto_central = false;
static bool alarma_activa = false;
static bool snooze_bloqueado = false;

static void manejar_estado(bool ok, bool cancel);
static void refrescar_display(void);
static void cargar_aux(hora_t origen);
static void guardar_aux(hora_t destino);

int main(void) {
    mi_placa = BoardCreate();
    reloj = RelojCreate(TICKS_POR_SEGUNDO, NULL);

    DisplayFlashDigits(mi_placa->display, 0, 3, TICKS_POR_SEGUNDO / 2);

    while (1) {
        ClockTick(reloj);
        tick_global++;

        if (tick_global >= TICKS_POR_SEGUNDO) {
            tick_global = 0;
            punto_central = (estado == ESTADO_NORMAL) ? !punto_central : false;
        }

        bool ok = DigitalInputHasActivated(mi_placa->accept);
        bool cancel = DigitalInputHasActivated(mi_placa->cancel);

        manejar_estado(ok, cancel);
        refrescar_display();

        DisplayRefresh(mi_placa->display);

        for (volatile int i = 0; i < 34000; i++) {
            __asm("NOP");
        }
    }
    return 0;
}

static void manejar_estado(bool ok, bool cancel) {
    hora_t tiempo;
    RelojGetCurrentTime(reloj, tiempo);

    if (minuto_prev != tiempo[3]) {
        minuto_prev = tiempo[3];
        snooze_bloqueado = false;
    }

    switch (estado) {
    case ESTADO_CONFIG_INICIAL:
        if (!DigitalInputGetState(mi_placa->f1)) {
            if (++tick_f1 >= HOLD_TIME) {
                tick_f1 = 0;
                cargar_aux(tiempo);
                estado = ESTADO_SET_MIN;
                DisplayFlashDigits(mi_placa->display, 2, 3, TICKS_POR_SEGUNDO / 4);
            }
        } else
            tick_f1 = 0;
        break;

    case ESTADO_NORMAL:
        if (!DigitalInputGetState(mi_placa->f1)) {
            if (++tick_f1 >= HOLD_TIME) {
                tick_f1 = 0;
                cargar_aux(tiempo);
                estado = ESTADO_SET_MIN;
                DisplayFlashDigits(mi_placa->display, 2, 3, TICKS_POR_SEGUNDO / 4);
            }
        } else
            tick_f1 = 0;

        if (!DigitalInputGetState(mi_placa->f2)) {
            if (++tick_f2 >= HOLD_TIME) {
                tick_f2 = 0;
                GetAlarmTime(reloj, tiempo);
                cargar_aux(tiempo);
                estado = ESTADO_SET_ALM_MIN;
                DisplayFlashDigits(mi_placa->display, 2, 3, TICKS_POR_SEGUNDO / 4);
            }
        } else
            tick_f2 = 0;
        break;

    case ESTADO_SET_MIN:
        if (DigitalInputHasActivated(mi_placa->f4))
            minutos_aux = (minutos_aux + 1) % 60;
        if (DigitalInputHasActivated(mi_placa->f3))
            minutos_aux = (minutos_aux + 59) % 60;
        if (ok) {
            estado = ESTADO_SET_HR;
            DisplayFlashDigits(mi_placa->display, 0, 1, TICKS_POR_SEGUNDO / 4);
        }
        break;

    case ESTADO_SET_HR:
        if (DigitalInputHasActivated(mi_placa->f4))
            horas_aux = (horas_aux + 1) % 24;
        if (DigitalInputHasActivated(mi_placa->f3))
            horas_aux = (horas_aux + 23) % 24;
        if (ok) {
            guardar_aux(tiempo);
            RelojSetCurrentTime(reloj, tiempo);
            DisplayFlashDigits(mi_placa->display, 0, 0, 0);
            estado = ESTADO_NORMAL;
        }
        break;

    case ESTADO_SET_ALM_MIN:
        if (DigitalInputHasActivated(mi_placa->f4))
            minutos_aux = (minutos_aux + 1) % 60;
        if (DigitalInputHasActivated(mi_placa->f3))
            minutos_aux = (minutos_aux + 59) % 60;
        if (ok) {
            estado = ESTADO_SET_ALM_HR;
            DisplayFlashDigits(mi_placa->display, 0, 1, TICKS_POR_SEGUNDO / 4);
        }
        break;

    case ESTADO_SET_ALM_HR:
        if (DigitalInputHasActivated(mi_placa->f4))
            horas_aux = (horas_aux + 1) % 24;
        if (DigitalInputHasActivated(mi_placa->f3))
            horas_aux = (horas_aux + 23) % 24;
        if (ok) {
            guardar_aux(tiempo);
            AlarmSetTime(reloj, tiempo);
            AlarmEnable(reloj, true);
            DisplayFlashDigits(mi_placa->display, 0, 0, 0);
            estado = ESTADO_NORMAL;
        }
        break;
    }

    hora_t alarma;
    GetAlarmTime(reloj, alarma);

    if (IsAlarmEnabled(reloj) && !snooze_bloqueado && memcmp(tiempo, alarma, 4) == 0) {
        alarma_activa = true;
    }

    if (alarma_activa) {
        if (ok) {
            snooze_bloqueado = true;
            alarma_activa = false;
            RelojGetCurrentTime(reloj, alarma);
            cargar_aux(alarma);
            minutos_aux += 5;
            if (minutos_aux >= 60) {
                minutos_aux -= 60;
                horas_aux = (horas_aux + 1) % 24;
            }
            guardar_aux(alarma);
            AlarmSetTime(reloj, alarma);
        }
        if (cancel) {
            snooze_bloqueado = true;
            alarma_activa = false;
        }
    }
}

static void refrescar_display(void) {
    uint8_t buf[4];
    hora_t tiempo;

    if (alarma_activa)
        DigitalOutputActivate(mi_placa->buzzer);
    else
        DigitalOutputDeactivate(mi_placa->buzzer);

    if (estado == ESTADO_CONFIG_INICIAL) {
        RelojGetCurrentTime(reloj, tiempo);
        memcpy(buf, tiempo, 4);
        DisplayWriteBCD(mi_placa->display, buf, 4);
    } else if (estado == ESTADO_NORMAL) {
        RelojGetCurrentTime(reloj, tiempo);
        memcpy(buf, tiempo, 4);
        DisplayWriteBCD(mi_placa->display, buf, 4);
        if (punto_central)
            DisplayToggleDots(mi_placa->display, 1, 1);
        if (IsAlarmEnabled(reloj))
            DisplayToggleDots(mi_placa->display, 0, 0);
    } else {
        buf[0] = horas_aux / 10;
        buf[1] = horas_aux % 10;
        buf[2] = minutos_aux / 10;
        buf[3] = minutos_aux % 10;
        DisplayWriteBCD(mi_placa->display, buf, 4);
        if (estado == ESTADO_SET_ALM_MIN || estado == ESTADO_SET_ALM_HR) {
            DisplayToggleDots(mi_placa->display, 0, 3);
        }
    }
}

static void cargar_aux(hora_t origen) {
    horas_aux = origen[0] * 10 + origen[1];
    minutos_aux = origen[2] * 10 + origen[3];
}

static void guardar_aux(hora_t destino) {
    destino[0] = horas_aux / 10;
    destino[1] = horas_aux % 10;
    destino[2] = minutos_aux / 10;
    destino[3] = minutos_aux % 10;
    destino[4] = 0;
    destino[5] = 0;
}
