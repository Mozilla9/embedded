/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 *
 * Flash memory driver implementation.
 *
 * How to use:
 * 1) Driver is required an implementation of low level functions for working with a spi.
 *    You should implement this functions for your system and install it in a
 *    "__flash_mem_api" structure.
 *
 * 2) Describe your flash memory chip in a "__flash_mem_descriptor" structure.
 *
 * 3) Fill structure "__flash_mem_descriptor" with flash mem opcodes.
 *
 * 4) Initialize "__flash_mem_handle".
 *
 * 5) Use public API functions with this handle.
 *
 */



#ifndef __FLASH_MEM_DRIVER_H
#define __FLASH_MEM_DRIVER_H


#include <stdint.h>


typedef enum {
    FMDR_OK = 0,
    FMDR_ERROR,
    FMDR_ADDR_ERROR,
    FMDR_DATA_ERROR,
    FMDR_BUSY_ERROR
} __flash_mem_op_status;


/**
 * Union that determines a address in the flash mem.
 *
 * @field addr32 - address.
 * @field addr - four bytes of address
 */
typedef union {
    uint32_t addr32;
    uint8_t addr[4];
} __flash_mem_address;


/**
 * Determines a data what need to read or write in the flash memory.
 *
 * @field faddr - address in the flash mem space.
 * @field len - length of data
 * @field buf - pointer to buffer
 */
typedef struct {
    __flash_mem_address faddr;
    uint8_t *buf;
    uint16_t len;
} __flash_mem_data;


/**
 * Flesh memory information.
 *
 */
typedef struct {
    uint8_t chip_id[4];
} __flash_mem_info;


/**
 * Determines opcodes of the flash memory.
 *
 */
typedef struct {
    const uint8_t READ_DATA;
    const uint8_t WRITE_DIS;
    const uint8_t WRITE_EN;
    const uint8_t READ_SREG;
    const uint8_t READ_CREG;
    const uint8_t WRITE_SREG;
    const uint8_t SECTOR_ERASE;
    const uint8_t BLOCK32_ERASE;
    const uint8_t BLOCK64_ERASE;
    const uint8_t CHIP_ERASE;
    const uint8_t PAGE_PROGRAM;
    const uint8_t READ_CHIP_ID;
} __flash_mem_opcodes;


/**
 * Descriptor of the flash memory.
 *
 */
typedef struct {
    const uint32_t FLASH_MEM_VOLUME;
    const uint32_t PAGE_SIZE;
    const uint32_t SECTOR_SIZE;
    const uint32_t BLOCK32_SIZE;
    const uint32_t BLOCK64_SIZE;

    const uint16_t DEVICE_ID_LENGHT;
    const uint16_t PAGE_WRITE_TIMEOUT_US;
    const uint16_t SECTOR_ERASE_TIMEOUT_MS;
    const uint16_t BLOCK32_ERASE_TIMEOUT_MS;
    const uint16_t BLOCK64_ERASE_TIMEOUT_MS;
    const uint16_t CHIP_ERASE_TIMEOUT_MS;
} __flash_mem_descriptor;


/**
 * Contains implementation of low level spi api.
 *
 * @field select - set CS pin to low state
 * @field deselect - set CS pin to high state
 * @field is_spi_busy - check the spi busy state
 * @filed spi_write - write a data to the spi
 * @field spi_read - read a data from the spi
 * @field delay - hw delay, usec
 */
typedef struct {
    void ( *select)(void);
    void (* deselect)(void);
    uint32_t (* is_spi_busy)(void);
    uint32_t (* spi_write)(const uint8_t *wbuf, const uint32_t len);
    uint32_t (* spi_read)(const uint8_t *rbuf, const uint32_t len);
    uint32_t (* delay)(const uint32_t delay);
} __flash_mem_api;

/**
 * Management structure
 */
typedef struct {

    const __flash_mem_descriptor * const descriptor;
    const __flash_mem_opcodes * const opcodes;
    const __flash_mem_api * const api;

} __flash_mem_handle;


/**
 * Public API functions
 */
__flash_mem_op_status flash_mem_read_sreg(const __flash_mem_handle * const handle, uint8_t *sreg);
__flash_mem_op_status flash_mem_read_info(const __flash_mem_handle * const handle, __flash_mem_info *info);
__flash_mem_op_status flash_mem_read_data(const __flash_mem_handle * const handle, __flash_mem_data *rdata);
__flash_mem_op_status flash_mem_write_page_data(const __flash_mem_handle * const handle, __flash_mem_data *wdata);
__flash_mem_op_status flash_mem_sector_erase(const __flash_mem_handle * const handle, const __flash_mem_address faddr);
__flash_mem_op_status flash_mem_block32_erase(const __flash_mem_handle * const handle, const __flash_mem_address faddr);
__flash_mem_op_status flash_mem_block64_erase(const __flash_mem_handle * const handle, const __flash_mem_address faddr);
__flash_mem_op_status flash_mem_chip_erase(const __flash_mem_handle * const handle);


#endif /* __FLASH_MEM_DRIVER_H */
