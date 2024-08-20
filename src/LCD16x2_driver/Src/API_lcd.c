/**
 * @file API_lcd.c
 * @brief  Implementación de funciones del
 * 		   módulo lcd.
 */

#include "API_lcd.h"
#include "API_types.h"

// constantes utilizadas para controlar el LCD
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

static uint8_t back_light = 1; // variable global privada para guardar el estado
                               // del backlight. 1 = encendido, 0 = apagado

/**
 *	@brief Funciones privadas para
 *		   enviar datos al LCD.
 */
static LCD_StatusTypedef LCD_sendMsg(uint8_t, uint8_t);
static LCD_StatusTypedef LCD_sendByte(uint8_t);
static LCD_StatusTypedef LCD_sendNibble(uint8_t, uint8_t);

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

/**
 *	@brief Realiza la secuencia de inicialización
 *		   del LCD.
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_init() {
    bool_t estadoI2C = port_init(); // inicializa el periférico I2C
    if (estadoI2C == false)
        return LCD_ERROR;

    port_delay(20);
    if (LCD_sendNibble(0x03, COMMAND) == LCD_ERROR)
        return LCD_ERROR;

    port_delay(10);

    if (LCD_sendNibble(0x03, COMMAND) == LCD_ERROR)
        return LCD_ERROR;

    port_delay(1);

    if (LCD_sendNibble(0x02, COMMAND) == LCD_ERROR)
        return LCD_ERROR;

    for (uint8_t indice = 0; indice < sizeof(LCD_INIT_CMD); indice++) {
        port_delay(1);
        if (LCD_sendMsg(LCD_INIT_CMD[indice], COMMAND) == LCD_ERROR)
            return LCD_ERROR;
    }
    return LCD_OK;
}

/**
 *	@brief Limpia la pantalla del LCD.
 *		   Para esto envía el comando CLR_LCD.
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_clear() {
    return LCD_sendMsg(CLR_LCD, COMMAND);
}

/**
 *	@brief Coloca el cursor del LCD en (fila, posición).
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_setCursor(uint8_t fila, uint8_t posicion) {
    if (fila != LCD_FILA_1 && fila != LCD_FILA_2)
        return LCD_ERROR;

    return LCD_sendMsg(SET_CURSOR | fila, COMMAND);
}

/**
 *	@brief Coloca una caracter en la pantalla del LCD
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_printChar(char dato) {
    return LCD_sendMsg(dato, DATA);
}

/**
 *	@brief Escribe un texto en el LCD. Para esto
 *		   recorre el puntero de entrada y verifica
 *		   que el caracter no sea NULL_CHAR. Utiliza la función
 *		   LCD_printChar.
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_printText(char * ptrTexto) {
    if (ptrTexto == NULL)
        return LCD_ERROR;

    LCD_clear();

    uint8_t contadorPosicion = 0;
    LCD_setCursor(LCD_FILA_1, 0);
    while ((*ptrTexto) != NULL_CHAR) {
        char caracter = *ptrTexto++;
        if (caracter == '\n') {
            LCD_setCursor(LCD_FILA_2, 0);
            contadorPosicion = 0;
            continue;
        }

        if (LCD_printChar(caracter) == LCD_ERROR)
            return LCD_ERROR;

        contadorPosicion++;
        if (contadorPosicion == LCD_CANTIDAD_COLUMNAS)
            LCD_setCursor(LCD_FILA_2, 0);
        else if (contadorPosicion == (LCD_CANTIDAD_COLUMNAS * LCD_CANTIDAD_FILAS))
            break; // CORTA EL TEXTO PERO NO DEVUELVE ERROR
    }

    return LCD_OK;
}

/**
 *	@brief Muestra un cursor que parpadea en
 *		   la pantalla del LCD.
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_cursorOn() {
    return LCD_sendMsg(DISPLAY_CONTROL | DISPLAY_ON | CURSOR_ON | CURSOR_BLINK, COMMAND);
}

/**
 *	@brief Apaga el cursor.
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_cursorOff() {
    return LCD_sendMsg(DISPLAY_CONTROL | DISPLAY_ON, COMMAND);
}

/**
 *	@brief Envía un mensaje al LCD, que puede
 *		   ser un comando (rs=0) o un dato (rs=1).
 *		   El envío de mensajes se realiza primero
 *		   con los 4 bits mas significativos del dato
 *		   y luego los 4 menos significativos (esto es así
 *		   porque se trabaja en modo 4BITS). También tiene
 *		   en cuenta el bit de back_light en cada envío.
 *	@retval Estado de ejecución.
 */
static LCD_StatusTypedef LCD_sendMsg(uint8_t dato, uint8_t rs) {
    if (LCD_sendByte(rs | (back_light << POS_BACKLIGHT) | (dato & 0xF0)) == LCD_ERROR)
        return LCD_ERROR;

    if (LCD_sendByte(rs | (back_light << POS_BACKLIGHT) | (dato & 0x0F) << 4) == LCD_ERROR)
        return LCD_ERROR;

    return LCD_OK;
}

/**
 * @brief Envía los 4 bits menos significativos de dato,
 * 		  junto con los bits de rs, backlight.
 *	@retval Estado de ejecución.
 */
static LCD_StatusTypedef LCD_sendNibble(uint8_t dato, uint8_t rs) {
    return LCD_sendByte(rs | (back_light << POS_BACKLIGHT) | (dato & 0x0F) << 4);
}

/**
 *	@brief Envía un byte al LCD.
 *		   El envío consiste en envíar
 *		   primero el byte con el bit de ENABLE
 *		   en alto, y luego de 1ms envíar el
 *		   byte sin el ENABLE. Esto genera el flanco
 *		   descendiente necesario para que el controlador
 *		   del LCD lea los datos.
 *	@retval Estado de ejecución.
 */
static LCD_StatusTypedef LCD_sendByte(uint8_t _byte) {
    if (!port_i2cWriteByte(_byte | ENABLE))
        return LCD_ERROR;

    port_delay(1);

    if (!port_i2cWriteByte(_byte))
        return LCD_ERROR;

    return LCD_OK;
}
