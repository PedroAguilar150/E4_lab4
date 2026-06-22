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

/* Test 3: La librería deberá permitir la configuración, una vez al inicio, de la cantidad de veces por
segundo que se llamará a la función para mantener la hora actualizada.*/
void test_configuracion_ticks_por_segundo(void) {
    clock_t reloj;
    hora_t hora_actual;

    uint8_t hora_inicial[6] = {0, 0, 0, 0, 0, 0};  // 00:00:00
    uint8_t hora_esperada[6] = {0, 0, 0, 0, 0, 1}; // 00:00:01

    reloj = CreateClockWithTicksPerSecond(5, NULL);

    bool set_ok = SetCurrentTime(reloj, hora_inicial);
    TEST_ASSERT_TRUE(set_ok);

    // Simulamos 5 ticks
    for (int i = 0; i < 5; i++) {
        ClockTick(reloj);
    }

    bool get_ok = GetCurrentTime(reloj, hora_actual);
    TEST_ASSERT_TRUE(get_ok);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(hora_esperada, hora_actual, 6);
}

/* Test 4: La librería deberá informar que la hora actual es inválida hasta que se llama a la función para
ajustar la hora por primera vez.*/
void test_reloj_inicia_con_hora_invalida(void) {
    static const hora_t INITIAL_TIME = {0, 0, 0, 0, 0, 0};
    clock_t mi_reloj;
    hora_t hora_leida = {9, 9, 9, 9, 9, 9};
    bool hora_valida;

    mi_reloj = ClockCreateWithTicks(1, NULL);

    hora_valida = IsCurrentTimeValid(mi_reloj);

    TEST_ASSERT_FALSE(hora_valida);

    ReadCurrentTime(mi_reloj, hora_leida);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(INITIAL_TIME, hora_leida, 6);
}

/* Test 5: La librería deberá proporcionar una función para fijar la hora de la alarma.*/
void test_establecer_y_leer_alarma(void) {
    clock_t mi_reloj;
    hora_t alarma_leida;

    uint8_t alarma_inicial[6] = {0, 7, 3, 0, 0, 0}; // 07:30:00
    uint8_t alarma_esperada[6] = {0, 7, 3, 0, 0, 0};

    mi_reloj = ClockCreate(1, NULL);

    bool set_ok = AlarmSetTime(mi_reloj, alarma_inicial);
    TEST_ASSERT_TRUE(set_ok);

    bool get_ok = AlarmReadTime(mi_reloj, alarma_leida);
    TEST_ASSERT_TRUE(get_ok);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(alarma_esperada, alarma_leida, 6);
}

/* Test 6: La librería deberá proporcionar una función para consultar la hora fijada para la alarma.*/
void test_consultar_hora_alarma(void) {
    clock_t reloj;
    hora_t alarma_leida;

    // Hora de alarma previamente fijada
    uint8_t alarma_inicial[6] = {0, 6, 3, 0, 0, 0}; // 06:30:00
    uint8_t alarma_esperada[6] = {0, 6, 3, 0, 0, 0};

    reloj = RelojCreate(1, NULL);

    bool set_ok = SetAlarmTime(reloj, alarma_inicial);
    TEST_ASSERT_TRUE(set_ok);

    bool get_ok = AlarmReadTime(reloj, alarma_leida);
    TEST_ASSERT_TRUE(get_ok);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(alarma_esperada, alarma_leida, 6);
}

/* Test 7: La librería deberá proporcionar una función para habilitar y deshabilitar la alarma.*/
void test_activar_y_desactivar_alarma(void) {
    clock_t reloj;

    reloj = ClockCreate(1, NULL);

    // Por defecto, la alarma debería iniciar desactivada
    TEST_ASSERT_FALSE(IsAlarmActive(reloj));

    // La activamos y comprobamos
    AlarmEnable(reloj, true);
    TEST_ASSERT_TRUE(IsAlarmActive(reloj));

    // La desactivamos y comprobamos
    AlarmEnable(reloj, false);
    TEST_ASSERT_FALSE(IsAlarmActive(reloj));
}
