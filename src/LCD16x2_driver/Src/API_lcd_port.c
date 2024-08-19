/**
 * @file API_lcd_port.c
 * @brief Módulo que implementa
 *        la comunicación por I2C
 *        con el LCD.
 */

#include "API_lcd_port.h"

/**
 *	@brief Variable global privada para controlar el periferico I2C.
 */
static I2C_HandleTypeDef I2C_HANDLE;

/**
 *	@brief Función privada para inicializar el I2C.
 *	@retval Estado de ejecución.
 */
static bool_t port_i2cInit();

/**
 *   @brief Inicializa el periférico I2C.
 *	@retval Estado de ejecución.
 **/
bool_t port_init() { return port_i2cInit(); }

/**
 *	@brief Función para inicializar el I2C.
 *		   Utiliza la HAL de STM32 para la configuración.
 *	@retval Estado de ejecución.
 */
static bool_t port_i2cInit() {
  I2C_HANDLE.Instance = I2C_INSTANCE;
  I2C_HANDLE.Init.ClockSpeed = I2C_CLOCK_SPEED;
  I2C_HANDLE.Init.DutyCycle = I2C_DUTYCYCLE_2;
  I2C_HANDLE.Init.OwnAddress1 = 0;
  I2C_HANDLE.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  I2C_HANDLE.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2C_HANDLE.Init.OwnAddress2 = 0;
  I2C_HANDLE.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2C_HANDLE.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  bool_t estado = false;

  if (HAL_I2C_Init(&I2C_HANDLE) == HAL_OK) {
    estado = true;
  }

  return estado;
}

/**
 *   @brief Escribe un byte por I2C.
 *		   Utiliza la función bloqueante para
 *		   transmitir.
 *	@retval Estado de ejecución.
 */
bool_t port_i2cWriteByte(uint8_t _byte) {
  if (HAL_I2C_Master_Transmit(&I2C_HANDLE, LCD_ADDRESS << 1, &_byte, 1,
                              I2C_TIMEOUT) == HAL_OK)
    return true;
  else
    return false;
}

/**
 *   @brief Implementa un delay bloqueante
 *		   utilizando HAL_Delay.
 */
void port_delay(uint32_t delay) { HAL_Delay(delay); }
