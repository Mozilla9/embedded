/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */



#include "mx25l3233fm2_config.h"



/**
 * Opcodes of mx25l3233fm2
 *
 */
const __flash_mem_opcodes mx25l3233fm2_opcodes = {
        .READ_DATA = 0x03,
        .WRITE_DIS = 0x04,
        .WRITE_EN = 0x06,
        .READ_SREG = 0x05,
        .READ_CREG = 0x15,
        .WRITE_SREG = 0x01,
        .SECTOR_ERASE = 0x20,
        .BLOCK32_ERASE = 0x52,
        .BLOCK64_ERASE = 0xd8,
        .CHIP_ERASE = 0x60,
        .PAGE_PROGRAM = 0x02,
        .READ_CHIP_ID = 0x9F
};


/**
 * Description of mx25l3233fm2
 *
 */
const __flash_mem_descriptor mx25l3233fm2_descriptor = {
        .FLASH_MEM_VOLUME = 0x400000,   /* 4mb */
        .PAGE_SIZE = 256,               /* 256 b*/
        .SECTOR_SIZE = 4096,            /* 4kb */

#ifdef FLASH_MEM_DRIVER_FAST_WRITE_EN
        .FAST_WRITE_EN = 1,
#else
        .FAST_WRITE_EN = 0,
#endif
        .DEVICE_ID_LENGHT = 2,
        .PAGE_WRITE_TIMEOUT_US = 60,
        .SECTOR_ERASE_TIMEOUT_MS = 5,
        .BLOCK32_ERASE_TIMEOUT_MS = 100,
        .BLOCK64_ERASE_TIMEOUT_MS = 200,
        .CHIP_ERASE_TIMEOUT_MS = 500
};


