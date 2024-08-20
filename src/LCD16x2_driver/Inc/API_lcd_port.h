/*
 * @file API_lcd_port.h
 * @brief Módulo que implementa
 *        la comunicación por I2C
 *        con el LCD.
 */

#ifndef API_INC_API_LCD_PORT_H_
#define API_INC_API_LCD_PORT_H_

#include "stm32f4xx.h"

// constantes para la comunicación I2C
#define I2C_INSTANCE    I2C1
#define I2C_CLOCK_SPEED 100000
#define I2C_TIMEOUT     10
#define LCD_ADDRESS     0x27

/**
 *   @brief Inicializa el periférico I2C.
 *	@retval Estado de ejecución.
 **/
bool_t port_init();

/**
 *   @brief Escribe un byte por I2C.
 *	@retval Estado de ejecución.
 */
bool_t port_i2cWriteByte(uint8_t);

/**
 *   @brief Implementa un delay bloqueante.
 */
void port_delay(uint32_t);

#endif /* API_INC_API_LCD_PORT_H_ */
