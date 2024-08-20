/**
 * @file test_API_lcd.c
 * @author Javier Mosconi (jfmosconi@gmail.com)
 * @brief Implementación de funciones de test del módulo LCD
 * @date 2024-08-20
 *
 * @copyright Copyright (c) 2024
 */

/*
    Requerimientos a probar:
    1- Se debe inicializar el LCD con la secuencia de configuración correspondiente
    2- Se debe poder escribir un caracter
    3- Se debe poder escribir un texto
    4- Se debe poder limpiar la pantalla del LCD
    5- Posicionar cursor
    6- Encender cursor
    7- Apagar cursor
*/

#include <stdbool.h>
#include <stdint.h>

#include "unity.h"

/**
 * @brief Include del módulo que va a ser probado.
 */
#include "API_lcd.h"

/**
 * @brief Include de un mock para las funciones que acceden al hardware.
 */
#include "mock_API_lcd_port.h"

/**
 * @brief Constantes utilizadas para controlar el LCD. Obtenidas del archivo API_lcd.c
 */
#define _4BIT_MODE      0x28
#define DISPLAY_CONTROL (1 << 3)
#define RETURN_HOME     (1 << 1)
#define ENTRY_MODE      (1 << 2)
#define AUTOINCREMENT   (1 << 1)
#define DISPLAY_ON      (1 << 2)
#define CLR_LCD         1
#define COMMAND         0
#define DATA            1
#define ENABLE          (1 << 2)
#define POS_BACKLIGHT   (3)
#define SET_CURSOR      (1 << 7)
#define CURSOR_ON       1 << 1
#define CURSOR_BLINK    1

#define NULL_CHAR       '\0' // caracter nulo

/**
 * @brief variable global privada para guardar el estado del backlight.
 * 1 = encendido, 0 = apagado.
 */
static uint8_t back_light = 1;

/**
 *	@brief Secuencia de comandos para
 *		   configurar el LCD.
 */
static const uint8_t LCD_INIT_CMD[] = {
    _4BIT_MODE,      // configura el LCD para trabajar en modo de 4bits
    DISPLAY_CONTROL, // apaga el LCD momentaneamente
    RETURN_HOME,     // TODO REVISAR //coloca el cursor en 0
    ENTRY_MODE | AUTOINCREMENT,
    DISPLAY_CONTROL | DISPLAY_ON, // enciende el LCD
    CLR_LCD                       // limpia la pantalla
};

/*
En el módulo API_lcd hay funciones privadas:
static LCD_StatusTypedef LCD_sendMsg(uint8_t, uint8_t);
static LCD_StatusTypedef LCD_sendByte(uint8_t);
static LCD_StatusTypedef LCD_sendNibble(uint8_t, uint8_t);

Las llamadas a LCD_sendByte se puden mockear como:
void LCD_sendByte_ExpectAndReturn(uint8_t _byte, bool ret_value){
    port_i2cWriteByte_ExpectAndReturn(_byte | ENABLE, ret_value);
    port_i2cWriteByte_ExpectAndReturn(_byte, ret_value);
}
y las otras dos funciones realizan llamadas a LCD_sendByte_ExpectAndReturn
*/

/**
 * @brief Función que hace de mock para las llamadas a LCD_sendByte.
 *
 * @param _byte byte que se quiere "envíar" al LCD
 * @param ret_value valor booleano que se quiere obtener a partir de la entrada esperada.
 */
static void LCD_sendByte_ExpectAndReturn(uint8_t _byte, bool ret_value) {
    port_i2cWriteByte_ExpectAndReturn(_byte | ENABLE, ret_value);
    port_i2cWriteByte_ExpectAndReturn(_byte, ret_value);
}

/**
 * @brief Función que simula el comportamiento de la función privada LCD_sendNibble
 *
 * @param dato dato que se quiere envíar
 * @param rs Indicador de COMMAND o DATA
 * @param ret_value valor booleano que se quiere obtener a partir de la entrada esperada.
 */
static void LCD_sendNibble_ExpectAndReturn(uint8_t dato, uint8_t rs, bool ret_value) {
    LCD_sendByte_ExpectAndReturn(rs | (back_light << POS_BACKLIGHT) | (dato & 0x0F) << 4,
                                 ret_value);
}

/**
 * @brief Función que simula el comportamiento de la función privada LCD_sendMsg
 *
 * @param dato dato que se quiere envíar
 * @param rs Indicador de COMMAND o DATA
 * @param ret_value valor booleano que se quiere obtener a partir de la entrada esperada.
 */
static void LCD_sendMsg_ExpectAndReturn(uint8_t dato, uint8_t rs, bool ret_value) {
    LCD_sendByte_ExpectAndReturn(rs | (back_light << POS_BACKLIGHT) | (dato & 0xF0), ret_value);
    LCD_sendByte_ExpectAndReturn(rs | (back_light << POS_BACKLIGHT) | (dato & 0x0F) << 4,
                                 ret_value);
}

/**
 * @brief Inicializa el entorno de test.
 * Configura el comportamiento por defecto de las funciones que se mockearán.
 */
void setUp(void) {
    port_delay_Ignore();
}

/**
 * @brief Test para verificar la secuencia de inicio del LCD
 * según el requerimiento 1.
 */
void test_secuencia_inicio() {
    port_init_ExpectAndReturn(true);

    LCD_sendNibble_ExpectAndReturn(0x03, COMMAND, true);
    LCD_sendNibble_ExpectAndReturn(0x03, COMMAND, true);
    LCD_sendNibble_ExpectAndReturn(0x02, COMMAND, true);

    for (uint8_t indice = 0; indice < sizeof(LCD_INIT_CMD); indice++) {
        LCD_sendMsg_ExpectAndReturn(LCD_INIT_CMD[indice], COMMAND, true);
    }

    TEST_ASSERT_EQUAL(LCD_init(), LCD_OK);
}

/**
 * @brief Test para verificar la correcta escritura de un caracter
 * según el requerimiento 2.
 */
void test_escribir_un_caracter() {
    char caracter = 'a';
    LCD_sendMsg_ExpectAndReturn(caracter, DATA, true);

    TEST_ASSERT_EQUAL(LCD_printChar(caracter), LCD_OK);
}
