/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */



#ifndef __MX25L3233FM2_CONFIG_H
#define __MX25L3233FM2_CONFIG_H


#include <stdint.h>
#include <flash_mem_driver.h>

/**
 * Memory volume - 4 megabytes
 * Sector size - 4 kBytes
 * Page size - 256 bytes
 *
 *
 * If you need a fast-write functionality, make the define
 * macros FLASH_MEM_DRIVER_FAST_WRITE_EN in system.
 *
 * E.g. #define FLASH_MEM_DRIVER_FAST_WRITE_EN
 *
 * Then you should implement write buffer with 4 empty lead bytes, where
 * the flash_mem's driver will be putting address and opcode.
 *
 */


extern const __flash_mem_opcodes mx25l3233fm2_opcodes;

extern const __flash_mem_descriptor mx25l3233fm2_descriptor;



#endif /* __MX25L3233FM2_CONFIG_H */

