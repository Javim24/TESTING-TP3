/**
 * @file API_lcd.h
 * @brief Módulo que controla las funcionalidades
 * 		  del display LCD. Utiliza el módulo API_lcd_port
 *		  para acceder a funciones de bajo nivel.
 */

#ifndef API_INC_API_LCD_H_
#define API_INC_API_LCD_H_

#include "API_lcd_port.h"

// constantes para cantidad de filas y columnas de un lcd 16x2
#define LCD_CANTIDAD_COLUMNAS 16
#define LCD_CANTIDAD_FILAS    2

// constantes para la posición inicial de cada fila
#define LCD_FILA_1 0x00
#define LCD_FILA_2 0x40

/**
 * @brief Enum para devolver resultado de acciones del LCD.
 */
typedef enum { LCD_OK, LCD_ERROR } LCD_StatusTypedef;

/**
 *	@brief Realiza la inicialización del LCD para
 *		   que quede listo para ser utilizado.
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_init();

/**
 *	@brief Borra el contenido de la
 *		   pantalla del LCD.
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_clear();

/**
 *	@brief Coloca un único caracter en
 *		   la pantalla del LCD.
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_printChar(char);

/**
 *	@brief Escribe un texto en la pantalla del LCD,
 *		   comenzando desde la FILA 1 y posición 0.
 *		   Si detecta el caracter '\n', pasa a la
 *		   siguiente linea.
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_printText(char *);

/**
 *	@brief Posiciona el cursor del LCD
 *		   en la posición indica por los
 *		   argumentos de la función.
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_setCursor(uint8_t, uint8_t);

/**
 *	@brief Muestra un cursor que parpadea en
 *		   la pantalla del LCD.
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_cursorOn();

/**
 *	@brief Apaga el cursor.
 *	@retval Estado de ejecución.
 */
LCD_StatusTypedef LCD_cursorOff();

#endif /* API_INC_API_LCD_H_ */
