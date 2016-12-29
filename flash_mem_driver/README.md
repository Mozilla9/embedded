# README #

1) _flash_mem_driver_ - low level implementation. You should provide the *spi/delay* low-level functions.
2) _flash_mem_layer_ - memory abstraction layer.

**How to use it (example for stm)**

```
#include <stdint.h>
#include <stm32l1xx_hal.h>
#include <flash_mem_layer.h>


static void select_chip(void);
static void deselect_chip(void);
static uint32_t is_spi_busy(void);
static uint32_t spi_write(const uint8_t *wbuf, const uint32_t len);
static uint32_t spi_read(const uint8_t *rbuf, const uint32_t len);
static uint32_t delay(const uint32_t delay_in_us);


/**
 * Opcodes of mx25l3233fm2 flash memory
 *
 */
static const __flash_mem_opcodes opcodes = {
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
static const __flash_mem_descriptor descr = {
        .FLASH_MEM_VOLUME = 0x3ffc00,
        .PAGE_SIZE = 0xff,
        .SECTOR_SIZE = 0xfff,
        .DEVICE_ID_LENGHT = 2,
        .PAGE_WRITE_TIMEOUT_US = 600,
        .SECTOR_ERASE_TIMEOUT_MS = 50,
        .BLOCK32_ERASE_TIMEOUT_MS = 250,
        .BLOCK64_ERASE_TIMEOUT_MS = 500,
        .CHIP_ERASE_TIMEOUT_MS = 20000
};

/**
 * API functions for flash_mem_driver
 *
 */
static const __flash_mem_api api = {
    .select = select_chip,
    .deselect = deselect_chip,
    .is_spi_busy = is_spi_busy,
    .spi_write = spi_write,
    .spi_read = spi_read,
    .delay = delay
};


/**
 * Handle structure
 *
 */
const __flash_mem_handle fmdr_handle = {
        .descriptor = &descr,
        .opcodes = &opcodes,
        .api = &api
};


/**
 *
 */
static void select_chip(void)
{
    CS_PIN_TO_LOW();

    while(CS_PIN_VALUE() != 1);
}


/**
 *
 */
static void deselect_chip(void)
{
    CS_PIN_TO_HIGH();

    while(CS_PIN_VALUE() != 0);
}


/**
 *
 */
static uint32_t is_spi_busy(void)
{
    return __HAL_SPI_GET_FLAG(&spi_handle, SPI_FLAG_BSY) ? 1 : 0;
}


/**
 *
 */
static uint32_t spi_write(const uint8_t *wbuf, const uint32_t len)
{
    uint32_t err = HAL_SPI_Transmit(&spi_handle, (uint8_t *)wbuf, len);

    return err;
}

static uint32_t spi_read(const uint8_t *rbuf, const uint32_t len)
{
    uint32_t err = HAL_SPI_Receive(&spi_handle, (uint8_t *)rbuf, len);

    return err;
}

static uint32_t delay(const uint32_t delay_in_us)
{
    HAL_Delay(delay_in_us/1000);
    return HAL_OK;
}

```
