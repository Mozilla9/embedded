/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */


#include "flash_mem_driver.h"


/**
 * Private useful macros
 *
 */
#define FMDR_SELECT_CHIP()           handle->api->select()
#define FMDR_DESELECT_CHIP()         handle->api->deselect()
#define FMDR_IS_SPI_BUSY()           handle->api->is_spi_busy()
#define FMDR_GET_OPCODE(OPCODE)      handle->opcodes->OPCODE
#define FMDR_WRITE_DATA(buff,len)    handle->api->spi_write((buff),(len))
#define FMDR_READ_DATA(buff,len)     handle->api->spi_read((buff),(len))
#define FMDR_DELAY(dl)               handle->api->delay((dl))
#define FMDR_RETURN_ERROR(err)       FMDR_DESELECT_CHIP(); return (err)

#define FMDR_CHECK_CHIP_BUSY(s)      ((s)&0x01)
#define FMDR_CHECK_CHIP_WEL(s)       ((s)&0x02)



static __flash_mem_op_status write_enable(const __flash_mem_handle * const handle);
static __flash_mem_op_status flash_mem_read_sreg(const __flash_mem_handle * const handle, uint8_t * sreg);


__flash_mem_op_status
flash_mem_read_info(const __flash_mem_handle * const handle, __flash_mem_info *info)
{
    uint8_t opcode = FMDR_GET_OPCODE(READ_CHIP_ID);
    uint32_t err = 0;

    FMDR_SELECT_CHIP();

    err = FMDR_WRITE_DATA(&opcode, 1);

    if (err || FMDR_IS_SPI_BUSY()) {
        FMDR_RETURN_ERROR(FMDR_ERROR);
    }

    err = FMDR_READ_DATA(info->chip_id, handle->descriptor->DEVICE_ID_LENGHT);

    if (err || FMDR_IS_SPI_BUSY()) {
        FMDR_RETURN_ERROR(FMDR_ERROR);
    }

    FMDR_DESELECT_CHIP();

    return FMDR_OK;
}

__flash_mem_op_status
flash_mem_read_data(const __flash_mem_handle * const handle, __flash_mem_data *rdata)
{
    uint8_t wbuf[4];
    uint32_t err = 0;

    if (rdata->faddr.addr32 >= handle->descriptor->FLASH_MEM_VOLUME) {
        return FMDR_ADDR_ERROR;
    }

    wbuf[0] = FMDR_GET_OPCODE(READ_DATA);
    wbuf[1] = rdata->faddr.addr[2];
    wbuf[2] = rdata->faddr.addr[1];
    wbuf[3] = rdata->faddr.addr[0];

    FMDR_SELECT_CHIP();

    err = FMDR_WRITE_DATA(wbuf, 4);

    if (err || FMDR_IS_SPI_BUSY()) {
        FMDR_RETURN_ERROR(FMDR_ERROR);
    }

    err = FMDR_READ_DATA(rdata->buf, rdata->len);

    if (err || FMDR_IS_SPI_BUSY()) {
        FMDR_RETURN_ERROR(FMDR_ERROR);
    }

    FMDR_DESELECT_CHIP();

    return FMDR_OK;
}


__flash_mem_op_status
flash_mem_write_page_data(const __flash_mem_handle * const handle, __flash_mem_data *wdata)
{
    uint32_t err = 0;
    uint8_t sreg;

    if (wdata->faddr.addr32 >= handle->descriptor->FLASH_MEM_VOLUME) {
        return FMDR_ADDR_ERROR;
    }

    const uint32_t page_size = handle->descriptor->PAGE_SIZE;
    const uint32_t free_size = page_size - (wdata->faddr.addr32 % page_size);

    if (wdata->len > free_size) {
        return FMDR_DATA_ERROR;
    }

    /* check BUSY/WRE */
    err = flash_mem_read_sreg(handle, &sreg);
    if (err) {
        return err;
    } else {
        /* check on busy */
        if (FMDR_CHECK_CHIP_BUSY(sreg)) {
            return FMDR_BUSY_ERROR;
        } else if (!FMDR_CHECK_CHIP_WEL(sreg)) {
            err = write_enable(handle);
            if (err != FMDR_OK) {
                return err;
            }
        }
    }

    /* fast write support */
    if (handle->descriptor->FAST_WRITE_EN) {

        wdata->buf[0] = FMDR_GET_OPCODE(PAGE_PROGRAM);
        wdata->buf[1] = wdata->faddr.addr[2];
        wdata->buf[2] = wdata->faddr.addr[1];
        wdata->buf[3] = wdata->faddr.addr[0];

        FMDR_SELECT_CHIP();

    } else {
        uint8_t wbuf[4];

        wbuf[0] = FMDR_GET_OPCODE(PAGE_PROGRAM);
        wbuf[1] = wdata->faddr.addr[2];
        wbuf[2] = wdata->faddr.addr[1];
        wbuf[3] = wdata->faddr.addr[0];

        FMDR_SELECT_CHIP();

        err = FMDR_WRITE_DATA(wbuf, 4);

        if (err || FMDR_IS_SPI_BUSY()) {
            FMDR_RETURN_ERROR(FMDR_ERROR);
        }
    }

    err = FMDR_WRITE_DATA(wdata->buf, wdata->len + (handle->descriptor->FAST_WRITE_EN ? 4 : 0));

    if (err || FMDR_IS_SPI_BUSY()) {
        FMDR_RETURN_ERROR(FMDR_ERROR);
    }

    FMDR_DESELECT_CHIP();

    /* check on busy */
    while(1) {
        err = flash_mem_read_sreg(handle, &sreg);

        if (err) {
            return err;
        }

        if (FMDR_CHECK_CHIP_BUSY(sreg)) {
            err = FMDR_DELAY(handle->descriptor->PAGE_WRITE_TIMEOUT_US);

            if (err) {
                return err;
            }
        } else {
            break;
        }
    }

    return FMDR_OK;
}


__flash_mem_op_status
flash_mem_sector_erase(const __flash_mem_handle * const handle, const __flash_mem_address faddr)
{
    uint32_t err = 0;
    uint8_t sreg;
    uint8_t wbuf[4];

    if (faddr.addr32 > handle->descriptor->FLASH_MEM_VOLUME - 1) {
        return FMDR_DATA_ERROR;
    }

    /* check BUSY/WRE */
    err = flash_mem_read_sreg(handle, &sreg);
    if (err) {
        return err;
    } else {
        /* check on busy */
        if (FMDR_CHECK_CHIP_BUSY(sreg)) {
            return FMDR_BUSY_ERROR;
        } else if (!FMDR_CHECK_CHIP_WEL(sreg)) {
            err = write_enable(handle);
            if (err != FMDR_OK) {
                return err;
            }
        }
    }

    wbuf[0] = FMDR_GET_OPCODE(SECTOR_ERASE);
    wbuf[1] = faddr.addr[2];
    wbuf[2] = faddr.addr[1];
    wbuf[3] = faddr.addr[0];

    FMDR_SELECT_CHIP();

    err = FMDR_WRITE_DATA(wbuf, 4);

    if (err || FMDR_IS_SPI_BUSY()) {
        FMDR_RETURN_ERROR(FMDR_ERROR);
    }

    FMDR_DESELECT_CHIP();

    /* check on busy */
    while(1) {
        err = flash_mem_read_sreg(handle, &sreg);

        if (err) {
            return err;
        }

        if (FMDR_CHECK_CHIP_BUSY(sreg)) {
            err = FMDR_DELAY(handle->descriptor->SECTOR_ERASE_TIMEOUT_MS * 1000);

            if (err) {
                return err;
            }
        } else {
            break;
        }
    }

    return FMDR_OK;
}


__flash_mem_op_status
flash_mem_block32_erase(const __flash_mem_handle * const handle, const __flash_mem_address faddr)
{
    uint32_t err = 0;
    uint8_t sreg;
    uint8_t wbuf[4];

    if (faddr.addr32 > handle->descriptor->FLASH_MEM_VOLUME - 1) {
        return FMDR_DATA_ERROR;
    }

    /* check BUSY/WRE */
    err = flash_mem_read_sreg(handle, &sreg);
    if (err) {
        return err;
    } else {
        /* check on busy */
        if (FMDR_CHECK_CHIP_BUSY(sreg)) {
            return FMDR_BUSY_ERROR;
        } else if (!FMDR_CHECK_CHIP_WEL(sreg)) {
            err = write_enable(handle);
            if (err != FMDR_OK) {
                return err;
            }
        }
    }

    wbuf[0] = FMDR_GET_OPCODE(BLOCK32_ERASE);
    wbuf[1] = faddr.addr[2];
    wbuf[2] = faddr.addr[1];
    wbuf[3] = faddr.addr[0];

    FMDR_SELECT_CHIP();

    err = FMDR_WRITE_DATA(wbuf, 4);

    if (err || FMDR_IS_SPI_BUSY()) {
        FMDR_RETURN_ERROR(FMDR_ERROR);
    }

    FMDR_DESELECT_CHIP();

    /* check on busy */
    while(1) {
        err = flash_mem_read_sreg(handle, &sreg);

        if (err) {
            return err;
        }

        if (FMDR_CHECK_CHIP_BUSY(sreg)) {
            err = FMDR_DELAY(handle->descriptor->BLOCK32_ERASE_TIMEOUT_MS * 1000);

            if (err) {
                return err;
            }
        } else {
            break;
        }
    }

    return FMDR_OK;
}


__flash_mem_op_status
flash_mem_block64_erase(const __flash_mem_handle * const handle, const __flash_mem_address faddr)
{
    uint32_t err = 0;
    uint8_t sreg;
    uint8_t wbuf[4];

    if (faddr.addr32 > handle->descriptor->FLASH_MEM_VOLUME - 1) {
        return FMDR_DATA_ERROR;
    }

    /* check BUSY/WRE */
    err = flash_mem_read_sreg(handle, &sreg);
    if (err) {
        return err;
    } else {
        /* check on busy */
        if (FMDR_CHECK_CHIP_BUSY(sreg)) {
            return FMDR_BUSY_ERROR;
        } else if (!FMDR_CHECK_CHIP_WEL(sreg)) {
            err = write_enable(handle);
            if (err != FMDR_OK) {
                return err;
            }
        }
    }

    wbuf[0] = FMDR_GET_OPCODE(BLOCK64_ERASE);
    wbuf[1] = faddr.addr[2];
    wbuf[2] = faddr.addr[1];
    wbuf[3] = faddr.addr[0];

    FMDR_SELECT_CHIP();

    err = FMDR_WRITE_DATA(wbuf, 4);

    if (err || FMDR_IS_SPI_BUSY()) {
        FMDR_RETURN_ERROR(FMDR_ERROR);
    }

    FMDR_DESELECT_CHIP();

    /* check on busy */
    while(1) {
        err = flash_mem_read_sreg(handle, &sreg);

        if (err) {
            return err;
        }

        if (FMDR_CHECK_CHIP_BUSY(sreg)) {
            err = FMDR_DELAY(handle->descriptor->BLOCK64_ERASE_TIMEOUT_MS * 1000);

            if (err) {
                return err;
            }
        } else {
            break;
        }
    }

    return FMDR_OK;
}


__flash_mem_op_status
flash_mem_chip_erase(const __flash_mem_handle * const handle)
{
    uint8_t opcode = FMDR_GET_OPCODE(CHIP_ERASE);
    uint32_t err = 0;
    uint8_t sreg;

    /* check BUSY/WRE */
    err = flash_mem_read_sreg(handle, &sreg);
    if (err) {
        return err;
    } else {
        /* check on busy */
        if (FMDR_CHECK_CHIP_BUSY(sreg)) {
            return FMDR_BUSY_ERROR;
        } else if (!FMDR_CHECK_CHIP_WEL(sreg)) {
            err = write_enable(handle);
            if (err != FMDR_OK) {
                return err;
            }
        }
    }

    FMDR_SELECT_CHIP();

    err = FMDR_WRITE_DATA(&opcode, 1);

    if (err || FMDR_IS_SPI_BUSY()) {
        FMDR_RETURN_ERROR(FMDR_ERROR);
    }

    FMDR_DESELECT_CHIP();

    /* check on busy */
    while(1) {
        err = flash_mem_read_sreg(handle, &sreg);

        if (err) {
            return err;
        }

        if (FMDR_CHECK_CHIP_BUSY(sreg)) {
            err = FMDR_DELAY(handle->descriptor->CHIP_ERASE_TIMEOUT_MS * 1000);

            if (err) {
                return err;
            }
        } else {
            break;
        }
    }

    return FMDR_OK;
}


/**
 * @brief Read the status register
 *        Byte1:
 *          0b - WIP(BUSY)
 *          1b - WEL
 *          2b..7b - see datasheet on your chip
 *
 * @param handle - pointer on management structure with low level API
 * @param sreg - pointer on sreg byte
 * @return status operation
 */
static __flash_mem_op_status
flash_mem_read_sreg(const __flash_mem_handle * const handle, uint8_t * sreg)
{
    uint8_t opcode = FMDR_GET_OPCODE(READ_SREG);
    uint32_t err = 0;

    FMDR_SELECT_CHIP();

    err = FMDR_WRITE_DATA(&opcode, 1);

    if (err || FMDR_IS_SPI_BUSY()) {
        FMDR_RETURN_ERROR(FMDR_ERROR);
    }

    err = FMDR_READ_DATA(sreg, 1);

    if (err || FMDR_IS_SPI_BUSY()) {
        FMDR_RETURN_ERROR(FMDR_ERROR);
    }

    FMDR_DESELECT_CHIP();

    return FMDR_OK;
}


/**
 * @brief Turn off write memory protection.
 *
 * @param handle - pointer on management structure with low level API
 * @return status operation
 */
static __flash_mem_op_status write_enable(const __flash_mem_handle * const handle)
{
    uint8_t opcode = FMDR_GET_OPCODE(WRITE_EN);
    uint32_t err = 0;

    FMDR_SELECT_CHIP();

    err = FMDR_WRITE_DATA(&opcode, 1);

    if (err || FMDR_IS_SPI_BUSY()) {
        FMDR_RETURN_ERROR(FMDR_ERROR);
    }

    FMDR_DESELECT_CHIP();

    return FMDR_OK;
}

