/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */


#ifndef __UTILS_CRC8_H
#define __UTILS_CRC8_H


#include <stdint.h>
#include <stdbool.h>



/**
 * @brief Composite calculation of crc8. Useful when you assemble your message step-by-step.
 *
 * @param buf - buffer with data
 * @param len - length of data
 * @param reset - true when need to start a new calculation
 * @return crc8
 */
uint8_t crc8_dallas_composite(const uint8_t * const buf, const uint32_t len, const bool reset);


/**
 * @brief Simple calculation of crc8
 *
 * @param buf - buffer with data
 * @param len - length of data
 * @return crc8
 */
uint8_t crc8_dallas(const uint8_t * const buff, const uint32_t len);



#endif /* __UTILS_CRC8_H */
