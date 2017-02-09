/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */


#include "stm32_eeprom_layer.h"

#include <stm32l1xx_hal.h>



/**
 * Private useful macros
 *
 */
#define  EEPR_EXTADDR_TO_HWADDR(eml,ea)    ((eml)->descriptor->START_ADDRESS + (ea))
#define  EEPR_IS_ADDR_IN_RANGE(eml,a)      (((a) < (eml)->descriptor->MEM_VOLUME) ? 1 : 0)
#define  EEPR_GET_SPACE_TO_END(eml,a)      (((a) < (eml)->descriptor->MEM_VOLUME) ? ((eml)->descriptor->MEM_VOLUME - (a)) : 0)


static __eeprom_layer_status stm32_eeprom_read_data(const __eeprom_layer_data * const rdata);
static __eeprom_layer_status stm32_eeprom_write_data(const __eeprom_layer_data * const wdata);
static __eeprom_layer_status stm32_eeprom_erase(const __eeprom_layer_data * const edata);


static __eeprom_layer_status write_data(struct __eeprom_layer * const eeprl, const __eeprom_layer_data * const wdata);
static __eeprom_layer_status read_data(struct __eeprom_layer * const eeprl, const __eeprom_layer_data * const rdata);
static __eeprom_layer_status erase_memory(struct __eeprom_layer * const eeprl);



/**
 *
 */
void create_eepromlayer(__eeprom_layer * const eeprl, const __eeprom_layer_descriptor * const descriptor)
{
    eeprl->descriptor = descriptor;

    /* interface */
    eeprl->erase = erase_memory;
    eeprl->read = read_data;
    eeprl->write = write_data;
}


/**
 * @brief Write a data to the eeprom.
 *
 * @param eeprl - pointer on "__eeprom_layer"
 * @param wdata - pointer on "__eeprom_layer_data"
 */
static __eeprom_layer_status write_data(struct __eeprom_layer * const eeprl, const __eeprom_layer_data * const wdata)
{
    uint32_t err;
    __eeprom_layer_data hw_data;

    if (!EEPR_IS_ADDR_IN_RANGE(eeprl, wdata->addr)) {
        return STM32_EEPR_ADDR_ERROR;
    }

    if (wdata->len > EEPR_GET_SPACE_TO_END(eeprl, wdata->addr) || !wdata->len) {
        return STM32_EEPR_DATA_ERROR;
    }

    /* erase */
    hw_data.addr = EEPR_EXTADDR_TO_HWADDR(eeprl, wdata->addr);
    hw_data.len = wdata->len;

    err = stm32_eeprom_erase(&hw_data);

    if (err != STM32_EEPR_OK) {
        return err;
    }

    /* write */
    hw_data.addr = EEPR_EXTADDR_TO_HWADDR(eeprl, wdata->addr);
    hw_data.len = wdata->len;
    hw_data.buf = wdata->buf;

    err = stm32_eeprom_write_data(&hw_data);

    return err;
}


/**
 * @brief Read data from eeprom.
 *
 * @param eeprl - pointer on "__eeprom_layer"
 * @param rdata - pointer on "__eeprom_layer_data"
 */
static __eeprom_layer_status read_data(struct __eeprom_layer * const eeprl, const __eeprom_layer_data * const rdata)
{
    uint32_t err;
    __eeprom_layer_data hw_data;

    if (!EEPR_IS_ADDR_IN_RANGE(eeprl, rdata->addr)) {
        return STM32_EEPR_ADDR_ERROR;
    }

    if (rdata->len > EEPR_GET_SPACE_TO_END(eeprl, rdata->addr) || !rdata->len) {
        return STM32_EEPR_DATA_ERROR;
    }

    /* read */
    hw_data.addr = EEPR_EXTADDR_TO_HWADDR(eeprl, rdata->addr);
    hw_data.len = rdata->len;
    hw_data.buf = rdata->buf;

    err = stm32_eeprom_read_data(&hw_data);

    return err;
}


/**
 * @brief Erase eeprom.
 *
 * @param eeprl - pointer on "__eeprom_layer"
 */
static __eeprom_layer_status erase_memory(struct __eeprom_layer * const eeprl)
{
    uint32_t err;
    __eeprom_layer_data hw_data;

    /* erase */
    hw_data.addr = eeprl->descriptor->START_ADDRESS;
    hw_data.len = eeprl->descriptor->MEM_VOLUME / 4;

    err = stm32_eeprom_erase(&hw_data);

    return err;
}


/**
 * @brief Read a data from eeprom.
 *
 * @param rdata - pointer on "__eeprom_layer_data"
 * @return status operation
 */
static __eeprom_layer_status stm32_eeprom_read_data(const __eeprom_layer_data * const rdata)
{
    uint32_t *addr;
    uint32_t len;
    uint32_t *buf;

    /* do read */
    len = rdata->len;
    addr = (uint32_t *)rdata->addr;
    buf = rdata->buf;

    while (len--) {
        *buf++ = *addr++;
    }

    return STM32_EEPR_OK;
}


/**
 * @brief Write a data in the eeprom.
 *
 * @param wdata - pointer on "__eeprom_layer_data"
 * @return status operation
 */
static __eeprom_layer_status stm32_eeprom_write_data(const __eeprom_layer_data * const wdata)
{
    uint32_t err;

    uint32_t addr;
    uint32_t len;
    uint32_t *buf;

    /* unlock eeprom */
    err = HAL_FLASHEx_DATAEEPROM_Unlock();

    if (err != HAL_OK) {
        return STM32_EEPR_ERROR;
    }

    /* do write */
    err = STM32_EEPR_OK;

    addr = wdata->addr;
    len = wdata->len;
    buf = wdata->buf;

    while (len--) {
        const uint32_t write_error = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, addr, *buf++);

        if (write_error != HAL_OK) {
            err = STM32_EEPR_WRITE_ERROR;
            break;
        }

        addr += 4;
    }

    /* lock eeprom */
    HAL_FLASHEx_DATAEEPROM_Lock();

    return err;
}


/**
 * @brief Erase data in the eeprom.
 *
 * @param edata - pointer on "__eeprom_layer_data"
 * @return status operation
 */
static __eeprom_layer_status stm32_eeprom_erase(const __eeprom_layer_data * const edata)
{
    uint32_t err;

    uint32_t addr;
    uint32_t len;

    /* unlock eeprom */
    err = HAL_FLASHEx_DATAEEPROM_Unlock();

    if (err != HAL_OK) {
        return STM32_EEPR_ERROR;
    }

    /* do erase */
    err = STM32_EEPR_OK;

    addr = edata->addr;
    len = edata->len;

    while (len--) {
        const uint32_t erase_error = HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_WORD, addr);

        if (erase_error != HAL_OK) {
            err = STM32_EEPR_ERASE_ERROR;
            break;
        }

        addr += 4;
    }

    /* lock eeprom */
    HAL_FLASHEx_DATAEEPROM_Lock();

    return err;
}

