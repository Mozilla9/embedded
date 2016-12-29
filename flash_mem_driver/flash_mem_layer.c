/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */


#include "flash_mem_layer.h"


/**
 * Private useful macros
 *
 */
#define  BLOCK64_SIZE                     0xffff
#define  BLOCK32_SIZE                     0x7fff

#define  FML_MEM_SIZE(fml)                ((fml)->descriptor->MEM_VOLUME)
#define  FML_EXTADDR_TO_HWADDR(fml,ea)    ((fml)->descriptor->START_ADDRESS + (ea))
#define  FML_GET_WRITE_DATALEN(fml,a)     (((((a) / (fml)->descriptor->fmh->descriptor->PAGE_SIZE) + 1) \
                                           * (fml)->descriptor->fmh->descriptor->PAGE_SIZE) - a)

#define  FML_IS_ADDR_IN_RANGE(fml,a)      (((a) < (fml)->descriptor->MEM_VOLUME) ? 1 : 0)
#define  FML_IS_NEW_SECTOR(fml,a)         (((a) % (fml)->descriptor->fmh->descriptor->SECTOR_SIZE) ? 1 : 0)
#define  FML_GET_SPACE_TO_END(fml,a)      (((a) < (fml)->descriptor->MEM_VOLUME) ? ((fml)->descriptor->MEM_VOLUME - (a)) : 0)

#define  FML_IS_ALIGN_BY_64K()            ((fml->descriptor->START_ADDRESS % BLOCK64_SIZE) ? 0 : 1)
#define  FML_IS_ALIGN_BY_32K()            ((fml->descriptor->START_ADDRESS % BLOCK32_SIZE) ? 0 : 1)




static __flash_mem_layer_status write_data(struct __fmem_layer * const fml, const __fmem_layer_data * const wdata);
static __flash_mem_layer_status change_data(struct __fmem_layer * const fml, const __fmem_layer_data * const wdata);
static __flash_mem_layer_status read_data(struct __fmem_layer * const fml, const __fmem_layer_data * const rdata);
static __flash_mem_layer_status erase_memory(struct __fmem_layer * const fml);




void create_fmemlayer(__fmem_layer * const fml, const __fmem_layer_descriptor * const descriptor)
{
    fml->descriptor = descriptor;

    /* interface */
    fml->erase = erase_memory;
    fml->read = read_data;
    fml->change = change_data;
    fml->write = write_data;
}


/**
 * @brief Write a data to the flash mem.
 *
 * @param f - pointer on "__fmem_layer"
 * @param wdata - pointer on "__fmem_layer_data"
 */
static __flash_mem_layer_status write_data(struct __fmem_layer * const fml, const __fmem_layer_data * const wdata)
{
    uint32_t count_addr;
    uint32_t count_data;
    uint32_t err;
    __flash_mem_data fmdr_data;

    if (!FML_IS_ADDR_IN_RANGE(fml, wdata->addr)) {
        return FML_ADDR_ERROR;
    }

    if (wdata->len > FML_GET_SPACE_TO_END(fml, wdata->addr) || !wdata->len) {
        return FML_DATA_ERROR;
    }

    /* write cycle */
    count_addr = wdata->addr;
    count_data = 0;

    while (count_data < wdata->len) {
        /* determine a data block to write */
        fmdr_data.len = FML_GET_WRITE_DATALEN(fml, count_addr);
        fmdr_data.len = ((wdata->len - count_data) > fmdr_data.len)
                              ? fmdr_data.len : (wdata->len - count_data);

        /* prepare params to write */
        fmdr_data.faddr.addr32 = FML_EXTADDR_TO_HWADDR(fml, count_addr);
        fmdr_data.buf = (wdata->buf + count_data);

        /* check at new sector */
        if (FML_IS_NEW_SECTOR(fml, count_addr)) {

            err = flash_mem_sector_erase(fml->descriptor->fmh, fmdr_data.faddr);

            if (err != FMDR_OK) {
                return FML_ERASE_ERROR;
            }
        }

        err = flash_mem_write_page_data(fml->descriptor->fmh, &fmdr_data);

        if (err != FMDR_OK) {
            return FML_PAGE_PRGR_ERROR;
        }

        /* shift counters */
        count_addr += fmdr_data.len;
        count_data += fmdr_data.len;
    }

    return FML_OK;
}


/**
 * @brief Write a data to the flash mem without erasing.
 *        It only changes bits from "1" to "0".
 *        E.g. 0xFF is possible to change on 0x00 state, but it won't work vise versa.
 *
 * @param f - pointer on "__fmem_layer"
 * @param wdata - pointer on "__fmem_layer_data"
 */
static __flash_mem_layer_status change_data(struct __fmem_layer * const fml, const __fmem_layer_data * const wdata)
{
    uint32_t err;
    __flash_mem_data fmdr_data;

    if (!FML_IS_ADDR_IN_RANGE(fml, wdata->addr)) {
        return FML_ADDR_ERROR;
    }

    if (wdata->len > FML_GET_SPACE_TO_END(fml, wdata->addr) || !wdata->len) {
        return FML_DATA_ERROR;
    }

    /* the data len should be no more then a page size */
    if (wdata->len > FML_GET_WRITE_DATALEN(fml, wdata->addr)) {
        return FML_DATA_ERROR;
    }

    /* change data == switch 1 -> 0 */
    fmdr_data.faddr.addr32 = FML_EXTADDR_TO_HWADDR(fml, wdata->addr);
    fmdr_data.buf = wdata->buf;
    fmdr_data.len = wdata->len;

    err = flash_mem_write_page_data(fml->descriptor->fmh, &fmdr_data);

    if (err != FMDR_OK) {
        return FML_PAGE_PRGR_ERROR;
    }

    return FML_OK;
}


/**
 * @brief Read a data from the flash mem.
 *
 * @param f - pointer on "__fmem_layer"
 * @param rdata - pointer on "__fmem_layer_data"
 */
static __flash_mem_layer_status read_data(struct __fmem_layer * const fml, const __fmem_layer_data * const rdata)
{
    uint32_t err;
    __flash_mem_data fmdr_data;

    if (!FML_IS_ADDR_IN_RANGE(fml, rdata->addr)) {
        return FML_ADDR_ERROR;
    }

    if (rdata->len > FML_GET_SPACE_TO_END(fml, rdata->addr) || !rdata->len) {
        return FML_DATA_ERROR;
    }

    fmdr_data.faddr.addr32 = FML_EXTADDR_TO_HWADDR(fml, rdata->addr);
    fmdr_data.buf = rdata->buf;
    fmdr_data.len = rdata->len;

    err = flash_mem_read_data(fml->descriptor->fmh, &fmdr_data);

    if (err != FMDR_OK) {
        return FML_DATA_READ_ERROR;
    }

    return FML_OK;
}


/**
 * @brief Erase all allocated volume of flash mem.
 *
 * @param f - pointer on "__fmem_layer"
 *
 */
static __flash_mem_layer_status erase_memory(struct __fmem_layer * const fml)
{
    uint32_t err;
    uint32_t iteration_addr = 0;

    __flash_mem_op_status (* erase_func) (const __flash_mem_handle * const, __flash_mem_address);
    __flash_mem_address fmdr_addr;

    const uint32_t sector = fml->descriptor->fmh->descriptor->SECTOR_SIZE;

    const uint32_t align64k = FML_IS_ALIGN_BY_64K();
    const uint32_t align32k = FML_IS_ALIGN_BY_32K();

    while (FML_IS_ADDR_IN_RANGE(fml, iteration_addr)) {

        fmdr_addr.addr32 = FML_EXTADDR_TO_HWADDR(fml, iteration_addr);

        /* choose a block size */
        const uint32_t space_to_end = FML_GET_SPACE_TO_END(fml, iteration_addr);

        if (align64k && space_to_end >= BLOCK64_SIZE) {

            erase_func = &flash_mem_block64_erase;
            iteration_addr += BLOCK64_SIZE;

        } else if (align32k && space_to_end >= BLOCK32_SIZE) {

            erase_func = &flash_mem_block32_erase;
            iteration_addr += BLOCK32_SIZE;

        } else {

            erase_func = &flash_mem_sector_erase;
            iteration_addr += sector;

        }

        /* do erase */
        err = (*erase_func)(fml->descriptor->fmh, fmdr_addr);

        if (err != FMDR_OK) {
            return FML_ERASE_ERROR;
        }
    }

    return FML_OK;
}

