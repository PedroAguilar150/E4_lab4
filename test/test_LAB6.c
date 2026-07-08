#include "unity.h"
#include "reloj.h"
#include <stdbool.h>
#include <stdint.h>

static int alarma_sonada_veces = 0;
static int evento_disparado = 0;

void MiCallbackAlarma(clock_t reloj) {
    (void)reloj;
    alarma_sonada_veces++;
}
void CallbackEvento(clock_t r) {
    (void)r;
    evento_disparado++;
}
/* Test 1: La librería deberá mantener la hora actual, con precisión de segundos, a partir de la llamada
a una función que se produce una cierta cantidad de veces por segundo.*/
void test_reloj_avanza_un_segundo(void) {
    clock_t reloj = RelojCreate(10, NULL);

    hora_t hora_inicial = {0, 0, 0, 0, 0, 0};
    hora_t hora_actual;
    hora_t esperado = {0, 0, 0, 0, 0, 1};

    TEST_ASSERT_TRUE(RelojSetCurrentTime(reloj, hora_inicial));

    for (int i = 0; i < 10; i++) {
        ClockTick(reloj);
    }

    TEST_ASSERT_TRUE(RelojGetCurrentTime(reloj, hora_actual));

    TEST_ASSERT_EQUAL_UINT8_ARRAY(esperado, hora_actual, 6);
}

/* Test 2: La librería deberá proporcionar una función para ajustar la hora actual.*/
void test_reloj_se_configura_correctamente_al_ajustar(void) {
    clock_t reloj = RelojCreate(1000, NULL);

    hora_t nueva_hora = {1, 2, 3, 4, 5, 6};
    hora_t hora_leida;

    TEST_ASSERT_TRUE(RelojSetCurrentTime(reloj, nueva_hora));

    TEST_ASSERT_TRUE(RelojGetCurrentTime(reloj, hora_leida));

    TEST_ASSERT_EQUAL_UINT8_ARRAY(nueva_hora, hora_leida, 6);
}

/* Test 3: La librería deberá permitir la configuración, una vez al inicio, de la cantidad de veces por
segundo que se llamará a la función para mantener la hora actualizada.*/
void test_configuracion_ticks_por_segundo(void) {
    clock_t reloj;
    hora_t hora_actual;

    hora_t hora_inicial = {0, 0, 0, 0, 0, 0};
    hora_t hora_esperada = {0, 0, 0, 0, 0, 1};

    reloj = RelojCreate(5, NULL);

    TEST_ASSERT_TRUE(RelojSetCurrentTime(reloj, hora_inicial));

    // simulo 5 ticks
    for (int i = 0; i < 5; i++) {
        ClockTick(reloj);
    }

    TEST_ASSERT_TRUE(RelojGetCurrentTime(reloj, hora_actual));

    TEST_ASSERT_EQUAL_UINT8_ARRAY(hora_esperada, hora_actual, 6);
}

/* Test 4: La librería deberá informar que la hora actual es inválida hasta que se llama a la función para
ajustar la hora por primera vez.*/
void test_reloj_inicia_con_hora_invalida(void) {
    clock_t reloj;
    hora_t hora_leida;

    reloj = RelojCreate(1, NULL);

    TEST_ASSERT_FALSE(RelojGetCurrentTime(reloj, hora_leida));
}

/* Test 5 y 6 unificados: La librería deberá proporcionar una función para fijar la hora de la alarma.
La librería deberá proporcionar una función para consultar la hora fijada para la alarma.*/
void test_establecer_y_consultar_alarma(void) {
    clock_t reloj = RelojCreate(1, NULL);
    hora_t hora_alarma = {1, 2, 3, 4, 0, 0}; // 12:34:00
    hora_t alarma_leida;

    // probamos que la API permita fijar la hora de la alarma
    TEST_ASSERT_TRUE(AlarmSetTime(reloj, hora_alarma));

    // probamos que la API permita leer la hora de la alarma
    TEST_ASSERT_TRUE(AlarmReadTime(reloj, alarma_leida));

    // comprobamos que la hora leida es la misma que la fijada
    TEST_ASSERT_EQUAL_UINT8_ARRAY(hora_alarma, alarma_leida, sizeof(hora_t));
}

/* Test 7 y 8 unificados: La librería deberá proporcionar una función para habilitar y deshabilitar la alarma.
La librería deberá proporcionar una función para consultar si la alarma está, o no, habilitada.*/

void test_activar_desactivar_y_consultar_estado_alarma(void) {
    clock_t reloj = RelojCreate(1, NULL);

    // la alarma inicia desactivada, por lo que la consulta debe devolver false
    TEST_ASSERT_FALSE(IsAlarmActive(reloj));

    // activamos la alarma y consultamos que el estado sea true
    AlarmEnable(reloj, true);
    TEST_ASSERT_TRUE(IsAlarmActive(reloj));

    // desactivamos la alarma y consultamos que el estado sea false
    AlarmEnable(reloj, false);
    TEST_ASSERT_FALSE(IsAlarmActive(reloj));
}

/* Test 9: La librería deberá generar un evento cuando la alarma esté habilitada y además hora actual
coincida con la hora de la alarma.*/
void test_evento_alarma_activa(void) {
    clock_t reloj;
    hora_t hora_actual = {0, 6, 5, 9, 5, 9}; // 06:59:59
    hora_t hora_alarma = {0, 7, 0, 0, 0, 0}; // 07:00:00 (La hora a la que llegará con el tick)

    evento_disparado = 0;
    reloj = RelojCreate(1, CallbackEvento);

    TEST_ASSERT_TRUE(RelojSetCurrentTime(reloj, hora_actual));
    TEST_ASSERT_TRUE(AlarmSetTime(reloj, hora_alarma));
    AlarmEnable(reloj, true);

    ClockTick(reloj); // el reloj avanza a las 07:00:00 y hace coincidir la alarma

    TEST_ASSERT_EQUAL_INT(1, evento_disparado);
}

/*Test 10 : La librería deberá proporcionar una función para posponer la alarma una cantidad arbitraria
de minutos.*/
void test_posponer_alarma_incrementa_minutos(void) {
    clock_t reloj;
    hora_t hora_inicial = {1, 2, 2, 9, 5, 9};    // 12:29:59
    hora_t alarma_original = {1, 2, 3, 0, 0, 0}; // Alarma a las 12:30:00
    hora_t alarma_leida;

    evento_disparado = 0;
    reloj = RelojCreate(1, CallbackEvento);

    TEST_ASSERT_TRUE(RelojSetCurrentTime(reloj, hora_inicial));
    TEST_ASSERT_TRUE(AlarmSetTime(reloj, alarma_original));
    AlarmEnable(reloj, true);

    // primer tick para que el reloj avance a las 12:30:00 y dispare la alarma
    ClockTick(reloj);
    TEST_ASSERT_EQUAL_INT(1, evento_disparado);

    // posponemos la alarma 10 minutos, lo que debería hacer que la alarma vuelva a sonar a las 12:40:00
    TEST_ASSERT_TRUE(PostponeAlarm(reloj, 10));

    // verificamos que la alarma pospuesta se haya establecido correctamente
    TEST_ASSERT_TRUE(AlarmReadTime(reloj, alarma_leida));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(alarma_original, alarma_leida, sizeof(hora_t));

    // avanzamos el reloj 600 ticks (10 minutos) para que la alarma vuelva a sonar, como ya consumimos el primer tick.
    for (int i = 0; i < 600; i++) {
        ClockTick(reloj);
    }

    // al finalizar los 600 ticks, la alarma debería haber sonado nuevamente, por lo que el contador de eventos debería
    // ser 2
    TEST_ASSERT_EQUAL_INT(2, evento_disparado);
}

/*Test 11: La librería deberá manejar todas las horas como un arreglo de bytes en formato BCD sin
compactar, con la decena de horas en la primera posición y la unidad de los segundos en la
última posición del vector.*/
void test_avance_segundos_formato_BCD(void) {
    clock_t reloj;
    hora_t hora_actual;

    hora_t hora_inicial = {0, 0, 0, 0, 0, 9};
    hora_t hora_esperada = {0, 0, 0, 0, 1, 0};

    reloj = RelojCreate(1, NULL);

    TEST_ASSERT_TRUE(RelojSetCurrentTime(reloj, hora_inicial));

    ClockTick(reloj);

    TEST_ASSERT_TRUE(RelojGetCurrentTime(reloj, hora_actual));

    TEST_ASSERT_EQUAL_UINT8_ARRAY(hora_esperada, hora_actual, 6);
}
