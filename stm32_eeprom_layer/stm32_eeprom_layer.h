/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 *
 * STM32 EEPROM Flash memory layer implementation.
 *
 *
 */

#ifndef __STM32_EEPROM_LAYER_H
#define __STM32_EEPROM_LAYER_H



#include <stdint.h>


/**
 * @brief Public API macros.
 *
 * @param eeprl - pointer to the "__eeprom_layer" structure
 * @param d - pointer to the "__eeprom_layer_data" structure
 */
#define EEPROML_ERASE(eeprl)          (eeprl)->erase((eeprl))
#define EEPROML_READ(eeprl,d)         (eeprl)->read((eeprl),(d))
#define EEPROML_WRITE(eeprl,d)        (eeprl)->write((eeprl),(d))



typedef enum {
    STM32_EEPR_OK = 0,
    STM32_EEPR_ERROR,
    STM32_EEPR_ADDR_ERROR,
    STM32_EEPR_DATA_ERROR,
    STM32_EEPR_ERASE_ERROR,
    STM32_EEPR_WRITE_ERROR,
    STM32_EEPR_READ_ERROR
} __eeprom_layer_status;


/**
 * @brief Determines a data what need to read or write.
 *
 * @field addr - address in the virtual space <0 - MEM_VOLUME>.
 * @field len - length of data
 * @field buf - pointer to buffer
 */
typedef struct {
    uint32_t addr;
    uint32_t * buf;
    uint32_t len;
} __eeprom_layer_data;


/**
 * @brief Describes an allocated block in the eeprom.
 *
 * @field START_ADDRESS - start address of allocated block in the eeprom,
 *                        should be aligned by 4 bytes.
 *
 * @field MEM_VOLUME - volume of allocated block, should be aligned by 4 bytes.
 */
typedef struct {
    const uint32_t START_ADDRESS;
    const uint32_t MEM_VOLUME;
} __eeprom_layer_descriptor;


/**
 * @brief Management structure.
 *        Use API macros instead a straight calling.
 *
 */
typedef struct __eeprom_layer {
    const __eeprom_layer_descriptor * descriptor;

    /**
     * interface
     */
    __eeprom_layer_status (* erase)(struct __eeprom_layer * const);
    __eeprom_layer_status (* read)(struct __eeprom_layer * const, const __eeprom_layer_data * const);
    __eeprom_layer_status (* write)(struct __eeprom_layer * const, const __eeprom_layer_data * const);
} __eeprom_layer;


/**
 * @brief Performs an initialization of "__eeprom_layer" structure.
 *
 * @param fml - pointer on "__eeprom_layer" structure which need to initialize.
 * @param descriptor - pointer on "__eeprom_layer_descriptor" structure which describes
 *                     allocated block of flash memory.
 */
void create_eepromlayer(__eeprom_layer * const eeprl, const __eeprom_layer_descriptor * const descriptor);


#endif /* __STM32_EEPROM_LAYER_H */
