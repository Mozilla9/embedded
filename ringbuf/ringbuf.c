/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */


#include "ringbuf.h"


static void inc_tile(__ringbuf * const r);
static void inc_head(__ringbuf * const r);
static void __reset(struct __ringbuf * const r);
static void __put(struct __ringbuf * const r, const uint8_t c);
static uint16_t __size(struct __ringbuf * const r);
static uint8_t __get(struct __ringbuf * const r);


void init_ringbuf(__ringbuf * const r, uint8_t * const data, const uint16_t size)
{
    r->data = data;
    r->sz = size;
    r->tile = 0;
    r->head = 0;

    /* initialize interface */
    r->reset = __reset;
    r->put = __put;
    r->get = __get;
    r->size = __size;
}


/**
 * @brief Increment tile
 *
 * @param r - pinter on the __ringbuf
 */
static void inc_tile(__ringbuf * const r)
{
    if (r->tile > r->head) {
        if (r->tile == r->sz - 1) {
            if (r->head > 0) {
                r->tile = 0;
            }
        } else {
            r->tile++;
        }
    } else if (r->tile < r->head) {
        if (r->tile != r->head - 1) {
            r->tile++;
        }
    } else {
        r->tile++;
    }
}


/**
 * @brief Increment head
 *
 * @param r - pinter on the __ringbuf
 */
static void inc_head(__ringbuf * const r)
{
    if (r->head == r->sz - 1) {
        r->head = 0;
    } else {
        r->head++;
    }

    if (r->tile == r->head) {
        r->tile = 0;
        r->head = 0;
    }
}


/**
 * @brief Put data in buf
 *
 * @param r - pinter on the __ringbuf
 * @param c - data byte
 */
static void __put(struct __ringbuf * const r, const uint8_t c)
{
    r->data[r->tile] = c;
    inc_tile(r);
}


/**
 * @brief Get data byte from buf
 *
 * @param r - pinter on the __ringbuf
 */
static uint8_t __get(struct __ringbuf * const r)
{
    const uint8_t byte = r->data[r->head];

    inc_head(r);

    return byte;
}

/**
 * @brief Get data size
 *
 * @param r - pinter on the __ringbuf
 */
static uint16_t __size(struct __ringbuf * const r)
{
    if (r->head > r->tile) {
        return r->sz - r->head + r->tile;
    } else if (r->head < r->tile) {
        return r->tile - r->head;
    } else {
        return 0;
    }
}

/**
 * @brief Reset ring buff
 *
 * @param r - pinter on the __ringbuf
 */
static void __reset(struct __ringbuf * const r)
{
    r->tile = 0;
    r->head = 0;
}
