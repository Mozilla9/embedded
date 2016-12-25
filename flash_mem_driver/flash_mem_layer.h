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
 * Public API macros.
 *
 * @param fml - pointer to the "__fmem_layer" structure
 * @param d - pointer to the "__fmem_layer_data" structure
 */
#define FMEM_ERASE(fml)          (fml)->erase(fml)
#define FMEM_READ(fml,d)         (fml)->read((fml),(d))
#define FMEM_CHANGE(fml,d)       (fml)->change((fml),(d))
#define FMEM_WRITE(fml,d)        (fml)->write((fml),(d))



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
 * Determines a data what need to read or write.
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
 * Describes an allocated block in the flash memory.
 *
 * @field START_ADDRESS - start address of allocated block in the flash memory,
 *                        should be aligned by sector size.
 *
 * @field MEM_VOLUME - volume of allocated block, should be aligned by sector size.
 * @field fmh - pointer on the "__flash_mem_handle" (low level driver implementation).
 */
typedef struct {
    const uint32_t START_ADDRESS;
    const uint32_t MEM_VOLUME;
    const __flash_mem_handle * const fmh;
} __fmem_layer_descriptor;


/**
 * Management structure.
 * Use API macros instead a straight calling.
 *
 */
typedef struct {
    const __fmem_layer_descriptor * descriptor;

    /**
     * interface
     */
    __flash_mem_layer_status (* erase)(void * const);
    __flash_mem_layer_status (* read)(void * const, const __fmem_layer_data * const);
    __flash_mem_layer_status (* write)(void * const, const __fmem_layer_data * const);
    __flash_mem_layer_status (* change)(void * const, const __fmem_layer_data * const);
} __fmem_layer;


void create_fmemlayer(__fmem_layer * const fml, const __fmem_layer_descriptor * const descriptor);


#endif /* __FLASH_MEM_LAYER_H */
