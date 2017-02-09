/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 *
 * Flash memory layer implementation.
 *
 * How to use:
 * 1) You should to implement all steps from "flash_memory_driver.h" file.
 *
 * 2) Choose a memory block in your flash chip (address + volume) and initialize
 * structure "__fmem_layer_descriptor".
 *
 * 3) Create a structure "__fmem_layer" and call the "create_fmemlayer(...)".
 * Pass pointers on your structures.
 *
 * 4) Use API macros for write/read/erase your allocated block in the flash memory.
 *
 */

#ifndef __FLASH_MEM_LAYER_H
#define __FLASH_MEM_LAYER_H


#include <stdint.h>
#include "flash_mem_driver.h"


/**
 * @brief Public API macros.
 *
 * @param fml - pointer to the "__fmem_layer" structure
 * @param d - pointer to the "__fmem_layer_data" structure
 */
#define FMEM_ERASE(fml)          fmem_erase_memory((fml))
#define FMEM_READ(fml,d)         fmem_read_data((fml),(d))
#define FMEM_CHANGE(fml,d)       fmem_change_data((fml),(d))
#define FMEM_WRITE(fml,d)        fmem_write_data((fml),(d))



typedef enum {
    FML_OK = 0,
    FML_ERROR,
    FML_ADDR_ERROR,
    FML_DATA_ERROR,
    FML_ERASE_ERROR,
    FML_PAGE_PRGR_ERROR,
    FML_DATA_READ_ERROR
} __flash_mem_layer_status;


/**
 * @brief Determines a data what need to read or write.
 *
 * @field addr - address in the virtual space <0 - MEM_VOLUME>.
 * @field len - length of data
 * @field buf - pointer to buffer
 */
typedef struct {
    uint32_t addr;
    uint8_t * buf;
    uint32_t len;
} __fmem_layer_data;


/**
 * @brief Describes an allocated block in the flash memory.
 *
 * @field START_ADDRESS - start address of allocated block in the flash memory,
 *                        should be aligned by sector size.
 *
 * @field MEM_VOLUME - volume of allocated block, should be aligned by sector size.
 * @field fmh - pointer on the "__flash_mem_handle" (low level driver implementation).
 */
typedef struct {
    uint32_t START_ADDRESS;
    uint32_t MEM_VOLUME;
    const __flash_mem_handle * const fmh;
} __fmem_layer_descriptor;


/**
 * @brief Management structure.
 *        Use API macros instead a straight calling.
 *
 */
typedef struct __fmem_layer {
    const __fmem_layer_descriptor * descriptor;
    
} __fmem_layer;


/**
 * @brief Performs an initialization of "__fmem_layer" structure.
 *
 * @param fml - pointer on "__fmem_layer" structure which need to initialize.
 * @param descriptor - pointer on "__fmem_layer_descriptor" structure which describes
 *                     allocated block of flash memory.
 */
void create_fmemlayer(__fmem_layer * const fml, const __fmem_layer_descriptor * const descriptor);


/**
 * @brief Write a data to the flash mem.
 *
 * @param f - pointer on "__fmem_layer"
 * @param wdata - pointer on "__fmem_layer_data"
 */
__flash_mem_layer_status fmem_write_data(struct __fmem_layer * const fml, const __fmem_layer_data * const wdata);


/**
 * @brief Write a data to the flash mem without erasing.
 *        It only changes bits from "1" to "0".
 *        E.g. 0xFF is possible to change on 0x00 state, but it won't work vise versa.
 *
 * @param f - pointer on "__fmem_layer"
 * @param wdata - pointer on "__fmem_layer_data"
 */
__flash_mem_layer_status fmem_change_data(struct __fmem_layer * const fml, const __fmem_layer_data * const wdata);


/**
 * @brief Read a data from the flash mem.
 *
 * @param f - pointer on "__fmem_layer"
 * @param rdata - pointer on "__fmem_layer_data"
 */
__flash_mem_layer_status fmem_read_data(struct __fmem_layer * const fml, const __fmem_layer_data * const rdata);


/**
 * @brief Erase all allocated volume of flash mem.
 *
 * @param f - pointer on "__fmem_layer"
 *
 */
__flash_mem_layer_status fmem_erase_memory(struct __fmem_layer * const fml);



#endif /* __FLASH_MEM_LAYER_H */
