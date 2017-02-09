/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */



#include "smfifo.h"

#include <shared_utils.h>


/**
 * Private macros
 *
 */
#define SM_FIFO_SIZE         (handle->fifo_size)
#define SM_FIFO_START        (handle->fifo_buf)
#define SM_FIFO_END          (handle->fifo_buf + handle->fifo_size - 1)
#define SM_FIFO_BOUNDARY     (handle->fifo_buf + handle->fifo_size)


/**
 *
 */
void smfifo_flush(struct __smem_fifo_handle * const handle)
{
    handle->in_cursor = handle->out_cursor = SM_FIFO_START;
    handle->data_len = 0;
}


/**
 *
 */
uint16_t smfifo_available_space(struct __smem_fifo_handle * const handle)
{
    return SM_FIFO_SIZE - handle->data_len;
}


/**
 *
 */
uint16_t smfifo_filled_space(struct __smem_fifo_handle * const handle)
{
    return handle->data_len;
}


/**
 *
 */
bool smfifo_push_byte(struct __smem_fifo_handle * const handle, const uint8_t byte)
{
    if (SM_FIFO_SIZE - handle->data_len == 0) {
        return false;
    }

    *handle->in_cursor++ = byte;
    handle->data_len++;

    if (handle->in_cursor > SM_FIFO_END) {
        handle->in_cursor = SM_FIFO_START;
    }

    return true;
}


/**
 *
 */
uint8_t smfifo_pop_byte(struct __smem_fifo_handle * const handle)
{
    uint8_t byte;

    byte = 0;
    if (handle->data_len) {
        byte = *handle->out_cursor++;
        handle->data_len--;

        if (handle->out_cursor > SM_FIFO_END) {
            handle->out_cursor = SM_FIFO_START;
        }

        return byte;
    }

    return byte;
}


/**
 *
 */
bool smfifo_push_data(struct __smem_fifo_handle * const handle, const uint8_t * const src, const uint16_t len)
{
    uint16_t push_len;

    if (len > SM_FIFO_SIZE - handle->data_len) {
        return false;
    }

    handle->data_len += len;

    if (handle->in_cursor >= handle->out_cursor) {
        push_len = SM_FIFO_BOUNDARY - handle->in_cursor;

        if (len >= push_len) {
            mem_copy(handle->in_cursor, src, push_len);
            handle->in_cursor = SM_FIFO_START;

            if (len - push_len) {
                mem_copy(handle->in_cursor, src + push_len, len - push_len);
                handle->in_cursor += len - push_len;
            }

            return true;
        }
    }

    mem_copy(handle->in_cursor, src, len);
    handle->in_cursor += len;

    return true;
}


/**
 *
 */
bool smfifo_pop_data(struct __smem_fifo_handle * const handle, uint8_t * const dst, const uint16_t len)
{
    uint16_t pop_len;

    if (len > handle->data_len) {
        return false;
    }

    handle->data_len -= len;

    if (handle->out_cursor >= handle->in_cursor) {
        pop_len = SM_FIFO_BOUNDARY - handle->out_cursor;

        if (len >= pop_len) {
            mem_copy(dst, handle->out_cursor, pop_len);

            handle->out_cursor = SM_FIFO_START;

            if (len - pop_len) {
                mem_copy(dst + pop_len, handle->out_cursor, len - pop_len);

                handle->out_cursor += len - pop_len;
            }

            return true;
        }

    }

    mem_copy(dst, handle->out_cursor, len);
    handle->out_cursor += len;

    return true;
}


/**
 *
 */
bool smfifo_replace_data(struct __smem_fifo_handle * const handle, const uint16_t pos, const uint8_t * const src, const uint16_t len)
{
    uint8_t * replace_cursor;
    uint16_t replace_idx;

    if (!len || (pos + len > handle->data_len)) {
        return false;
    }

    if (handle->out_cursor >= handle->in_cursor) {

        const uint16_t last_pos = SM_FIFO_END - handle->out_cursor;

        replace_cursor = pos > last_pos ? SM_FIFO_START + (pos - last_pos - 1) : handle->out_cursor + pos;
    } else {
        replace_cursor = handle->out_cursor + pos;
    }

    replace_idx = 0;

    do {
        *replace_cursor++ = src[replace_idx++];

        if (replace_cursor > SM_FIFO_END) {
            replace_cursor = SM_FIFO_START;
        }
    }
    while (replace_idx < len);

    return true;
}


/**
 *
 */
void smfifo_get_cursor(struct __smem_fifo_handle * const handle, const uint8_t ** cursor, uint16_t * len)
{
    if (!handle->data_len) {
        *cursor = NULL;
        *len = 0;
    } else {
        *cursor = handle->out_cursor;
        *len = handle->out_cursor >= handle->in_cursor
                ? SM_FIFO_BOUNDARY - handle->out_cursor : handle->data_len;
    }
}


/**
 *
 */
bool smfifo_shift_cursor(struct __smem_fifo_handle * const handle, const uint16_t len)
{
    uint16_t shift_len;

    if (!len || !handle->data_len || len > handle->data_len) {
        return false;
    }

    handle->data_len -= len;

    if (handle->out_cursor >= handle->in_cursor) {
        shift_len = SM_FIFO_BOUNDARY - handle->out_cursor;

        if (len >= shift_len) {
            handle->out_cursor = SM_FIFO_START;
            handle->out_cursor += len - shift_len;

            return true;
        }
    }

    handle->out_cursor += len;
    return true;
}


