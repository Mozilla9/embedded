/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */



#include "fifotxt.h"

#include <shared_utils.h>




static uint8_t *__next_fifo_cell(__fifotxt_handle * const fh);
static void __add_msg_in_queue(__fifotxt_handle * const fh);



/**
 *
 */
void create_fifotxt(__fifotxt_handle * const fh)
{
    fh->fbuf->index = 0;
    fh->fqueue->index = 0;
    fh->fqueue->first = 0;
}


/**
 *
 */
void fifotxt_flush_fifo(__fifotxt_handle * const fh)
{
    fh->fqueue->index = 0;
    fh->fqueue->first = 0;
    fh->fbuf->index = 0;
}


/**
 *
 */
bool is_fifotxt_not_empty(__fifotxt_handle * const fh)
{
    return fh->fqueue->index > 0;
}


/**
 *
 */
bool fifotxt_cmp_last_msg(__fifotxt_handle * const fh, const char * msg)
{
    if (fh->fqueue->index) {
        const uint16_t sum = fh->fqueue->first + (fh->fqueue->index - 1);
        const uint16_t last_idx = sum < fh->fqueue->size ? sum : sum - fh->fqueue->size;

        uint8_t  *last_msg = fh->fqueue->queue + (last_idx * fh->fbuf->size);

        return str_cmp((const char *)last_msg, msg);
    }

    return false;
}


/**
 *
 */
bool fifotxt_cmp_first_msg(__fifotxt_handle * const fh, const char * msg)
{
    if (fh->fqueue->index) {

        uint8_t *first_msg = fh->fqueue->queue + (fh->fqueue->first * fh->fbuf->size);

        return str_cmp((const char *)first_msg, (const char *)msg);
    }

    return false;
}


/**
 *
 */
bool fifotxt_push_byte(__fifotxt_handle * const fh, const uint8_t byte)
{
    if (fh->fbuf->index < fh->fbuf->size) {

        if (fh->fbuf->index > 0 &&  byte != '\0' && byte != fh->IGNORED_TOKEN) {
            if (fh->END_TOKEN == byte) {

                fh->fbuf->buf[fh->fbuf->index++] = 0;
                __add_msg_in_queue(fh);

                return true;
            } else {
                fh->fbuf->buf[fh->fbuf->index++] = byte;
                fh->fbuf->buf[fh->fbuf->index] = 0;
            }
        } else {
            if (byte != '\0' && byte != fh->END_TOKEN && byte != fh->IGNORED_TOKEN) {
                fh->fbuf->buf[fh->fbuf->index++] = byte;
                fh->fbuf->buf[fh->fbuf->index] = 0;
            }
        }
    }

    return false;
}


/**
 *
 */
void fifotxt_push_msg(__fifotxt_handle * const fh)
{
    if (fh->fbuf->index) {
        __add_msg_in_queue(fh);
    }
}


/**
 *
 */
uint16_t fifotxt_pop_msg(__fifotxt_handle * const fh, uint8_t * in)
{
    uint16_t len;

    if (fh->fqueue->index > 0) {
        uint8_t * msg = fh->fqueue->queue + (fh->fqueue->first * fh->fbuf->size);

        len = str_copy(in, (const char *)msg);

        if (--fh->fqueue->index > 0) {
            fh->fqueue->first = (fh->fqueue->first + 1 < fh->fqueue->size)
                    ? fh->fqueue->first + 1 : 0;
        }

    } else {
        len = 0;
        in[0] = 0;
    }

    return len;
}


/**
 *
 */
void fifotxt_discard_msg(__fifotxt_handle * const fh)
{
    if (fh->fqueue->index) {
        fh->fqueue->index--;

        if (fh->fqueue->index) {
            fh->fqueue->first = (fh->fqueue->first + 1 < fh->fqueue->size)
                    ? fh->fqueue->first + 1 : 0;
        } else {
            fh->fqueue->first = 0;
        }
    }
}


/**
 *
 */
static uint8_t *__next_fifo_cell(__fifotxt_handle * const fh)
{
    const uint16_t sum = fh->fqueue->first + fh->fqueue->index;
    const uint16_t next_idx = sum < fh->fqueue->size ? sum : sum - fh->fqueue->size;

    return fh->fqueue->queue + (next_idx * fh->fbuf->size);
}


/**
 *
 */
static void __add_msg_in_queue(__fifotxt_handle * const fh)
{
    uint8_t * cell;

    if (fh->fqueue->index < fh->fqueue->size
            && fh->fbuf->index > 0 && fh->fbuf->index < fh->fbuf->size) {

        cell = __next_fifo_cell(fh);
        str_copy(cell, (const char *)fh->fbuf->buf);

        fh->fqueue->index++;
    }

    fh->fbuf->index = 0;
}

