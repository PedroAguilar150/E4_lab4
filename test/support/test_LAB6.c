#include "unity.h"
#include "reloj.h"
#include <stdbool.h>
#include <stdint.h>

/* Test 1: La librería deberá mantener la hora actual, con precisión de segundos, a partir de la llamada
a una función que se produce una cierta cantidad de veces por segundo.*/

void test_reloj_avanza_un_segundo(void) {
    reloj_configurar_ticks_por_segundo(10);

    uint8_t hora_inicial[6] = {0, 0, 0, 0, 0, 0};
    reloj_ajustar_hora(hora_inicial);

    for (int i = 0; i < 10; i++) {
        reloj_tick();
    }

    // consulto hora actual
    uint8_t hora_actual[6];
    reloj_consultar_hora(hora_actual);

    uint8_t esperado[6] = {0, 0, 0, 0, 0, 1};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(esperado, hora_actual, 6);
}
/* Test 2: La librería deberá proporcionar una función para ajustar la hora actual.*/

void test_reloj_se_configura_correctamente_al_ajustar(void) {
    // Crear el reloj
    clock_t reloj = RelojCreate(1000, NULL);

    hora_t nueva_hora = {1, 2, 3, 4, 5, 6};
    hora_t hora_leida;

    // Ajustar la hora
    TEST_ASSERT_TRUE(RelojSetCurrentTime(reloj, nueva_hora));

    // Avanzar un tick
    ClockTick(reloj);

    // Leer la hora
    TEST_ASSERT_TRUE(RelojGetCurrentTime(reloj, hora_leida));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(nueva_hora, hora_leida, 6);
}
